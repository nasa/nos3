This directory holds the unit tests for the CI application.

To build and run the unit tests:
1. Be sure an appropriate *_ci_types.h is in the apps/inc directory by:
  a. cd ../examples
  b. ./setup.sh -m CFS_TST udp     (see below)
  c. cd ../unit_test
2. Do the same for TO.  Be sure an appropriate *_to_types.h is in the 
   apps/inc directory by:
  a. cd ../../../to/fsw/examples
  b. ./setup.sh -m CFS_TST udp     (see below)
  c. cd ../../../ci/fsw/unit_test
3. make clean
4. make
5. make run
6. make gcov

Background:
The unit tests also expect (like the apps) to find the 
 apps/inc/CFS_TST_ci_types.h
 apps/inc/CFS_TST_to_types.h
where the mission name, CFS_TST, is assumed by default.

These are put into place by the [ci/to]/fsw/examples/setup.py scripts.
Choose the appropriate name for your code to compile if you aren't 
using "CFS_TST".
