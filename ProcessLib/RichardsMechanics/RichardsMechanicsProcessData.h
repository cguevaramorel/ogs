/**
 * \copyright
 * Copyright (c) 2012-2019, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include "ParameterLib/Parameter.h"

#include <memory>
#include <utility>

#include <Eigen/Dense>

#include "ProcessLib/RichardsFlow/RichardsFlowMaterialProperties.h"

namespace MaterialLib
{
namespace Solids
{
template <int DisplacementDim>
struct MechanicsBase;
}
}  // namespace MaterialLib
namespace ProcessLib
{
namespace RichardsMechanics
{
template <int DisplacementDim>
struct RichardsMechanicsProcessData
{
    RichardsMechanicsProcessData(
        MeshLib::PropertyVector<int> const* const material_ids_,
        std::unique_ptr<
            ProcessLib::RichardsFlow::RichardsFlowMaterialProperties>&&
            flow_material_,
        std::map<int,
                 std::unique_ptr<
                     MaterialLib::Solids::MechanicsBase<DisplacementDim>>>&&
            solid_materials_,
        ParameterLib::Parameter<double> const& fluid_bulk_modulus_,
        ParameterLib::Parameter<double> const& biot_coefficient_,
        ParameterLib::Parameter<double> const& solid_density_,
        ParameterLib::Parameter<double> const& solid_bulk_modulus_,
        ParameterLib::Parameter<double> const& temperature_,
        ParameterLib::Parameter<double> const* const sigma_0neq_,
        ParameterLib::Parameter<double> const* const pressure_0neq_,
        Eigen::Matrix<double, DisplacementDim, 1>
            specific_body_force_)
        : material_ids(material_ids_),
          flow_material{std::move(flow_material_)},
          solid_materials{std::move(solid_materials_)},
          fluid_bulk_modulus(fluid_bulk_modulus_),
          biot_coefficient(biot_coefficient_),
          solid_density(solid_density_),
          solid_bulk_modulus(solid_bulk_modulus_),
          temperature(temperature_),
          sigma_0neq(sigma_0neq_),
          pressure_0neq(pressure_0neq_),
          specific_body_force(std::move(specific_body_force_))
    {
    }

    RichardsMechanicsProcessData(RichardsMechanicsProcessData&& other) =
        default;

    //! Copies are forbidden.
    RichardsMechanicsProcessData(RichardsMechanicsProcessData const&) = delete;

    //! Assignments are not needed.
    void operator=(RichardsMechanicsProcessData const&) = delete;
    void operator=(RichardsMechanicsProcessData&&) = delete;

    MeshLib::PropertyVector<int> const* const material_ids;

    std::unique_ptr<ProcessLib::RichardsFlow::RichardsFlowMaterialProperties>
        flow_material;

    /// The constitutive relation for the mechanical part.
    std::map<
        int,
        std::unique_ptr<MaterialLib::Solids::MechanicsBase<DisplacementDim>>>
        solid_materials;
    /// Fluid's bulk modulus. A scalar quantity,
    /// ParameterLib::Parameter<double>.
    ParameterLib::Parameter<double> const& fluid_bulk_modulus;
    /// Biot coefficient. A scalar quantity, ParameterLib::Parameter<double>.
    ParameterLib::Parameter<double> const& biot_coefficient;
    /// Density of the solid. A scalar quantity,
    /// ParameterLib::Parameter<double>.
    ParameterLib::Parameter<double> const& solid_density;
    /// Solid's bulk modulus. A scalar quantity,
    /// ParameterLib::Parameter<double>.
    ParameterLib::Parameter<double> const& solid_bulk_modulus;
    /// Reference temperature for material properties. A scalar quantity,
    /// ParameterLib::Parameter<double>.
    ParameterLib::Parameter<double> const& temperature;
    /// Specific body forces applied to solid and fluid.
    /// It is usually used to apply gravitational forces.
    /// A vector of displacement dimension's length.
    ParameterLib::Parameter<double> const* const sigma_0neq;
    ParameterLib::Parameter<double> const* const pressure_0neq;

    Eigen::Matrix<double, DisplacementDim, 1> const specific_body_force;
    double dt = 0.0;
    double t = 0.0;

    MeshLib::PropertyVector<double>* element_saturation = nullptr;
    MeshLib::PropertyVector<double>* pressure_interpolated = nullptr;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

}  // namespace RichardsMechanics
}  // namespace ProcessLib
