
#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "spacecop_table_defs.h"

RuleTable SPACECOP_RuleTable =
{
    .num_rules = 22,
    .rules = 
    {
       /*
       *      Rule 1: GNTM-6 Unexpected Time Delta
       *      STIX Pattern: [x-opencti-system:component = 'time_controller' AND x-opencti-time:delta_value != 'expected_delta_value']
       *      Tolerance is in seconds
       */
       {
              .enabled = 1,
              .iob_id = "GNTM-6",
              .mode = RULE_PERIODIC, //Run every tick
              .period_ticks = 3, //Limit one run every 3 seconds
              .call_index = 0, //look in spacecop_registry.c for index
              .lhs = { .kind = SRC_CALL_NEW, .type = VT_U32 },
              .op = OP_ABS_DIFF_GT,
              .rhs = { .kind = SRC_CALL_OLD, .type = VT_U32 },
              .tol = { .type = VT_U32, .v = { .u32 = 5 }},
       },
       /*
       *      Rule 2: SMSR-2 High CPU Utilization Due to Anomalous/Malicious Activity 
       *      STIX Pattern: [x-opencti-processor-usage:cpu_load > 'threshold' AND x-opencti-processor-usage:activity_type = 'unexpected' AND x-opencti-processor-usage:process_name NOT IN ('list_of_known_processes')]
       *      Tolerance is in percent
       */
       {
              .enabled = 1,
              .iob_id = "SMSR-2", 
              .mode = RULE_TRIGGER,
              .period_ticks = 10,
              .call_index = 1, //look in spacecop_registry.c for index
              .tol = { .type = VT_F32, .v = { .f32 = 40.0f }},
       },
       /*
       *      Rule 3: SIUU-11 Suspicious Binary or Script Execution
       *      STIX Pattern: [process:image_ref.name != 'expected_binary_or_script']
       */
       {
              .enabled = 1,
              .iob_id = "SIUU-11", 
              .mode = RULE_TRIGGER,
              .period_ticks = 5,
              .call_index = 2, //look in spacecop_registry.c for index
       },
       /*
       *      Rule 4: DISE-1: File or Data Integrity Check Failure
       *      STIX Pattern: [file:hashes != 'expected_hash_value' AND file:name = 'data_file']
       */
       {
              .enabled = 1,
              .iob_id = "DISE-1", 
              .mode = RULE_TRIGGER,
              .period_ticks = 5,
              .call_index = 3, //look in spacecop_registry.c for index
       },
       /*
       *      NOS3 ENVIRONMENT LIMITATION -- Rules 5 through 10 are disabled.
       *
       *      These six rules all use call_index 4 (syscall-monitor), which is fed
       *      by the aerospace.ko kernel module over a Netlink socket. The module
       *      creates that socket with netlink_kernel_create(&init_net, ...), so it
       *      only exists in the host initial network namespace. Under NOS3 the
       *      flight software runs in a container attached to a Docker bridge
       *      network, which is a separate namespace, so the module and the app
       *      cannot reach each other no matter where the module is loaded.
       *
       *      Loading the module inside the container is not an option either:
       *      containers share the host kernel, and the FSW container is given only
       *      --cap-add=sys_nice, not CAP_SYS_MODULE.
       *
       *      This fails silently, which is why the rules are disabled rather than
       *      left on. module_loaded_proc() reads /proc/modules, which reports the
       *      HOST module list even from inside a container, so the check passes.
       *      The Netlink socket() and bind() both succeed in the container
       *      namespace. No events ever arrive and nothing reports an error, so
       *      leaving these enabled means an IDS that reports all-clear on six
       *      detections that cannot physically fire.
       *
       *      This is a limitation of the NOS3 simulation environment, not of
       *      SpaceCOP. On flight hardware there is no container and no namespace
       *      boundary, and these rules work as designed. Re-enable them for any
       *      target where the flight software shares the kernel namespace with
       *      the module.
       */
       /*
       *      Rule 5: CSNE-0017: Creation of FIFO for Data Exfiltration or Command Injection
       *      STIX Pattern: [process:image_ref.name = 'mkfifo' AND process:x_execution_time = 'unexpected_time']
       */
       {
              .enabled = 0,
              .iob_id = "CSNE-17", 
              .mode = RULE_ONEANDDONE,
              .call_index = 4,
       },
       /*
       *      Rule 6: SIUU-0015: Repeated File Access to /zero or /null Devices
       *      STIX Pattern: [file:path = '/dev/null' OR file:path = '/dev/zero' AND file:access_time != 'expected_time']
       *      Tolerance: 10 times before reporting
       */
       {
              .enabled = 0,
              .iob_id = "SIUU-15", 
              .mode = RULE_ONEANDDONE,
              .call_index = 4,     
              .tol = { .type = VT_I32, .v = { .i32 = 100 }}
       },
       /*
       *      Rule 7: SIUU-0012: Loading of Malicious Kernel Modules
       *      STIX Pattern: [process:image_ref.name = 'insmod' OR process:image_ref.name = 'init_module']
       */
       {
              .enabled = 0,
              .iob_id = "SIUU-12", 
              .mode = RULE_ONEANDDONE,
              .call_index = 4,     
       },
       /*
       *      Rule 8: SIUU-0010: Process Executing Priority Modification
       *      STIX Pattern: [process:image_ref.name = 'renice' OR process:image_ref.name = 'setpriority' AND process:x_execution_time != 'authorized_period']
       */
       {
              .enabled = 0,
              .iob_id = "SIUU-10", 
              .mode = RULE_ONEANDDONE,
              .call_index = 4,     
       },
       /*
       *      Rule 9: SIUU-0016: Execution of System Commands
       *      STIX Pattern: [process:image_ref.name = 'grep' OR process:image_ref.name = 'ps' OR process:image_ref.name = 'awk' OR process:image_ref.name = 'chmod' OR process:image_ref.name = 'dd' OR process:image_ref.name = 'cat' OR process:image_ref.name = 'sh' AND process:x_execution_time != 'authorized_time']
       */
       {
              .enabled = 0,
              .iob_id = "SIUU-16", 
              .mode = RULE_ONEANDDONE,
              .call_index = 4,     
       },
       /*
       *      Rule 10: MIRE-0017: Unauthorized System Call to Open Flash Memory Blocks (/dev/mtd)
       *      STIX Pattern: [process:image_ref.name = 'open' AND file:path LIKE '/dev/mtd%' AND file:access_time != 'authorized_access_time']
       */
       {
              .enabled = 0,
              .iob_id = "MIRE-17", 
              .mode = RULE_ONEANDDONE,
              .call_index = 4,     
       },
       /*
       *      Rule 11: MIRE-14: Abnormal Memory Consumption by Malicious Process
       *      STIX Pattern: [process:x_memory_usage > 'threshold' AND process:image_ref.name != 'authorized_process']
       *      Threshold is in kilobytes
       */
       {
              .enabled = 1,
              .iob_id = "MIRE-14", 
              .mode = RULE_TRIGGER,
              .period_ticks = 10,
              .call_index = 5, //look in spacecop_registry.c for index
              .tol = { .type = VT_U32, .v = { .u32 = 1000000 }},
       },
       /*
       *        Rule 12: UACE-0017: Abnormal Burn Duration Detected in Propulsion Subsystem
       *        STIX Pattern: [x-opencti-propulsion-system:burn_duration > 'expected_max_duration' OR x-opencti-propulsion-system:burn_duration < 'expected_min_duration']
       *        Threshold is in seconds
       */
       {
                .enabled = 1,
                .iob_id = "UACE-17",
                .mode = RULE_PERIODIC,
                .period_ticks = 3,
                .call_index = 6,
                .lhs = { .kind = SRC_CALL_NEW, .type = VT_U32 },
                .rhs = { .kind = SRC_TOL, .type = VT_U32 },
                .tol = { .type = VT_U32, .v = { .u32 = 3  }},
                .op = OP_GT,
       },
       /*
       *      Rule 13: DISE-0004: Storage Exhaustion (Disk Full)
       *      STIX Pattern: [x-opencti-file-system:available_space < 'threshold']
       *      Threshold is in percent
       */
       {
              .enabled = 1,
              .iob_id = "DISE-4", 
              .mode = RULE_TRIGGER,
              .period_ticks = 10,
              .call_index = 7, //look in spacecop_registry.c for index
              .tol = { .type = VT_F32, .v = { .f32 = 20.0f }},
       },
       /*
       *      Rule 14: GNTM-0007: Time Adjustment Commands Detected
       *      STIX Pattern: [x-opencti-system:component = 'time_controller' AND x-opencti-command:command = 'adjust_time' AND x-opencti-command:execution_count > 'threshold']
       *      Threshold is in seconds
       */
       {
              .enabled = 1,
              .iob_id = "GNTM-7",
              .mode = RULE_PERIODIC,
              .period_ticks = 3,
              .call_index = 8,
              .lhs = { .kind = SRC_CALL_NEW, .type = VT_U32 },
              .rhs = { .kind = SRC_TOL, .type = VT_U32 },
              .tol = { .type = VT_U32, .v = { .u32 = 3  }},
              .op = OP_GT,
       },
       /*
       *      Rule 15: MIRE-15: Unexpected Modification of Memory Location Associated with Payload Data
       *      STIX Pattern: [x-opencti-memory:block = 'payload_memory_block' AND x-opencti-memory:write_operation = 'unexpected']
       *      Check Interval: Every 10 ticks during normal ops
       */
       {
              .enabled = 1,
              .iob_id = "MIRE-15", 
              .mode = RULE_TRIGGER,
              .period_ticks = 10,
              .call_index = 9, //look in spacecop_registry.c for index
       },
        /*
       *      Rule 16: MIRE-16: Unexpected Access and Changes in Boot Memory Region
       *      STIX Pattern: [x-opencti-memory:block = 'boot' AND x-opencti-memory-log:block = 'boot' AND x-opencti-memory-log:status != 'expected']
       *      Check Interval: Every 5 ticks (5 seconds) during steady state - boot memory should be static
       */
       {
              .enabled = 1,
              .iob_id = "MIRE-16", 
              .mode = RULE_TRIGGER,
              .period_ticks = 5,     /* Check every 5 ticks (5 seconds) - boot memory rarely changes */
              .call_index = 10,        /* memory-monitor-boot */
       },
       /*
       *      Rule 17: MIRE-18: Error Detection Status Failed in Critical Memory Regions
       *      STIX Pattern: [x-opencti-memory-log:error_detection_status = 'failed' AND x-opencti-memory-log:memory_region IN ('critical_region_1','critical_region_2')]
       *      Check Interval: Every 5 ticks 
       */
       {
              .enabled = 1,
              .iob_id = "MIRE-18", 
              .mode = RULE_TRIGGER,
              .period_ticks = 5,      /* Check every 5 ticks for radiation-induced errors */
              .call_index = 11,        /* memory-monitor-critical */
       },
       /*
       *      Rule 18: CSNE-41: High Latency Detected in Downlink Communication
       *      STIX Pattern: [network-traffic:latency > 'acceptable_latency_threshold' AND network-traffic:direction = 'downlink']
       *      Acceptable latency 5s
       */
       {
              .enabled = 1,
              .iob_id = "CSNE-41",
              .mode = RULE_PERIODIC,
              .period_ticks = 3,
              .call_index = 12,
              .lhs = { .kind = SRC_CALL_NEW, .type = VT_U32 },
              .rhs = { .kind = SRC_TOL, .type = VT_U32 },
              .tol = { .type = VT_U32, .v = { .u32 = 5  }},
              .op = OP_GT,
       },
       /*
       *      ML RULES DISABLED -- Rules 19 through 21.
       *
       *      These three rules use RULE_ML / call_index 13 (ml-monitor), which
       *      scores packets forwarded to SpaceCOP ML over port 9111. Every
       *      command monitor entry carrying enableML=1 has been removed from
       *      spacecop_command_monitor_table.c, so nothing reaches the ML pipe
       *      and these rules have no input. They are disabled rather than left
       *      enabled so they do not report as active while unable to fire.
       *
       *      scml_models/CMD and scml_models/TLM are also empty: no models have
       *      been trained, so the rules could not score anything even with a
       *      feed restored.
       *
       *      Pending guidance from the SpaceCOP developers on how the ML side is
       *      intended to be fed and trained under NOS3.
       */
       /*
       *      Rule 19: UACE-16: Irregular Orbit Maneuver Commands Detected on Attitude Control
       *      STIX Pattern: [x-opencti-command:observable_type = 'adcs-command' AND x-opencti-command:value != 'expected_orbit_maneuver_commands']
       */
       {
              .enabled = 0,
              .iob_id = "UACE-16",
              .mode = RULE_ML,
              .call_index = 13,
       },
      /*
       *      Rule 20: UACE-9: Valid Command Flooding
       *      STIX Pattern: [x-opencti-command-data:command_type = 'satellite_vehicle_command' AND x-opencti-command-data:command_frequency > 'expected_rate' AND x-opencti-command-data:source = 'external' AND x-opencti-command-data:command_validity = 'valid']
       */
       {
              .enabled = 0,
              .iob_id = "UACE-9",
              .mode = RULE_ML,
              .call_index = 13,
       },
       /*
       *      Rule 21: UACE-7: Duplicate Command Packet Executions
       *      STIX Pattern: [x-opencti-command-log:command_id = 'duplicate' AND x-opencti-command-log:timestamp = 'unexpected_time']
       */
       {
              .enabled = 0,
              .iob_id = "UACE-7",
              .mode = RULE_ML,
              .call_index = 13,
       },
       /*
       *      Rule 22: SMSR-3: Unauthorized State Changes in Critical Sensors
       *      STIX Pattern: [x-opencti-sensor-log:state_change = 'unauthorized' AND x-opencti-sensor-log:sensor_type = 'critical']
       */
       {
              .enabled = 1,
              .iob_id = "SMSR-3",
              .mode = RULE_ONEANDDONE,
              .call_index = 14,
       },
    }
};


/* Macro for table structure */
CFE_TBL_FILEDEF(SPACECOP_RuleTable, SPACECOP.SC_RuleTbl, SPACECOP Rule Table, spacecop_rule.tbl)
