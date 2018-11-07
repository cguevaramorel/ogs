/**
* \copyright
* Copyright (c) 2012-2017, OpenGeoSys Community (http://www.opengeosys.org)
*            Distributed under a Modified BSD License.
*              See accompanying file LICENSE.txt or
*              http://www.opengeosys.org/project/license
*
*/

#include "CreateBHE1U.h"
#include "MaterialLib/Fluid/FluidProperty.h"
#include "MaterialLib/Fluid/Density/createFluidDensityModel.h"
#include "MaterialLib/Fluid/Viscosity/createViscosityModel.h"
#include "MaterialLib/Fluid/SpecificHeatCapacity/CreateSpecificFluidHeatCapacityModel.h"
#include "MaterialLib/Fluid/ThermalConductivity/CreateFluidThermalConductivityModel.h"

namespace BHE
{
    BHE::BHE_1U *
        CreateBHE1U(BaseLib::ConfigTree const& config,
            BaseLib::ConfigTree const& bhe_conf,
            std::map < std::string,
            std::unique_ptr < MathLib::PiecewiseLinearInterpolation >> const& curves)
    {
        // BHE type is clear
        BHE::BHE_TYPE bhe_type = BHE_TYPE::TYPE_1U;

        // read in the parameters
        const std::string bhe_ply_name = bhe_conf.getConfigParameter<std::string>("bhe_polyline");
        const std::string bhe_bound_type_str = bhe_conf.getConfigParameter<std::string>("bhe_bound_type");
        const bool bhe_if_use_flow_rate_curve = false;
        const double bhe_length = bhe_conf.getConfigParameter<double>("bhe_length");
        const double bhe_diameter = bhe_conf.getConfigParameter<double>("bhe_diameter");
        const double bhe_refrigerant_flow_rate = bhe_conf.getConfigParameter<double>("bhe_refrigerant_flow_rate");
        const double bhe_pipe_inner_radius = bhe_conf.getConfigParameter<double>("bhe_pipe_inner_radius");
        const double bhe_pipe_outer_radius = bhe_conf.getConfigParameter<double>("bhe_pipe_outer_radius");
        const double bhe_pipe_in_wall_thickness = bhe_conf.getConfigParameter<double>("bhe_pipe_in_wall_thickness");
        const double bhe_pipe_out_wall_thickness = bhe_conf.getConfigParameter<double>("bhe_pipe_out_wall_thickness");
        const std::size_t bhe_fluid_idx = bhe_conf.getConfigParameter<std::size_t>("bhe_fluid_idx");
        const double bhe_fluid_longitudinal_dispsion_length = bhe_conf.getConfigParameter<double>("bhe_fluid_longitudinal_dispsion_length");
        const double bhe_grout_density = bhe_conf.getConfigParameter<double>("bhe_grout_density");
        const double bhe_grout_porosity = bhe_conf.getConfigParameter<double>("bhe_grout_porosity");
        const double bhe_grout_heat_capacity = bhe_conf.getConfigParameter<double>("bhe_grout_heat_capacity");
        const double bhe_pipe_wall_thermal_conductivity = bhe_conf.getConfigParameter<double>("bhe_pipe_wall_thermal_conductivity");
        const double bhe_grout_thermal_conductivity = bhe_conf.getConfigParameter<double>("bhe_grout_thermal_conductivity");
        const double bhe_pipe_distance = bhe_conf.getConfigParameter<double>("bhe_pipe_distance");

        // optional parameters
        double bhe_power_in_watt_val;
        double bhe_intern_resistance;
        double bhe_therm_resistance;
        double bhe_R_fig;
        double bhe_R_fog;
        double bhe_R_gg1;
        double bhe_R_gg2;
        double bhe_R_gs;
        double bhe_delta_T_val;
        double bhe_switch_off_threshold;

        // give default values to optional parameters
        // if the BHE is using external given thermal resistance values
        bool bhe_use_ext_therm_resis = false;
        if (auto const bhe_use_ext_therm_resis_conf =
            config.getConfigParameterOptional<bool>("bhe_use_external_therm_resis"))
        {
            DBUG("If using external given thermal resistance values : %s",
                (*bhe_use_ext_therm_resis_conf) ? "true" : "false");
            bhe_use_ext_therm_resis = *bhe_use_ext_therm_resis_conf;

            // only when using it, read the two resistance values
            if (bhe_use_ext_therm_resis)
            {
                bhe_intern_resistance = *bhe_conf.getConfigParameterOptional<double>("bhe_internal_resistance");

                bhe_therm_resistance = bhe_conf.getConfigParameterOptional<double>("bhe_therm_resistance").get();
            }
        }

        // if the BHE is using user defined thermal resistance values
        bool bhe_user_defined_therm_resis = false;
        if (auto const bhe_user_defined_therm_resis_conf =
            config.getConfigParameterOptional<bool>("bhe_user_defined_therm_resis"))
        {
            DBUG("If appplying user defined thermal resistance values : %s",
                (*bhe_user_defined_therm_resis_conf) ? "true" : "false");
            bhe_user_defined_therm_resis = *bhe_user_defined_therm_resis_conf;

            // only when using it, read the two resistance values
            if (bhe_user_defined_therm_resis)
            {
                bhe_R_fig = bhe_conf.getConfigParameterOptional<double>("bhe_R_fig").get();
                bhe_R_fog = bhe_conf.getConfigParameterOptional<double>("bhe_R_fog").get();
                bhe_R_gg1 = bhe_conf.getConfigParameterOptional<double>("bhe_R_gg1").get();
                bhe_R_gg2 = bhe_conf.getConfigParameterOptional<double>("bhe_R_gg2").get();
                bhe_R_gs = bhe_conf.getConfigParameterOptional<double>("bhe_R_gs").get();
            }
        }

        // convert BHE boundary type
        BHE::BHE_BOUNDARY_TYPE bhe_bound_type;
        if (bhe_bound_type_str.compare("FIXED_INFLOW_TEMP") == 0)
            bhe_bound_type = BHE_BOUNDARY_TYPE::FIXED_INFLOW_TEMP_BOUNDARY;
        else if (bhe_bound_type_str.compare("FIXED_INFLOW_TEMP_CURVE") == 0)
            bhe_bound_type = BHE_BOUNDARY_TYPE::FIXED_INFLOW_TEMP_CURVE_BOUNDARY;
        else if (bhe_bound_type_str.compare("POWER_IN_WATT") == 0)
        {
            bhe_bound_type = BHE_BOUNDARY_TYPE::POWER_IN_WATT_BOUNDARY;
            bhe_power_in_watt_val = bhe_conf.getConfigParameterOptional<double>("bhe_power_in_watt_value").get();
            bhe_switch_off_threshold = bhe_conf.getConfigParameterOptional<double>("bhe_switch_off_threshold").get();
        }
        else if (bhe_bound_type_str.compare("POWER_IN_WATT_CURVE_FIXED_DT") == 0)
        {
            bhe_bound_type = BHE_BOUNDARY_TYPE::POWER_IN_WATT_CURVE_FIXED_DT_BOUNDARY;
            bhe_switch_off_threshold = bhe_conf.getConfigParameterOptional<double>("bhe_switch_off_threshold").get();
        }
        else if (bhe_bound_type_str.compare("BHE_BOUND_BUILDING_POWER_IN_WATT_CURVE_FIXED_DT") == 0)
        {
            bhe_bound_type = BHE_BOUNDARY_TYPE::BUILDING_POWER_IN_WATT_CURVE_FIXED_DT_BOUNDARY;
            bhe_switch_off_threshold = bhe_conf.getConfigParameterOptional<double>("bhe_switch_off_threshold").get();
        }
        else if (bhe_bound_type_str.compare("BHE_BOUND_BUILDING_POWER_IN_WATT_CURVE_FIXED_FLOW_RATE") == 0)
        {
            bhe_bound_type = BHE_BOUNDARY_TYPE::BUILDING_POWER_IN_WATT_CURVE_FIXED_FLOW_RATE_BOUNDARY;
            bhe_switch_off_threshold = bhe_conf.getConfigParameterOptional<double>("bhe_switch_off_threshold").get();
        }
        else if (bhe_bound_type_str.compare("POWER_IN_WATT_CURVE_FIXED_FLOW_RATE") == 0)
        {
            bhe_bound_type = BHE_BOUNDARY_TYPE::POWER_IN_WATT_CURVE_FIXED_FLOW_RATE_BOUNDARY;
            bhe_switch_off_threshold = bhe_conf.getConfigParameterOptional<double>("bhe_switch_off_threshold").get();
        }
        else if (bhe_bound_type_str.compare("FIXED_TEMP_DIFF") == 0)
        {
            bhe_bound_type = BHE_BOUNDARY_TYPE::FIXED_TEMP_DIFF_BOUNDARY;
            bhe_delta_T_val = bhe_conf.getConfigParameterOptional<double>("bhe_inout_delta_T_value").get();
        }

        // get the refrigerant properties from fluid property class
        //! \ogs_file_param{prj__processes__process__HEAT_TRANSPORT_BHE__material_property__fluid}
        auto const& fluid_config = config.getConfigSubtree("fluid");
        //! \ogs_file_param{prj__processes__process__HEAT_TRANSPORT_BHE__material_property__refrigerant_density}
        auto const& rho_conf = fluid_config.getConfigSubtree("refrigerant_density");
        auto bhe_refrigerant_density =
            MaterialLib::Fluid::createFluidDensityModel(rho_conf);
        //! \ogs_file_param{prj__processes__process__HEAT_TRANSPORT_BHE__material_property__refrigerant_viscosity}
        auto const& mu_conf = fluid_config.getConfigSubtree("refrigerant_viscosity");
        auto bhe_refrigerant_viscosity =
            MaterialLib::Fluid::createViscosityModel(mu_conf);
        //! \ogs_file_param{prj__processes__process__HEAT_TRANSPORT_BHE__material_property__refrigerant_specific_heat_capacity}
        auto const& cp_conf = fluid_config.getConfigSubtree("refrigerant_specific_heat_capacity");
        auto bhe_refrigerant_heat_capacity =
            MaterialLib::Fluid::createSpecificFluidHeatCapacityModel(cp_conf);
        //! \ogs_file_param{prj__processes__process__HEAT_TRANSPORT_BHE__material_property__refrigerant_thermal_conductivity}
        auto const& lambda_conf = fluid_config.getConfigSubtree("refrigerant_thermal_conductivity");
        auto bhe_regrigerant_heat_conductivity =
            MaterialLib::Fluid::createFluidThermalConductivityModel(lambda_conf);

        MaterialLib::Fluid::FluidProperty::ArrayType vars;
        vars[static_cast<int>(MaterialLib::Fluid::PropertyVariableType::T)] = 298.15;
        vars[static_cast<int>(MaterialLib::Fluid::PropertyVariableType::p)] = 101325.0;

        BHE::BHE_1U * m_bhe_1u = m_bhe_1u = new BHE::BHE_1U(
            bhe_ply_name, 
            bhe_bound_type, 
            curves, 
            { bhe_length, bhe_diameter } /* Borehole Geometry */, 
            { bhe_pipe_inner_radius, bhe_pipe_outer_radius, 
              bhe_pipe_in_wall_thickness, bhe_pipe_out_wall_thickness, 
              bhe_pipe_wall_thermal_conductivity } /* Pipe Parameters */,
            { bhe_refrigerant_viscosity->getValue(vars), bhe_refrigerant_density->getValue(vars),
              bhe_regrigerant_heat_conductivity->getValue(vars), bhe_refrigerant_heat_capacity->getValue(vars),
              bhe_fluid_longitudinal_dispsion_length } /* Refrigerant Parameters */,
            { bhe_grout_density, bhe_grout_porosity,
              bhe_grout_heat_capacity, bhe_grout_thermal_conductivity } /* Grout Parameters */,
            { bhe_use_ext_therm_resis, 
              bhe_intern_resistance, bhe_therm_resistance } /* If using given Ra Rb values*/, 
            { bhe_user_defined_therm_resis, 
              bhe_R_fig, bhe_R_fog, bhe_R_gg1, bhe_R_gg2, bhe_R_gs } /* If using customed thermal resistance values*/,
            bhe_refrigerant_flow_rate,
            bhe_pipe_distance, 
            bhe_power_in_watt_val,
            bhe_delta_T_val, 
            bhe_if_use_flow_rate_curve,
            bhe_switch_off_threshold);

        return m_bhe_1u;
    }
}