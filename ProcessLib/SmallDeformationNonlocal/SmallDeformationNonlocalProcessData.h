/**
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include <memory>
#include <utility>

#include <Eigen/Eigen>

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
namespace SmallDeformationNonlocal
{
template <int DisplacementDim>
struct SmallDeformationNonlocalProcessData
{
    SmallDeformationNonlocalProcessData(
        std::unique_ptr<MaterialLib::Solids::MechanicsBase<DisplacementDim>>&&
            material,
        double const reference_temperature_,
        double const internal_length_)
        : material{std::move(material)},
          reference_temperature(reference_temperature_),
          internal_length_squared(internal_length_ * internal_length_)
    {
    }

    SmallDeformationNonlocalProcessData(
        SmallDeformationNonlocalProcessData&& other) = default;

    //! Copies are forbidden.
    SmallDeformationNonlocalProcessData(
        SmallDeformationNonlocalProcessData const&) = delete;

    //! Assignments are not needed.
    void operator=(SmallDeformationNonlocalProcessData const&) = delete;

    //! Assignments are not needed.
    void operator=(SmallDeformationNonlocalProcessData&&) = delete;

    std::unique_ptr<MaterialLib::Solids::MechanicsBase<DisplacementDim>>
        material;
    /// Solid's density. A scalar quantity, Parameter<double>.
    Parameter<double> const& solid_density;
    /// Specific body forces applied to the solid.
    /// It is usually used to apply gravitational forces.
    /// A vector of displacement dimension's length.
    Eigen::Matrix<double, DisplacementDim, 1> const specific_body_force;
    double dt = 0;
    double t = 0;
    double const reference_temperature;
    double const internal_length_squared;

    double injected_volume = 0.0;
    double crack_volume_old = 0.0;
    double crack_volume = 0.0;
    bool propagating_crack = true;

    double stiffness = 2.15e11;

    double pressure = 0.0;
    double pressure_old = 0.0;
    double pressure_error = 0.0;
};

}  // namespace SmallDeformationNonlocal
}  // namespace ProcessLib
