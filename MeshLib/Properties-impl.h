/**
 * \file
 * \brief  Implemenatiom of the template part of the class Properties.
 *
 * \copyright
 * Copyright (c) 2012-2020, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

template <typename T>
PropertyVector<T>* Properties::createNewPropertyVector(
    std::string const& name,
    MeshItemType mesh_item_type,
    std::size_t n_components)
{
    if (hasPropertyVector(name, mesh_item_type))
    {
        ERR("A property of the name '{:s}' and mesh item type {} is already "
            "assigned to the mesh.",
            name, mesh_item_type);
        return nullptr;
    }
    auto entry_info(
        _properties.insert(
            std::make_pair(
                name, new PropertyVector<T>(name, mesh_item_type, n_components)
            )
        )
    );
    return static_cast<PropertyVector<T>*>(entry_info->second);
}

template <typename T>
PropertyVector<T>* Properties::createNewPropertyVector(
    std::string const& name,
    std::size_t n_prop_groups,
    std::vector<std::size_t> const& item2group_mapping,
    MeshItemType mesh_item_type,
    std::size_t n_components)
{
    if (hasPropertyVector(name, mesh_item_type))
    {
        ERR("A property of the name '{:s}' and mesh item type {} is already "
            "assigned to the mesh.",
            name, mesh_item_type);
        return nullptr;
    }

    // check entries of item2group_mapping for consistence
    for (std::size_t k(0); k<item2group_mapping.size(); k++) {
        std::size_t const group_id (item2group_mapping[k]);
        if (group_id >= n_prop_groups) {
            ERR("The mapping to property {:d} for item {:d} is not in the "
                "correct range [0,{:d}).",
                group_id, k, n_prop_groups);
            return nullptr;
        }
    }

    auto entry_info(
        _properties.insert(
            std::pair<std::string, PropertyVectorBase*>(
                name,
                new PropertyVector<T>(n_prop_groups,
                    item2group_mapping, name, mesh_item_type, n_components)
            )
        )
    );
    return static_cast<PropertyVector<T>*>(entry_info->second);
}

template <typename T>
PropertyVector<T>* Properties::findPropertyVector(
    std::string const& name, MeshItemType const mesh_item_type)
{
    return dynamic_cast<PropertyVector<T>*>(
        findPropertyVector(name, mesh_item_type));
}

template <typename T>
PropertyVector<T>* Properties::findPropertyVector(
    std::string const& name, MeshItemType const mesh_item_type,
    int const number_of_components)
{
    if (auto const pv = findPropertyVector<T>(name, mesh_item_type);
        (pv != nullptr) &&
        (pv->getNumberOfComponents() == number_of_components))
    {
        return pv;
    }
    return nullptr;
}

template <typename T>
PropertyVector<T> const* Properties::findPropertyVector(
    std::string const& name, MeshItemType const mesh_item_type) const
{
    return findPropertyVector<T>(name, mesh_item_type);
}

template <typename T>
PropertyVector<T> const* Properties::findPropertyVector(
    std::string const& name,
    MeshItemType const mesh_item_type,
    int const number_of_components) const
{
    return findPropertyVector<T>(name, mesh_item_type, number_of_components);
}

template <typename T>
bool Properties::existsPropertyVector(std::string const& name,
                                      MeshItemType const mesh_item_type) const
{
    return findPropertyVector<T>(name, mesh_item_type) != nullptr;
}

template <typename T>
bool Properties::existsPropertyVector(std::string const& name,
                                      MeshItemType const mesh_item_type,
                                      int const number_of_components) const
{
    return findPropertyVector<T>(name, mesh_item_type, number_of_components) !=
           nullptr;
}

template <typename T>
PropertyVector<T>& Properties::getPropertyVector(
    std::string const& name, MeshItemType const mesh_item_type)
{
    auto const pv = findPropertyVector<T>(name, mesh_item_type);
    if (pv == nullptr)
    {
        OGS_FATAL(
            "A PropertyVector '{:s}' of mesh item type {} and value type {} is "
            "not available in the mesh.",
            name, mesh_item_type, typeid(T).name());
    }
    return *pv;
}

template <typename T>
PropertyVector<T>& Properties::getPropertyVector(
    std::string const& name, MeshItemType const mesh_item_type,
    int const n_components)
{
    auto const pv = findPropertyVector<T>(name, mesh_item_type, n_components);
    if (pv == nullptr)
    {
        OGS_FATAL(
            "A PropertyVector '{:s}' of mesh item type {}, value type {} and "
            "{} components is not available in the mesh.",
            name, mesh_item_type, typeid(T).name(), n_components);
    }
    return *pv;
}

template <typename T>
PropertyVector<T> const& Properties::getPropertyVector(
    std::string const& name, MeshItemType const mesh_item_type) const
{
    return getPropertyVector<T>(name, mesh_item_type);
}

template <typename T>
PropertyVector<T> const& Properties::getPropertyVector(
    std::string const& name, MeshItemType const mesh_item_type,
    int const n_components) const
{
    return getPropertyVector<T>(name, mesh_item_type, n_components);
}
