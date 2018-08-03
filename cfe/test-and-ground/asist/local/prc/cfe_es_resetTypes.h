#ifndef _cfe_es_resetTypes_
#define _cfe_es_resetTypes_

/*
**  Reset types
*/
#define CFE_ES_PROCESSOR_RESET   1       /*Volatile disk, Critical Data Store and User Reserved memory could still be valid */
#define CFE_ES_POWERON_RESET     2       /*All memory has been cleared */
#define CFE_ES_APP_RESTART       3       /*Application only was reset */

/*
** Reset Sub-Types
*/
#define CFE_ES_POWER_CYCLE           1   /*Reset caused by power having been removed and restored */
#define CFE_ES_PUSH_BUTTON           2   /*Reset caused by reset button on the board having been pressed */
#define CFE_ES_HW_SPECIAL_COMMAND    3   /*Reset was caused by a reset line having been stimulated by a hardware special command */
#define CFE_ES_HW_WATCHDOG           4   /*Reset was caused by a watchdog timer expiring */
#define CFE_ES_RESET_COMMAND         5   /*Reset was caused by cFE ES processing a Reset Command */
#define CFE_ES_EXCEPTION             6   /*Reset was caused by a Processor Exception */
#define CFE_ES_UNDEFINED_RESET       7   /*Reset was caused in an unknown manner */
#define CFE_ES_HWDEBUG_RESET         8   /*Reset was caused by a JTAG or BDM connection */
#define CFE_ES_BANKSWITCH_RESET      9   /*Reset reverted to a cFE POWERON due to a boot bank switch */

#endif  /* _cde_es_resetTypes_ */
