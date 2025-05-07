import os
import shutil
import xml.etree.ElementTree as ET

# Component to XML mapping
components = {
    "arducam": "cam",
    "cryptolib": "radio",
    "generic_adcs": "adcs",
    "generic_css": "css",
    "generic_eps": "eps",
    "generic_fss": "fss",
    "generic_imu": "imu",
    "generic_mag": "mag",
    "generic_radio": "radio",
    "generic_reaction_wheel": "rw",
    "generic_star_tracker": "st",
    "generic_thruster": "thruster",
    "generic_torquer": "torquer",
    "mgr": "mgr",
    "novatel_oem615": "gps",
    "onair": "onair",
    "sample": "sample",
    "syn": "syn"
}

def clean_target_lines(input_file, output_file, sc_root):
    """Remove TARGET lines for disabled components (and their _RADIO versions)."""
    with open(input_file, 'r') as f:
        lines = f.readlines()

    lines_to_remove = set()

    for comp_key, xml_name in components.items():
        component_upper = comp_key.upper()
        node = sc_root.find(f"components/{xml_name}/enable")
        enabled = node is not None and node.text.strip().lower() == 'true'

        if enabled:
            continue

        print(f"[REMOVE] {comp_key} disabled — removing TARGET lines.")
        lines_to_remove.update({
            f"TARGET {component_upper}",
            f"TARGET {component_upper}_RADIO"
        })

        # Special case for 'syn' → SYNOPSIS
        if comp_key == "syn":
            lines_to_remove.update({
                "TARGET SYNOPSIS",
                "TARGET SYNOPSIS_RADIO"
            })

    # Filter out unwanted lines
    filtered_lines = [line for line in lines if line.strip() not in lines_to_remove]

    with open(output_file, 'w') as f:
        f.writelines(filtered_lines)

def main():
    # === Parse XML to get SC config ===
    mission_file = 'nos3-mission.xml'
    mission_path = "./cfg/build/temp_mission/" + os.path.basename(mission_file)
    
    try:
        mission_tree = ET.parse(mission_path)
        mission_root = mission_tree.getroot()
        sc_cfg = mission_root.find("sc-1-cfg").text
        sc_cfg_path = './cfg/' + sc_cfg
        sc_tree = ET.parse(sc_cfg_path)
        sc_root = sc_tree.getroot()
    except Exception as e:
        print(f"Error parsing XML: {e}")
        return

    # === File paths ===
    input_path = './gsw/cosmos/config/tools/cmd_tlm_server/stash/cmd_tlm_server.txt'
    output_path = './gsw/cosmos/config/tools/cmd_tlm_server/cmd_tlm_server.txt'

    shutil.copyfile(input_path, output_path)

    clean_target_lines(output_path, output_path, sc_root)

if __name__ == '__main__':
    main()
