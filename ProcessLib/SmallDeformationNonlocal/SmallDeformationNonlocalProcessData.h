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
class PressureInjectionInCrack
{
public:
    explicit PressureInjectionInCrack(double const stiffness)
        : _stiffness(stiffness)
    {
    }

    void setInjectionVolume(double const volume)
    {
        _injected_volume = volume;
        INFO("Volume injected: %g", _injected_volume);
    }

    /// Update the pressure based on the injected volume and the crack volume
    /// and returns the pressure increment.
    double updatePressure(double const crack_volume)
    {
        _pressure_old = _pressure;
        double const increment = (_injected_volume - crack_volume) * _stiffness;
        _pressure += increment;

        INFO("Internal pressure: %g and pressure increment: %.4e", _pressure,
             increment);

        return increment;
    }

    double pressure() const { return _pressure; }

private:
    double _injected_volume = std::numeric_limits<double>::quiet_NaN();

    double const _stiffness;  // = 2.15e11;

    double _pressure = std::numeric_limits<double>::quiet_NaN();
    double _pressure_old = std::numeric_limits<double>::quiet_NaN();
};

template <int DisplacementDim>
struct SmallDeformationNonlocalProcessData
{
    SmallDeformationNonlocalProcessData(
        MeshLib::PropertyVector<int> const* const material_ids_,
        std::map<int,
                 std::unique_ptr<
                     MaterialLib::Solids::MechanicsBase<DisplacementDim>>>&&
            solid_materials_,
        Parameter<double> const& solid_density_,
        Eigen::Matrix<double, DisplacementDim, 1>
            specific_body_force_,
        double const reference_temperature_,
        double const internal_length_)
        : material_ids(material_ids_),
          solid_materials{std::move(solid_materials_)},
          solid_density(solid_density_),
          specific_body_force(std::move(specific_body_force_)),
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

    MeshLib::PropertyVector<int> const* const material_ids;

    std::map<
        int,
        std::unique_ptr<MaterialLib::Solids::MechanicsBase<DisplacementDim>>>
        solid_materials;
    /// Solid's density. A scalar quantity, Parameter<double>.
    Parameter<double> const& solid_density;
    /// Specific body forces applied to the solid.
    /// It is usually used to apply gravitational forces.
    /// A vector of displacement dimension's length.
    Eigen::Matrix<double, DisplacementDim, 1> const specific_body_force;

    boost::optional<PressureInjectionInCrack> pressure_injection_in_crack;
    double crack_volume_old = 0.0;
    double crack_volume = 0.0;

    double dt = 0;
    double t = 0;
    double const reference_temperature;
    double const internal_length_squared;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

}  // namespace SmallDeformationNonlocal
}  // namespace ProcessLib
