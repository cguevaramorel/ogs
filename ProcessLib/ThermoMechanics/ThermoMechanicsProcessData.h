/**
 * \copyright
 * Copyright (c) 2012-2019, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include <memory>
#include <utility>

#include <Eigen/Eigen>

#include "ParameterLib/Parameter.h"

namespace MaterialLib
{
namespace Solids
{
template <int DisplacementDim>
struct MechanicsBase;
}
}
namespace ProcessLib
{
namespace ThermoMechanics
{
template <int DisplacementDim>
struct ThermoMechanicsProcessData
{
    ThermoMechanicsProcessData(
        MeshLib::PropertyVector<int> const* const material_ids_,
        std::map<int,
                 std::unique_ptr<
                     MaterialLib::Solids::MechanicsBase<DisplacementDim>>>&&
            solid_materials_,
        ParameterLib::Parameter<double> const& reference_solid_density_,
        ParameterLib::Parameter<double> const&
            linear_thermal_expansion_coefficient_,
        ParameterLib::Parameter<double> const& specific_heat_capacity_,
        ParameterLib::Parameter<double> const& thermal_conductivity_,
        Eigen::Matrix<double, DisplacementDim, 1> const& specific_body_force_,
        ParameterLib::Parameter<double> const* const nonequilibrium_stress_)
        : material_ids(material_ids_),
          solid_materials{std::move(solid_materials_)},
          reference_solid_density(reference_solid_density_),
          linear_thermal_expansion_coefficient(
              linear_thermal_expansion_coefficient_),
          specific_heat_capacity(specific_heat_capacity_),
          thermal_conductivity(thermal_conductivity_),
          nonequilibrium_stress(nonequilibrium_stress_),
          specific_body_force(specific_body_force_)
    {
    }

    ThermoMechanicsProcessData(ThermoMechanicsProcessData&& other) = default;

    //! Copies are forbidden.
    ThermoMechanicsProcessData(ThermoMechanicsProcessData const&) = delete;

    //! Assignments are not needed.
    void operator=(ThermoMechanicsProcessData const&) = delete;

    //! Assignments are not needed.
    void operator=(ThermoMechanicsProcessData&&) = delete;

    MeshLib::PropertyVector<int> const* const material_ids;

    /// The constitutive relation for the mechanical part.
    std::map<
        int,
        std::unique_ptr<MaterialLib::Solids::MechanicsBase<DisplacementDim>>>
        solid_materials;

    ParameterLib::Parameter<double> const& reference_solid_density;
    ParameterLib::Parameter<double> const& linear_thermal_expansion_coefficient;
    ParameterLib::Parameter<double> const& specific_heat_capacity;
    ParameterLib::Parameter<double> const* const nonequilibrium_stress;
    ParameterLib::Parameter<double> const&
        thermal_conductivity;  // TODO To be changed as a matrix type variable.
    Eigen::Matrix<double, DisplacementDim, 1> const specific_body_force;
    double dt = 0;
    double t = 0;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

}  // namespace ThermoMechanics
}  // namespace ProcessLib
