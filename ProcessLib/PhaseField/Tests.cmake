AddTest(
    NAME PhaseField_3D_Unconfined_Compression
    PATH PhaseField
    EXECUTABLE ogs
    EXECUTABLE_ARGS cube_1e0.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT (OGS_USE_LIS OR OGS_USE_MPI)
    DIFF_DATA
    expected_cube_1e0_pcs_0_ts_10000_t_1.000000.vtu cube_1e0_pcs_0_ts_10000_t_1.000000.vtu displacement displacement 1e-6 1e-6
    expected_cube_1e0_pcs_0_ts_10000_t_1.000000.vtu cube_1e0_pcs_0_ts_10000_t_1.000000.vtu phasefield phasefield 1e-6 1e-6
   )

AddTest(
    NAME PhaseField_2D_StaticHydraulicFracture
    PATH PhaseField
    EXECUTABLE ogs
    EXECUTABLE_ARGS 2D_static.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT (OGS_USE_LIS OR OGS_USE_MPI)
    DIFF_DATA
    expected_2D_StaticCrack_pcs_1_ts_1_t_1.000000.vtu 2D_StaticCrack_pcs_1_ts_1_t_1.000000.vtu displacement displacement 1e-15 1e-15
    expected_2D_StaticCrack_pcs_1_ts_1_t_1.000000.vtu 2D_StaticCrack_pcs_1_ts_1_t_1.000000.vtu phasefield phasefield 1e-15 1e-15
   )

AddTest(
    NAME PhaseField_3D_beam_LARGE
    PATH PhaseField
    EXECUTABLE ogs
    EXECUTABLE_ARGS beam3d_stag_1pcs.prj
    WRAPPER time
    TESTER vtkdiff
    REQUIREMENTS NOT (OGS_USE_LIS OR OGS_USE_MPI)
    DIFF_DATA
    expected_beam3_stag1pcsAT2_pcs_1_ts_10_t_1.000000.vtu beam3_stag1pcsAT2_pcs_1_ts_10_t_1.000000.vtu displacement displacement 1e-15 1e-15
    expected_beam3_stag1pcsAT2_pcs_1_ts_10_t_1.000000.vtu beam3_stag1pcsAT2_pcs_1_ts_10_t_1.000000.vtu phasefield phasefield 1e-15 1e-15
   )
