###############################################################################
# File: app-rules.mak
#
# Purpose: cFE Application make rules. This file contains common make rules to
#          be included by cFE application make files. 
#
#
# History:
#
###############################################################################

##
## Default rule for building the App Loadable file
##
default: $(APPTARGET).$(APP_EXT)

##
## Default Compiler rule
##
.c.o:
	$(COMPILER) $(LOCAL_COPTS) $(INCLUDE_PATH) $(COPTS) $(DEBUG_OPTS) -DOS_DEBUG_LEVEL=$(DEBUG_LEVEL) -c -o $@ $<

## 
## Default Assembly Code Rule
##
.s.o:
	$(COMPILER) $(LOCAL_COPTS) $(INCLUDE_PATH) $(ASOPTS) $(COPTS) $(DEBUG_OPTS) -DOS_DEBUG_LEVEL=$(DEBUG_LEVEL) -c -o $@ $<

##
## Rule to generate the dependancy list
##
depend: $(SOURCES)
	$(COMPILER) -MM $(LOCAL_COPTS) $(INCLUDE_PATH) $(COPTS_D) $(DEBUG_OPTS) -DOS_DEBUG_LEVEL=$(DEBUG_LEVEL) -c $^ > $(APPTARGET).d

##
## Include the rule for linking the cFE application.
## This is architecture/compiler/os specific, so it must be included from the ARCH directory
##
include $(CFE_PSP_SRC)/$(PSP)/make/link-rules.mak

##
## Default rule for installing app
##
install:
	$(CP) $(APPTARGET).$(APP_EXT) $(EXEDIR)

installdocs:
	-mkdir -p $(CFS_MISSION)/docs/users_guide/html/$(CPUNAME)/$(APPTARGET)/
	-mkdir -p $(CFS_MISSION)/docs/users_guide/latex/$(CPUNAME)/$(APPTARGET)/
	-$(CP) ../docs/users_guide/html/$(APPTARGET)/*.*      $(CFS_MISSION)/docs/users_guide/html/$(CPUNAME)/$(APPTARGET)/
	-$(CP) ../docs/users_guide/latex/$(APPTARGET)/*.*     $(CFS_MISSION)/docs/users_guide/latex/$(CPUNAME)/$(APPTARGET)/
	-mkdir -p $(CFS_MISSION)/docs/detailed_design/html/$(CPUNAME)/$(APPTARGET)/
	-mkdir -p $(CFS_MISSION)/docs/detailed_design/latex/$(CPUNAME)/$(APPTARGET)/
	-$(CP) ../docs/detailed_design/html/$(APPTARGET)/*.*  $(CFS_MISSION)/docs/detailed_design/html/$(CPUNAME)/$(APPTARGET)/
	-$(CP) ../docs/detailed_design/latex/$(APPTARGET)/*.* $(CFS_MISSION)/docs/detailed_design/latex/$(CPUNAME)/$(APPTARGET)/

##
## Default rule for generating a users guide
##
usersguide::
	-mkdir -p ../docs/users_guide/html/$(APPTARGET)/
	-mkdir -p ../docs/users_guide/latex/$(APPTARGET)/
	doxygen user_doxy
	
##
## Default rule for generating a detailed design documentation
##
detaileddocs::
	-mkdir -p ../docs/detailed_design/html/$(APPTARGET)/
	-mkdir -p ../docs/detailed_design/latex/$(APPTARGET)/
	doxygen detail_doxy
	
##
## Default rule for generating code walk-through documentation
##
codewalkdocs::
	sh ../docs/dox_src/get_date.sh
	-mkdir -p $(CFS_APP_SRC)/$(APPTARGET)/docs/cwt/html
	doxygen codewalk_doxy
	

##
## Rule for cleaning
## All of the possible extentions are included here so we can clean up after
## an old build
##
clean::
	-$(RM) *.o
	-$(RM) *.g*
	-$(RM) *.lis
	-$(RM) *.bundle
	-$(RM) *.so
	-$(RM) *.dll
	-$(RM) *.d
	-$(RM) *.tbl

cleandocs::
	-$(RM) ../docs/users_guide/html/$(APPTARGET)/*.html
	-$(RM) ../docs/users_guide/html/$(APPTARGET)/*.css
	-$(RM) ../docs/users_guide/html/$(APPTARGET)/*.png
	-$(RM) ../docs/users_guide/html/$(APPTARGET)/*.dot
	-$(RM) ../docs/users_guide/html/$(APPTARGET)/*.gif
	-$(RM) ../docs/users_guide/latex/$(APPTARGET)/*.tex
	-$(RM) ../docs/users_guide/latex/$(APPTARGET)/*.dot
	-$(RM) ../docs/users_guide/latex/$(APPTARGET)/*.sty
	-$(RM) ../docs/users_guide/latex/$(APPTARGET)/*.ttf
	-$(RM) ../docs/users_guide/latex/$(APPTARGET)/Makefile
	-$(RM) ../docs/detailed_design/html/$(APPTARGET)/*.html
	-$(RM) ../docs/detailed_design/html/$(APPTARGET)/*.css
	-$(RM) ../docs/detailed_design/html/$(APPTARGET)/*.png
	-$(RM) ../docs/detailed_design/html/$(APPTARGET)/*.dot
	-$(RM) ../docs/detailed_design/html/$(APPTARGET)/*.gif
	-$(RM) ../docs/detailed_design/latex/$(APPTARGET)/*.tex
	-$(RM) ../docs/detailed_design/latex/$(APPTARGET)/*.dot
	-$(RM) ../docs/detailed_design/latex/$(APPTARGET)/*.sty
	-$(RM) ../docs/detailed_design/latex/$(APPTARGET)/*.ttf
	-$(RM) ../docs/detailed_design/latex/$(APPTARGET)/Makefile

#
# Include the dependancy list
#
-include $(APPTARGET).d


# eof
