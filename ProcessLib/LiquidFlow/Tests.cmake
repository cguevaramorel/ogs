# Liquid flow
AddTest(
    NAME LiquidFlow_LineDirichletNeumannBC
    PATH Parabolic/LiquidFlow/LineDirichletNeumannBC
    EXECUTABLE ogs
    EXECUTABLE_ARGS line_dirichlet_neumannBC.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    sat1D.vtu sat_1D_pcs_0_ts_1_t_1.000000.vtu AnalyticPressure pressure 1e-8 1e-8
    sat1D.vtu sat_1D_pcs_0_ts_1_t_1.000000.vtu AnalyticVec v 1e-8 1e-8
)
AddTest(
    NAME LiquidFlow_PressureBCatCornerOfAnisotropicSquare
    PATH Parabolic/LiquidFlow/PressureBCatCornerOfAnisotropicSquare
    EXECUTABLE ogs
    EXECUTABLE_ARGS pressureBC_at_corner_of_anisotropic_square.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    mesh2D.vtu sat_2D_lflow_pcs_0_ts_1_t_1.000000.vtu OGS5_Results pressure 1e-8 1e-8
)
AddTest(
    NAME LiquidFlow_GravityDriven
    PATH Parabolic/LiquidFlow/GravityDriven
    EXECUTABLE ogs
    EXECUTABLE_ARGS gravity_driven.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    mesh2D.vtu gravity_driven_pcs_0_ts_1_t_1.000000.vtu AnalyticPressure pressure 1e-8 1e-8
    mesh2D.vtu gravity_driven_pcs_0_ts_1_t_1.000000.vtu v_ref v 1e-8 1e-8
)
AddTest(
    NAME LiquidFlow_AxisymTheis
    PATH Parabolic/LiquidFlow/AxiSymTheis
    EXECUTABLE ogs
    EXECUTABLE_ARGS axisym_theis.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    axisym_theis.vtu liquid_pcs_pcs_0_ts_30_t_1728.000000.vtu OGS5_pressure pressure 1e-8 1e-8
)
AddTest(
    NAME LiquidFlow_BuildupTest
    PATH Parabolic/LiquidFlow/BuildupTest
    EXECUTABLE ogs
    EXECUTABLE_ARGS buildup_test.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    standard_solution_buildup_test_pcs_0_ts_107_t_424800.000000.vtu buildup_test_pcs_0_ts_107_t_424800.000000.vtu pressure pressure 5e-6 0.0
    standard_solution_buildup_test_pcs_0_ts_211_t_720000.000000.vtu buildup_test_pcs_0_ts_211_t_720000.000000.vtu pressure pressure 5e-5 0.0
)

AddTest(
    NAME LARGE_LiquidFlow_Anisotropic_GravityDriven3D
    PATH Parabolic/LiquidFlow/GravityDriven3D
    RUNTIME 70
    EXECUTABLE ogs
    EXECUTABLE_ARGS anisotropic_gravity_driven3D.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    hex.vtu anisotropic_gravity_driven3D_pcs_0_ts_1_t_1.000000.vtu analytic_pressure pressure 1e-6 1e-6
)

AddTest(
    NAME LARGE_LiquidFlow_Isotropic_GravityDriven3D
    PATH Parabolic/LiquidFlow/GravityDriven3D
    RUNTIME 70
    EXECUTABLE ogs
    EXECUTABLE_ARGS isotropic_gravity_driven3D.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    hex.vtu isotropic_gravity_driven3D_pcs_0_ts_1_t_1.000000.vtu analytic_pressure pressure 1e-6 1e-6
)

AddTest(
    NAME LiquidFlowDirichletBCWithinTimeInterval
    PATH Parabolic/LiquidFlow/TimeIntervalDirichletBC
    EXECUTABLE ogs
    EXECUTABLE_ARGS TimeIntervalDirichletBC.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    mesh2D.vtu dirichlet_bc_wihin_interval_pcs_0_ts_2_t_10.000000.vtu analytical_solution_t_lt_10 pressure 1e-6 1e-12
    mesh2D.vtu dirichlet_bc_wihin_interval_pcs_0_ts_4_t_20.000000.vtu analytical_solution_t_gt_10 pressure 1e-6 1e-12
)

AddTest(
    NAME LiquidFlow_h1_1Dsource
    PATH Parabolic/LiquidFlow/Verification/h1_1Dsource
    EXECUTABLE_ARGS h1_1Dsource.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    h1_1Dsource_pcs_0_ts_1_t_1.000000.vtu h1_1Dsource_pcs_0_ts_1_t_1.000000.vtu pressure pressure 5e-8 0.0
)

AddTest(
    NAME LiquidFlow_h1_1Dsteady
    PATH Parabolic/LiquidFlow/Verification/h1_1Dsteady
    EXECUTABLE_ARGS h1_1Dsteady.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    h1_1Dsteady_pcs_0_ts_1_t_1.000000.vtu h1_1Dsteady_pcs_0_ts_1_t_1.000000.vtu pressure pressure 1e-9 0.0
)

AddTest(
    NAME LiquidFlow_h1_3Dhydstat
    PATH Parabolic/LiquidFlow/Verification/h1_3Dhydstat
    EXECUTABLE_ARGS h1_3Dhydstat.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    h1_3Dhydstat_pcs_0_ts_1_t_1.000000.vtu h1_3Dhydstat_pcs_0_ts_1_t_1.000000.vtu pressure pressure 1e-5 0.0
)

AddTest(
    NAME LiquidFlow_h2_1D1bt
    PATH Parabolic/LiquidFlow/Verification/h2_1D1bt
    EXECUTABLE_ARGS h2_1D1bt.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    h2_1D1bt_pcs_0_ts_500_t_21600.000000.vtu h2_1D1bt_pcs_0_ts_500_t_21600.000000.vtu pressure pressure 1e-5 0.0
    h2_1D1bt_pcs_0_ts_1000_t_43200.000000.vtu h2_1D1bt_pcs_0_ts_1000_t_43200.000000.vtu pressure pressure 1e-5 0.0
)

AddTest(
    NAME LiquidFlow_h2_1D2bt
    PATH Parabolic/LiquidFlow/Verification/h2_1D2bt
    EXECUTABLE_ARGS h2_1D2bt.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT OGS_USE_MPI
    DIFF_DATA
    h2_1D2bt_pcs_0_ts_1500_t_3888.000000.vtu h2_1D2bt_pcs_0_ts_1500_t_3888.000000.vtu pressure pressure 1e-5 0.0
    h2_1D2bt_pcs_0_ts_3000_t_7776.000000.vtu h2_1D2bt_pcs_0_ts_3000_t_7776.000000.vtu pressure pressure 1e-5 0.0
)

#===============================================================================
# PETSc/MPI
AddTest(
    NAME LiquidFlow_LineDirichletNeumannBC
    PATH Parabolic/LiquidFlow/LineDirichletNeumannBC
    EXECUTABLE_ARGS line_dirichlet_neumannBC.prj
    WRAPPER mpirun
    WRAPPER_ARGS -np 1
    TESTER vtkdiff
    REQUIREMENTS OGS_USE_MPI
    DIFF_DATA
    sat1D.vtu sat_1D_pcs_0_ts_1_t_1_000000_0.vtu AnalyticPressure pressure 1e-8 1e-8
#    sat1D.vtu sat_1D_pcs_0_ts_1_t_1_000000_0.vtu AnalyticVec v 1e-8 1e-8
)
AddTest(
    NAME LiquidFlow_GravityDriven
    PATH Parabolic/LiquidFlow/GravityDriven
    EXECUTABLE_ARGS gravity_driven.prj
    WRAPPER mpirun
    WRAPPER_ARGS -np 1
    TESTER vtkdiff
    REQUIREMENTS OGS_USE_MPI
    DIFF_DATA
    mesh2D.vtu gravity_driven_pcs_0_ts_1_t_1_000000_0.vtu AnalyticPressure pressure 1e-8 1e-8
#    mesh2D.vtu gravity_driven_pcs_0_ts_1_t_1_000000_0.vtu v_ref v 1e-8 1e-8
)
AddTest(
    NAME LiquidFlow_PressureBCatCornerOfAnisotropicSquare
    PATH Parabolic/LiquidFlow/PressureBCatCornerOfAnisotropicSquare
    EXECUTABLE_ARGS pressureBC_at_corner_of_anisotropic_square.prj
    WRAPPER mpirun
    WRAPPER_ARGS -np 1
    TESTER vtkdiff
    REQUIREMENTS OGS_USE_MPI
    DIFF_DATA
    mesh2D.vtu sat_2D_lflow_pcs_0_ts_1_t_1_000000_0.vtu OGS5_Results pressure 1e-8 1e-8
)
AddTest(
    NAME LiquidFlow_AxisymTheis
    PATH Parabolic/LiquidFlow/AxiSymTheis
    EXECUTABLE_ARGS axisym_theis.prj
    WRAPPER mpirun
    WRAPPER_ARGS -np 1
    TESTER vtkdiff
    REQUIREMENTS OGS_USE_MPI
    DIFF_DATA
    axisym_theis.vtu liquid_pcs_pcs_0_ts_30_t_1728_000000_0.vtu OGS5_pressure pressure 1e-8 1e-8
)
AddTest(
    NAME LARGE_LiquidFlow_Anisotropic_GravityDriven3D
    PATH Parabolic/LiquidFlow/GravityDriven3D
    EXECUTABLE_ARGS anisotropic_gravity_driven3D.prj
    WRAPPER mpirun
    WRAPPER_ARGS -np 1
    TESTER vtkdiff
    REQUIREMENTS OGS_USE_MPI
    DIFF_DATA
    hex.vtu anisotropic_gravity_driven3D_pcs_0_ts_1_t_1_000000_0.vtu analytic_pressure pressure 1e-6 1e-6
)

AddTest(
    NAME LARGE_LiquidFlow_Isotropic_GravityDriven3D
    PATH Parabolic/LiquidFlow/GravityDriven3D
    EXECUTABLE_ARGS isotropic_gravity_driven3D.prj
    WRAPPER mpirun
    WRAPPER_ARGS -np 1
    TESTER vtkdiff
    REQUIREMENTS OGS_USE_MPI
    DIFF_DATA
    hex.vtu isotropic_gravity_driven3D_pcs_0_ts_1_t_1_000000_0.vtu analytic_pressure pressure 1e-6 1e-6
)

AddTest(
    NAME LiquidFlowDirichletBCWithinTimeInterval
    PATH Parabolic/LiquidFlow/TimeIntervalDirichletBC
    EXECUTABLE_ARGS TimeIntervalDirichletBC.prj
    WRAPPER mpirun
    WRAPPER_ARGS -np 1
    TESTER vtkdiff
    REQUIREMENTS OGS_USE_MPI
    DIFF_DATA
    mesh2D.vtu dirichlet_bc_wihin_interval_pcs_0_ts_2_t_10_000000_0.vtu analytical_solution_t_lt_10 pressure 1e-6 1e-12
    mesh2D.vtu dirichlet_bc_wihin_interval_pcs_0_ts_4_t_20_000000_0.vtu analytical_solution_t_gt_10 pressure 1e-6 1e-12
)
