This is the preliminary documentation for the JSC SBN application. The
application relies on a library one or more back-end networking modules
loaded at runtime by the SBN application.

In order to build using the CMake system, ensure:

1) copy sbn/fsw/src/SbnModuleData.dat to your defs folder as
	cpu<n>_SbnModuleData.dat, and edit as appropriate
2) copy sbn/fsw/src/SbnPeerData.dat to your defs folder as
	cpu<n>_SbnPeerData.dat and edit as appropriate
3) edit targets.cmake:
	a) TGT<n>_APPLIST to contain something like "sbn_lib sbn ipv4"
	b) TGT<n>_FILELIST to also contain "SbnModuleData.dat SbnPeerData.dat"
