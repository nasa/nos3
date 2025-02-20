# Simulators
NOS3 simulator code has been developed in C++ with Boost and relies on the NASA Operational Simulator (NOS) Engine for providing the software busses, nodes, and other connections that simulate the hardware busses such as UART (universal asynchronous receiver/transmitter), I2C (Inter-Integrated Circuit), SPI (Serial Peripheral Interface), CAN (Controller Area Network), and discrete I/O (input/output) signals/connections/busses. NOS Engine also provides the mechanism to distribute time to all the simulators (and to the flight software).

## Architectural Whys
### Why NOS Engine?
NOS Engine was chosen as the abstract hardware bus interface for two reasons.
1.  On the flight software side, writing a single hardware library with an API to various common hardware bus types (UART, SPI, I2C, etc.) makes it almost trivial to build for both real hardware and (NOS3) simulated hardware.  
1.  On the simulator side, NOS Engine provides an easy to use interface and (future capability) the ability to model things like hardware faulting, bus dropouts/errors, etc.

### Why Hardware Models, plug-ins, abstract factories, etc.?
This enables hardware simulators to be extremely XML driven.  As long as the simulator executable knows of (point it to a directory) or can find (specify shared object library file(s) in XML) a source of simulator hardware library(s), the simulator executable can be tiny and extensible, relying on plug-in shared object libraries to provide the needed hardware simulators.  For examples of the power this plug-in architecture provides, consult `all_simulators.cpp` / `nos3-all-simulators` (run all active hardware models in the XML configuration file, each in its own thread) and `single_simulator.cpp` / `nos3-single-simulator` (run a single active hardware mode in the XML file by specifying its name on the command line) in the `sim_common` submodule repository.

### Why Data Providers?
NOS Engine provides a clean interface for hardware models to various typical hardware bus types and plug-ins provide the motivation for plug and play hardware models.  

But why have plug-in data providers?  

The reasoning for this is to decouple the hardware model from its source of "truth" data, especially about things like sun vectors, sun angles, positions, etc.  There can be many models of GPS hardware, but pretty much any model of GPS hardware will need a source of position data to transmit across the hardware bus (in something like NEMA format over a UART or perhaps some proprietary binary format).  This source of data could be a file (used extensively in early NOS3 development) or 42 (used extensively for "truth" data in current simulators).  In addition, this allows for easy extensibility to future data providers that have not even been thought of yet or to alternatives for 42.  As long as you, the hardware simulator developer, agree (with yourself) on an interface between your hardware model plug-in(s) and your data provider plug-in(s), the sky is the limit for current and future flexibility/extensibility.  And you have complete control over that (hardware model to/from data provider) interface, unlike the interface to an external "truth" provider (e.g. 42).

## Background and Supporting Concepts
### Abstract Factory Design Pattern
C++ is a programming language that supports the Object Oriented programming paradigm, and within that paradigm, one of the most powerful design abstractions built on top of that paradigm are design patterns. The specific design pattern which has been heavily used within the NOS3 simulators to make them flexible and extensible is the Abstract Factory design pattern. This design pattern is described in many places, but one fairly easy to understand description is in the article ["Abstract Factory Step-by-Step Implementation in C++"](http://www.codeproject.com/Articles/751869/Abstract-Factory-Step-by-Step-Implementation-in-Cp).

It is this factory design pattern that allows additional simulators to be easily constructed and built as plug-in libraries, even after the development of the initial NOS3 simulator code base. Instead of the shapes and shape factory in the article, the components in NOS3 simulators which are constructed via factories are hardware models and data providers.

### XML Configuration
In addition to using the factory design pattern, each particular simulator must be configured to specify the hardware model to create. In addition, the hardware model may need parameters for configuring how the hardware acts. Also, hardware has connections for communication such as discrete I/O, I2C, or UART, and so in the simulation the hardware model will need to create software versions of these connections and these connections may also need configuration data such as bus type, bus name, and bus address. In addition, some hardware models (such as a GPS or magnetometer simulator) may need environmental data, and so the hardware model will need to create a data provider which will provide environmental data. The data provider may need configuration data such as the type of data provider and a filename or host and port.

The configuration for a specific simulation executable will be specified in a file via XML (eXtensible Markup Language), which will provide a list of simulators that are to be instantiated within that executable. Each simulator will specify a hardware model, which might have additional configuration parameters. The hardware model might specify reliance on an optional data provider with data provider configuration parameters. The hardware model might also specify one or more software communication connections with connection configuration parameters.

## Implementing Your Own Hardware Model (and Data Provider, and Connections)
The following sections describe how to implement your own hardware model.

### Configuration Data Property Tree
If configuration data from the XML file, which is represented as a configuration data property tree, is needed, it is retrieved using code like the following:

```c
std::string param = config.get("simulator.<subname>.<subsubname>", "LITERAL");
```

The following are a few notes regarding this code. First, `config` is a variable of type `const boost::property_tree::ptree&`. Each hardware model and data provider must provide a constructor that takes a single parameter of this type (see below), and thus this parameter will be available to constructor code to perform any necessary configuration and initialization.

Second, when the code above is executed, the data type of the literal `"LITERAL"` determines the data type that the `ptree` tries to return your parameters as (here it is a literal string, and the variable the value is assigned to is declared accordingly as a `std::string`). Also note that you separate the XML tag names with periods in the key name to retrieve to indicate nested XML tag levels. Note also that you do not include the `"nos3-configuration"` or `"simulators"` prefixes in the key name (these appear in the default configuration file); they are stripped off by the `SimConfig` object which is used to read and parse the configuration data in the main program. Thus key names should either begin `"common."` or `"simulator."` If the key cannot be found in the property tree (which represents the XML), the value `"LITERAL"` is used as the default value.

The following is a list of common keys:
1. `common.log-config-file` – The name of the configuration file for logging using the ITC Logger class; you should not normally need to do anything with this.
1. `common.nos-connection-string` – Connection to NOS Engine Server
1. `common.absolute-start-time` – The absolute start time of the simulation in decimal seconds from the J2000 epoch.
1. `common.sim-microseconds-per-tick` – The integer number of microseconds the simulation should advance for every time tick. Note that NOS Engine distributes time on bus(s) as a count of ticks. So if your hardware model or data provider receive the number of ticks (from a bus that has time driven by a time master) that represents the simulation time, it can convert this to NOS3 synchronized simulation real world time using:
```c
double abs_time =_absolute_start_time + (double(ticks *_sim_microseconds_per_tick)) / 1000000.0;
```
5. `common.real-microseconds-per-tick` – Normally used by a single main time driver to determine how long to delay between sending simulated ticks on NOS Engine bus(s).  Rarely but possibly used by hardware simulators if they need to delay for an amount of real time (a hardware simulator should typically be using simulated time).
1. `simulator.name` – The name you gave your simulator; it should agree with the string you specify when running `nos3-single-simulator`.
1. `simulator.active` – Normally true; if false, then your simulator will not be run when the `SimConfig::run_simulator` method is called in the main function (see below).
1. `simulator.hardware-model.type` – The name string for your hardware model.  Matched against the name string given in the `REGISTER_HARDWARE_MODEL` call that must be present in the source code for every hardware model that conforms to the plug-in model.
1. `simulator.hardware-model.connections` – A list of \<connection\>\</connection\> tags which describes the connections that the hardware model has.
1. `simulator.hardware-model.data-provider` – Information on the data provider (if one is used and created using the data provider factory).
1. `simulator.hardware-model.data-provider.type` – The name string for your data provider (if one is used).  Matched against the name string given in the `REGISTER_DATA_PROVIDER` call that must be present in the source code for every data provider that conforms to the plug-in model.

### Hardware Model
The formula for creating a new hardware model is the following:
1. In namespace `Nos3`, create a class (e.g. `FooHardwareModel`) that inherits publicly from `SimIHardwareModel`.
1. Create a constructor that takes a `const boost::property_tree::ptree&` parameter which contains configuration data. Have the constructor retrieve configuration data and save any parameters and create any connections, data providers, or perform any other initialization that needs done for the hardware model.
1. Create a `void run(void)` method. This method should perform whatever tasks are supposed to be done when the hardware model is running.
1. Create a name string for your hardware model (e.g. `FOOHARDWARE`) and add a line like the following to your source file:
```c
REGISTER_HARDWARE_MODEL(FooHardwareModel,"FOOHARDWARE");
```
5. If the hardware model uses a data provider, the hardware model could have a member variable of type `SimIDataProvider *`, which can be set in the hardware model constructor based on configuration data by lines like (assuming the member variable name is `_sim_data_provider`)):
```c
std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "BARPROVIDER");
_sim_data_provider = SimDataProviderFactory::Instance().Create(dp_name, config);
```

### Data Provider
The formula for creating a new data provider is the following:
1. In namespace `Nos3`, create a class (e.g. `BarDataProvider`) that inherits publicly from `SimIDataProvider`.
1. Create a constructor that takes a `const boost::property_tree::ptree&` parameter which contains configuration data. Have the constructor retrieve configuration data and save any parameters or do any initialization that needs done for the data provider.
1. Create a `virtual boost::shared_ptr<SimIDataPoint> get_data_point(void) const;` method… that does whatever is supposed to be done to retrieve (or compute or whatever) a data point when your data provider is asked for a data point and which returns a pointer to the retrieved data point. You should also create a class that inherits publicly from `SimIDataPoint` to hold the data that you return from the data provider.
1. Create a name string for your data provider (e.g. `BARPROVIDER`) and add a line like the following to your source file:
```c
REGISTER_DATA_PROVIDER(BarDataProvider,"BARPROVIDER");
```

### Connections
The general procedure for creating a connection is to create an object that is called a hub (a default constructed object can be used), then create bus and node objects or a connection object (depending on the connection type). With the node or connection object, various things can be done to handle the connection such as registering a callback so that when a message is received on the connection, the hardware model can respond to it and send a response. The basics for using a few of the connection types are described below, but for examples, please consult the example code and existing simulators.

#### Command Connection
The command connection of a simulation hardware model is not a normal connection in the sense of a connection that the hardware would have to a hardware bus. It is used just to perform out of band commanding of the simulation itself. One way to perform this commanding is to use the SimTerminal executable that is part of NOS3. This terminal starts up and registers as a node on the command bus. It can then be used to send messages to any other node on the command bus. These messages can be ASCII or hexadecimal bytes.

The base `SimIHardwareModel` creates a node on a command bus so that any hardware model simulation can be commanded. In order for a simulation to perform actions based on commands received on the command bus, the only thing that needs done in the hardware model is the following:
1. In the hardware model class, override the `SimIHardwareModel` method:
```c
void command_callback(NosEngine::Common::Message msg)
```

For an example of how data is received by and returned from the hardware model in response to a command, refer to the `command_callback` method in the base `SimIHardwareModel` class.

#### Time Connection
For the hardware simulator to have a notion of time in the real world, it registers a node with NOS Engine as a time client node. The formula for creating and using a time client node is:
In the hardware model class, add member variables for the bus and time node, e.g.:
```c
std::unique_ptr<NosEngine::Client::Bus> _time_bus;
NosEngine::Client::TimeClient* _time_node;
```
In the hardware model constructor:
1. The base `SimIHardwareModel` class has an existing hub, member variable `_hub` for the bus to connect to. The connection string for NOS Engine can be retrieved from the XML configuration data by a call like:
```c
std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");
```
2. Add a "time" type connection to the XML configuration file something like:
```xml
<connection><type>time</type><bus-name>command</bus-name><node-name>my-time-node</node-name></connection>
```
3. Retrieve the bus name and node name into `std::string` variables like `time_bus_name` and `time_node_name`. For an example of how to do so, please see the example simulator.
4. Create a bus object:
```c
_time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
```
5. Create a time client node on the bus:
```c
_time_node = _time_bus->get_or_create_time_client(time_node_name);
```
In hardware model methods that need time:
1. To get the number of "ticks" that have elapsed, call:
```c
_time_node->get_last_time()
```
2. To convert this to real world time, the `SimIHardwareModel` has member variables `_absolute_start_time` and `_sim_microseconds_per_tick` (set from data in the common section of the XML configuration file), and they can be used to compute real world time by:
```c
_absolute_start_time + (double(_time_node->get_last_time() * _sim_microseconds_per_tick)) / 1000000.0);
```
To clean up, in the hardware model destructor, call:
```c
_time_bus.reset();
```

#### UART Connection
For hardware that is connected via UART, the formula for the hardware to create and use a node on the UART bus is the following:
In the hardware model class, add a member variable for the UART connection like the following:
```c
std::unique_ptr<NosEngine::Uart::Uart> _uart_connection;
```
In the hardware model constructor:
1. The base `SimIHardwareModel` class has an existing hub, member variable `_hub` for the bus to connect to. The connection string for NOS Engine can be retrieved from the XML configuration data by a call like:
```c
std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");
```
2. Add a "usart" type connection to the XML configuration file something like:
```xml
<connection><type>usart</type><bus-name>usart_0</bus-name><node-port>99999</node-port></connection>
```
3. Retrieve the bus name and node port into `std::string` variables like `bus_name` and `node_port`. For an example of how to do so, please see the example simulator.
4. Create a UART connection object:
```c
_uart_connection.reset(new NosEngine::Uart::Uart(_hub, config.get("simulator.name", "foosim"), connection_string, bus_name));
```
5. Open the connection and set a callback for when the hardware UART is read:
```c
_uart_connection->open(node_port);
_uart_connection->set_read_callback(std::bind(&FooHardwareModel::uart_read_callback, this, std::placeholders::_1, std::placeholders::_2));
```
Create a hardware model method for the callback (here is where most of the custom work for a specific hardware model would be done):
1. The signature should be like:
```c
void FooHardwareModel::uart_read_callback(const uint8_t *buf, size_t len);
```
2. To return data, use the UART method:
```c
size_t UART::write(const uint8_t *const buf, size_t len);
```
For an example, consult the example sim code.

3. In the hardware model destructor, make the call:
```c
_uart_connection->close();
```

## Writing Your Own Simulator
The following formula describes how to create a simulator using a hardware model (and optionally a data provider) created using the formulas above:
1. Add XML like the following inside the `<simulators></simulators>` tags in the standard configuration file (the standard configuration file name is `nos3-simulator.xml`)
```xml
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
2. Customizing the XML:
   1. The `simulator.name` should be the name you pass to `nos3-single-simulator` to execute.
   2. The `simulator.active` tag should be true unless you do not want your simulator to run in which case it should be false.
   3. The `simulator.library` tag should contain the name of the example simulator shared object library file (normally `lib<project>.so` where `<project>` is the project name given the project in the `CMakeLists.txt` file; see below)
   4. The `simulator.hardware-model.type` should be the same as the string you used in the `REGISTER_HARDWARE_MODEL` line above.
   5. The `simulator.hardware-model.data-provider.type` should be the same as the string you used in the `REGISTER_DATA_PROVIDER` line above.
   6. All other tags are up to you… create your own names and then use the information above for accessing the data. Note that there are examples in the source code for using several common connection types such as UART, I2C and the command connection (used to control the simulator with the simulator terminal). Also note that the command connection is automatically configured for you in the `SimIHardwareModel` base class. To have your simulator respond to commands to it on the command bus, all you need to do is override the `SimIHardwareModel::command_callback` method in your hardware model class (the default implementation does nothing).

## Example Simulator
Hopefully this introduction is useful in describing the flexible, extensible framework employed in developing NOS3 simulators. This introduction has attempted to describe the design pattern used within NOS3 simulators and described how to add hardware models (and data providers and other supporting items), and put hardware models together into standalone simulators that can be part of the NOS3 simulation environment.

For a complete example, refer to the source code and `CMakeLists.txt` file in the `nos3` git repository, subdirectory `components/sample/sim/` and refer to the configuration file in the `nos3` git repository, file `cfg/sims/nos3-simulator.xml` (see the simulator section with name `"sample_sim"`). Note also that if a new simulator’s `CMakeLists.txt` file for a simulator has a project name line like `"project(sample_sim)"` at the beginning, the line `"add_subdirectory(sample_sim)"` may be added under # NOS3 Sim Core in the `sims/CMakeLists.txt` file in the `nos3` git repository so that the new simulator will be built, but the `sims/CMakeLists.txt` file is written to find all properly structured and properly named directories following the form of the parent folder in nos3/sims/ being: `"<name-of-your-sim>_sim"`.
