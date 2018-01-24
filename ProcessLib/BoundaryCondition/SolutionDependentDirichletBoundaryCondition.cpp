/**
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "SolutionDependentDirichletBoundaryCondition.h"

#include <algorithm>
#include <logog/include/logog.hpp>
#include <vector>
#include "ProcessLib/Utils/ProcessUtils.h"

namespace ProcessLib
{
void SolutionDependentDirichletBoundaryCondition::getEssentialBCValues(
    const double /*t*/, NumLib::IndexValueVector<GlobalIndexType>& bc_values) const
{
    SpatialPosition pos;

    bc_values.ids.clear();
    bc_values.values.clear();

    // convert mesh node ids to global index for the given component
    bc_values.ids.reserve(bc_values.ids.size() + _bc_values.ids.size());
    bc_values.values.reserve(bc_values.values.size() +
                             _bc_values.values.size());

    std::copy(_bc_values.ids.begin(), _bc_values.ids.end(),
              std::back_inserter(bc_values.ids));
    std::copy(_bc_values.values.begin(), _bc_values.values.end(),
              std::back_inserter(bc_values.values));
}

// update new values and corresponding indices.
void SolutionDependentDirichletBoundaryCondition::preTimestep(
    const double /*t*/, const GlobalVector& x)
{
    double irrevDamage = 0.05;

    _bc_values.ids.clear();
    _bc_values.values.clear();

    auto const mesh_id = _mesh.getID();
    auto const& nodes = _mesh.getNodes();
    for (auto const* n : nodes)
    {
        std::size_t id = n->getID();
        MeshLib::Location l(mesh_id, MeshLib::MeshItemType::Node, id);
        const auto g_idx =
            _dof_table.getGlobalIndex(l, _variable_id, _component_id);

        if (x[id] <= irrevDamage)
        {
            _bc_values.ids.emplace_back(g_idx);
            _bc_values.values.emplace_back(0.0);
        }
    }
}

std::unique_ptr<SolutionDependentDirichletBoundaryCondition>
createSolutionDependentDirichletBoundaryCondition(
    BaseLib::ConfigTree const& config,
    NumLib::LocalToGlobalIndexMap const& dof_table, MeshLib::Mesh const& mesh,
    int const variable_id, int const component_id)
{
    DBUG(
        "Constructing SolutionDependentDirichletBoundaryCondition from "
        "config.");
    //! \ogs_file_param{prj__process_variables__process_variable__boundary_conditions__boundary_condition__type}
    config.checkConfigParameter("type", "SolutionDependentDirichlet");

    return std::make_unique<SolutionDependentDirichletBoundaryCondition>(
        dof_table, mesh, variable_id, component_id);
}

}  // namespace ProcessLib
