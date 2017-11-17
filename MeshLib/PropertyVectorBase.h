/**
 * \file
 *
 * \copyright
 * Copyright (c) 2012-2017, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include <string>
#include <vector>

namespace MeshLib
{
class PropertyVectorBase
{
public:
    virtual PropertyVectorBase* clone(
        std::vector<std::size_t> const& exclude_positions) const = 0;
    virtual ~PropertyVectorBase() = default;

    MeshItemType getMeshItemType() const { return _mesh_item_type; }
    std::string const& getPropertyName() const { return _property_name; }
    std::size_t getNumberOfComponents() const { return _n_components; }

protected:
    PropertyVectorBase(std::string property_name,
                       MeshItemType mesh_item_type,
                       std::size_t n_components)
        : _n_components(n_components),
          _mesh_item_type(mesh_item_type),
          _property_name(std::move(property_name))
    {
    }

    std::size_t const _n_components;
    MeshItemType const _mesh_item_type;
    std::string const _property_name;
};
}  // end namespace MeshLib
