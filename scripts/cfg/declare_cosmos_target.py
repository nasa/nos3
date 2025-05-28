import os
import shutil
import xml.etree.ElementTree as ET

# Map component names to their XML identifiers
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

def clean_declare_targets(input_file, output_file, sc_root, component_key, xml_name):
    """Remove DECLARE_TARGET lines related to a disabled component."""
    component_upper = component_key.upper()
    node = sc_root.find(f"components/{xml_name}/enable")
    enabled = node is not None and node.text.strip().lower() == 'true'

    if enabled:
        return

    # Lines to remove if the component is disabled
    patterns_to_remove = {
        f'DECLARE_TARGET ../../COMPONENTS/{component_upper} {component_upper}',
        f'DECLARE_TARGET ../../COMPONENTS/GENERIC_{component_upper} GENERIC_{component_upper}',
        f'DECLARE_TARGET ../../COMPONENTS/{component_upper} {component_upper}_RADIO',
        f'DECLARE_TARGET ../../COMPONENTS/GENERIC_{component_upper} GENERIC_{component_upper}_RADIO',
        f'DECLARE_TARGET ../../COMPONENTS/{component_upper} SYNOPSIS',
        f'DECLARE_TARGET ../../COMPONENTS/{component_upper} SYNOPSIS_RADIO'
    }

    with open(input_file, 'r') as f:
        lines = f.readlines()

    # Remove any line that matches exactly one of the patterns
    filtered_lines = [line for line in lines if line.strip() not in patterns_to_remove]

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
    input_path = './gsw/cosmos/config/system/stash/system.txt'
    output_path = './gsw/cosmos/config/system/system.txt'

    # Start fresh from stash copy
    shutil.copyfile(input_path, output_path)

    # Process each component
    for comp_key, xml_name in components.items():
        clean_declare_targets(output_path, output_path, sc_root, comp_key, xml_name)

if __name__ == '__main__':
    main()
