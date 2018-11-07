AddTest(
    NAME HeatTransportBHE_1U_3D_beier_sandbox
    PATH Parabolic/T/3D_Beier_sandbox
    EXECUTABLE ogs
    EXECUTABLE_ARGS beier_sandbox.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    beier_sandbox_pcs_0_ts_10_t_600.000000.vtu beier_sandbox_pcs_0_ts_10_t_600.000000.vtu temperature_BHE1 temperature_BHE1 0 5e-15
    beier_sandbox_pcs_0_ts_10_t_600.000000.vtu beier_sandbox_pcs_0_ts_10_t_600.000000.vtu temperature_soil temperature_soil 0 1e-13
)

AddTest(
    NAME HeatTransportBHE_1U_beier_sandbox_fixed_power_constant_flow
    PATH Parabolic/T/3D_Beier_sandbox
    EXECUTABLE ogs
    EXECUTABLE_ARGS fixed_power_constant_flow.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    fixed_power_constant_flow_pcs_0_ts_10_t_600.000000.vtu fixed_power_constant_flow_pcs_0_ts_10_t_600.000000.vtu temperature_BHE1 temperature_BHE1 0 5e-15
    fixed_power_constant_flow_pcs_0_ts_10_t_600.000000.vtu fixed_power_constant_flow_pcs_0_ts_10_t_600.000000.vtu temperature_soil temperature_soil 0 1e-13
)
AddTest(
    NAME HeatTransportBHE_CXA_3D_deep_borehole
    PATH Parabolic/T/3D_deep_borehole_heat_exchanger
    EXECUTABLE ogs
    EXECUTABLE_ARGS deep_borehole_heat_exchanger.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    deep_borehole_heat_exchanger_pcs_0_ts_10_t_600.000000.vtu deep_borehole_heat_exchanger_pcs_0_ts_10_t_600.000000.vtu temperature_BHE1 temperature_BHE1 0 1e-14
    deep_borehole_heat_exchanger_pcs_0_ts_10_t_600.000000.vtu deep_borehole_heat_exchanger_pcs_0_ts_10_t_600.000000.vtu temperature_soil temperature_soil 0 1e-13
)
