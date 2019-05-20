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
namespace SmallDeformation
{
template <int DisplacementDim>
struct SmallDeformationProcessData
{
    SmallDeformationProcessData(
        MeshLib::PropertyVector<int> const* const material_ids_,
        std::map<int,
                 std::unique_ptr<
                     MaterialLib::Solids::MechanicsBase<DisplacementDim>>>&&
            solid_materials_,
        ParameterLib::Parameter<double> const& solid_density_,
        Eigen::Matrix<double, DisplacementDim, 1>
            specific_body_force_,
        double const reference_temperature_,
        ParameterLib::Parameter<double> const* const reference_sigma_)
        : material_ids(material_ids_),
          solid_materials{std::move(solid_materials_)},
          solid_density(solid_density_),
          reference_sigma(reference_sigma_),
          specific_body_force(std::move(specific_body_force_)),
          reference_temperature(reference_temperature_)
    {
    }

    SmallDeformationProcessData(SmallDeformationProcessData&& other) = default;

    //! Copies are forbidden.
    SmallDeformationProcessData(SmallDeformationProcessData const&) = delete;

    //! Assignments are not needed.
    void operator=(SmallDeformationProcessData const&) = delete;

    //! Assignments are not needed.
    void operator=(SmallDeformationProcessData&&) = delete;

    MeshLib::PropertyVector<int> const* const material_ids;

    std::map<
        int,
        std::unique_ptr<MaterialLib::Solids::MechanicsBase<DisplacementDim>>>
        solid_materials;
    /// Solid's density. A scalar quantity, ParameterLib::Parameter<double>.
    ParameterLib::Parameter<double> const& solid_density;

    ParameterLib::Parameter<double> const* const reference_sigma;
    /// Specific body forces applied to the solid.
    /// It is usually used to apply gravitational forces.
    /// A vector of displacement dimension's length.
    Eigen::Matrix<double, DisplacementDim, 1> const specific_body_force;
    double dt = 0;
    double t = 0;
    double const reference_temperature;
};

}  // namespace SmallDeformation
}  // namespace ProcessLib
