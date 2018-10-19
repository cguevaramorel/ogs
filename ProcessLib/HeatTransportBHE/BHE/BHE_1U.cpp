/**
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "BHE_1U.h"
#include "Physics.h"

using namespace ProcessLib::HeatTransportBHE::BHE;

constexpr std::pair<int, int> BHE_1U::inflow_outflow_bc_component_ids[];

void BHE_1U::updateHeatTransferCoefficients(double const flow_rate)

{
    double tmp_u = pipeFlowVelocity(flow_rate, pipe_param.r_inner);
    _u(0) = tmp_u;
    _u(1) = tmp_u;

    double const Re = reynoldsNumber(std::abs(_u(0)),
                                     2.0 * pipe_param.r_inner,
                                     refrigerant_param.mu_r,
                                     refrigerant_param.rho_r);
    double const Pr = prandtlNumber(refrigerant_param.mu_r,
                                    refrigerant_param.heat_cap_r,
                                    refrigerant_param.lambda_r);

    // calculate Nusselt number
    double tmp_Nu = nusseltNumber(Re, Pr, 2.0 * pipe_param.r_inner,
                                  borehole_geometry.length);
    _Nu(0) = tmp_Nu;
    _Nu(1) = tmp_Nu;

    calcThermalResistances();
    calcHeatTransferCoefficients();
}

/**
 * calculate thermal resistance
 */
void BHE_1U::calcThermalResistances()
{
    // thermal resistance due to the grout transition
    double chi;
    double d0;  // the average outer diameter of the pipes
    // double s; // diagonal distances of pipes
    double R_adv, R_con;
    double const& D = borehole_geometry.diameter;
    // double const& L = borehole_geometry.L;
    double const& lambda_r = refrigerant_param.lambda_r;
    double const& lambda_g = grout_param.lambda_g;
    double const& lambda_p = pipe_param.lambda_p;

    constexpr double PI = boost::math::constants::pi<double>();
    // thermal resistance due to thermal conductivity of the pip wall material
    // Eq. 36 in Diersch_2011_CG
    _R_adv_i1 = 1.0 / (_Nu(0) * lambda_r * PI);
    _R_adv_o1 = 1.0 / (_Nu(1) * lambda_r * PI);

    d0 = 2.0 * pipe_param.r_outer;
    // s = _omega * std::sqrt(2);
    // Eq. 49
    _R_con_a_i1 = _R_con_a_o1 =
        std::log(pipe_param.r_outer / pipe_param.r_inner) /
        (2.0 * PI * lambda_p);
    // Eq. 51
    chi = std::log(std::sqrt(D * D + 2 * d0 * d0) / 2 / d0) /
          std::log(D / std::sqrt(2) / d0);
    if (extern_Ra_Rb.use_extern_Ra_Rb)
    {
        R_adv = 0.5 * (_R_adv_i1 + _R_adv_o1);
        R_con = 0.5 * (_R_con_a_i1 + _R_con_a_o1);
        _R_g = 2 * extern_Ra_Rb.ext_Rb - R_adv - R_con;
    }
    else
    {
        // Eq. 52
        _R_g = acosh((D * D + d0 * d0 - _omega * _omega) / (2 * D * d0)) /
               (2 * PI * lambda_g) * (1.601 - 0.888 * _omega / D);
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
        _R_fig = _R_adv_i1 + _R_con_a_i1 + _R_con_b;
        _R_fog = _R_adv_o1 + _R_con_a_o1 + _R_con_b;
    }

    // thermal resistance due to grout-soil exchange
    if (extern_def_thermal_resistances.if_use_defined_therm_resis)
        _R_gs = extern_def_thermal_resistances.ext_Rgs;
    else
        _R_gs = (1 - chi) * _R_g;

    // thermal resistance due to inter-grout exchange
    double R_ar;
    if (extern_Ra_Rb.use_extern_Ra_Rb)
    {
        R_ar = extern_Ra_Rb.ext_Ra - 2 * (R_adv + R_con);
    }
    else
    {
        R_ar = acosh((2.0 * _omega * _omega - d0 * d0) / d0 / d0) /
               (2.0 * PI * lambda_g);
    }

    if (extern_def_thermal_resistances.if_use_defined_therm_resis)
        _R_gg = extern_def_thermal_resistances.ext_Rgg1;
    else
        _R_gg = 2.0 * _R_gs * (R_ar - 2.0 * chi * _R_g) /
                (2.0 * _R_gs - R_ar + 2.0 * chi * _R_g);

    if (!std::isfinite(_R_gg))
    {
        OGS_FATAL(
            "Error!!! Grout Thermal Resistance is an infinite number! The "
            "simulation will be stopped!");
    }

    // check if constraints regarding negative thermal resistances are violated
    // apply correction procedure
    // Section (1.5.5) in FEFLOW White Papers Vol V.
    double constraint = 1.0 / ((1.0 / _R_gg) + (1.0 / (2.0 * _R_gs)));
    int count = 0;
    while (constraint < 0.0)
    {
        if (extern_def_thermal_resistances.if_use_defined_therm_resis ||
            extern_Ra_Rb.use_extern_Ra_Rb)
        {
            OGS_FATAL(
                "Error!!! Constraints on thermal resistances are violated! "
                "Correction procedure can't be applied due to user defined "
                "thermal resistances! The simulation will be stopped!");
        }
        if (count == 0)
        {
            chi *= 0.66;
            _R_gs = (1 - chi) * _R_g;
            _R_gg = 2.0 * _R_gs * (R_ar - 2.0 * chi * _R_g) /
                    (2.0 * _R_gs - R_ar + 2.0 * chi * _R_g);
        }
        if (count == 1)
        {
            chi *= 0.5;
            _R_gs = (1 - chi) * _R_g;
            _R_gg = 2.0 * _R_gs * (R_ar - 2.0 * chi * _R_g) /
                    (2.0 * _R_gs - R_ar + 2.0 * chi * _R_g);
        }
        if (count == 2)
        {
            chi = 0.0;
            _R_gs = (1 - chi) * _R_g;
            _R_gg = 2.0 * _R_gs * (R_ar - 2.0 * chi * _R_g) /
                    (2.0 * _R_gs - R_ar + 2.0 * chi * _R_g);
            break;
        }
        DBUG(
            "Warning! Correction procedure was applied due to negative thermal "
            "resistance! Correction step #%d.\n",
            count);
        constraint = 1.0 / ((1.0 / _R_gg) + (1.0 / (2.0 * _R_gs)));
        count++;
    }

    // kleep the following lines------------------------------------------------
    // when debugging the code, printing the R and phi values are needed--------
    // std::cout << "Rfig =" << _R_fig << " Rfog =" << _R_fog << " Rgg =" <<
    // _R_gg << " Rgs =" << _R_gs << "\n"; double phi_fig = 1.0 / (_R_fig *
    // S_i); double phi_fog = 1.0 / (_R_fog * S_o); double phi_gg = 1.0 / (_R_gg
    // * S_g1); double phi_gs = 1.0 / (_R_gs * S_gs); std::cout << "phi_fig ="
    // << phi_fig << " phi_fog =" << phi_fog << " phi_gg =" << phi_gg << "
    // phi_gs =" << phi_gs << "\n";
    // -------------------------------------------------------------------------
}

/**
 * calculate heat transfer coefficient
 */
void BHE_1U::calcHeatTransferCoefficients()
{
    boundary_heat_exchange_coefficients[0] = 1.0 / _R_fig;
    boundary_heat_exchange_coefficients[1] = 1.0 / _R_fog;
    boundary_heat_exchange_coefficients[2] = 1.0 / _R_gg;
    boundary_heat_exchange_coefficients[3] = 1.0 / _R_gs;
}

double BHE_1U::getTinByTout(double const T_out, double const current_time)
{
    double const& rho_r = refrigerant_param.rho_r;
    double const& heat_cap_r = refrigerant_param.heat_cap_r;

    auto update_Q_r_and_initialize = [&](double const t) {
        if (!use_flowrate_curve)
            return;
        double const Q_r = flowrate_curve->getValue(t);
        updateHeatTransferCoefficients(Q_r);
    };

    if (boundary_type == BHE_BOUNDARY_TYPE::FIXED_INFLOW_TEMP_CURVE_BOUNDARY)
    {
        return inflow_temperature_curve->getValue(current_time);
    }
    if (boundary_type == BHE_BOUNDARY_TYPE::POWER_IN_WATT_BOUNDARY)
    {
        update_Q_r_and_initialize(current_time);
        return power_in_watt_val / Q_r / heat_cap_r / rho_r + T_out;
    }
    if (boundary_type == BHE_BOUNDARY_TYPE::FIXED_TEMP_DIFF_BOUNDARY)
    {
        update_Q_r_and_initialize(current_time);
        return T_out + delta_T_val;
    }
    if (boundary_type ==
        BHE_BOUNDARY_TYPE::POWER_IN_WATT_CURVE_FIXED_DT_BOUNDARY)
    {
        // get the power value in the curve
        // power_tmp = GetCurveValue(power_in_watt_curve_idx, 0,
        // current_time, &flag_valid);
        double const power_tmp = power_in_watt_curve->getValue(current_time);

        // if power value exceeds threshold, calculate new values
        if (std::fabs(power_tmp) > threshold)
        {
            double const fac_dT = power_tmp < 0 ? -1 : 1;
            // calculate the corresponding flow rate needed using the defined
            // delta_T value
            double const Q_r =
                power_tmp / (fac_dT * delta_T_val) / heat_cap_r / rho_r;
            // update all values dependent on the flow rate
            updateHeatTransferCoefficients(Q_r);
            // calculate the new T_in
            return T_out + (fac_dT * delta_T_val);
            // print out updated flow rate
            // std::cout << "Qr: " << Q_r_tmp << std::endl;
        }
        else
        {
            double const Q_r = 1.0e-12;  // this has to be a small value to
                                         // avoid division by zero
            // update all values dependent on the flow rate
            updateHeatTransferCoefficients(Q_r);
            // calculate the new T_in
            return T_out;
            // print out updated flow rate
            // std::cout << "Qr: " << Q_r_tmp << std::endl;
        }
    }
    if (boundary_type ==
        BHE_BOUNDARY_TYPE::BUILDING_POWER_IN_WATT_CURVE_FIXED_DT_BOUNDARY)
    {
        // get the building power value in the curve
        // building_power_tmp = GetCurveValue(power_in_watt_curve_idx, 0,
        // current_time, &flag_valid);
        double const building_power_tmp =
            power_in_watt_curve->getValue(current_time);
        double power_tmp;
        double COP_tmp;
        if (building_power_tmp <= 0.0)
        {
            // get COP value based on T_out in the curve
            COP_tmp = heating_cop_curve->getValue(T_out);

            // now calculate how much power needed from BHE
            power_tmp = building_power_tmp * (COP_tmp - 1.0) / COP_tmp;
            // also how much power from electricity
            // power_elect_tmp = building_power_tmp - power_tmp;
            // print the amount of power needed
            // std::cout << "COP: " << COP_tmp << ", Q_bhe: " << power_tmp
            // << ", Q_elect: " << power_elect_tmp << std::endl;
        }
        else
        {
            // get COP value based on T_out in the curve
            COP_tmp = cooling_cop_curve->getValue(T_out);

            // now calculate how much power needed from BHE
            power_tmp = building_power_tmp * (COP_tmp + 1.0) / COP_tmp;
            // also how much power from electricity
            // power_elect_tmp = -building_power_tmp + power_tmp;
            // print the amount of power needed
            // std::cout << "COP: " << COP_tmp << ", Q_bhe: " << power_tmp
            // << ", Q_elect: " << power_elect_tmp << std::endl;
        }
        // if power value exceeds threshold, calculate new values
        if (std::fabs(power_tmp) > threshold)
        {
            double const fac_dT = building_power_tmp <= 0 ? -1 : 1;

            // calculate the corresponding flow rate needed using the defined
            // delta_T value
            double const Q_r =
                power_tmp / (fac_dT * delta_T_val) / heat_cap_r / rho_r;
            // update all values dependent on the flow rate
            updateHeatTransferCoefficients(Q_r);
            // calculate the new T_in
            return T_out + (fac_dT * delta_T_val);
            // print out updated flow rate
            // std::cout << "Qr: " << Q_r_tmp << std::endl;
        }
        else
        {
            double const Q_r = 1.0e-12;  // this has to be a small value to
                                         // avoid division by zero
            // update all values dependent on the flow rate
            updateHeatTransferCoefficients(Q_r);
            // calculate the new T_in
            return T_out;
            // print out updated flow rate
            // std::cout << "Qr: " << Q_r_tmp << std::endl;
        }
    }
    if (boundary_type ==
        BHE_BOUNDARY_TYPE::
            BUILDING_POWER_IN_WATT_CURVE_FIXED_FLOW_RATE_BOUNDARY)
    {
        // get the building power value in the curve
        // building_power_tmp = GetCurveValue(power_in_watt_curve_idx, 0,
        // current_time, &flag_valid);
        double const building_power_tmp =
            power_in_watt_curve->getValue(current_time);

        double power_tmp;
        double COP_tmp;
        if (building_power_tmp <= 0)
        {
            // get COP value based on T_out in the curve
            COP_tmp = heating_cop_curve->getValue(T_out);
            // now calculate how much power needed from BHE
            power_tmp = building_power_tmp * (COP_tmp - 1.0) / COP_tmp;
            // also how much power from electricity
            // power_elect_tmp = building_power_tmp - power_tmp;
            // print the amount of power needed
            // std::cout << "COP: " << COP_tmp << ", Q_bhe: " << power_tmp
            // << ", Q_elect: " << power_elect_tmp << std::endl;
        }
        else
        {
            // get COP value based on T_out in the curve
            COP_tmp = cooling_cop_curve->getValue(T_out);
            // now calculate how much power needed from BHE
            power_tmp = building_power_tmp * (COP_tmp + 1.0) / COP_tmp;
            // also how much power from electricity
            // power_elect_tmp = -building_power_tmp + power_tmp;
            // print the amount of power needed
            // std::cout << "COP: " << COP_tmp << ", Q_bhe: " << power_tmp
            // << ", Q_elect: " << power_elect_tmp << std::endl;
        }
        // Assign Qr whether from curve or fixed value
        update_Q_r_and_initialize(current_time);
        if (std::fabs(power_tmp) < threshold)
        {
            double const Q_r = 1.0e-12;  // this has to be a small value to
                                         // avoid division by zero update all
                                         // values dependent on the flow rate
            updateHeatTransferCoefficients(Q_r);
            // calculate the new T_in
            return T_out;
            // print out updated flow rate
            // std::cout << "Qr: " << Q_r_tmp << std::endl;
        }
        else
        {
            // calculate the dT value based on fixed flow rate
            delta_T_val = power_tmp / Q_r / heat_cap_r / rho_r;
            // calcuate the new T_in
            return T_out + delta_T_val;
        }
    }
    if (boundary_type ==
        BHE_BOUNDARY_TYPE::POWER_IN_WATT_CURVE_FIXED_FLOW_RATE_BOUNDARY)
    {
        // get the power value in the curve
        // power_tmp = GetCurveValue(power_in_watt_curve_idx, 0,
        // current_time, &flag_valid);
        double const power_tmp = power_in_watt_curve->getValue(current_time);

        // Assign Qr whether from curve or fixed value
        update_Q_r_and_initialize(current_time);
        // calculate the dT value based on fixed flow rate
        if (std::fabs(power_tmp) < threshold)
        {
            double const Q_r = 1.0e-12;  // this has to be a small value to
                                         // avoid division by zero update all
                                         // values dependent on the flow rate
            updateHeatTransferCoefficients(Q_r);
            // calculate the new T_in
            return T_out;
            // print out updated flow rate
            // std::cout << "Qr: " << Q_r_tmp << std::endl;
        }
        else
        {
            // calculate the dT value based on fixed flow rate
            delta_T_val = power_tmp / Q_r / heat_cap_r / rho_r;
            // calcuate the new T_in
            return T_out + delta_T_val;
        }
    }
    return T_out;
}
