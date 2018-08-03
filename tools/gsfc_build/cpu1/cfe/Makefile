
all:
	make -C osal 
	make -C psp 
	make -C es 
	make -C sb 
	make -C evs 
ifneq ($(EXCLUDE_CFE_TBL), TRUE)
	make -C tbl
endif
	make -C time 
	make -C fs 
	make -C link 

clean:
	make -C osal clean
	make -C psp clean
	make -C es  clean
	make -C sb  clean
	make -C evs clean
ifneq ($(EXCLUDE_CFE_TBL), TRUE)
	make -C tbl clean
endif
	make -C time clean
	make -C fs clean
	make -C link clean

# Since cFE documentation is treated as one unit,
# the clean rule for one subsystem is sufficient
# to clean all cFE documentation.
cleandocs:
	make -C es cleandocs

depend:
	make -C osal depend
	make -C psp depend
	make -C es  depend
	make -C sb  depend
	make -C evs depend
ifneq ($(EXCLUDE_CFE_TBL), TRUE)
	make -C tbl depend
endif
	make -C time depend
	make -C fs depend

install:
	make -C link install
	
installdocs:
	-mkdir -p $(CFS_MISSION)/docs/users_guide/html/$(CPUNAME)/cfe/
	-mkdir -p $(CFS_MISSION)/docs/users_guide/latex/$(CPUNAME)/cfe/
	-cp ../docs/users_guide/html/cfe/*.*      $(CFS_MISSION)/docs/users_guide/html/$(CPUNAME)/cfe/
	-cp ../docs/users_guide/latex/cfe/*.*     $(CFS_MISSION)/docs/users_guide/latex/$(CPUNAME)/cfe/
	-mkdir -p $(CFS_MISSION)/docs/detailed_design/html/$(CPUNAME)/cfe/
	-mkdir -p $(CFS_MISSION)/docs/detailed_design/latex/$(CPUNAME)/cfe/
	-cp ../docs/detailed_design/html/cfe/*.*  $(CFS_MISSION)/docs/detailed_design/html/$(CPUNAME)/cfe/
	-cp ../docs/detailed_design/latex/cfe/*.* $(CFS_MISSION)/docs/detailed_design/latex/$(CPUNAME)/cfe/

usersguide:
	-mkdir -p ../docs/users_guide/html/cfe/
	-mkdir -p ../docs/users_guide/latex/cfe/
	doxygen user_doxy

detaileddocs:
	-mkdir -p ../docs/detailed_design/html/cfe/
	-mkdir -p ../docs/detailed_design/latex/cfe/
	doxygen detail_doxy
	
