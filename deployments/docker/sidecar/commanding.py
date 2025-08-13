from yamcs.client import YamcsClient

client = YamcsClient('active-gs:8090')
processor = client.get_processor(instance='nos3', processor='realtime')

command_name = "/GENERIC_RADIO/CMD/GENERIC_RADIO_NOOP_CC"
arguments = {"voltage_num": 1} # Example arguments, NOT RELEVANT FOR NOOP command

command_handle = processor.issue_command(command_name, args=arguments)
print(f"Issued command: {command_handle}")
