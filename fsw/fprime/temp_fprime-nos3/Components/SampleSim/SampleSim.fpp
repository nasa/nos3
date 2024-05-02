module Components {
    @ Component for F Prime FSW framework.
    active component SampleSim {

        # One async command/port is required for active components
        # This should be overridden by the developers with a useful command/port
        @ Command to issue greeting with maximum length of 20 characters
        async command SAY_HELLO(
            greeting: string size 20 @< Greeting to repeat in the Hello event
        )

        @ Greeting event with maximum greeting length of 20 characters
        event Hello(
            greeting: string size 20 @< Greeting supplied from the SAY_HELLO command
        ) severity activity high format "I say: {}"

        @ Command to Request Housekeeping
        async command REQUEST_HOUSEKEEPING(
        )

        @ Command to issue noop
        async command NOOP(
        )

        @ Greeting event with maximum greeting length of 30 characters
        event TELEM(
            log_info: string size 30 @< 
        ) severity activity high format "SampleSim: {}"

        @ A count of the number of greetings issued
        telemetry GreetingCount: U32

         @ A count of the number of greetings issued
        telemetry DeviceCounter: U32

         @ A count of the number of greetings issued
        telemetry DeviceConfig: U32

         @ A count of the number of greetings issued
        telemetry DeviceStatus: U32

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################

        # @ Example async command
        # async command COMMAND_NAME(param_name: U32)

        # @ Example telemetry counter
        # telemetry ExampleCounter: U64

        # @ Example event
        # event ExampleStateEvent(example_state: Fw.On) severity activity high id 0 format "State set to {}"

        # @ Example port: receiving calls from the rate group
        # sync input port run: Svc.Sched

        # @ Example parameter
        # param PARAMETER_NAME: U32

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @Port to set the value of a parameter
        param set port prmSetOut


    }
}