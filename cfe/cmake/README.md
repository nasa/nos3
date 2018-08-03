HOW TO BUILD USING CMAKE SCRIPTS
================================


This README file only covers the very basics of how to build a CFS mission
using the CMake build scripts.  For a more in-depth guide and specific
examples, see Appendix A of the cFS Deployment Guide.

Special Notes for the CFE 6.5.0 Open Source Release
---------------------------------------------------

Quick instructions for doing a CMake based build:

* Get into the directory containing psp, cfe, tools, and apps.

* Get a proper copy of OSAL per instructions in osal/readme.txt

* Delete any existing "build" tree (or rename it away).

* Copy the sample CMake configuration into the top:

	cp cfe/cmake/Makefile.sample Makefile
	cp -r cfe/cmake/sample\_defs sample\_defs

The files in "sample\_defs" will need to be adjusted to meet the
needs of your mission, but the sample values provided are sufficient
for an initial build.

* optional step: ask CMake to generate Makefiles, then stop:

	make prep

   This step is optional as it will automatically be done by
   subsequent steps. It may be usful

* optional step: compile programs for the target, then stop:

	make

   This prepares all objects for the target, but but will not copy
   them into place in the final installation location. This step is
   optional as it will automatically be done by the next step.

* normal "all in one" command: build and prepare for deployment:

	make install

   Running this command at the top level of the tree will do all the
   necessary steps to constuct the installation images as
   subtrees found in:

   * ``build/exe/host`` contains programs to be deployed to the ground
     station (or ground-station like machine).

   * ``build/exe/cpu1`` contains a file system tree suitable for
     deployment to the embedded target.


Prerequisites
-------------

The build uses 3rd party applications and libraries to provide some
base functionality.  For libraries the "-dev" (Debian/Ubuntu) or "-devel"
(RedHat/CentOS) versions of these packages should be installed in order to
compile source code using these libraries.

* gcc, make, libc-dev, etc. - Standard prerequisites for any build environment.
* cmake - The build system uses this.  Any recent version should work.
* libexpat (XML parser) - Any recent version should work.
* git - Required if you want to access the official CFS distribution directly
from the CFS community source repositories.

Mission Configuration
---------------------

Before any builds can be done, the mission configuration must be specified. This
is centralized in a directory below the top level source directory.  The
directory is identified by a "\_defs" suffix in the directory name.

The example build provides a "sample\_defs" directory that is simply an
example that can be modified to suit the project needs.  This contains:

* Global configuration file to serve as "cfe_mission_cfg.h"
* Target-specific osconfig.h files for OSAL for each different cross platform
* Startup scripts for each CPU (cfe\_es\_startup.scr)
* Any other project specific files, such as table source files or generic data
files can be placed here.

Note that the build scripts support using more than one "defs" directory within
a single project. This is intended to support different mixtures of
processor-in-the-loop and hardware-in-the-loop style debugging environments.
However, in this case, the directory cannot be automatically determined by the
build system and must be selected by the user at preparation time (see wiki for
this).



Preparing to build CFE/CFS
--------------------------

The build procedure always uses "out-of-tree" builds; all generated files
are stored in a separate directory from the source code.  This is important for
version control purposes, as the build will not modify or add any files within
the source tree, and multiple builds can be done from a single source tree.

A special ``prep`` target is implemented in the example top-level makefile to
facilitate setting up a new build directory.  This in turn calls "cmake" with
the right options to set up the build.

Special variables used by the top-level makefile wrapper:

* ``O``: Specifies the output directory, may be absolute or relative.  If not
specified then "build" is assumed.
* ``SIMULATION``: If set, this will override the architecture(s) specified in
the targets file.  The "native" keyword here will indicate to use the host
machine's native compiler (e.g. /usr/bin/gcc) for all compilation tasks.  For
any other keyword/string, the scripts will search for a file called
"toolchain-KEYWORD.cmake" that specifies the details of the toolchain.
"i686-rtems4.11", "powerpc-440-linux-gnu" and "arm-cortexa8_neon-linux-gnueabi"
are included as examples of how this works.

### Examples ###

To prepare a normal build for the target hardware:

	make prep

To prepare a workstation simulation build (this overrides cross-compilation
such that all binaries are built to run directly on the native host platform):

	make SIMULATION=native O=build-sim prep


To prepare a "processor-in-the-loop" simulation build, assuming that the target
processor toolchain is defined by "arm-cortexa8_neon-linux-gnueabi":

	make SIMULATION=arm-cortexa8_neon-linux-gnueabi O=build-pil prep


**NOTE**: For any of these builds, the ``O`` variable can be supplied or
modified to override the default output directory.  This allows all the builds
to coexist simultaneously from the same source tree.

For **ALL** builds, once the ``prep`` step has been completed, the variables
used (such as "SIMULATION") are cached and therefore **DO NOT** need to be
passed with any future invocation of the "make" commands.  This is important
because it allows the makefiles to be easily called from an IDE without needing
to set up specific environment variables first.

After this initial preparation step, CMake is not used again unless the build
scripts are modified.  However, dependencies are built into the generated
makefiles such that CMake is generally run automatically when necessary.  The
only major exception to this is if a source file is _added_ to a build script
that was using "wildcards" or shell glob commands to find source files.   In
this specific case of source file additions with no modifications to existing
build scripts, the ``prep`` step will need to be manually re-run to pick up the
new added files.


Building and Installing
-----------------------

The CFS mission binary files may be generated by simply running:

	make

The CMake build scripts will generate the binary files in a deep build tree
based on the original location of the source file from which it was built. This
is generally not suitable for copying to a target machine, so an "install"
target is also implemented to make this process easier:

	make install

Will copy all the generated executable files (typically the "core" executable
plus any loadable object files and/or configuration files specified in the
mission configuration) to a common directory.  The directory structure is
organized by CPU name so that it is easy to copy the entire tree to a target,
or the directory could be NFS mounted in the case of Linux targets.

Note that the default "all" target (which generates the binaries) is also a
prerequisite of the "install" target, so running "make install" directly is
generally sufficient to build the entire project.



