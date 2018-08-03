###############################################################################
# File: table-rules.mak
#
# Purpose: cFE table make rules. This file contains common make rules to
#          be included by cFE table make files. 
#
# History:
#
###############################################################################
.SUFFIXES:
.SUFFIXES:.c .C .o .O .tbl .TBL

default::$(TABLES)

#
# Default table rule
# The absolute path is not ideal, but this prevents the 
# incorrect version of the utility from being used.
#
.o.tbl:$(OBJS)
	../$(TABLE_BIN)/$(TABLE_BIN) $<

##
## Default Compiler rule
##
.c.o:$(SOURCES)
	$(COMPILER) $(LOCAL_COPTS) $(INCLUDE_PATH) $(COPTS) $(TABLE_DEBUG_OPTS) -c -o $@ $<

##
## Default rule for installing app
##
install:
	$(CP) $(TABLES) $(EXEDIR)

##
## Rule for cleaning
## All of the possible extentions are included here so we can clean up after
## an old build
##
clean::
	-$(RM) *.o
	-$(RM) *.tbl
	-$(RM) *.lis
