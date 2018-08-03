##
##
## Common Make Rules for building cFE core subsystems
## This file is included by the individual cFE Core subsystems ( bsp, es, etc )
##
##  

##
## The default "make" target is the subsystem object module.
## 
default::$(TARGET)

##
## Compiler rule
##
.c.o:
	$(COMPILER) $(LOCAL_COPTS) $(INCLUDE_PATH) $(COPTS) $(DEBUG_OPTS) -DOS_DEBUG_LEVEL=$(DEBUG_LEVEL) -c -o $@ $<

## 
## Assembly Code Rule
##
.s.o:
	$(COMPILER) $(LOCAL_COPTS) $(INCLUDE_PATH) $(ASOPTS) $(COPTS) $(DEBUG_OPTS) -DOS_DEBUG_LEVEL=$(DEBUG_LEVEL) -c -o $@ $<

##
## Rule to generate the dependancy list
##
depend: $(SOURCES)
	$(COMPILER) -MM $(LOCAL_COPTS) $(INCLUDE_PATH) $(COPTS_D) $(DEBUG_OPTS) -DOS_DEBUG_LEVEL=$(DEBUG_LEVEL) -c $^ > $(SUBSYS).d


##
## Subsystem Link rule
## 
$(TARGET): $(OBJS)
	$(LINKER) -o $(TARGET) -r $(OBJS)

##
## Make clean rule
##
clean:
	-$(RM) $(OBJS) $(TARGET)
	-$(RM) *.lis
	-$(RM) *.g*
	-$(RM) *.d

cleandocs:
	-$(RM) ../../docs/users_guide/html/cfe/*.html
	-$(RM) ../../docs/users_guide/html/cfe/*.css
	-$(RM) ../../docs/users_guide/html/cfe/*.png
	-$(RM) ../../docs/users_guide/html/cfe/*.dot
	-$(RM) ../../docs/users_guide/html/cfe/*.gif
	-$(RM) ../../docs/users_guide/latex/cfe/*.tex
	-$(RM) ../../docs/users_guide/latex/cfe/*.dot
	-$(RM) ../../docs/users_guide/latex/cfe/*.sty
	-$(RM) ../../docs/users_guide/latex/cfe/*.ttf
	-$(RM) ../../docs/users_guide/latex/cfe/Makefile
	-$(RM) ../../docs/detailed_design/html/cfe/*.html
	-$(RM) ../../docs/detailed_design/html/cfe/*.css
	-$(RM) ../../docs/detailed_design/html/cfe/*.png
	-$(RM) ../../docs/detailed_design/html/cfe/*.dot
	-$(RM) ../../docs/detailed_design/html/cfe/*.gif
	-$(RM) ../../docs/detailed_design/latex/cfe/*.tex
	-$(RM) ../../docs/detailed_design/latex/cfe/*.dot
	-$(RM) ../../docs/detailed_design/latex/cfe/*.sty
	-$(RM) ../../docs/detailed_design/latex/cfe/*.ttf
	-$(RM) ../../docs/detailed_design/latex/cfe/Makefile

#
# Include the dependancy list
#
-include $(SUBSYS).d

