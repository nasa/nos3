import sys

from yamcs.client import YamcsClient

SERVER=sys.argv[1]
PORT=sys.argv[2]

client = YamcsClient(f"{SERVER}:{PORT}")
processor = client.get_processor(instance='nos3', processor='realtime')

command_name = "/CFS/CMD/TO_ENABLE_OUTPUT"
arguments = {} # Example arguments, NOT RELEVANT FOR NOOP command

command_handle = processor.issue_command(command_name, args=arguments)

print(f"Issued command: {command_handle}")
