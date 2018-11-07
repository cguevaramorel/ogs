/**
 * \file
 *
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include "Physics.h"
#include "PipeParameters.h"
#include "RefrigerantParameters.h"

namespace ProcessLib
{
namespace HeatTransportBHE
{
namespace BHE
{
struct ThermoMechanicalFlowProperties
{
    double velocity;
    double nusselt_number;
};

inline ThermoMechanicalFlowProperties
calculateThermoMechanicalFlowPropertiesPipe(PipeParameters const& pipe,
                                            double const length,
                                            RefrigerantParameters const& fluid,
                                            double const flow_rate)
{
    double const Pr =
        prandtlNumber(fluid.mu_r, fluid.heat_cap_r, fluid.lambda_r);

    double const velocity = pipeFlowVelocity(flow_rate, pipe.r_inner);
    double const Re =
        reynoldsNumber(velocity, 2.0 * pipe.r_inner, fluid.mu_r, fluid.rho_r);
    double const nusselt_number =
        nusseltNumber(Re, Pr, 2 * pipe.r_inner, length);
    return {velocity, nusselt_number};
}

inline ThermoMechanicalFlowProperties
calculateThermoMechanicalFlowPropertiesAnnulus(
    PipeParameters const& pipe, double const length,
    RefrigerantParameters const& fluid, double const flow_rate)
{
    double const Pr =
        prandtlNumber(fluid.mu_r, fluid.heat_cap_r, fluid.lambda_r);

    double const velocity =
        annulusFlowVelocity(flow_rate, pipe.r_outer, pipe.r_inner + pipe.b_in);

    double const Re = reynoldsNumber(
        velocity, 2.0 * (pipe.r_outer - (pipe.r_inner + pipe.b_in)), fluid.mu_r,
        fluid.rho_r);

    double const diameter_ratio = (pipe.r_inner + pipe.b_in) / pipe.r_outer;
    double const pipe_aspect_ratio =
        (2 * pipe.r_outer - 2 * (pipe.r_inner + pipe.b_in)) / length;
    double const nusselt_number =
        nusseltNumberAnnulus(Re, Pr, diameter_ratio, pipe_aspect_ratio);
    return {velocity, nusselt_number};
}
}  // namespace BHE
}  // namespace HeatTransportBHE
}  // namespace ProcessLib

