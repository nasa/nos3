Applications to operate with cFE release 6.6.0
Started from the tag: apps-rel-6.5.0

Apps from apps-rel-6.5.0 that did not build for cFS 6.6.0 are listed below followed by the fix
ci_lab     - copied from ci_lab-for-cfe-660 branch
sch        - copied from sch-for-cfe-660 branch
           - added HTONS to table to compile for linux in sample defs
cf         - copied from cf-for-cfe-660 branch
           - updated CMake to build the PRI objects as static lib
cs         - copied from cs-for-cfe-660 branch
hs         - copied from hs-for-cfe-660 branch
lc         - copied from lc-for-cfe-660 branch
ds         - added include to ds_verify.h
hk         - added include to hs_verify.h
sc         - payload->parameter struct fixed in sc_cmds.c
ci         - updated CMake to build properly
to         - updated CMake to build properly
io_lib     - updated CMake to build properly

Generic Fixes:
- Some applications still used the old location for tables "cf/apps/<tbl>.tbl", this is changed to "cf/<tbl>.tbl in all relevant locations
- Added the cfe table build macros to CMakeLists where needed