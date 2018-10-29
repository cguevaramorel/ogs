/**
 * \copyright
 * Copyright (c) 2012-2017, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "CreateSmallDeformationNonlocalProcess.h"

#include <cassert>

#include "MaterialLib/SolidModels/CreateConstitutiveRelation.h"
#include "ProcessLib/Output/CreateSecondaryVariables.h"

#include "SmallDeformationNonlocalProcess.h"
#include "SmallDeformationNonlocalProcessData.h"

namespace ProcessLib
{
namespace SmallDeformationNonlocal
{
template <int DisplacementDim>
class SmallDeformationNonlocalProcess;

extern template class SmallDeformationNonlocalProcess<2>;
extern template class SmallDeformationNonlocalProcess<3>;

template <int DisplacementDim>
std::unique_ptr<Process> createSmallDeformationNonlocalProcess(
    MeshLib::Mesh& mesh,
    std::unique_ptr<ProcessLib::AbstractJacobianAssembler>&& jacobian_assembler,
    std::vector<ProcessVariable> const& variables,
    std::vector<std::unique_ptr<ParameterBase>> const& parameters,
    unsigned const integration_order,
    BaseLib::ConfigTree const& config)
{
    //! \ogs_file_param{prj__processes__process__type}
    config.checkConfigParameter("type", "SMALL_DEFORMATION_NONLOCAL");
    DBUG("Create SmallDeformationNonlocalProcess.");

    // Process variable.

    //! \ogs_file_param{prj__processes__process__SMALL_DEFORMATION__process_variables}
    auto const pv_config = config.getConfigSubtree("process_variables");

    auto per_process_variables = findProcessVariables(
        variables, pv_config,
        {//! \ogs_file_param_special{prj__processes__process__SMALL_DEFORMATION__process_variables__process_variable}
         "process_variable"});

    DBUG("Associate displacement with process variable \'%s\'.",
         per_process_variables.back().get().getName().c_str());

    if (per_process_variables.back().get().getNumberOfComponents() !=
        DisplacementDim)
    {
        OGS_FATAL(
            "Number of components of the process variable '%s' is different "
            "from the displacement dimension: got %d, expected %d",
            per_process_variables.back().get().getName().c_str(),
            per_process_variables.back().get().getNumberOfComponents(),
            DisplacementDim);
    }
    std::vector<std::vector<std::reference_wrapper<ProcessVariable>>>
        process_variables;
    process_variables.push_back(std::move(per_process_variables));

    auto solid_constitutive_relations =
        MaterialLib::Solids::createConstitutiveRelations<DisplacementDim>(
            parameters, config);


    {
    }

    // Reference temperature
    const auto& reference_temperature =
        //! \ogs_file_param{prj__processes__process__SMALL_DEFORMATION_NONLOCAL__reference_temperature}
        config.getConfigParameter<double>(
            "reference_temperature", std::numeric_limits<double>::quiet_NaN());

    auto const internal_length =
        //! \ogs_file_param{prj__processes__process__SMALL_DEFORMATION__internal_length}
        config.getConfigParameter<double>("internal_length");

    SmallDeformationNonlocalProcessData<DisplacementDim> process_data{
        std::move(material), internal_length};

    SecondaryVariableCollection secondary_variables;

    NumLib::NamedFunctionCaller named_function_caller(
        {"SmallDeformationNonlocal_displacement"});

    ProcessLib::createSecondaryVariables(config, secondary_variables,
                                         named_function_caller);

    return std::make_unique<SmallDeformationNonlocalProcess<DisplacementDim>>(
        mesh, std::move(jacobian_assembler), parameters, integration_order,
        std::move(process_variables), std::move(process_data),
        std::move(secondary_variables), std::move(named_function_caller));
}

template std::unique_ptr<Process> createSmallDeformationNonlocalProcess<2>(
    MeshLib::Mesh& mesh,
    std::unique_ptr<ProcessLib::AbstractJacobianAssembler>&& jacobian_assembler,
    std::vector<ProcessVariable> const& variables,
    std::vector<std::unique_ptr<ParameterBase>> const& parameters,
    unsigned const integration_order,
    BaseLib::ConfigTree const& config);

template std::unique_ptr<Process> createSmallDeformationNonlocalProcess<3>(
    MeshLib::Mesh& mesh,
    std::unique_ptr<ProcessLib::AbstractJacobianAssembler>&& jacobian_assembler,
    std::vector<ProcessVariable> const& variables,
    std::vector<std::unique_ptr<ParameterBase>> const& parameters,
    unsigned const integration_order,
    BaseLib::ConfigTree const& config);

}  // namespace SmallDeformationNonlocal
}  // namespace ProcessLib
