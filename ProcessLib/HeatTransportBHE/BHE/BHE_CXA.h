/**
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#pragma once

#include "BHEAbstract.h"

namespace ProcessLib
{
namespace HeatTransportBHE
{
namespace BHE  // namespace of borehole heat exchanger
{
class BHE_CXA final : public BHEAbstract
{
public:
    /**
     * constructor
     */
    BHE_CXA(
        const std::string name /* name of the BHE */,
        BHE::BHE_BOUNDARY_TYPE bound_type /* type of BHE boundary */,
        std::map<std::string,
                 std::unique_ptr<MathLib::PiecewiseLinearInterpolation>> const&
            bhe_curves /* bhe related curves */,
        BoreholeGeometry borehole_geometry = {100, 0.013},
        PipeParameters pipe_geometry =
            {0.024 /* inner radius of the pipline */,
             0.05 /* outer radius of the pipline */,
             0.004 /* pipe-in wall thickness*/,
             0.003 /* pipe-out wall thickness*/,
             0.38 /* thermal conductivity of the pipe wall */,
             0.38 /* thermal conductivity of the inner pipe wall */,
             0.38 /* thermal conductivity of the outer pipe wall */},
        RefrigerantParameters refrigerant_param =
            {
                0.00054741 /* dynamic viscosity of the refrigerant */,
                988.1 /* density of the refrigerant */,
                0.6405 /* thermal conductivity of the refrigerant */,
                4180 /* specific heat capacity of the refrigerant */, 1.0e-4 /* longitudinal dispersivity of the refrigerant in the pipeline */},
        GroutParameters grout_param =
            {2190 /* density of the grout */, 0.5 /* porosity of the grout */,
             1000 /* specific heat capacity of the grout */,
             2.3 /* thermal conductivity of the grout */},
        ExternallyDefinedRaRb extern_Ra_Rb =
            {false /* whether Ra and Rb values are used */,
             0.0 /* external defined borehole internal thermal resistance */,
             0.0 /* external defined borehole thermal resistance */},
        ExternallyDefinedThermalResistances extern_def_thermal_resistances =
            {false /* whether user defined R values are used */,
             0.0 /* external defined borehole thermal resistance */,
             0.0 /* external defined borehole thermal resistance */,
             0.0 /* external defined borehole thermal resistance */,
             0.0 /* external defined borehole thermal resistance */,
             0.0 /* external defined borehole thermal resistance */},
        double my_Qr = 21.86 /
                       86400 /* total refrigerant flow discharge of BHE */,
        double my_power_in_watt = 0.0 /* injected or extracted power */,
        double my_delta_T_val =
            0.0 /* Temperature difference btw inflow and outflow temperature */,
        // double my_ext_Ra = 0.0             /* external defined borehole
        // internal thermal resistance */, double my_ext_Rb = 0.0             /*
        // external defined borehole thermal resistance */, double my_ext_Rfig =
        // 0.0           /* external defined borehole thermal resistance */,
        // double my_ext_Rfog = 0.0           /* external defined borehole
        // thermal resistance */, double my_ext_Rgg1 = 0.0           /* external
        // defined borehole thermal resistance */, double my_ext_Rgg2 = 0.0 /*
        // external defined borehole thermal resistance */, double my_ext_Rgs =
        // 0.0           /* external defined borehole thermal resistance */,
        bool if_flowrate_curve = false /* whether flowrate curve is used*/,
        double my_threshold = 0.0) /* Threshold Q value for switching off the
                                      BHE when using Q_Curve_fixed_dT B.C.*/
    : BHEAbstract(name, BHE_TYPE::TYPE_CXA, borehole_geometry, pipe_geometry,
                  refrigerant_param, grout_param, extern_Ra_Rb,
                  extern_def_thermal_resistances, std::move(bhe_curves),
                  bound_type, if_flowrate_curve)
    {
        _u = Eigen::Vector2d::Zero();
        _Nu = Eigen::Vector2d::Zero();

        Q_r = my_Qr;

        power_in_watt_val = my_power_in_watt;
        delta_T_val = my_delta_T_val;
        threshold = my_threshold;

        // get the corresponding curve
        std::map<std::string,
                 std::unique_ptr<MathLib::PiecewiseLinearInterpolation>>::
            const_iterator it;
        if (bound_type ==
                BHE_BOUNDARY_TYPE::POWER_IN_WATT_CURVE_FIXED_DT_BOUNDARY ||
            bound_type == BHE_BOUNDARY_TYPE::
                              POWER_IN_WATT_CURVE_FIXED_FLOW_RATE_BOUNDARY ||
            bound_type ==
                BHE_BOUNDARY_TYPE::
                    BUILDING_POWER_IN_WATT_CURVE_FIXED_FLOW_RATE_BOUNDARY)
        {
            it = bhe_curves.find("power_in_watt_curve");
            if (it == bhe_curves.end())
            {
                // curve not found, fatal error
                OGS_FATAL(
                    "Required pow_in_watt_curve cannot be found in the BHE "
                    "parameters!");
            }

            // curve successfully found
            power_in_watt_curve = it->second.get();
        }

        if (if_flowrate_curve)
        {
            use_flowrate_curve = true;

            it = bhe_curves.find("flow_rate_curve");
            if (it == bhe_curves.end())
            {
                OGS_FATAL(
                    "Required flow_rate_curve annot be found in the BHE "
                    "parameters!");
            }

            // curve successfully found
            flowrate_curve = it->second.get();
        }

        constexpr double PI = boost::math::constants::pi<double>();
        // Table 1 in Diersch_2011_CG
        S_i = PI * 2.0 * pipe_geometry.r_outer;
        S_io = PI * 2.0 * pipe_geometry.r_inner;
        S_gs = PI * borehole_geometry.diameter;

        // cross section area calculation
        CSA_i = PI * (pipe_geometry.r_outer * pipe_geometry.r_outer -
                      (pipe_geometry.r_inner + pipe_geometry.b_in) *
                          (pipe_geometry.r_inner + pipe_geometry.b_in));
        CSA_o = PI * pipe_geometry.r_inner * pipe_geometry.r_inner;
        CSA_g = PI * (0.25 * borehole_geometry.diameter *
                          borehole_geometry.diameter -
                      (pipe_geometry.r_outer + pipe_geometry.b_out) *
                          (pipe_geometry.r_outer + pipe_geometry.b_out));

        // initialization calculation
        initialize();
    };

    /**
     * return the number of unknowns needed for CXA BHE
     */
    std::size_t getNumUnknowns() const { return 3; }

    void initialize();

    void updateFlowRateFromCurve(double current_time)
    {
        if (use_flowrate_curve)
        {
            double Q_r_tmp(0.0);
            Q_r_tmp = flowrate_curve->getValue(current_time);
            updateFlowRate(Q_r_tmp);
        }
    };

    /**
     * calculate thermal resistance
     */
    void calcThermalResistances();

    /**
     * Nusselt number calculation
     */
    void calcNusseltNum(double const Pr, std::pair<double, double> Re);

    /**
     * calculate heat transfer coefficient
     */
    void calcHeatTransferCoefficients();

    /**
     * return the coeff of mass matrix,
     * depending on the index of unknown.
     */
    double getMassCoeff(std::size_t idx_unknown) const;

    /**
     * return the coeff of laplace matrix,
     * depending on the index of unknown.
     */
    void getLaplaceMatrix(std::size_t idx_unknown,
                          Eigen::MatrixXd& mat_laplace) const;

    /**
     * return the coeff of advection matrix,
     * depending on the index of unknown.
     */
    void getAdvectionVector(std::size_t idx_unknown,
                            Eigen::VectorXd& vec_advection) const;

    /**
     * return the _R_matrix, _R_pi_s_matrix, _R_s_matrix
     */
    void setRMatrices(
        const int idx_bhe_unknowns, const int NumNodes,
        Eigen::MatrixXd& matBHE_loc_R,
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>&
            R_matrix,
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>&
            R_pi_s_matrix,
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>&
            R_s_matrix) const;
    /**
     * return the coeff of boundary heat exchange matrix,
     * depending on the index of unknown.
     */
    double getBoundaryHeatExchangeCoeff(std::size_t idx_unknown) const;

    /**
     * return the inflow temperature based on outflow temperature and fixed
     * power.
     */
    double getTinByTout(double T_out, double current_time);

    std::vector<std::pair<int, int>> const& inflowOutflowBcComponentIds()
        const override
    {
        return _inflow_outflow_bc_component_ids;
    }

    /**
     * required by eigen library,
     * to make sure the dynamically allocated class has
     * aligned "operator new"
     */
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    std::vector<std::pair<int, int>> const _inflow_outflow_bc_component_ids = {
        {0, 1}};
    /**
     * thermal resistances
     */
    double _R_ff, _R_fig;

    /**
     * thermal resistances due to advective flow of refrigerant in the pipes
     */
    double _R_adv_o1, _R_adv_a_i1, _R_adv_b_i1;

    /**
     * thermal resistances due to the grout transition
     */
    double _R_con_b;

    /**
     * thermal resistances of the grout
     */
    double _R_g;

    /**
     * thermal resistances of the grout soil exchange
     */
    double _R_gs;

    /**
     * heat transfer coefficients
     */
    double _PHI_fig, _PHI_ff, _PHI_gs;

    /**
     * specific exchange surfaces S
     */
    double S_i, S_io, S_gs;
    /**
     * cross section area
     */
    double CSA_i, CSA_o, CSA_g;
    /**
     * Nusselt number
     */
    Eigen::Vector2d _Nu;
    /**
     * flow velocity inside the pipeline
     */
    Eigen::Vector2d _u;
};
}  // namespace BHE
}  // namespace HeatTransportBHE
}  // namespace ProcessLib
