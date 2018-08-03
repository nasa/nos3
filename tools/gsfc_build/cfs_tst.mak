################################################################################
#
# File: CFS supplemental makefile for CFS testings
#
#################################################################################

#===============================================================================
# cFE testings
#===============================================================================

cfe_ut_build:: cfs_prebuild
	$(MAKE) -C cfe/unit-test

cfe_ut_clean:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/unit-test clean

cfe_ut_run::
	$(MAKE) -C cfe/unit-test gcov

cfe_ut_build_es:: cfs_prebuild
	$(MAKE) -C cfe/unit-test/es

cfe_ut_clean_es:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/unit-test/es clean

cfe_ut_run_es::
	$(MAKE) -C cfe/unit-test/es gcov

cfe_ut_build_evs:: cfs_prebuild
	$(MAKE) -C cfe/unit-test/evs

cfe_ut_clean_evs:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/unit-test/evs clean

cfe_ut_run_evs::
	$(MAKE) -C cfe/unit-test/evs gcov

cfe_ut_build_fs:: cfs_prebuild
	$(MAKE) -C cfe/unit-test/fs

cfe_ut_clean_fs:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/unit-test/fs clean

cfe_ut_run_fs::
	$(MAKE) -C cfe/unit-test/fs gcov

cfe_ut_build_sb:: cfs_prebuild
	$(MAKE) -C cfe/unit-test/sb

cfe_ut_clean_sb:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/unit-test/sb clean

cfe_ut_run_sb::
	$(MAKE) -C cfe/unit-test/sb gcov

cfe_ut_build_tbl:: cfs_prebuild
	$(MAKE) -C cfe/unit-test/tbl

cfe_ut_clean_tbl:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/unit-test/tbl clean

cfe_ut_run_tbl::
	$(MAKE) -C cfe/unit-test/tbl gcov

cfe_ut_build_time:: cfs_prebuild
	$(MAKE) -C cfe/unit-test/time

cfe_ut_clean_time:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/unit-test/time clean

cfe_ut_run_time::
	$(MAKE) -C cfe/unit-test/time gcov

#===============================================================================
# OSAL testings
#===============================================================================

osal_ut1_build:: cfs_prebuild
	$(MAKE) -C cfe/osal/unit-test1

osal_ut1_clean:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/osal/unit-test1 clean

# Not available yet
#osal_ut1_run::
#	$(MAKE) -C cfe/osal/unit-test1 gcov

osal_ut2_build:: cfs_prebuild
	$(MAKE) -C cfe/osal/unit-test2

osal_ut2_clean:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/osal/unit-test2 clean

osal_ut2_run::
	$(MAKE) -C cfe/osal/unit-test2 gcov

osal_ut_build_oscore:: cfs_prebuild
	$(MAKE) -C cfe/osal/unit-test2/oscore-test

osal_ut_clean_oscore:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/osal/unit-test2/oscore-test clean

osal_ut_run_oscore::
	$(MAKE) -C cfe/osal/unit-test2/oscore-test gcov

osal_ut_build_osfile:: cfs_prebuild
	$(MAKE) -C cfe/osal/unit-test2/osfile-test

osal_ut_clean_osfile:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/osal/unit-test2/osfile-test clean

osal_ut_run_osfile::
	$(MAKE) -C cfe/osal/unit-test2/osfile-test gcov

osal_ut_build_osfilesys:: cfs_prebuild
	$(MAKE) -C cfe/osal/unit-test2/osfilesys-test

osal_ut_clean_osfilesys:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/osal/unit-test2/osfilesys-test clean

osal_ut_run_osfilesys::
	$(MAKE) -C cfe/osal/unit-test2/osfilesys-test gcov

osal_ut_build_osloader:: cfs_prebuild
	$(MAKE) -C cfe/osal/unit-test2/osloader-test

osal_ut_clean_osloader:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/osal/unit-test2/osloader-test clean

osal_ut_run_osloader::
	$(MAKE) -C cfe/osal/unit-test2/osloader-test gcov

osal_ut_build_osnetwork:: cfs_prebuild
	$(MAKE) -C cfe/osal/unit-test2/osnetwork-test

osal_ut_clean_osnetwork:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/osal/unit-test2/osnetwork-test clean

osal_ut_run_osnetwork::
	$(MAKE) -C cfe/osal/unit-test2/osnetwork-test gcov

osal_ut_build_osprintf:: cfs_prebuild
	$(MAKE) -C cfe/osal/unit-test2/osprintf-test

osal_ut_clean_osprintf:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/osal/unit-test2/osprintf-test clean

osal_ut_run_osprintf::
	$(MAKE) -C cfe/osal/unit-test2/osprintf-test gcov

osal_ut_build_ostimer:: cfs_prebuild
	$(MAKE) -C cfe/osal/unit-test2/ostimer-test

osal_ut_clean_ostimer:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/osal/unit-test2/ostimer-test clean

osal_ut_run_ostimer::
	$(MAKE) -C cfe/osal/unit-test2/ostimer-test gcov

#===============================================================================
# PSP testings
#===============================================================================

psp_ut_build:: cfs_prebuild
	$(MAKE) -C cfe/psp/unit-test

psp_ut_clean:: cfs_clean_build_mission_incs cfs_clean_build_platform_incs
	$(MAKE) -C cfe/psp/unit-test clean

#psp_ut_run::
	$(MAKE) -C cfe/psp/unit-test gcov

#===============================================================================
# Core app testings
#===============================================================================

################################################################################
# End of CFS supplemental makefile for CFS testings
################################################################################

