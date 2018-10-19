/**
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "BHE_2U.h"
#include "Physics.h"

using namespace ProcessLib::HeatTransportBHE::BHE;

constexpr std::pair<int, int> BHE_2U::inflow_outflow_bc_component_ids[];

void ProcessLib::HeatTransportBHE::BHE::BHE_2U::initialize()
{
    double tmp_u = pipeFlowVelocity(Q_r, pipe_param.r_inner);
    // depending on whether it is parallel or serially connected
    if (_discharge_type == BHE_DISCHARGE_TYPE::BHE_DISCHARGE_TYPE_PARALLEL)
    {
        tmp_u *= 0.5;
    }
    // serially connected 2U BHE type does not to do anything.

    _u(0) = tmp_u;
    _u(1) = tmp_u;
    _u(2) = tmp_u;
    _u(3) = tmp_u;

    double const Re = reynoldsNumber(std::abs(_u(0)),
                                     2.0 * pipe_param.r_inner,
                                     refrigerant_param.mu_r,
                                     refrigerant_param.rho_r);
    // calculate Prandtl number
    double const Pr = prandtlNumber(refrigerant_param.mu_r,
                                    refrigerant_param.heat_cap_r,
                                    refrigerant_param.lambda_r);

    // calculate Nusselt number
    double tmp_Nu = nusseltNumber(Re, Pr, 2.0 * pipe_param.r_inner,
                                  borehole_geometry.length);
    _Nu(0) = tmp_Nu;
    _Nu(1) = tmp_Nu;
    _Nu(2) = tmp_Nu;
    _Nu(3) = tmp_Nu;

    calcThermalResistances();
    calcHeatTransferCoefficients();
}

/**
 * calculate thermal resistance
 */
void BHE_2U::calcThermalResistances()
{
    // thermal resistance due to the grout transition
    double chi;
    double d0;  // the average outer diameter of the pipes
    double s;   // diagonal distances of pipes
    double R_adv, R_con;
    double const& D = borehole_geometry.diameter;
    double const& lambda_r = refrigerant_param.lambda_r;
    double const& lambda_g = grout_param.lambda_g;
    double const& r_outer = pipe_param.r_outer;
    double const& r_inner = pipe_param.r_inner;
    double const& lambda_p = pipe_param.lambda_p;

    constexpr double PI = boost::math::constants::pi<double>();
    // thermal resistance due to advective flow of refrigerant in the pipes
    // Eq. 31 in Diersch_2011_CG
    _R_adv_i1 = 1.0 / (_Nu(0) * lambda_r * PI);
    _R_adv_o1 = 1.0 / (_Nu(1) * lambda_r * PI);
    _R_adv_i2 = 1.0 / (_Nu(2) * lambda_r * PI);
    _R_adv_o2 = 1.0 / (_Nu(3) * lambda_r * PI);

    // thermal resistance due to thermal conductivity of the pip wall material
    // Eq. 36 in Diersch_2011_CG
    // double _R_con_a;
    // _R_con_a = std::log(r_outer / r_inner) / ( 2.0 * PI * lambda_p );

    d0 = 2.0 * r_inner;
    s = _omega * std::sqrt(2);
    chi = std::log(std::sqrt(D * D + 4 * d0 * d0) / 2 / std::sqrt(2) / d0) /
          std::log(D / 2 / d0);
    // Eq. 36
    _R_con_a_i1 = _R_con_a_i2 = _R_con_a_o1 = _R_con_a_o2 =
        std::log(r_outer / r_inner) / (2.0 * PI * lambda_p);

    if (extern_Ra_Rb.use_extern_Ra_Rb)
    {
        R_adv = 0.25 * (_R_adv_i1 + _R_adv_i2 + _R_adv_o1 + _R_adv_o2);
        R_con = 0.25 * (_R_con_a_i1 + _R_con_a_i2 + _R_con_a_o1 + _R_con_a_o2);
        _R_g = 4 * extern_Ra_Rb.ext_Rb - R_adv - R_con;
    }
    else
    {
        _R_g = acosh((D * D + d0 * d0 - s * s) / (2 * D * d0)) /
               (2 * PI * lambda_g * lambda_g) *
               (3.098 - 4.432 * s / D + 2.364 * s * s / D / D);
    }

    _R_con_b = chi * _R_g;

    // Eq. 29 and 30
    if (extern_def_thermal_resistances.if_use_defined_therm_resis)
    {
        _R_fig = extern_def_thermal_resistances.ext_Rfig;
        _R_fog = extern_def_thermal_resistances.ext_Rfog;
    }
    else
    {
        _R_fig = _R_adv_i1 + _R_adv_i2 + _R_con_a_i1 + _R_con_a_i2 + _R_con_b;
        _R_fog = _R_adv_o1 + _R_adv_o2 + _R_con_a_o1 + _R_con_a_o2 + _R_con_b;
    }

    // thermal resistance due to grout-soil exchange
    if (extern_def_thermal_resistances.if_use_defined_therm_resis)
        _R_gs = extern_def_thermal_resistances.ext_Rgs;
    else
        _R_gs = (1 - chi) * _R_g;

    // thermal resistance due to inter-grout exchange
    double R_ar_1, R_ar_2;
    if (extern_Ra_Rb.use_extern_Ra_Rb)
    {
        R_ar_1 = (2.0 + std::sqrt(2.0)) * _R_g *
                 (extern_Ra_Rb.ext_Ra - R_adv - R_con) /
                 (_R_g + extern_Ra_Rb.ext_Ra - R_adv - R_con);
        R_ar_2 = std::sqrt(2.0) * R_ar_1;
    }
    else
    {
        R_ar_1 = acosh((s * s - d0 * d0) / d0 / d0) / (2.0 * PI * lambda_g);
        R_ar_2 =
            acosh((2.0 * s * s - d0 * d0) / d0 / d0) / (2.0 * PI * lambda_g);
    }
    if (extern_def_thermal_resistances.if_use_defined_therm_resis)
    {
        _R_gg_1 = extern_def_thermal_resistances.ext_Rgg1;
        _R_gg_2 = extern_def_thermal_resistances.ext_Rgg2;
    }
    else
    {
        _R_gg_1 = 2.0 * _R_gs * (R_ar_1 - 2.0 * chi * _R_g) /
                  (2.0 * _R_gs - R_ar_1 + 2.0 * chi * _R_g);
        _R_gg_2 = 2.0 * _R_gs * (R_ar_2 - 2.0 * chi * _R_g) /
                  (2.0 * _R_gs - R_ar_2 + 2.0 * chi * _R_g);
    }

    if (!std::isfinite(_R_gg_1) || !std::isfinite(_R_gg_2))
    {
        OGS_FATAL(
            "Error!!! Grout Thermal Resistance is an infinite number! The "
            "simulation will be stopped! ");
    }

    // check if constraints regarding negative thermal resistances are violated
    // apply correction procedure
    // Section (1.5.5) in FEFLOW White Papers Vol V.
    double constraint1 = 1.0 / ((1.0 / _R_gg_1) + (1.0 / (2.0 * _R_gs)));
    double constraint2 = 1.0 / ((1.0 / _R_gg_2) + (1.0 / (2.0 * _R_gs)));
    int count = 0;
    while (constraint1 < 0.0 || constraint2 < 0.0)
    {
        if (extern_def_thermal_resistances.if_use_defined_therm_resis ||
            extern_Ra_Rb.use_extern_Ra_Rb)
        {
            OGS_FATAL(
                "Error!!! Constraints on thermal resistances are violated! "
                "Correction procedure can't be applied due to user defined "
                "thermal resistances! The simulation will be stopped! ");
        }
        if (count == 0)
        {
            chi *= 0.66;
            _R_gs = (1 - chi) * _R_g;
            _R_gg_1 = 2.0 * _R_gs * (R_ar_1 - 2.0 * chi * _R_g) /
                      (2.0 * _R_gs - R_ar_1 + 2.0 * chi * _R_g);
            _R_gg_2 = 2.0 * _R_gs * (R_ar_2 - 2.0 * chi * _R_g) /
                      (2.0 * _R_gs - R_ar_2 + 2.0 * chi * _R_g);
        }
        if (count == 1)
        {
            chi *= 0.5;
            _R_gs = (1 - chi) * _R_g;
            _R_gg_1 = 2.0 * _R_gs * (R_ar_1 - 2.0 * chi * _R_g) /
                      (2.0 * _R_gs - R_ar_1 + 2.0 * chi * _R_g);
            _R_gg_2 = 2.0 * _R_gs * (R_ar_2 - 2.0 * chi * _R_g) /
                      (2.0 * _R_gs - R_ar_2 + 2.0 * chi * _R_g);
        }
        if (count == 2)
        {
            chi = 0.0;
            _R_gs = (1 - chi) * _R_g;
            _R_gg_1 = 2.0 * _R_gs * (R_ar_1 - 2.0 * chi * _R_g) /
                      (2.0 * _R_gs - R_ar_1 + 2.0 * chi * _R_g);
            _R_gg_2 = 2.0 * _R_gs * (R_ar_2 - 2.0 * chi * _R_g) /
                      (2.0 * _R_gs - R_ar_2 + 2.0 * chi * _R_g);
            break;
        }
        DBUG(
            "Warning! Correction procedure was applied due to negative thermal "
            "resistance! Correction step #%d.\n",
            count);
        constraint1 = 1.0 / ((1.0 / _R_gg_1) + (1.0 / (2.0 * _R_gs)));
        constraint2 = 1.0 / ((1.0 / _R_gg_2) + (1.0 / (2.0 * _R_gs)));
        count++;
    }
}

/**
 * calculate heat transfer coefficient
 */
void BHE_2U::calcHeatTransferCoefficients()
{
    boundary_heat_exchange_coefficients[0] = 1.0 / _R_fig;
    boundary_heat_exchange_coefficients[1] = 1.0 / _R_fog;
    boundary_heat_exchange_coefficients[2] = 1.0 / _R_gg_1;
    boundary_heat_exchange_coefficients[3] = 1.0 / _R_gg_2;
    boundary_heat_exchange_coefficients[4] = 1.0 / _R_gs;
}

double BHE_2U::getTinByTout(double const T_out, double const current_time)
{
    double T_in(0.0);
    double power_tmp(0.0);
    double const& rho_r = refrigerant_param.rho_r;
    double const& heat_cap_r = refrigerant_param.heat_cap_r;

    switch (this->boundary_type)
    {
        case BHE_BOUNDARY_TYPE::POWER_IN_WATT_BOUNDARY:
            T_in = power_in_watt_val / Q_r / heat_cap_r / rho_r + T_out;
            break;
        case BHE_BOUNDARY_TYPE::FIXED_TEMP_DIFF_BOUNDARY:
            T_in = T_out + delta_T_val;
            break;
        case BHE_BOUNDARY_TYPE::POWER_IN_WATT_CURVE_FIXED_DT_BOUNDARY:
            OGS_FATAL(
                "BHE_BOUND_POWER_IN_WATT_CURVE_FIXED_DT feature has not been "
                "implemented for BHE_2U yet. ");
            break;
        case BHE_BOUNDARY_TYPE::POWER_IN_WATT_CURVE_FIXED_FLOW_RATE_BOUNDARY:
            // get the power value in the curve
            // power_tmp = GetCurveValue(power_in_watt_curve_idx, 0,
            // current_time, &flag_valid);
            power_tmp = power_in_watt_curve->getValue(current_time);

            // calculate the dT value based on fixed flow rate
            delta_T_val = power_tmp / Q_r / heat_cap_r / rho_r;
            // calcuate the new T_in
            T_in = T_out + delta_T_val;
            break;
        default:
            T_in = T_out;
            break;
    }

    return T_in;
}
