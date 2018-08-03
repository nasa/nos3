About the "private" cFE include directory
-----------------------------------------

This directory is for include files that are used within the cFE core library 
only, including types and definitions that are shared between more than one 
cFE module.

External libraries and applications should *NOT* include anything from this 
directory.  Doing so may cause the build to break.  The reason is because
directly using definitions in here at compile-time will cause the external
entity (lib or app) to become hard-linked to the same specific cFE config
used to build the cFE - and it will surely break if that app is used with
a cFE instance that has been built with a different config (and probably 
in a non-obvious way).  

Instead, the external app or lib should use the configuration exported from
the cFE library via the proper external ABI.

Include files in here should check that _CFE_CORE_ is defined to prevent 
accidental usage by external entities.


