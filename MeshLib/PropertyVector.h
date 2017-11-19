/**
 *
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include <cassert>
#include <iterator>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "BaseLib/Error.h"
#include "BaseLib/excludeObjectCopy.h"
#include "Location.h"

#include "PropertyVectorBase.h"

namespace MeshLib
{
/// Class template PropertyVector is a std::vector with template parameter
/// PROP_VAL_TYPE. The reason for the derivation of std::vector is
/// the template specialisation for pointer types below.
/// \tparam PROP_VAL_TYPE typical this is a scalar, a vector or a matrix
template <typename PROP_VAL_TYPE>
class PropertyVector final : public PropertyVectorBase
{
    friend class Properties;

public:
    using value_type = PROP_VAL_TYPE;
    using iterator = typename std::vector<value_type>::iterator;
    using const_iterator = typename std::vector<value_type>::const_iterator;

public:
    PropertyVector(PropertyVector const& other) {}

    std::size_t getNumberOfTuples() const { return size() / _n_components; }

    //! Returns the value for the given component stored in the given tuple.
    PROP_VAL_TYPE& getComponent(std::size_t const tuple_index,
                                int const component)
    {
        assert(component < _n_components);
        assert(tuple_index < getNumberOfTuples());
        return _values[tuple_index * getNumberOfComponents() + component];
    }

    //! Returns the value for the given component stored in the given tuple.
    PROP_VAL_TYPE const& getComponent(std::size_t const tuple_index,
                                      int component) const
    {
        assert(component < _n_components);
        assert(tuple_index < getNumberOfTuples());
        return _values[tuple_index * getNumberOfComponents() + component];
    }

    /*
    PropertyVectorBase* clone(
        std::vector<std::size_t> const& exclude_positions) const override
    {
        auto* t(new PropertyVector<PROP_VAL_TYPE>(
            _property_name, _mesh_item_type, _n_components));
        BaseLib::excludeObjectCopy(*this, exclude_positions, *t);
        return t;
    }
    */

    PROP_VAL_TYPE const& operator[](std::size_t const index) const
    {
        return _values[index];
    }

    PROP_VAL_TYPE& operator[](std::size_t const index) { return _values[index]; }

    /// Method returns the number of tuples times the number of tuple components.
    std::size_t size() const { return _values.size(); }

    bool empty() const { return _values.empty(); }

    auto cbegin() const { return _values.cbegin(); }
    auto cend() const { return _values.cend(); }
    auto begin() const { return _values.begin(); }
    auto end() const { return _values.end(); }
    auto begin() { return _values.begin(); }
    auto end() { return _values.end(); }
    iterator insert(const_iterator pos, value_type const& value)
    {
        return _values.insert(pos, value);
    }
    iterator insert(const_iterator pos, std::size_t count,
                    value_type const& value)
    {
        return _values.insert(pos, count, value);
    }
    template <typename InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        return _values.insert(pos, first, last);
    }
    void reserve(std::size_t const count) { _values.reserve(count); }
    void resize(std::size_t const count) { _values.resize(count); }
    void resize(std::size_t const count, value_type const& value)
    {
        _values.resize(count, value);
    }

    void push_back(PROP_VAL_TYPE const& value) { _values.push_back(value); }
    void push_back(PROP_VAL_TYPE&& value) { _values.push_back(std::move(value)); }

    auto data() { return _values.data(); }

protected:
    /// @brief The constructor taking meta information for the data.
    /// @param property_name a string describing the property
    /// @param mesh_item_type the values of the property are either assigned to
    /// nodes or cells (see enumeration MeshItemType)
    /// @param n_components the number of components of a property
    explicit PropertyVector(std::string const& property_name,
                            MeshItemType mesh_item_type,
                            std::size_t n_components)
        : PropertyVectorBase(property_name, mesh_item_type, n_components)
    {}

    /// @brief The constructor taking meta information for the data.
    /// @param n_property_values number of property values (value can be a tuple
    /// with several entries)
    /// @param property_name a string describing the property
    /// @param mesh_item_type the values of the property are either assigned to
    /// nodes or cells (see enumeration MeshItemType)
    /// @param n_components the number of components of a property
    PropertyVector(std::size_t n_property_values,
                   std::string const& property_name,
                   MeshItemType mesh_item_type,
                   std::size_t n_components)
        : _values(n_property_values * n_components),
          PropertyVectorBase(property_name, mesh_item_type, n_components)
    {}

private:
    std::vector<PROP_VAL_TYPE> _values;
};

/// Class template PropertyVector is a std::vector with template parameter
/// T, where T is a pointer type.
/// The behaviour has changed for the constructor, destructor and the operator[].
/// The user has to provide the size and an item to group mapping for construction.
/// The destructor takes care to delete the entries of the vector.
/// The operator[] uses an item-to-group property map to access the
/// correct property.
/// \tparam T pointer type, the type the type points to is typical a scalar,
/// a vector or a matrix type
template <typename T>
class PropertyVector<T*> final : public PropertyVectorBase
{
friend class Properties;
public:
    /// Destructor ensures the deletion of the heap-constructed objects.
    ~PropertyVector() override
    {
        for (auto v : _values)
            delete [] v;
    }

    /// The operator[] uses the item to group property map to access to the
    /// correct property value/object.
    T* const& operator[](std::size_t const id) const
    {
        return _values[_pointers[id]];
    }

    T*& operator[](std::size_t const id) { return _values[_pointers[id]]; }

    void initPropertyValue(std::size_t group_id, T const& value)
    {
        if (_n_components != 1)
            OGS_FATAL("Single-component version of initPropertyValue() is called "
                      "for a multi-components PropertyVector<T*>");
        auto* p = new T[1];
        p[0] = value;
        _values[group_id] = p;
    }

    void initPropertyValue(std::size_t group_id, std::vector<T> const& values)
    {
        if (_n_components != static_cast<int>(values.size()))
            OGS_FATAL("The size of provided values in initPropertyValue() is "
                      "not same as the number of components in PropertyVector<T*>");

        auto* p = new T[values.size()];
        for (unsigned i=0; i<values.size(); i++)
            p[i] = values[i];
        _values[group_id] = p;
    }

    std::size_t getNumberOfTuples() const { return _pointers.size(); }

    /// Method returns the number of tuples times the number of tuple components.
    std::size_t size() const { return _n_components * _pointers.size(); }

    /*
    PropertyVectorBase* clone(
        std::vector<std::size_t> const& exclude_positions) const override
    {
        // create new PropertyVector with modified mapping
        PropertyVector<T*> *t(new PropertyVector<T*>
            (
                _values.size()/_n_components,
                BaseLib::excludeObjectCopy(*this, exclude_positions),
                _property_name, _mesh_item_type, _n_components
            )
        );
        // copy pointers to property values
        for (std::size_t j(0); j<_values.size(); j++) {
            std::vector<T> values(_values[j], _values[j] + _n_components);
            t->initPropertyValue(j, values);
        }
        return t;
    }
    */

    //! Returns the value for the given component stored in the given tuple.
    T const& getComponent(std::size_t tuple_index, int component) const
    {
        assert(component < _n_components);
        assert(tuple_index < getNumberOfTuples());
        const double* p = _pointers[tuple_index];
        if (p == nullptr)
            OGS_FATAL("No data found in the property vector %s "
                      "for the tuple index %d and component %d",
                      getPropertyName().c_str(), tuple_index, component);
        return p[component];
    }

#ifndef NDEBUG
    std::ostream& print(std::ostream &os) const
    {
        os << "\nPropertyVector<T*> at address: " << this << ":\n";
        os << "\tmapping (" << size() <<"):\n";
        std::copy(_pointers.cbegin(), _pointers.cend(),
            std::ostream_iterator<std::size_t>(os, " "));
        os << "\n\tvalues (" << _values.size() << "):\n";
        for (std::size_t k(0); k < _values.size(); k++)
        {
            os << "val: " << *(_values[k]) << ", address: " << _values[k] << "\n";
        }
        return os;
    }
#endif

protected:
    /// @brief The constructor taking meta information for the data.
    /// @param n_prop_groups number of different property values
    /// @param item2group_mapping Class Mesh has a mapping from the mesh items
    /// (Node or Element) to an index (position in the data structure).
    /// The vector item2group_mapping must have the same number of entries as
    /// the above mapping and the values have to be in the range
    /// \f$[0, \text{n_prop_groups})\f$.
    /// @param property_name a string describing the property
    /// @param mesh_item_type the values of the property are either assigned to
    /// nodes or cells (see enumeration MeshItemType)
    /// @param n_components the number of elements of a tuple
    PropertyVector(std::size_t n_prop_groups,
                   std::vector<std::size_t>
                       item2group_mapping,
                   std::string const& property_name,
                   MeshItemType mesh_item_type,
                   std::size_t n_components)
        : _pointers(std::move(item2group_mapping)),
          PropertyVectorBase(property_name, mesh_item_type, n_components),
          _values(n_prop_groups * n_components)
    {}

private:
    std::vector<T*> _values;
    std::vector<std::size_t> _pointers;
};

} // end namespace MeshLib
