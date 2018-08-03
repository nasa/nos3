################################################################################
#
# File: CFS supplemental makefile for applications
#
################################################################################

GSFC_BUILD_VERSION := 6.5.90

# Centralise management of nontrivial "cp" and "rm" flags
# and stderr outputs.

CP_MAYBE := /bin/cp -vn 2>/dev/null
CP_ALWAYS := /bin/cp -vf 2>/dev/null
RM_FILES := /bin/rm -f 2>/dev/null
RM_TREES := /bin/rm -fr 2>/dev/null

cfs_envs::
	@echo ""
	@echo "GSFC Classic Build Version $(GSFC_BUILD_VERSION)"
	@echo ""
	@echo ">>> [ENVS] MAKEFILE ENVIRONMENT VARIABLES <<<"
	@echo ""
	@echo "MISSION = $(MISSION)"
	@echo "THE_APPS = $(THE_APPS)"
	@echo "THE_TBLS = $(THE_TBLS)"
	@echo ""
	@echo "CFS_MISSION = $(CFS_MISSION)"
	@echo "CFS_MISSION_INC = $(CFS_MISSION_INC)"
	@echo "CFS_APP_SRC = $(CFS_APP_SRC)"
	@echo "CFE_FSW = $(CFE_FSW)"
	@echo "CFE_CORE_SRC = $(CFE_CORE_SRC)"
	@echo "CFE_CORE_PLATFORM_INC_SRC = $(CFE_CORE_PLATFORM_INC_SRC)"
	@echo "CFE_TOOLS = $(CFE_TOOLS)"
	@echo "CFE_PSP_SRC = $(CFE_PSP_SRC)"
	@echo "OSAL = $(OSAL)"
	@echo "OSAL_SRC = $(OSAL_SRC)"
	@echo ""
	@echo "BUILD_PATH = $(BUILD_PATH)"
	@echo "CFS_EXE = $(CFS_EXE)"
	@echo "CFS_PROM = $(CFS_PROM)"
	@echo "CFS_PROM_APPS = $(CFS_PROM_APPS)"
	@echo "CFS_RAM = $(CFS_RAM)"

#===============================================================================
# PRE-BUILD rules
#===============================================================================

# Sometimes this rule fires even if the target exists.

$(OSAL)/build/inc/osconfig.h::
	@echo ""
	@echo ">>> [PRE-BUILD] VERIFY OSAL CONFIGURED <<<"
	@echo ""
	@ test -f $@                                                    \
	|| $(MAKE) -C $${OSAL}/build config

#-------------------------------------------------------------------------------

cfs_copy_core_incs:: $(OSAL)/build/inc/osconfig.h
	@echo ""
	@echo ">>> [PRE-BUILD] GET MISSING HEADER FILES FROM <CFS_MISSION> CORE <<<"
	@echo ""
	@mkdir -p $(CFS_MISSION_INC)
	@mkdir -p inc
	@- $(CP_MAYBE) $(CFE_FSW)/mission_inc/*.h $(CFS_MISSION_INC)
	@- $(CP_MAYBE) $(CFE_FSW)/platform_inc/$(CFE_CORE_PLATFORM_INC_SRC)/*.h inc
	@- $(CP_MAYBE) $(OSAL)/build/inc/osconfig.h inc

#-------------------------------------------------------------------------------

cfs_copy_app_shared_incs::
	@echo ""
	@echo ">>> [PRE-BUILD] GET MISSING HEADER FILES FROM <CFS_APP_SRC>/INC <<<"
	@echo ""
	@mkdir -p $(CFS_MISSION_INC)
	@- $(CP_MAYBE) $(CFS_APP_SRC)/inc/*.h $(CFS_MISSION_INC)

#-------------------------------------------------------------------------------

cfs_copy_app_public_incs::
	@echo ""
	@echo ">>> [PRE-BUILD] GET MISSING HEADER FILES FROM <APP>/PUBLIC_INC <<<"
	@echo ""
	@for app in $(THE_APPS); do                                     \
	   $(CP_MAYBE) $(CFS_APP_SRC)/$$app/fsw/public_inc/*.h inc      \
	   || true ;                                                    \
	 done

#-------------------------------------------------------------------------------

cfs_copy_app_mission_incs::
	@echo ""
	@echo ">>> [PRE-BUILD] GET MISSING HEADER FILES FROM <APP>/MISSION_INC <<<"
	@echo ""
	@for app in $(THE_APPS); do                                     \
	   $(CP_MAYBE) $(CFS_APP_SRC)/$$app/fsw/mission_inc/*.h $(CFS_MISSION_INC) \
	   || true ;                                                    \
	 done

#-------------------------------------------------------------------------------

cfs_copy_app_platform_incs::
	@echo ""
	@echo ">>> [PRE-BUILD] GET MISSING HEADER FILES FROM <APP>/PLATFORM_INC <<<"
	@echo ""
	@for app in $(THE_APPS); do                                     \
	   $(CP_MAYBE) $(CFS_APP_SRC)/$$app/fsw/platform_inc/*.h inc    \
	   || true ;                                                    \
	 done

#-------------------------------------------------------------------------------

cfs_make_app_build_dirs::
	@echo ""
	@echo ">>> [PRE-BUILD] CREATING MISSING <APP> BUILD DIRECTORIES <<<"
	@echo ""
	@mkdir -p $(THE_APPS)

#-------------------------------------------------------------------------------

cfs_copy_app_makefiles::
	@echo ""
	@echo ">>> [PRE-BUILD] GET MISSING MAKEFILES FROM <APP>/FOR_BUILD <<<"
	@echo ""
	@for app in $(THE_APPS); do                                     \
	   $(CP_MAYBE) $(CFS_APP_SRC)/$${app}/fsw/for_build/* $${app}   \
	   || true ;                                                    \
	 done

#-------------------------------------------------------------------------------

cfs_prebuild:: cfs_copy_core_incs                                       \
	       cfs_copy_app_shared_incs                                 \
	       cfs_copy_app_public_incs                                 \
	       cfs_copy_app_mission_incs                                \
	       cfs_copy_app_platform_incs                               \
	       cfs_make_app_build_dirs                                  \
	       cfs_copy_app_makefiles

#===============================================================================
# IN-BUILD RULES
#===============================================================================

cfs_build_core::
	@echo ""
	@echo "GSFC Classic Build Version $(GSFC_BUILD_VERSION)"
	@echo ""
	@echo ">>> [BUILD] BUILD CFE <<<"
	@echo ""
	@$(MAKE) -C cfe

#-------------------------------------------------------------------------------

cfs_build_apps::
	@echo ""
	@echo "GSFC Classic Build Version $(GSFC_BUILD_VERSION)"
	@echo ""
	@echo ">>> [BUILD] BUILD APPS <<<"
	@echo ""
	@for app in $(THE_APPS); do                                     \
	   $(MAKE) -C $${app} || exit 1;                                \
	   ! test $${app} = "sch"                                       \
	   || $(MAKE) -f schlib.mak -C sch                              \
	   || exit 1;                                                   \
	 done

#-------------------------------------------------------------------------------

cfs_build_app_tbls::
	@echo ""
	@echo "GSFC Classic Build Version $(GSFC_BUILD_VERSION)"
	@echo ""
	@echo ">>> [BUILD] BUILD APP TABLES <<<"
	@echo ""
	@for app in $(THE_TBLS); do                                     \
	   test ! -f ./$${app}/$${app}tables.mak                        \
	   || $(MAKE) -f $${app}tables.mak -C $${app}                   \
	   || exit 1;                                                   \
	   test ! -f ./$${app}/$${app}_tables.mak                       \
	   || $(MAKE) -f $${app}_tables.mak -C $${app}                  \
	   || exit 1;                                                   \
	 done

#-------------------------------------------------------------------------------

cfs_inbuild:: cfs_build_core                                            \
	      cfs_build_apps                                            \
	      cfs_build_app_tbls 

#===============================================================================
# POST-BUILD RULES
#===============================================================================

cfs_install_core::
	@echo ""
	@echo ">>> [POST-BUILD] INSTALL CORE <<<"
	@echo ""
	$(MAKE) -C cfe install

#-------------------------------------------------------------------------------

cfs_install_apps::
	@echo ""
	@echo ">>> [POST-BUILD] INSTALL APPS <<<"
	@echo ""
	@for app in $(THE_APPS); do                                     \
	    $(MAKE) -C $${app} install                                  \
	    || exit 1;                                                  \
	    test $${app} != "sch"                                       \
	    || $(MAKE) -f schlib.mak -C sch install                     \
	    || exit 1;                                                  \
	 done

#-------------------------------------------------------------------------------

cfs_install_app_tbls::
	@echo ""
	@echo ">>> [POST-BUILD] INSTALL APP TABLES <<<"
	@echo ""
	@for app in $(THE_TBLS); do                                     \
	    test ! -f $${app}/$${app}tables.mak                         \
	    || $(MAKE) -f $${app}tables.mak -C $${app} install          \
	    || exit 1;                                                  \
	    test ! -f $${app}/$${app}_tables.mak                        \
	    || $(MAKE) -f $${app}_tables.mak -C $${app} install         \
	    || exit 1;                                                  \
	 done

#-------------------------------------------------------------------------------

cfs_copy_installs::
	@echo ""
	@echo ">>> [POST-BUILD] COPY BUILT FILES TO <CFS_PROM_APPS> <<<"
	@echo " >> Create Directories <<"
	-mkdir -p $(CFS_PROM)
	-mkdir -p $(CFS_PROM_APPS)
	-mkdir -p $(CFS_PROM)/log
	-mkdir -p $(CFS_PROM)/upload
	-mkdir -p $(CFS_PROM)/download
	-mkdir -p $(CFS_RAM)
	@echo ""
	@echo " >> Change Permissions <<"
	-chmod 775 $(CFS_EXE)/*.$(CFE_CORE_EXE_TYPE) 
	-chmod 644 $(CFS_EXE)/*.$(CFS_APP_EXE_TYPE) 
	-chmod 644 $(CFS_EXE)/*.tbl
	-chmod 644 $(CFS_EXE)/*.scr
	@echo ""
	@echo " >> Copy Files <<"
	@- $(CP_ALWAYS) $(CFS_EXE)/*.$(CFE_CORE_EXE_TYPE) $(CFS_PROM_APPS)
	@- $(CP_ALWAYS) $(CFS_EXE)/*.$(CFS_APP_EXE_TYPE) $(CFS_PROM_APPS)
	@- $(CP_ALWAYS) $(CFS_EXE)/*.tbl $(CFS_PROM_APPS)
	@echo ""
	@- $(CP_ALWAYS) $(CFS_EXE)/run $(CFS_PROM_APPS)
	@- $(CP_ALWAYS) $(CFS_EXE)/cfe_es_startup.scr $(CFS_PROM_APPS)

#-------------------------------------------------------------------------------

cfs_postbuild:: cfs_install_core                                        \
	        cfs_install_apps                                        \
	        cfs_install_app_tbls                                    \
	        cfs_copy_installs
	@echo ""
	@echo ">>> DONE! <<<"
	@echo ""

#===============================================================================
# CLEAN BUILD RULES
#===============================================================================

cfs_clean_core::
	@echo ""
	@echo ">>> [CLEAN] CLEAN CORE <<<"
	@echo ""
	$(MAKE) -C cfe clean

#-------------------------------------------------------------------------------

cfs_clean_apps::
	@echo ""
	@echo ">>> [CLEAN] CLEAN APPS <<<"
	@echo ""
	@for app in $(THE_APPS); do                                     \
	    test ! -d $${app}                                           \
	    || $(MAKE) -C $${app} clean                                 \
	    || exit 1;                                                  \
	 done

#-------------------------------------------------------------------------------

cfs_clean_build_exe::
	@echo ""
	@echo ">>> [CLEAN] REMOVE BUILD/EXE BUILT FILES <<<"
	@echo ""
	@- $(RM_FILES) exe/*.$(CFE_CORE_EXE_TYPE)
	@- $(RM_FILES) exe/*.$(CFS_APP_EXE_TYPE)
	@- $(RM_FILES) exe/*.tbl

#-------------------------------------------------------------------------------

cfs_clean_build_mission_incs::
	@echo ""
	@echo ">>> [CLEAN] - REMOVE <CFS_MISSION_INC> HEADER FILES <<<"
	@echo ""
	@- $(RM_FILES) $(CFS_MISSION_INC)/*

#-------------------------------------------------------------------------------

cfs_clean_build_platform_incs::
	@echo ""
	@echo ">>> [CLEAN] REMOVE BUILD/INC HEADER FILES <<<"
	@echo ""
	@- $(RM_FILES) inc/*

#-------------------------------------------------------------------------------

cfs_clean:: cfs_clean_core                                              \
	    cfs_clean_apps                                              \
	    cfs_clean_build_exe           
	@echo ""
	@echo ">>> DONE! <<<"
	@echo ""

#-------------------------------------------------------------------------------

cfs_clean_install_dirs::
	@echo ""
	@echo ">>> [REALCLEAN] REMOVE INSTALL DIRS <<<"
	@echo ""
	@- $(RM_TREES) $(CFS_PROM)
	@- $(RM_TREES) $(CFS_RAM)

#-------------------------------------------------------------------------------

cfs_clean_app_build_dirs::
	@echo ""
	@echo ">>> [REALCLEAN] REMOVE APP BUILD DIRS <<<"
	@echo ""
	@- $(RM_TREES) $(THE_APPS)

#-------------------------------------------------------------------------------

cfs_realclean:: cfs_clean_core                                          \
	        cfs_clean_apps                                          \
	        cfs_clean_build_exe                                     \
	        cfs_clean_build_mission_incs                            \
	        cfs_clean_build_platform_incs                           \
	        cfs_clean_app_build_dirs                                \
	        cfs_clean_install_dirs        
	@echo ""
	@echo ">>> DONE! <<<"
	@echo ""

#===============================================================================
# DEPEND BUILD RULES
#===============================================================================

cfs_depend_core::
	@echo ""
	@echo ">>> [DEPEND] RUN CORE DEPENDENCIES <<<"
	@echo ""
	$(MAKE) -C cfe depend

#-------------------------------------------------------------------------------

cfs_depend_apps::
	@echo ""
	@echo ">>> [DEPEND] RUN APP DEPENDENCIES <<<"
	@echo ""
	@for app in $(THE_APPS); do                                     \
	    $(MAKE) -C $${app} depend                                   \
	    || exit 1 ;                                                 \
	 done

#-------------------------------------------------------------------------------

cfs_depend:: cfs_depend_core                                            \
	     cfs_depend_apps 

#===============================================================================
# Tool RULES
#===============================================================================
cfs_tool:: cfs_copy_core_incs cfs_make_tool_build_dirs
	@echo ""
	@echo ">>> [TOOL] BUILD elf2cfetbl TOOL <<<"
	@echo ""
	@$(MAKE) -C elf2cfetbl 
	@echo ""
	@echo ">>> DONE! <<<"
	@echo ""

cfs_cleantool::
	@echo ""
	@echo ">>> [CLEANTOOL] REMOVE elf2cfetbl TOOL <<<"
	@echo ""
	@- $(RM_TREES) elf2cfetbl
	@echo ""
	@echo ">>> DONE! <<<"
	@echo ""

cfs_make_tool_build_dirs::
	@echo ""
	@echo ">>> [PRE-BUILD] CREATE MISSING TOOL BUILD DIRECTORIES <<<"
	@echo ""
	@mkdir -p elf2cfetbl
	@ $(CP_MAYBE) $(CFE_TOOLS)/elf2cfetbl/for_build/Makefile elf2cfetbl/Makefile

################################################################################
# End of CFS supplemental makefile for applications
################################################################################

