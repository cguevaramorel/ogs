/**
 * \copyright
 * Copyright (c) 2012-2017, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "CreateTH2MProcess.h"

#include <cassert>

#include "MaterialLib/SolidModels/CreateLinearElasticIsotropic.h"
#include "ProcessLib/Output/CreateSecondaryVariables.h"

#include "TH2MProcess.h"
#include "TH2MProcessData.h"

namespace ProcessLib
{
namespace TH2M
{
void validateTH2MProcessVariables(
    std::vector<std::reference_wrapper<ProcessVariable>> process_variables,
    int DisplacementDim)
{
    if (process_variables.size() != 4)
    {
        OGS_FATAL(
            "Wrong number of process variables found. TH2M requires four"
            " primary unknowns: gas pressure, capilalry pressure,"
            " temperature, and displacement!");
    }

    const int DisplacementDimVector[4] = {1, 1, 1, DisplacementDim};
    const std::string process_variable_name[4] = {
        "gas pressure", "capillary pressure", "temperature", "displacement"};

    for (int i = indexGasPressure; i <= indexDisplacement; i++)
    {
        DBUG("Associate %s with process variable \'%s\'.",
             process_variable_name[i].c_str(),
             process_variables[i].get().getName().c_str());

        if (process_variables[i].get().getNumberOfComponents() !=
            DisplacementDimVector[i])
        {
            OGS_FATAL(
                "Number of components of the process variable '%s' is "
                "different "
                "from the displacement dimension: got %d, expected %d",
                process_variables[i].get().getName().c_str(),
                process_variables[i].get().getNumberOfComponents(),
                DisplacementDimVector[i]);
        }
    }
}

template <int DisplacementDim>
std::unique_ptr<Process> createTH2MProcess(
    MeshLib::Mesh& mesh,
    std::unique_ptr<ProcessLib::AbstractJacobianAssembler>&& jacobian_assembler,
    std::vector<ProcessVariable> const& variables,
    std::vector<std::unique_ptr<ParameterBase>> const& parameters,
    unsigned const integration_order,
    BaseLib::ConfigTree const& config,
    std::unique_ptr<MaterialPropertyLib::Medium>&& medium)
{
    //! \ogs_file_param{prj__processes__process__type}
    config.checkConfigParameter("type", "TH2M");
    DBUG("Create TH2MProcess.");

    // Process variable.

    //! \ogs_file_param{prj__processes__process__HYDRO_MECHANICS__process_variables}
    auto const pv_config = config.getConfigSubtree("process_variables");

    auto per_process_variables = findProcessVariables(
        variables, pv_config,
        {//! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__process_variables__pressure}
         "gas_pressure",
         //! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__process_variables__pressure}
         "capillary_pressure",
         //! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__process_variables__pressure}
         "temperature",
         //! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__process_variables__displacement}
         "displacement"});

    std::vector<std::vector<std::reference_wrapper<ProcessVariable>>>
        process_variables;

    process_variables.push_back(std::move(per_process_variables));

    validateTH2MProcessVariables(process_variables[0], DisplacementDim);

    // Constitutive relation.
    // read type;
    auto const constitutive_relation_config =
        //! \ogs_file_param{prj__processes__process__HYDRO_MECHANICS__constitutive_relation}
        config.getConfigSubtree("constitutive_relation");

    auto const type =
        //! \ogs_file_param{prj__processes__process__HYDRO_MECHANICS__constitutive_relation__type}
        constitutive_relation_config.peekConfigParameter<std::string>("type");

    std::unique_ptr<MaterialLib::Solids::MechanicsBase<DisplacementDim>>
        material = nullptr;
    if (type == "LinearElasticIsotropic")
    {
        material =
            MaterialLib::Solids::createLinearElasticIsotropic<DisplacementDim>(
                parameters, constitutive_relation_config);
    }
    else
    {
        OGS_FATAL(
            "Cannot construct constitutive relation of given type \'%s\'.",
            type.c_str());
    }

    // Intrinsic permeability
    auto& intrinsic_permeability = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__intrinsic_permeability}
        "intrinsic_permeability", parameters, 1);

    DBUG("Use \'%s\' as intrinsic conductivity parameter.",
         intrinsic_permeability.name.c_str());

    // Storage coefficient
    auto& specific_storage = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__specific_storage}
        "specific_storage", parameters, 1);

    DBUG("Use \'%s\' as storage coefficient parameter.",
         specific_storage.name.c_str());

    // Fluid viscosity
    auto& fluid_viscosity = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__fluid_viscosity}
        "fluid_viscosity", parameters, 1);
    DBUG("Use \'%s\' as fluid viscosity parameter.",
         fluid_viscosity.name.c_str());

    // Fluid density
    auto& fluid_density = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__fluid_density}
        "fluid_density", parameters, 1);
    DBUG("Use \'%s\' as fluid density parameter.", fluid_density.name.c_str());

    // Biot coefficient
    auto& biot_coefficient = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__biot_coefficient}
        "biot_coefficient", parameters, 1);
    DBUG("Use \'%s\' as Biot coefficient parameter.",
         biot_coefficient.name.c_str());

    // Porosity
    auto& porosity = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__porosity}
        "porosity", parameters, 1);
    DBUG("Use \'%s\' as porosity parameter.", porosity.name.c_str());

    // Solid density
    auto& solid_density = findParameter<double>(
        config,
        //! \ogs_file_param_special{prj__processes__process__HYDRO_MECHANICS__solid_density}
        "solid_density", parameters, 1);
    DBUG("Use \'%s\' as solid density parameter.", solid_density.name.c_str());

    // Specific body force
    Eigen::Matrix<double, DisplacementDim, 1> specific_body_force;
    {
        std::vector<double> const b =
            //! \ogs_file_param{prj__processes__process__HYDRO_MECHANICS__specific_body_force}
            config.getConfigParameter<std::vector<double>>(
                "specific_body_force");
        if (b.size() != DisplacementDim)
            OGS_FATAL(
                "The size of the specific body force vector does not match the "
                "displacement dimension. Vector size is %d, displacement "
                "dimension is %d",
                b.size(), DisplacementDim);

        std::copy_n(b.data(), b.size(), specific_body_force.data());
    }

    TH2MProcessData<DisplacementDim> process_data{std::move(material),
                                                  intrinsic_permeability,
                                                  specific_storage,
                                                  fluid_viscosity,
                                                  fluid_density,
                                                  biot_coefficient,
                                                  porosity,
                                                  solid_density,
                                                  specific_body_force,
                                                  std::move(medium)};

    SecondaryVariableCollection secondary_variables;

    NumLib::NamedFunctionCaller named_function_caller(
        {"TH2M_gas_pressure", "TH2M_capillary_pressure", "TH2M_temperature",
         "TH2M_displacement"});

    ProcessLib::createSecondaryVariables(config, secondary_variables,
                                         named_function_caller);

    return std::make_unique<TH2MProcess<DisplacementDim>>(
        mesh, std::move(jacobian_assembler), parameters, integration_order,
        std::move(process_variables), std::move(process_data),
        std::move(secondary_variables), std::move(named_function_caller));
}

template std::unique_ptr<Process> createTH2MProcess<2>(
    MeshLib::Mesh& mesh,
    std::unique_ptr<ProcessLib::AbstractJacobianAssembler>&& jacobian_assembler,
    std::vector<ProcessVariable> const& variables,
    std::vector<std::unique_ptr<ParameterBase>> const& parameters,
    unsigned const integration_order,
    BaseLib::ConfigTree const& config,
    std::unique_ptr<MaterialPropertyLib::Medium>&& medium);

template std::unique_ptr<Process> createTH2MProcess<3>(
    MeshLib::Mesh& mesh,
    std::unique_ptr<ProcessLib::AbstractJacobianAssembler>&& jacobian_assembler,
    std::vector<ProcessVariable> const& variables,
    std::vector<std::unique_ptr<ParameterBase>> const& parameters,
    unsigned const integration_order,
    BaseLib::ConfigTree const& config,
    std::unique_ptr<MaterialPropertyLib::Medium>&& medium);

}  // namespace TH2M
}  // namespace ProcessLib