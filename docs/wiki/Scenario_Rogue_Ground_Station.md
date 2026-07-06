# Scenario - Rogue Ground Station

This scenario was developed to demonstrate how an adversary can compromise a secondary ground station to execute a Denial of Service (DoS) attack, lock out legitimate operators, and physically manipulate the spacecraft.

This scenario was last updated on 07/06/2026.

## Learning Goals

By the end of this scenario, you should be able to:

* Understand how to configure and operate multiple Ground Data Systems (GDS) simultaneously within NOS3.
* Understand the implications of an insider threat or compromised trusted entity holding valid cryptographic keys.
* Analyze how command flooding can overload cryptographic processing, resulting in an operator lockout.
* Understand how to manipulate the Attitude Determination and Control System (ADCS) to induce a physical spacecraft tumble.

## Prerequisites

* [Getting Started](./NOS3_Getting_Started.md)
  * [Installation](./NOS3_Getting_Started.md#installation)
  * [Running](./NOS3_Getting_Started.md#running)
* [Scenario - Demonstration](./Scenario_Demo.md)
* **IMPORTANT:** Checkout the `857-Rogue-GroundStation` branch
  * May require running `git submodule sync` and `git submodule update --init --recursive` after


## Walkthrough

The goal of this scenario is to build off previous knowledge to execute and observe a cyberattack from the perspective of both the attacker - Red Team - and the legitimate flight controllers - Blue Team. You will utilize COSMOS and YAMCS to represent two distinct ground systems. 

It is critical to note that in this scenario, both ground stations are technically valid. Both systems possess the correct cryptographic keys required to communicate with the spacecraft. However, the COSMOS instance represents a ground station that has been taken over and gone rogue. Because the attacker leverages valid cryptographic material to establish the connection, this scenario accurately simulates a highly privileged insider threat. By flooding the flight software with automated, cryptographically valid scripts from the rogue station, the attacker will force the spacecraft's security processing to desynchronize and lock out YAMCS. Once exclusive control is achieved, the attacker will spoof the ADCS to force the spacecraft into a tumble. 

### Step 1 - Scenario Setup

In order to set up this scenario, a few configuration changes need to be made to NOS3 to enable the Multiple GDS architecture and prevent the simulated radio from dropping connections. 

* First, navigate to the `cfg/nos3-mission.xml` file.
  * Under the Ground Software section, change "cosmos" to "multiple". You can do this via the terminal by using nano `cfg/nos3-mission.xml`, making the change, and saving the file.
  * Ensure "STF1" is selected under the Flight Software section.
* Next, navigate to `cfg/sims/nos3-simulator-multipleGDS.xml` and search for `GENERIC_RADIO_42`.
  * Ensure that both the uplink-close-criteria and downlink-close-criteria are set to "none".

Now, rebuild and launch NOS3 as you would normally by executing make prep, make, and make launch.

### Step 2 - Multiple GDS Verification

Before executing the attack, we must verify that both ground stations are actively connected, authenticated, and receiving the exact same telemetry from the spacecraft.

  * Open COSMOS and launch the Command and Telemetry Server.
  * Open Firefox in your VM and navigate to http://localhost:8090 to access YAMCS. Click on nos3 under the Instance menu.
  * Navigate to Commanding/Send a command. In the search bar, type `TO_ENABLE_OUTPUT`, select `/CFS/CMD/TO_ENABLE_OUTPUT`, and hit Send. You can verify this was successfully sent by observing the sc01 - NOS3 Flight software terminal.
  * Navigate to Links tab in YAMCS.
  * Compare the two systems. Verify that the `RADIO` telemetry packet values under `TIM PKTS` (for example: 412) in COSMOS match the `radio-in` value in YAMCS.
    * You can press p to verify the packets are the same in the NOS Time Driver.
    * Ensure to press p again to resume the packet counting. 

![Packet Verification Between COSMOS and YAMCS](./_static/scenario_rogue_ground_Station/COSMOS_YAMCS_Packet_Verification.png)

### Step 3 - Executing the Attack via Operator Lockout

This section will cover how the adversary utilizes their foothold in the compromised ground station to overload the system and seize exclusive control. Because they possess the cryptographic keys, the spacecraft views their incoming commands as entirely legitimate.

#### Part A: Initiating the Attack

  * Type `vcid 2` in both sc01 - CryptoLib GSW an sc01 - CryptoLib GSW2. Open sc01 - NOS3 Flightware. (Note: VCID stands for Virtual Channel Identifier. By typing `vcid 2`, you are forcing both ground software instances to share the exact same virtual channel and cryptographic sequence counter, setting the stage for the lockout.)
  * In COSMOS, open the Script Runner to generate a Ruby script. This will act as the adversary's automated flooding tool, locking out GSW2 to then take command of the spacecraft. Type the following code into the Script Runner (you may copy and paste this code into the script):

    ```
    20.times do
      cmd("SAMPLE_RADIO SAMPLE_NOOP_CC")
      wait(0.01)
    end 
    ```

  * Run the script. This code will send the command 20 times in rapid succession, spamming the flight software. The `SAMPLE_NOOP_CC` command is a benign command that tells the flight software to do nothing but acknowledge receipt, forcing the flight software to repeatedly process cryptographic authentications. You can view the sc01 NOS3 Flight Software terminal to see the NOOP command being authenticated and received repeatedly. 

#### Part B: Verifying the Lockout

  * Now, transition to the perspective of the legitimate operator. In YAMCS, attempt to send a standard `SAMPLE/CMD/SAMPLE_NOOP_CC` command via the Commanding interface. 
  * View sc01 NOS3 Flight Software and note "Crypto_TC_ProcessSecurity returned error -23".
    * Because the rogue ground station flooded the system with valid commands, it overloaded the cryptographic sequence and processing counters for GSW2. The rogue ground station now has complete control over the spacecraft, and YAMCS cannot command the spacecraft (no commands will go through). 

  ![Error Message - YAMCS Lockout Successful](./_static/scenario_rogue_ground_Station/Crypto_TC_ProcessSecurityreturnederror-23YAMCS_Lockout.png)

### Step 4 - Inducing a Tumble 

Now that the adversary has complete control, they will attempt to physically destabilize the spacecraft by spoofing attitude coordinates.

#### Part A: Spoofing the ADCS

  * In the COSMOS Command Sender, set the Target to `GENERIC_STAR_TRACKER` and the Command to `GENERIC_STAR_TRACKER_ENABLE_CC`, then send the command.
  * Next, set the Target to `GENERIC_ADCS` and the Command to `GENERIC_ADCS_SET_MODE_CC`. Set the state to `INERTIAL_MODE` and send the command. This tells the spacecraft to lock onto a specific coordinate.
  * Open a new script in the Script Runner. Type the following Ruby script:
    
    ```
    20.times do
      cmd("GENERIC_ADCS GENERIC_ADCS_INERTIAL_QUATERNION_CC with GNC_INER_QUAT1 1.0, GNC_INER_QUAT2 0, GNC_INER_QUAT3 0, GNC_INER_QUAT4 0")
      wait(30.0)
      cmd("GENERIC_ADCS GENERIC_ADCS_INERTIAL_QUATERNION_CC with GNC_INER_QUAT1 0, GNC_INER_QUAT2 1.0, GNC_INER_QUAT3 0, GNC_INER_QUAT4 0")
      wait(30.0)
      cmd("GENERIC_ADCS GENERIC_ADCS_INERTIAL_QUATERNION_CC with GNC_INER_QUAT1 0, GNC_INER_QUAT2 0, GNC_INER_QUAT3 1.0, GNC_INER_QUAT4 0")
      wait(30.0)
      cmd("GENERIC_ADCS GENERIC_ADCS_INERTIAL_QUATERNION_CC with GNC_INER_QUAT1 0, GNC_INER_QUAT2 0, GNC_INER_QUAT3 0, GNC_INER_QUAT4 1.0")
      wait(30.0)
    end
    ```

This script sets the spacecraft to continuously move between different quaternion values every 30 seconds, inducing a tumble. 

#### Part B: Observe the Tumble

  * Start the Ruby Script in COSMOS.
  * Transition immediately to the perspective of the legitimate flight controllers. In YAMCS, navigate to Telemetry/Parameters/GENERIC_IMU/GENERIC_IMU_DATA_TLM. Find the `X_ANGULAR_ACCELERATION`, `Y_ANGULAR_ACCELERATION`, and `Z_ANGULAR_ACCELERATION` parameters.
    * Watch said parameters until you observe the angular values exceeding .17 rads/sec. Reaching .17 rads/sec may take a few minutes. Because the spacecraft is continuously adjusting to new coordinates every 30 seconds, you will see the values oscillate - climbing, dropping to smaller values, and then climbing even higher with each cycle. The values do not need to remain continuously at or above .17 rads/sec; simply breaching this threshold confirms the tumble is successful. 

Because YAMCS was cryptographically locked out in, the legitimate operators are reduced to mere spectators. They can watch their spacecraft tumble out of control in real-time via telemetry, but if they attempt to send any corrective commands to stop it, the flight software will reject them. They are completely powerless to save the spacecraft.

  * The adversary can also verify that their tumble was successful. By opening the COSMOS Packet Viewer and setting the Target to `GENERIC_IMU` and the Packet to `GENERIC_IMU_DATA_TLM`, the attacker can watch the same extreme angular values -  `X_ANGULAR_RATE`, `Y_ANGULAR_RATE`, `Z_ANGULAR_RATE` - confirming the cyberattack was successful.
  
![Angular Values Confirmed both on COSMOS and YAMCS](./_static/scenario_rogue_ground_Station/COSMOS_YAMCS_Angular_Confirmed.png)

## Conclusion

This scenario highlights the danger of insider threats and compromised ground stations. By combining command flooding to achieve a Denial of Service (lockout) with Attitude Determination and Control System (ADCS) spoofing, it demonstrates how valid cryptographic keys can be weaponized to render a spacecraft effectively unrecoverable simply by using the spacecraft's own commands against it.

Try your own and let us know in the [GitHub Discussions](https://github.com/nasa/nos3/discussions)!
