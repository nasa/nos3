# Generating a New Component

NOS3 is organized around the foundational concept of a component.
The intent is that a spacecraft be made up of a core set of common functionality and then a custom set of components.  
Each component is represented by a FSW application which is placed in an `fsw` subdirectory of the component.
In order for the ground software to control the component application, a component has a collection of command and telemetry tables which are placed in a `gsw` subdirectory of the component.
In many cases (but not all), a component is a hardware component on the spacecraft and thus it is appropriate to have a NOS3 hardware simulator for the component which is placed in a `sims` subdirectory of the component.

Below is a step by step guide on how to generate a new component. Note that this guide follows the Sample component created in NOS3. It is up to the user to implement their own cFS app, and specific hardware model as they see fit for their application. The steps below are to guide a new user through the integration process of integrating a new component in NOS3.

## Step-by-Step Guide

**Note, this guide is for creating a new component in NOS3 using cFS FSW and Cosmos GSW.**

1. First, the user will want to run the generate_template.sh script found in the sample component. This script will create a new component with the name supplied by the user in the /nos3/components directory. To execute the script, open a terminal in your nos3 repo and change directory to components/sample/ and execute as seen below. You will need to specify the name of your new component. Note, if the script is not executable simply enter the following command:
```
chmod +x generate_template.sh
```
   ![generate_template](./_static/adding_nos3_component/generate_template.png)

After creating your component, you will see a folder created with the name you supplied in the script in /nos3/components. Here are the component files you will need to edit for your specific component. The next steps will detail what is needed to add your component to cFS, and cosmos. The component created is identical to the sample component, so for the remainder of this guide we will be exploring the code in the sample component necessary to add/modify in order for your newly created component to integrate into nos3. So, in each step if you see "sample" simply replace with the name of your new component.

Fidelity to the component is ultimately left up to the user, and "TODO" statements are called out in the component files for what the user needs to modify that may be specific to the new component. For examples of different levels fidelity, you can compare and contrast in between the various generic_components, Arducam, Novatel GPS, etc in NOS3.

2. You will need to start editing your component files to be unique. Starting with editing your cFS app, which can be found in **nos3/compoments/name_of_your_component/fsw/cfs**. First, you will need to modify the perfid value in **name_of_your_component_perfids.h** to be different than what is already defined in NOS3. For reference to what is already defined refer to the Component Information found previously in this documentation. 

    ![perf_ids](./_static/adding_nos3_component/perf_ids.png)

3. After changing the perf_ID, you will need to modify your message ids of your component's commands. These can be found in **name_of_your_component_msgids.h**. Note, for the purpose of this tutorial we are not deviating from the sample component fidelity so the commands themselves are not changing. For reference to what msg_ids are already defined refer to the Component Information found previously in this documentation.

    ![msg_ids](./_static/adding_nos3_component/msg_ids.png)

4. Next, editing the **name_of_your_component_platform_cfg.h**, you will need to change the communication interface. For this example, lets assume you are using a Uart communication interface. So we will need to modify the Uart number below since Uart 16 is already taken by sample. you can reference the nos3-simulator.xml found in **nos3/cfg/sims/** to see what communication devices are defined and what port numbers they are utilizing. We will be editing this file later in this guide. If the user intends to use something other than Uart like spi or I2C, other communication interfaces defined can be found in other NOS3 Components for reference.

    ![platform_cfg](./_static/adding_nos3_component/platform_cfg.png)

5. We will know modify the opcodes for our cosmos Command and TLM definitions to match the opcodes defined in **xxxx.msgids.h**. These command and tlm definitions for cosmos are found in **nos3/components/name_of_your_component/gsw/name_of_your_component/cmd_tlm/**. Edit the two files as seen below with the opcodes you defined.

    ![Sample_cmd](./_static/adding_nos3_component/Sample_cmd_opcodes.png)

    ![sample_tlm](./_static/adding_nos3_component/Sample_tlm_opcodes.png)


6. Adding your component to the nos3-Simulator.xml is necessary so the NOS3 system knows how to find and configure your new component's simulator. For a component using a Uart device copy Sample's code block and modify based on what you defined in your platform_cfg.h. 

    ![nos3-simulator](./_static/adding_nos3_component/nos3_simulator_sample_xml.png)

7. Next we will add your newly defined component to the TO tables found in **nos3/cfg/tables**. In to_config.c, include your new component and add your component to the MID config table with the same syntax pictured below. Note, be sure to add the TLM_MIDs in the code blocks defined sequentially for organization. You'll notice sample has a Sample_HK_TLM_MID and a Sample_Device_TLM_MID, so copy those lines and add to the end of the table/codeblock and modify the name for your component.

    ![to_config_include_msgids](./_static/adding_nos3_component/to_config_include_msgIds.png)


    ![to_config_table](./_static/adding_nos3_component/To_config_toConfigTable.png)


8. Next, in to_lab_sub.c we will need to include your component and add your MIDs to the table. See the syntax below for sample and copy the two lines to the end of the table.

    ![to_lab_sub_include](./_static/adding_nos3_component/to_lab_sub_includeMsgIds.png)


    ![to_lab_sub_table](./_static/adding_nos3_component/to_lab_sub_toLabSubsTable.png)

9. In **cfg/nos3_defs/** we will add the component app to the **cpu1_cfe_es_startup_scr**. Copy the syntax for sample and assign a priority number for your component that is appropriate. Sample's Priority number is 71. The lower the number, the higher priority. These fields are documented in the cpu1_cfe_es_startup_scr.

    ![cpu1_cfe_es_startup_scr](./_static/adding_nos3_component/cpu1_cfe_es_startup_scr_cfeAppTable.png)

10. Next, we will add our component's cfs directory to the targets.cmake found in **cfg/nos3_defs/**. copy the syntax for sample.

    ![targets_cmake](./_static/adding_nos3_component/targets_cmake_nos3Defs.png)

11. There are multiple files under nos3/cfg/spacecraft/ directory that you will need to add the enable flag for the spacecraft configuration. For example one of these files are the **sc-research-config.xml**. Note, you can add the same flag to each file, just make sure your nos3-mission.xml is referencing the spacecraft configuration you would like to use. Copy the syntax for sample in the components section of the file and change the name for your component.

    ![SC_config](./_static/adding_nos3_component/research_cfg_xml.png)

12. Next we will want to declare a target for the new component in Cosmos. To this we will modify the system.txt file found in **nos3/gsw/cosmos/config/**. Decalre your target like sample below for both the component and for the radio. See the syntax below.

    ![Cosmos_config_system_txt](./_static/adding_nos3_component/cosmos_system_txt_declare_target.png)

13. Now that we have declared the target, we will add the target to the cmd_tlm_server for our interfaces defined in cosmos. The cmd_tlm_server.txt can be found in **nos3/gsw/cosmos/config/tools/cmd_tlm_server/**. Add the target for both the debug and radio interfaces like sample below.

    ![Cosmos_cmd_tlm_server](./_static/adding_nos3_component/cosmos_cmd_tlm_server_target_interface.png)

14. Now we are ready to modify the NOS3 scripts to launch either the component checkout or launching with the normal NOS3 operations. First lets add the standalone checkout to the checkout script. The checkout.sh script can be found in **nos3/scripts/**. Here you will add the targets to launch your standalone checkout like sample below. Simply copy the syntax and change sample to your component's name. Note, you would need to follow the standalone checkout documentation to build and launch your checkout with "make checkout".

    ![checkout_script_sample](./_static/adding_nos3_component/checkout_script_sample.png)

15. Next, we will add the component simulator launch to the fsw_cfs_launch.sh script found in **nos3/scripts/fsw/**. Copy the syntax below for sample and modify with your component's name.

    ![cfs_launch_script](./_static/adding_nos3_component/fsw_cfs_launch_adding_component_simulator.png)

16. Finally, to ensure we configure the NOS3 System with the new component we need to modify our configure script to add in the appropriate file parsing based on our previous component additions to NOS3 configurations in the xml. You will need to edit the configure.py script found in **nos3/scripts/cfg/**. Below are the lines of code you will need to copy and add for your new component. Some lines are commented and are necessary for 42 configurations. Since sample is not connected to 42, those lines are commented out, but you will see other components are connected to 42. Below add the lines necessary based off of sample in the python script.

    ![configurePY_1](./_static/adding_nos3_component/configurePY_1.png)

    ![configurePY_2](./_static/adding_nos3_component/configurePY_2.png)

    ![configurePY_3](./_static/adding_nos3_component/configurePY_3.png)

    ![configurePY_4](./_static/adding_nos3_component/configurePY_4.png)

    ![configurePY_5](./_static/adding_nos3_component/configurePY_5_only42.png)

    ![configurePY_6](./_static/adding_nos3_component/configurePY_6_only42.png)

    ![configurePY_7](./_static/adding_nos3_component/configurePY_7_only42.png)

    ![configurePY_8](./_static/adding_nos3_component/configurePY_8.png)

    ![configurePY_9](./_static/adding_nos3_component/configurePY_9.png)

    ![configurePY_10](./_static/adding_nos3_component/configurePY_10.png)



## Key Takeaways

You have now added the necessary files and modifications to build and run a newly generated component in NOS3! 

You can now build and run NOS3 with your new component. It is always recommended to build and run the standalone checkout of the component first before running with all of nos3. Again, this step by step guide is for reference and the user will need to modify the cFS app and hardware model simulator (found in **nos3/components/name_of_your_component/sim/) for their specific use case for their new component. However, this guide will allow you to verify basic sample functionality with a newly generated component from the template generation.

