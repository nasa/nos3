#
# Convenience script for NOS3 development
# Configures NOS3 flight software (FSW) based on mission XML files
#   Script assumes run from top level directory of NOS3 repo
#

import datetime
import xml.etree.ElementTree as ET

# Parse mission configuration
mission_tree = ET.parse('./cfg/nos3-mission.xml')
mission_root = mission_tree.getroot()
mission_start_time = mission_root.find('start-time').text
print('  start-time:', mission_start_time)
mission_start_time_utc = datetime.datetime(2000, 1, 1, 12, 0) + datetime.timedelta(seconds=float(mission_start_time))
print('  start-time-utc:', mission_start_time_utc)

mission_number_spacecraft = mission_root.find('number-spacecraft').text
print('  number-spacecraft:', mission_number_spacecraft)
num_sc = int(mission_number_spacecraft)

# Check number of spacecraft valid
spacecraft_cfg = []
if (num_sc < 1):
    print('Invalid number of spacecraft in configuration file!')
    print('Exiting due to error...')
else:
    # Iterate through spacecraft configurations
    for x in range(1, int(mission_number_spacecraft) + 1):
        sc_str = 'sc-' + str(x) + '-cfg'
        sc_cfg = mission_root.find(sc_str).text
        #print(' ', sc_str, ':', sc_cfg)
        spacecraft_cfg.append(sc_cfg)

        # Open spacecraft configuration
        sc_cfg_str = './cfg/' + sc_cfg
        sc_tree = ET.parse(sc_cfg_str)
        sc_root = sc_tree.getroot()

        # Parse spacecraft configuration
        sc_cf_en = sc_root.find('applications/cf/enable').text
        sc_ds_en = sc_root.find('applications/ds/enable').text
        sc_fm_en = sc_root.find('applications/fm/enable').text
        sc_lc_en = sc_root.find('applications/lc/enable').text
        sc_sc_en = sc_root.find('applications/sc/enable').text

        sc_adcs_en = sc_root.find('components/adcs/enable').text
        sc_cam_en = sc_root.find('components/cam/enable').text
        sc_css_en = sc_root.find('components/css/enable').text
        sc_eps_en = sc_root.find('components/eps/enable').text
        sc_fss_en = sc_root.find('components/fss/enable').text
        sc_gps_en = sc_root.find('components/gps/enable').text
        sc_imu_en = sc_root.find('components/imu/enable').text
        sc_mag_en = sc_root.find('components/mag/enable').text
        sc_radio_en = sc_root.find('components/radio/enable').text
        sc_rw_en = sc_root.find('components/rw/enable').text
        sc_sample_en = sc_root.find('components/sample/enable').text
        sc_st_en = sc_root.find('components/st/enable').text
        sc_torquer_en = sc_root.find('components/torquer/enable').text

        sc_gui_en = sc_root.find('gui/enable').text
        sc_orbit_tipoff_x = sc_root.find('orbit/tipoff_x').text
        sc_orbit_tipoff_y = sc_root.find('orbit/tipoff_y').text
        sc_orbit_tipoff_z = sc_root.find('orbit/tipoff_z').text

        ###
        ### Flight Software - Startup Script
        ###
        
        # Capture lines to be used if enabled in startup script
        with open('./cfg/nos3_defs/cpu1_cfe_es_startup.scr', 'r') as fp:
            lines = fp.readlines()
            
            # Initialize variables
            sc_startup_eof = 999
            cf_line = ""
            ds_line = ""
            fm_line = ""
            lc_line = ""
            sc_line = ""
            adcs_line = ""
            cam_line = ""
            css_line = ""
            eps_line = ""
            fss_line = ""
            gps_line = ""
            imu_line = ""
            mag_line = ""
            radio_line = ""
            rw_line = ""
            sample_line = ""
            st_line = ""
            torquer_line = ""
            
            # Parse lines
            for line in lines:
                if line.find('!') != -1:
                    if (lines.index(line)) < sc_startup_eof:
                        sc_startup_eof = lines.index(line)
                if line.find('CF,') != -1:
                    if (sc_cf_en == 'true'):
                        cf_line = line
                if line.find('DS,') != -1:
                    if (sc_ds_en == 'true'):
                        ds_line = line
                if line.find('FM,') != -1:
                    if (sc_fm_en == 'true'):
                        fm_line = line
                if line.find('LC,') != -1:
                    if (sc_lc_en == 'true'):
                        lc_line = line
                if line.find('SC,') != -1:
                    if (sc_sc_en == 'true'):
                        sc_line = line
                if line.find('ADCS,') != -1:
                    if (sc_adcs_en == 'true'):
                        adcs_line = line
                if line.find('CAM,') != -1:
                    if (sc_cam_en == 'true'):
                        cam_line = line
                if line.find('CSS,') != -1:
                    if (sc_css_en == 'true'):
                        css_line = line
                if line.find('EPS,') != -1:
                    if (sc_eps_en == 'true'):
                        eps_line = line
                if line.find('FSS,') != -1:
                    if (sc_fss_en == 'true'):
                        fss_line = line
                if line.find('IMU,') != -1:
                    if (sc_imu_en == 'true'):
                        imu_line = line
                if line.find('MAG,') != -1:
                    if (sc_mag_en == 'true'):
                        mag_line = line
                if line.find('RADIO,') != -1:
                    if (sc_radio_en == 'true'):
                        radio_line = line
                if line.find('RW,') != -1:
                    if (sc_rw_en == 'true'):
                        rw_line = line
                if line.find('NAV,') != -1:
                    if (sc_gps_en == 'true'):
                        gps_line = line
                if line.find('SAMPLE,') != -1:
                    if (sc_sample_en == 'true'):
                        sample_line = line
                if line.find('ST,') != -1:
                    if (sc_st_en == 'true'):
                        st_line = line
                if line.find('TORQUER,') != -1:
                    if (sc_torquer_en == 'true'):
                        torquer_line = line

        # Modify startup script per spacecraft configuration
        lines.insert(sc_startup_eof, "\n")
        lines.insert(sc_startup_eof, torquer_line)
        lines.insert(sc_startup_eof, st_line)
        lines.insert(sc_startup_eof, sample_line)
        lines.insert(sc_startup_eof, rw_line)
        lines.insert(sc_startup_eof, radio_line)
        lines.insert(sc_startup_eof, mag_line)
        lines.insert(sc_startup_eof, imu_line)
        lines.insert(sc_startup_eof, gps_line)
        lines.insert(sc_startup_eof, fss_line)
        lines.insert(sc_startup_eof, eps_line)
        lines.insert(sc_startup_eof, css_line)
        lines.insert(sc_startup_eof, cam_line)
        lines.insert(sc_startup_eof, adcs_line)
        lines.insert(sc_startup_eof, sc_line)
        lines.insert(sc_startup_eof, lc_line)
        lines.insert(sc_startup_eof, fm_line)
        lines.insert(sc_startup_eof, ds_line)
        lines.insert(sc_startup_eof, cf_line)
                        
        # Write startup script file
        with open('./cfg/build/nos3_defs/cpu1_cfe_es_startup.scr', 'w') as fp:
            lines = "".join(lines)
            fp.write(lines)

        ###
        ### 42 - InOut Files
        ###

        # Inp_Sim.txt
        gui_index = 999
        date_index = 999
        time_index = 999
        with open('./cfg/InOut/Inp_Sim.txt', 'r') as fp:
            lines = fp.readlines()
            for line in lines:
                if line.find('Graphics Front End') != -1:
                    if (lines.index(line)) < gui_index:
                        gui_index = lines.index(line)
                if line.find('Date (UTC)') != -1:
                    if (lines.index(line)) < date_index:
                        date_index = lines.index(line)
                if line.find('Time (UTC)') != -1:
                    if (lines.index(line)) < time_index:
                        time_index = lines.index(line)

        if (sc_gui_en == 'false'):
            lines[gui_index] = 'FALSE                           !  Graphics Front End?\n'

        lines[date_index] = mission_start_time_utc.strftime('%m %d %Y') + '  !  Date (UTC) (Month, Day, Year)\n'
        lines[time_index] = mission_start_time_utc.strftime('%H %M %S') + '  !  Time (UTC) (Hr,Min,Sec)\n'

        with open('./cfg/build/InOut/Inp_Sim.txt', 'w') as fp:
            lines = "".join(lines)
            fp.write(lines)

        # SC_NOS3.txt
        tipoff_index = 999
        with open('./cfg/InOut/SC_NOS3.txt', 'r') as fp:
            lines = fp.readlines()
            for line in lines:
                if line.find('Ang Vel (deg/sec)') != -1:
                    if (lines.index(line)) < tipoff_index:
                        tipoff_index = lines.index(line)
        
        lines[tipoff_index] = sc_orbit_tipoff_x + ' ' + sc_orbit_tipoff_y + ' ' + sc_orbit_tipoff_z + '  ! Ang Vel (deg/sec)\n'

        with open('./cfg/build/InOut/SC_NOS3.txt', 'w') as fp:
            lines = "".join(lines)
            fp.write(lines)

        # Inp_IPC.txt
        css_index = 999
        fss_index = 999
        gps_index = 999
        imu_index = 999
        mag_index = 999
        rw0_to_index = 999
        rw0_from_index = 999
        rw1_to_index = 999
        rw1_from_index = 999
        rw2_to_index = 999
        rw2_from_index = 999
        sample_index = 999
        st_index = 999
        torquer_index = 999

        with open('./cfg/InOut/Inp_IPC.txt', 'r') as fp:
            lines = fp.readlines()
            for line in lines:
                if line.find('CSS IPC') != -1:
                    if (lines.index(line)) < css_index:
                        css_index = lines.index(line) + 1
                if line.find('FSS IPC') != -1:
                    if (lines.index(line)) < fss_index:
                        fss_index = lines.index(line) + 1
                if line.find('GPS IPC') != -1:
                    if (lines.index(line)) < gps_index:
                        gps_index = lines.index(line) + 1
                if line.find('IMU IPC') != -1:
                    if (lines.index(line)) < imu_index:
                        imu_index = lines.index(line) + 1
                if line.find('MAG IPC') != -1:
                    if (lines.index(line)) < mag_index:
                        mag_index = lines.index(line) + 1
                if line.find('RW 0 to 42') != -1:
                    if (lines.index(line)) < rw0_to_index:
                        rw0_to_index = lines.index(line) + 1
                if line.find('RW 0 from 42') != -1:
                    if (lines.index(line)) < rw0_from_index:
                        rw0_from_index = lines.index(line) + 1
                if line.find('RW 1 to 42') != -1:
                    if (lines.index(line)) < rw1_to_index:
                        rw1_to_index = lines.index(line) + 1
                if line.find('RW 1 from 42') != -1:
                    if (lines.index(line)) < rw1_from_index:
                        rw1_from_index = lines.index(line) + 1
                if line.find('RW 2 to 42') != -1:
                    if (lines.index(line)) < rw2_to_index:
                        rw2_to_index = lines.index(line) + 1
                if line.find('RW 2 from 42') != -1:
                    if (lines.index(line)) < rw2_from_index:
                        rw2_from_index = lines.index(line) + 1
                if line.find('Star Tracker IPC') != -1:
                    if (lines.index(line)) < st_index:
                        st_index = lines.index(line) + 1
                if line.find('Torquer IPC') != -1:
                    if (lines.index(line)) < torquer_index:
                        torquer_index = lines.index(line) + 1
        
        ipc_off = 'OFF                                     ! IPC Mode (OFF,TX,RX,TXRX,ACS,WRITEFILE,READFILE)\n'
        if (sc_css_en != 'true'):
            lines[css_index] = ipc_off
        if (sc_fss_en != 'true'):
            lines[fss_index] = ipc_off
        if (sc_gps_en != 'true'):
            lines[gps_index] = ipc_off
        if (sc_imu_en != 'true'):
            lines[imu_index] = ipc_off
        if (sc_mag_en != 'true'):
            lines[mag_index] = ipc_off
        if (sc_rw_en != 'true'):
            lines[rw0_to_index] = ipc_off
            lines[rw0_from_index] = ipc_off
            lines[rw1_to_index] = ipc_off
            lines[rw1_from_index] = ipc_off
            lines[rw2_to_index] = ipc_off
            lines[rw2_from_index] = ipc_off
        if (sc_sample_en != 'true'):
            lines[sample_index] = ipc_off
        if (sc_st_en != 'true'):
            lines[st_index] = ipc_off
        if (sc_torquer_en != 'true'):
            lines[torquer_index] = ipc_off

        with open('./cfg/build/InOut/Inp_IPC.txt', 'w') as fp:
            lines = "".join(lines)
            fp.write(lines)

        ###
        ### Simulators - nos3-simulator.xml
        ###
        cam_index = 999
        css_index = 999
        eps_index = 999
        fss_index = 999
        gps_index = 999
        imu_index = 999
        mag_index = 999
        radio_index = 999
        rw0_index = 999
        rw1_index = 999
        rw2_index = 999
        sample_index = 999
        st_index = 999
        torquer_index = 999

        with open('./cfg/build/sims/nos3-simulator.xml', 'r') as fp:
            lines = fp.readlines()
            for line in lines:
                if line.find('camsim</name>') != -1:
                    if (lines.index(line)) < cam_index:
                        cam_index = lines.index(line) + 1
                if line.find('css_sim</name>') != -1:
                    if (lines.index(line)) < css_index:
                        css_index = lines.index(line) + 1
                if line.find('eps_sim</name>') != -1:
                    if (lines.index(line)) < eps_index:
                        eps_index = lines.index(line) + 1
                if line.find('fss_sim</name>') != -1:
                    if (lines.index(line)) < fss_index:
                        fss_index = lines.index(line) + 1
                if line.find('gps</name>') != -1:
                    if (lines.index(line)) < gps_index:
                        gps_index = lines.index(line) + 1
                if line.find('imu_sim</name>') != -1:
                    if (lines.index(line)) < imu_index:
                        imu_index = lines.index(line) + 1
                if line.find('mag_sim</name>') != -1:
                    if (lines.index(line)) < mag_index:
                        mag_index = lines.index(line) + 1
                if line.find('radio_sim</name>') != -1:
                    if (lines.index(line)) < radio_index:
                        radio_index = lines.index(line) + 1
                if line.find('reactionwheel-sim0</name>') != -1:
                    if (lines.index(line)) < rw0_index:
                        rw0_index = lines.index(line) + 1
                if line.find('reactionwheel-sim1</name>') != -1:
                    if (lines.index(line)) < rw1_index:
                        rw1_index = lines.index(line) + 1
                if line.find('reactionwheel-sim2</name>') != -1:
                    if (lines.index(line)) < rw2_index:
                        rw2_index = lines.index(line) + 1
                if line.find('sample_sim</name>') != -1:
                    if (lines.index(line)) < sample_index:
                        sample_index = lines.index(line) + 1
                if line.find('star_tracker_sim</name>') != -1:
                    if (lines.index(line)) < st_index:
                        st_index = lines.index(line) + 1
                if line.find('generic_torquer_sim</name>') != -1:
                    if (lines.index(line)) < torquer_index:
                        torquer_index = lines.index(line) + 1

        sim_disabled = '            <active>false</active>\n'
        if (sc_cam_en != 'true'):
            lines[css_index] = sim_disabled
        if (sc_css_en != 'true'):
            lines[css_index] = sim_disabled
        if (sc_eps_en != 'true'):
            lines[css_index] = sim_disabled
        if (sc_fss_en != 'true'):
            lines[fss_index] = sim_disabled
        if (sc_gps_en != 'true'):
            lines[gps_index] = sim_disabled
        if (sc_imu_en != 'true'):
            lines[imu_index] = sim_disabled
        if (sc_mag_en != 'true'):
            lines[mag_index] = sim_disabled
        if (sc_radio_en != 'true'):
            lines[mag_index] = sim_disabled
        if (sc_rw_en != 'true'):
            lines[rw0_index] = sim_disabled
            lines[rw1_index] = sim_disabled
            lines[rw2_index] = sim_disabled
        if (sc_sample_en != 'true'):
            lines[sample_index] = sim_disabled
        if (sc_st_en != 'true'):
            lines[st_index] = sim_disabled
        if (sc_torquer_en != 'true'):
            lines[torquer_index] = sim_disabled

        with open('./cfg/build/sims/nos3-simulator.xml', 'w') as fp:
            lines = "".join(lines)
            fp.write(lines)
