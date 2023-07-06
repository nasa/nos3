##########################################################################
#
# CFE-specific configuration options for OSAL
#
# This file specifies the CFE-specific values for various compile options
# supported by OSAL.
#
# OSAL has many configuration options, which may vary depending on the
# specific version of OSAL in use.  The complete list of OSAL options,
# along with a description of each, can be found OSAL source in the file:
#
#    osal/default_config.cmake
#
# A CFE framework build utilizes mostly the OSAL default configuration.
# This file only contains a few specific overrides that tune for a debug
# environment, rather than a deployment environment.
#
# ALSO NOTE: There is also an arch-specific addendum to this file
# to allow further tuning on a per-arch basis, in the form of:
#
#    ${TOOLCHAIN_NAME}_osconfig.cmake
#
# See "native_osconfig.cmake" for options which apply only to "native" builds.
#
##########################################################################

#
# OSAL_CONFIG_DEBUG_PRINTF
# ------------------------
#
# For CFE builds this can be helpful during debugging as it will display more
# specific error messages for various OSAL error/warning events, such as if a
# module cannot be loaded or a file cannot be opened for some reason.
#
set(OSAL_CONFIG_DEBUG_PRINTF TRUE)

#
# OSAL_CONFIG_QUEUE_MAX_DEPTH
# ------------------------
#
# The maximum depth of an OSAL message queue.
# On some implementations this may affect the overall OSAL memory footprint
# so it may be beneficial to set this limit accordingly.
#
# This value has been increased from the default of 50 to support performance
# tuning for the CFDP application.
#
set (OSAL_CONFIG_QUEUE_MAX_DEPTH 512)

#
# OSAL_CONFIG_INCLUDE_SHELL
# ----------------------------------
#
# Whether to include features which utilize the operating system shell.
#
# Remote Shell commands can be very powerful tool for remotely diagnosing
# and mitigating runtime issues in the field, but also have significant
# security implications.  If this is set to "false" then shell functionality
# is disabled and OSAL functions which invoke the shell will return
# OS_ERR_NOT_IMPLEMENTED.
#
set(OSAL_CONFIG_INCLUDE_SHELL TRUE)

# The maximum number of loadable modules to support
# Note that emulating module loading for statically-linked objects also
# requires a slot in this table, as it still assigns an OSAL ID.
set(OSAL_CONFIG_MAX_MODULES 40)

