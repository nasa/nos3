# Scenario - Simulator Expansion

This scenario was developed to capture how to expand a simulator.
In this scenario, we will expand the Sample simulator to use additional data from 42.
This scenario will show how data flows through the entire system.
This data starts at 42, passes to the simulator, who talks to the component application, that publishes COSMOS telemetry.

## Learning Goals

By the end of this scenario, you should be able to:
* Turn on 42 output in the 42 window
* Enhance the Sample simulator to retrieve additional data from 42
* Enhance the Sample simulator to provide additional data to flight software
* Enhance the Sample flight software to receive additional data from the simulator and to provide additional telemetry for the additional data
* Enhance the Sample telemetry definition to view additional telemetry for the additional data
* Run NOS3 and view the additional data in the Sample telemetry

## Prerequisites

Before running the scenario, ensure the following steps are completed:
* [Getting Started](./Getting_Started.md)
  * [Installation](./Getting_Started.md#installation)
  * [Running](./Getting_Started.md#running)
* No additional file changes or special setup is needed for this scenario

## Walkthrough

### Turn on 42 output in the 42 window
Edit `cfg/sims/nos3-simulator.xml`.
Locate the "sample_sim" simulator section.  
Comment out the "SAMPLE_PROVIDER" section and uncomment the "SAMPLE_42_PROVIDER" section.

![Scenario Simulator Expansion - Sample Provider](./_static/scenario_simulator_expansion/scenario_sample_provider.png)

Edit `cfg/InOut/Inp_IPC.txt`.
Add an 11 line section to the end that is like the previous section.  Change the filename to "SAMPLE.42" and the host port to "4242" and echo to stdout to "TRUE".

![Scenario Simulator Expansion - Inp_IPC.txt](./_static/scenario_simulator_expansion/inp_ipc.png)

Change line 2 to be one more than the current value.
Run `make`, then `make launch`
The 42 window should now show 42 output.

![Scenario Simulator Expansion - 42 Output](./_static/scenario_simulator_expansion/42_output.png)

Run `make stop`.

### Enhance the Sample simulator to retrieve additional data from 42
Edit the `components/sample/sim/src/sample_data_point.cpp` file.
Add another key to extract the "SC[0].bvb" data.
Add line to get value for key.
Add parsing and saving of the "bvb" data.

![Scenario Simulator Expansion - sample_data_point.cpp](./_static/scenario_simulator_expansion/sample_data_point.cpp.png)

Add "_sample_bvb" array to the "SampleDataPoint" and add getter methods (`sample_data_point.hpp`). This file can be found `components/sample/sim/inc`.

![Scenario Simulator Expansion - sample_data_point.hpp](./_static/scenario_simulator_expansion/sample_data_point.hpp.png)

### Enhance the Sample simulator to provide additional data to flight software
Edit the `components/sample/sim/src/sample_hardware_model.cpp` file.
Add "bvb" data to the out_data in the "create_sample_data()" method. Note be sure to change the size of the out_data (out_data.resize(14, 0x00)) to 20, increasing the original value from 14. This is defined at top of the method.

![Scenario Simulator Expansion - sample_hardware_model.hpp](./_static/scenario_simulator_expansion/sample_hardware_model.cpp.png)

Run `make clean`, then `make`, and`make launch`.
The 42 window should now show 42 output and the sample sim window should show a connection to get the 42 data.
Run `make stop`.

### Enhance the Sample flight software to receive additional data from the simulator and to provide additional telemetry for the additional data
Edit the `components/sample/fsw/shared/sample_device.c` file.
Add "bvb" data reads in the "SAMPLE_RequestData()" function. Note, since we have increased our data buffer, we are changing the conditional to check read_data indices 18 and 19 in order to find the appropriate trailer message.

![Scenario Simulator Expansion - sample_device.c](./_static/scenario_simulator_expansion/sample_device.c.png)

Edit the `components/sample/fsw/shared/sample_device.h` file.
Add "bvb" members to the "SAMPLE_Device_Data_tlm_t" structure.

![Scenario Simulator Expansion - sample_device.h](./_static/scenario_simulator_expansion/sample_device.h.png)

### Enhance the Sample telemetry definition to view additional telemetry for the additional data
Edit the `components/sample/gsw/SAMPLE/cmd_tlm/SAMPLE_TLM.txt` telemetry definition file.
Add "bvb" telemetry points to the "SAMPLE_DATA_TLM" telemetry packet.

![Scenario Simulator Expansion - SAMPLE_TLM_txt](./_static/scenario_simulator_expansion/SAMPLE_TLM.txt.png)

### Run NOS3 and view the additional data in the Sample telemetry
Run `make clean`, then `make`, and`make launch`.

![Scenario Simulator Expansion - TRUTH and SAMPLE packets](./_static/scenario_simulator_expansion/TRUTH_and_SAMPLE_packets.png)
