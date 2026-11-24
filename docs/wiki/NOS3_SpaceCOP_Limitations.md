# SpaceCOP Limitations in the NOS3 Environment

SpaceCOP is an onboard intrusion detection system that runs as a cFS application.
Most of its detection rules work unchanged under NOS3, however one group is unable to run effectively. This is due to running in a simulated envrionment for FSW with Docker Containers vs executing FSW natively on hardware and is detailed in this document.

## Disabled: syscall monitoring (6 SPARTA IOBs)

The following rules in `components/spacecop/fsw/cfs/table/spacecop_call_table.c`
are defined with `.enabled = 0`:

| Rule | IOB | Description |
| --- | --- | --- |
| 5 | CSNE-17 | Creation of FIFO for Data Exfiltration or Command Injection |
| 6 | SIUU-15 | Repeated File Access to /zero or /null Devices |
| 7 | SIUU-12 | Loading of Malicious Kernel Modules |
| 8 | SIUU-10 | Process Executing Priority Modification |
| 9 | SIUU-16 | Execution of System Commands |
| 10 | MIRE-17 | Unauthorized System Call to Open Flash Memory Blocks (/dev/mtd) |

All six use `call_index = 4` (`syscall-monitor`), the only detection function
that depends on the `aerospace.ko` kernel module.

## Why they cannot work

Kernel modules cannot be loaded specifically in a single docker container.
Containers share the host kernel — there is no per-container kernel to load into.
Therefore, to run the module, it would have to be loaded on the host machine and would
send a detection for every SYSCALL exectued on the host, not specifically in the docker container.

## Why they are disabled rather than left enabled

The failure is silent in both directions:

- `module_loaded_proc()` reads `/proc/modules`, which reports the **host's**
  module list even from inside a container. The "is the module loaded" check
  therefore passes.
- `socket(PF_NETLINK, SOCK_RAW, NETLINK_USER)` succeeds; Netlink sockets can be
  created in any namespace.
- `bind()` succeeds.
- No events ever arrive, and nothing logs an error.

Left enabled, these rules would report as active and never fire. An IDS that
reports all-clear on detections that cannot physically occur is worse than one
that declares the gap, so they are disabled explicitly.

## Future or SpaceCOP with NOS3

On flight hardware there is no container and no namespace boundary between the
module and the flight software, and these rules operate as designed. Re-enable
them for any target where the flight software shares a network namespace with
the kernel module. Currntly, a nos3 user would need to Split FSW to run separately on its own host and network to the rest of the NOS3 environment accordingly in order to utlize the disabled IOBs.

The NOS3 team plans to create NOS3 configuration that exectues FSW natively on the host if the user would like to execute the specific disable IOBs by default. 

