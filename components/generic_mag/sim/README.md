# Generic_mag - NOS<sup>3</sup> Simulator

This repository contains the Generic_mag NOS<sup>3</sup> Simulator.

## Overview
The example assumes a UART based device that streams telemetry at a fixed rate.
A single configuration command is recognized that allows modifying the streaming rate.
The device confirms receipt of a valid command by echoing the data back.

## Example Configuration
The default configuration returns a payload that is two times the request count:
```
<simulator>
    <name>generic_mag_sim</name>
    <active>true</active>
    <library>libgeneric_mag_sim.so</library>
    <hardware-model>
        <type>GENERIC_MAG</type>
        <connections>
            <connection><type>command</type>
                <bus-name>command</bus-name>
                <node-name>generic_mag-sim-command-node</node-name>
            </connection>
            <connection><type>usart</type>
                <bus-name>usart_29</bus-name>
                <node-port>29</node-port>
            </connection>
            <connection><type>period</type>
                <init-time-seconds>5.0</init-time-seconds>
                <ms-period>1000</ms-period>
            </connection>
        </connections>
        <data-provider>
            <type>GENERIC_MAG_PROVIDER</type>
        </data-provider>
    </hardware-model>
</simulator>
```

Optionally the 42 data provider can be used:
```
        <data-provider>
            <type>GENERIC_MAG_42_PROVIDER</type>
            <hostname>localhost</hostname>
            <port>4242</port>
            <max-connection-attempts>5</max-connection-attempts>
            <retry-wait-seconds>5</retry-wait-seconds>
            <spacecraft>0</spacecraft>
        </data-provider>
```

## Documentation
###	Hardware Simulator Framework / Example Simulator

NOS<sup>3</sup> simulator code has been developed in C++ with Boost and relies on the NASA Operational Simulator (NOS) Engine for providing the software busses, nodes, and other connections that simulate the hardware busses such as UART (universal asynchronous receiver/transmitter), I2C (Inter-Integrated Circuit), SPI (Serial Peripheral Interface), CAN (Controller Area Network), PWM (pulse width modulation), and discrete I/O (input/output) signals/connections/ busses.  NOS Engine also provides the mechanism to distribute time to all the simulators (and to the flight software and 42).

![NOS<sup>3</sup> Simulator Architecture](NOS3-Sim-Architecture.png)

The common architecture shown in the diagram above has been developed for the NOS<sup>3</sup> simulations.  This architecture has been developed for the following reasons:
* __Bus level__ communication is the goal of NOS<sup>3</sup> simulations
* __Decoupling__ the HW Model and Data Provider allows for mix and match, e.g. for a GPS:
* NovAtel 615 vs. FOTON vs. some other model:  
  *	The byte communication is different, commands and telemetry different, etc., but any GPS model needs PVT data
  *	PVT data could be provided by:
    * 42 over a TCP/IP socket
    * A fixed file of data
    * Some other data provider
*	This architecture allows __mix and match__ between hardware models and data providers

####	Background and Supporting Concepts

#####	Abstract Factory Design Pattern

C++ is a programming language that supports the Object Oriented programming paradigm, and within that paradigm, one of the most powerful design abstractions built on top of that paradigm are design patterns.  The specific design pattern which has been heavily used within the NOS<sup>3</sup> simulators to make them flexible and extensible is the Abstract Factory design pattern.  This design pattern is described in many places, but one fairly easy to understand description is in the article “Abstract Factory Step-by-Step Implementation in C++” at http://www.codeproject.com/Articles/751869/Abstract-Factory-Step-by-Step-Implementation-in-Cp .

It is this factory design pattern that allows additional simulators to be easily constructed and built as plug-in libraries, even after the development of the initial NOS<sup>3</sup> simulator code base.  Instead of the shapes and shape factory in the article, the components in NOS<sup>3</sup> simulators which are constructed via factories are hardware models and data providers.

#####	XML Configuration

In addition to using the factory design pattern, each particular simulator must be configured to specify the hardware model to create.  In addition, the hardware model may need parameters for configuring how the hardware acts.  Also, hardware has connections for communication such as discrete I/O, I2C, or UART, and so in the simulation the hardware model will need to create software versions of these connections and these connections may also need configuration data such as bus type, bus name, and bus address.  In addition, some hardware models (such as a GPS or magnetometer simulator) may need environmental data, and so the hardware model will need to create a data provider which will provide environmental data.  The data provider may need configuration data such as the type of data provider and a filename or host and port.

The configuration for a specific simulation executable will be specified in a file via XML (eXtensible Markup Language), which will provide a list of simulators that are to be instantiated within that executable.  Each simulator will specify a hardware model, which might have additional configuration parameters.  The hardware model might specify reliance on an optional data provider with data provider configuration parameters.  The hardware model might also specify one or more software communication connections with connection configuration parameters.




####	Implementing Your Own Hardware Model (and Data Provider, and Connections)

The following sections describe how to implement your own hardware model.

#####	Configuration Data Property Tree

If configuration data from the XML file, which is represented as a configuration data property tree, is needed, it is retrieved using code like the following:

>    ```std::string param = config.get("simulator.<subname>.<subsubname>", “LITERAL”);```

The following are a few notes regarding this code.  First, config is a variable of type ```const boost::property_tree::ptree&```.  Each hardware model and data provider must provide a constructor that takes a single parameter of this type (see below), and thus this parameter will be available to constructor code to perform any necessary configuration and initialization.

Second, when the code above is executed, the data type of the literal ```“LITERAL”``` determines the data type that the ptree tries to return your parameters as (here it is a literal string, and the variable the value is assigned to is declared accordingly as a ```std::string```).  Also note that you separate the XML tag names with periods in the key name to retrieve to indicate nested XML tag levels.  Note also that you do not include the ```“nos3-configuration”``` or ```“simulators”``` prefixes in the key name (these appear in the default configuration file); they are stripped off by the ```SimConfig``` object which is used to read and parse the configuration data in the main program.  Thus key names should either begin ```“common.”``` or ```“simulator.”```  If the key cannot be found in the property tree (which represents the XML), the value ```“LITERAL”``` is used as the default value.

  The following is a list of common keys:

1.	```common.log-config-file``` – The name of the configuration file for logging using the ITC Logger class; you should not normally need to do anything with this as it specifies the sim_log_config.xml file that is provided by default.
2.	```common.nos-connection-string``` – Connection to NOS Engine server ( “tcp://ip:port”)
3.	```common.absolute-start-time``` – The absolute start time of the simulation in decimal seconds from the J2000 epoch.  This must be synchronized with the epoch specified in the 42 Inp_Sim.txt file.
4.	```common.sim-microseconds-per-tick``` – The integer number of microseconds the simulation should advance for every time tick.  Note that NOS Engine distributes time on its busses as a count of ticks.  So if your hardware model or data provider receive the number of ticks that represents the simulation time, it can convert this to real world simulation time using:
    >    ```double abs_time =_absolute_start_time + (double(ticks *_sim_microseconds_per_tick)) / 1000000.0```; where ```_absolute_start_time``` is the value from ```common.absolute-start-time``` and ```_sim_microseconds_per_tick``` is the value from ```common.sim-microseconds-per-tick```.
5.	```simulator.name``` – The name you gave your simulator; it should agree with the string you put in the main function (see below).
6.	```simulator.active``` – Normally true; if false, then your simulator will not be run when the ```SimConfig::run_simulator``` method is called in the ```main``` function (see below).
7.	```simulator.hardware-model.type``` – The name string for your hardware model.  This is the key used for looking up the hardware model plugin that provides the code for your model.
8.	```simulator.hardware-model.connections``` – A list of <connection></connection> tags which describes the bus connections that the hardware model has.  
9.	```simulator.hardware-model.data-provider``` – Information on the data provider (if one is used and created using the data provider factory).
10.	```simulator.hardware-model.data-provider.type``` – The name string for your data provider (if one is used).  This is the key used for looking up the data provider plugin that provides the code for your data provider.

#####	Hardware Model

The basic outline for creating a new hardware model is the following:

1.	In ```namespace Nos3```, create a class (e.g. ```FooHardwareModel```) that inherits publicly from ```SimIHardwareModel```.
2.	Create a constructor that takes a ```const boost::property_tree::ptree&``` parameter which contains configuration data.  Have the constructor retrieve configuration data and save any parameters and create any connections, data providers, or perform any other initialization that needs done for the hardware model.
3.	Create a name string for your hardware model (e.g. ```FOOHARDWARE```) and add a line like the following to your source file:

    >    ```REGISTER_HARDWARE_MODEL(FooHardwareModel,"FOOHARDWARE");```

This name is the key specified in the ```simulator.hardware-model.type``` key (in the simulator configuration XML file) to specify that this is the hardware model plugin that provides the code for this hardware model.
4.	If the hardware model uses a data provider, the hardware model could have a member variable of type ```SimIDataProvider *```, which can be set in the hardware model constructor based on configuration data by lines like (assuming the member variable name is ```_sim_data_provider```)):

    >    ```std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "BARPROVIDER");```
    >    ```_sim_data_provider = SimDataProviderFactory::Instance().Create(dp_name, config);```
5. If the hardware model should respond to bytes written on its hardware interface, create a method such as ```void uart_read_callback(const uint8_t *buf, size_t len)``` (UART example) and use the interface's ```set_read_callback()``` method so the method gets called when bytes are received (see the ```Generic_magHardwareModel``` constructor for how to use ```set_read_callback```).
6. If the hardware model can stream messages, create one or more methods such as ```send_streaming_data(NosEngine::Common::SimTime time)``` and add it (them) to a map of names and methods that can be used to stream data (see the ```Generic_magHardwareModel``` constructor for how to add to the map and set configurations to stream data).
7. If the hardware model can respond to out of band messages (e.g. commands to the sim, for things like fault injection, etc.), create a method named (exactly) ```command_callback(NosEngine::Common::Message msg)``` and have it perform the appropriate responses (see the ```Generic_magHardwareModel``` class for an example of such a method).
8. Note that the hardware model is responsible for any formatting of bytes to send over the hardware interface; any dynamic data from a data provider should be received as is and then manipulated into the proper bytes by the hardware model. 

#####	Data Provider

The basic outline for creating a new data provider is the following:

1.	In ```namespace Nos3```, create a class (e.g. ```BarDataProvider```) that inherits publicly from ```SimIDataProvider```.
2.	Create a constructor that takes a ```const boost::property_tree::ptree&``` parameter which contains configuration data.  Have the constructor retrieve configuration data and save any parameters or do any initialization that needs done for the data provider.
3.	Create a ```virtual boost::shared_ptr<SimIDataPoint> get_data_point(void) const;``` method… that does whatever is supposed to be done to retrieve (or compute or whatever) a data point when your data provider is asked for a data point and which returns a pointer to the retrieved data point.  You should also create a class that inherits publicly from ```SimIDataPoint``` to hold the data that you return from the data provider.
4.	Create a name string for your data provider (e.g. ```BARPROVIDER```) and add a line like the following to your source file:

    >    ```REGISTER_DATA_PROVIDER(BarDataProvider,"BARPROVIDER");```

This name is the key specified in the ```simulator.hardware-model.data-provider.type``` key (in the simulator configuration XML file) to specify that this is the data provider plugin that provides the code for this data provider.
10.2.3.1	42 Data Provider Framework/Interface
Since 42 is anticipated to be a common provider of dynamic data over a socket interface, some common code, in ```nos3/sims/sim_common``` has been developed.  It was decided that the following common functionality would be provided:
*    __```send_command_to_socket(std::string)```__ method to send commands to 42 (*sim_data_42socket_provider.cpp*)
*    __```get_data_point(void)```__ method allows easy hook up of a sim to 42 IPC and returning a __```Sim42DataPoint```__ (*sim_data_42socket_provider.hpp*)
*    The ```Sim42DataPoint``` (*sim_42data_point.hpp*) contains a ```std::vector<std::string>``` of text lines for one “message” from 42 (call __```.get_lines()```__ to retrieve the text lines)

It is up to the individual data provider in the individual simulator to parse the lines of data contained in a Sim42DataPoint.  Note, however, that the data provider/data point should **not** be responsible for doing any units conversions/scaling, manipulations, byte conversions, etc.  That is the responsibility of the hardware model (so that the data provider and data point can remain hardware agnostic).  An example from the generic reaction wheel simulator of using the common functionality together with a simulator specific parser is shown in the figure below.

![42 Data Provider Framework/Interface](42-DataProvider-Framework-Interface.png)

#####	Connections

The general procedure for creating a connection is to create an object that is called a hub (a default constructed object can be used), then create bus and node objects or a connection object (depending on the connection type).  With the node or connection object, various things can be done to handle the connection such as registering a callback so that when a message is received on the connection, the hardware model can respond to it and send a response.  The basics for using a few of the connection types are described below, but for examples, please consult the example code and existing simulators.

######	Command Connection

The command connection of a simulation hardware model is not a normal connection in the sense of a connection that the hardware would have to a hardware bus.  It is used to perform out of band commanding of the simulation itself.  One way to perform this commanding is to use the SimTerminal executable that is part of NOS<sup>3</sup>.  This terminal starts up and registers as a node on the command bus.  It can then be used to send messages to any other node on the command bus.  These messages can be ASCII or hexadecimal bytes.

The base ```SimIHardwareModel``` creates a node on a command bus so that any hardware model simulation can be commanded.  In order for a simulation to perform actions based on commands received on the command bus, the only thing that needs done in the hardware model is the following:

1.	In the hardware model class, override the ```SimIHardwareModel``` method:
```void command_callback(NosEngine::Common::Message msg)```

For an example of how data is received by and returned from the hardware model in response to a command, refer to the ```command_callback``` method in the base ```SimIHardwareModel``` class.

######	Time Connection

For the hardware simulator to have a notion of time in the real world, it registers a node with NOS Engine as a time client node.  The **formula** for creating and using a time client node is:
1.	In the hardware model class, add member variables for the bus and time node, e.g.:
```std::unique_ptr<NosEngine::Client::Bus> _time_bus;```
2.	In the hardware model constructor:
    *	The base ```SimIHardwareModel``` class has an existing hub, member variable ```_hub``` for the bus to connect to.  The connection string for NOS Engine can be retrieved from the XML configuration data by a call like:
```std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");```
    *	Add a “time” type connection to the XML configuration file something like:
```<connection><type>time</type><bus-name>command</bus-name><node-name>my-time-node</node-name></connection>```
    *	Retrieve the bus name into a ```std::string``` variables like ```time_bus_name```.  For an example of how to do so, please see the example simulator.
    *	Create a bus object:
```_time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));```
3.	In hardware model methods that need time:
    *	To get the number of “ticks” that have elapsed, call:
```_time_bus->get_ time()```
    *	To convert this to real world time, the ```SimIHardwareModel``` has member variables ```_absolute_start_time``` and ```_sim_microseconds_per_tick``` (set from data in the common section of the XML configuration file), and they can be used to compute real world time by:
```_absolute_start_time + (double(_time_bus->get_ time() * _sim_microseconds_per_tick)) / 1000000.0);```
4.	To clean up, in the hardware model destructor, call:
```_time_bus.reset();```

######	UART Connection

For hardware that is connected via UART, the **formula** for the hardware to creating and using a node on the UART bus is the following:
1.	In the hardware model class, add a member variable for the UART connection like the following:
```std::unique_ptr<NosEngine::Uart::Uart>  _uart_connection;```
2.	In the hardware model constructor:
    1.	The base ```SimIHardwareModel``` class has an existing hub, member variable ```_hub``` for the bus to connect to.  The connection string for NOS Engine can be retrieved from the XML configuration data by a call like:

        >    ```std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");```

    2.	Add a “usart” type connection to the XML configuration file something like:

        >   ```<connection><type>usart</type><bus-name>usart_0</bus-name><node-port>99999</node-port></connection>```

    3.	Retrieve the bus name and node port into ```std::string``` variables like ```bus_name``` and ```node_port```.  For an example of how to do so, please see the example simulator.
    4.	Create a UART connection object:

        >    ```_uart_connection.reset(new NosEngine::Uart::Uart(_hub, config.get("simulator.name", "foosim"), connection_string, bus_name));```

    5.	Open the connection and set a callback for when the hardware UART is read:

        >    ```_uart_connection->open(node_port);
        >    ```_uart_connection->set_read_callback(std::bind(&FooHardwareModel::uart_read_callback, this, std::placeholders::_1, std::placeholders::_2));```

3.	Create a hardware model method for the callback (here is where most of the custom work for a specific hardware model would be done):
    1.	The signature should be like:

        >    ```void FooHardwareModel::uart_read_callback(const uint8_t *buf, size_t len);```

    2.	To return data, use the UART method: 

        >    ```size_t UART::write(const uint8_t *const buf, size_t len);```

    3.	For an example, consult the example sim code.
4.	In the hardware model destructor, make the call:

    >    ```_uart_connection->close();```

####	Writing Your Own Simulator

The following **formula** describes how to create a simulator using a hardware model (and optionally a data provider) created using the formulas above:

1.	Create a main source file with the following contents:
```
#include <ItcLogger/Logger.hpp>
#include <sim_config.hpp>

namespace Nos3
{
    ItcLogger::Logger *sim_logger;
}

int
main(int argc, char *argv[])
{
    std::string simulator_name = "foosim"; // this is the ONLY simulator specific line!

    // Determine the configuration and run the simulator
    Nos3::SimConfig sc(argc, argv);
    Nos3::sim_logger->info("main:  %s simulator starting", simulator_name.c_str());
    sc.run_simulator(simulator_name);
    Nos3::sim_logger->info("main:  %s simulator terminating", simulator_name.c_str());
}
```
2.	Change “```foosim```” to whatever you would like the name of your simulator to be
3.	Add XML like the following inside the ```<simulators></simulators>``` tags in the standard configuration file (the standard configuration file name is ```nos3-simulator.xml```)
```
        <simulator>
            <name>foosim</name>
            <active>true</active>
            <library>libexample_sim.so</library>
            <hardware-model>
                <type>FOOHARDWARE</type>
                <connections>
                    <connection>
                        <connection-param1>cp1</connection-param1>
                        <!-- ... -->
                        <connection-paramN>cpN</connection-paramN>
                    </connection>
                </connections>
                <data-provider>
                    <type>FOOPROVIDER</type>
                    <provider-param1>fpp1</provider-param1>
                    <!-- ... -->
                    <provider-paramN>fppN</provider-paramN>
                </data-provider>
                <other-hardware-parameter1>OTHER-FOO</other-hardware-parameter1>
                <!-- ... -->
                <other-hardware-parameterN>OTHER-FOO</other-hardware-parameterN>
            </hardware-model>
        </simulator>
```
4.	Customizing the XML:
    *	The ```simulator.name``` should be the same as in your main function in #1.  
    *	The ```simulator.active``` tag should be true unless you do not want your simulator to run in which case it should be false.
    *	The ```simulator.library``` tag should contain the name of the example simulator shared object library file (normally ```lib<project>.so``` where ```<project>``` is the project name given the project in the ```CMakeLists.txt``` file; see below) 
    *	The ```simulator.hardware-model.type``` should be the same as the string you used in the ```REGISTER_HARDWARE_MODEL``` line above.
    *	The simulator hardware-model data-provider type should be the same as the string you used in the ```REGISTER_DATA_PROVIDER``` line above.
    *	All other tags are up to you… create your own names and then use the information above for accessing the data.  Note that there are examples in the source code for using several common connection types such as UART, I2C and the command connection (used for out of band control of the simulator with the simulator terminal).  Also note that the command connection is automatically configured for you in the ```SimIHardwareModel``` base class.  To have your simulator respond to commands to it on the command bus, all you need to do is override the ```SimIHardwareModel::command_callback``` method in your hardware model class (the default implementation does nothing).

####	Example Simulator

Hopefully this introduction is useful in describing the flexible, extensible framework employed in developing NOS<sup>3</sup> simulators.  This introduction has attempted to describe the design pattern used within NOS<sup>3</sup> simulators and described how to add hardware models (and data providers and other supporting items), and put hardware models together into standalone simulators that can be part of the NOS<sup>3</sup> simulation environment.

For a complete example, refer to the source code and ```CMakeLists.txt``` file in the ```nos3``` git repository, subdirectory ```sims```, submodule ```generic_mag_sim``` and refer to the configuration file in the ```nos3``` git repository, file ```sims/cfg/nos3-simulator.xml``` (see the simulator section with name “```generic_mag_sim```”).   Note also that if a new simulator’s ```CMakeLists.txt``` file for a simulator has a project name line like ```“project(generic_mag_sim)”``` at the beginning, the line ```“add_subdirectory(generic_mag_sim)”``` may be added under ```# NOS3 Sim Core``` in the ```sims/CMakeLists.txt``` file in the ```nos3``` git repository so that the new simulator will be built, but the ```sims/CMakeLists.txt``` file is written to find all properly structured and properly named directories following the form of the parent folder in nos3/sims/ being: ```“<name-of-your-sim>_sim”```.
