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
#include "ProcessLib/NonequilibriumInitialState.h"

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
    MeshLib::PropertyVector<int> const* const material_ids = nullptr;

    std::map<
        int,
        std::unique_ptr<MaterialLib::Solids::MechanicsBase<DisplacementDim>>>
        solid_materials;
    /// Solid's density. A scalar quantity, ParameterLib::Parameter<double>.
    ParameterLib::Parameter<double> const& solid_density;

    /// Specific body forces applied to the solid.
    /// It is usually used to apply gravitational forces.
    /// A vector of displacement dimension's length.
    Eigen::Matrix<double, DisplacementDim, 1> const specific_body_force;

    double const reference_temperature =
        std::numeric_limits<double>::quiet_NaN();

    std::unique_ptr<NonequilibriumInitialState const>
        nonequilibrium_initial_state = nullptr;

    double dt = std::numeric_limits<double>::quiet_NaN();
    double t = std::numeric_limits<double>::quiet_NaN();
};

}  // namespace SmallDeformation
}  // namespace ProcessLib
