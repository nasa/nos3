# Scenario - GPS Spoofing

This scenario was developed to prove that sensor level spoofing, without modifying flight software, was possible using NOS3.

This functionality allows the user to spoof GPS data on-demand and develops the layout and thought process that may be followed for spoofing other sensors. The goal of this scenario is to trick the spacecraft into entering science mode while it is not over CONUS, AK, or HI.

This scenario was last updated on 1/6/2026 and leverages the `gps-sensor-spoofing` branch [1bb73b1]. We currently have no plans of merging this into our production code.

## Learning Goals

By the end of this scenario, you should be able to:

* Spoof data at the sensor level within the GPS hardware model.
* Understand why and how this spoofing works.

## Prerequisites

* [Getting Started](./NOS3_Getting_Started.md)
  * [Installation](./NOS3_Getting_Started.md#installation)
  * [Running](./NOS3_Getting_Started.md#running)
* [Scenario - Demonstration](./Scenario_Demo.md)
* **IMPORTANT:** Checkout the `gps-sensor-spoofing` branch
  * May require running `git submodule sync` and `git submodule update --init --recursive` after

## Walkthrough

In short, you will:

1. send `MGR MGR_SET_MODE_CC` with `SPACECRAFT_MODE SCIENCE`
2. send `MGR MGR_SET_CONUS_CC` with `CONUS_STATUS ENABLE`
3. send `SIM_CMDBUS_BRIDGE NOVATEL_OEM615_SIM_TOGGLE_SPOOF` with default parameters

This will enable a predefined data point to be read by the hardware model. That data point will propogate to the GPS SIM, and will be visible in telemetry (`NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM`).

![GPS Data Point](./_static/scenario_gps_spoofing/spoof_data_point.png)

This data point is located within the parameters for the spacecraft to enter science mode. While the spoofing is enabled, the spacecraft will continuously perform science mode operations no matter where it is in orbit, despite only being enabled for CONUS.

![Science Mode Map](./_static/scenario_gps_spoofing/science_mode_map.png)

### How to Replicate

First connect the GPS Sim to SIM_CMDBUS with the following in `/cfg/sims/nos3-simulator.xml`:

![Connect Sim to CMD Bus](./_static/scenario_gps_spoofing/spoof_simulator_file_code.png)

And then add a `command_callback` function and a global `_spoof` variable to the hardware model that can enable, disable, and spoof the GPS:

![GPS Command Callback](./_static/scenario_gps_spoofing/spoof_hardware_model_cmdcallback.png)

When the `SIM_CMDBUS_BRIDGE NOVATEL_OEM615_SIM_TOGGLE_SPOOF` command is sent, the `_spoof` variable is toggled. This instructs the hardware model to use the predefined data point in the Walkthrough section above.

![Spoofing Command](./_static/scenario_gps_spoofing/spoof_cmd_def.png)

When the GPS simulator requests data from the sensors, it will recieve the spoofed data point instead of real-time data. This method can be applied to virtually any hardware model in the NOS3 environment.

Try your own and let us know in the [GitHub Discussions](https://github.com/nasa/nos3/discussions)!
