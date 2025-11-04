import customtkinter as ctk
import tkinter as tk
from tkinter import filedialog, messagebox
import xml.etree.ElementTree as ET
from xml.dom import minidom
import os
from datetime import datetime, timedelta, timezone
from PIL import Image

from classes.add_dialog import SimpleNameDialog
from classes.datetime_dialog import DateTimeDialog

J2000_EPOCH = datetime(2000, 1, 1, 12, 0, 0, tzinfo=timezone.utc)
J2000_TIMESTAMP = J2000_EPOCH.timestamp()

class NOS3ConfigGUI:
    # Update the __init__ method to initialize mission data
    def __init__(self):
        # Set appearance mode and color theme
        ctk.set_appearance_mode("dark")  # or "light"
        ctk.set_default_color_theme("./cfg/gui/resources/orange.json")  # or "green", "dark-blue"
        
        # Create main window
        self.root = ctk.CTk()
        self.root.title("NOS3 IGNITER")
        self.root.geometry("1200x800")
        self.root.minsize(800, 600)
        
        # Set window icon
        self.set_window_icon()
        
        # Data structures
        # Data structures
        self.current_mission_file = None
        self.current_spacecraft_file = None
        self.spacecraft_config_path = None  # Full path to spacecraft config
        self.apps_data = {}  # Dictionary of app_name: enabled
        self.components_data = {}  # Dictionary of component_name: enabled
        self.mission_data = {}  # Mission configuration data
        self.additional_data = {}
        self.modified = False
        
        # setup options selections
        self.default_mission_config = "./cfg/nos3-mission.xml"
        self.fsw_options = ["cfs", "fprime"]
        self.gsw_options = ["cosmos", "openc3", "fprime", "yamcs"]
        config_dir = "./cfg/spacecraft/"
        self.config_filenames = {
            filename for filename in os.listdir(config_dir) if os.path.isfile(os.path.join(config_dir, filename))
        }
        
        # Initialize mission variables (these will be used by the mission config tab)
        self.gsw_var = None
        self.fsw_var = None
        self.sc_count_var = None
        self.sc1_config_var = None
        
        self.setup_ui()
            
    def set_window_icon(self):
        """Set the window icon with proper processing"""
        try:
            icon_path = "./cfg/gui/resources/nos3_original.png"
            
            if os.path.exists(icon_path):
                # Open with PIL and process
                with Image.open(icon_path) as img:
                    # Resize to standard icon size (use Image.LANCZOS instead of Image.Resampling.LANCZOS)
                    img = img.resize((64, 64), Image.LANCZOS)
                    
                    # Convert RGBA to RGB with white background if it has transparency
                    if img.mode in ('RGBA', 'LA'):
                        background = Image.new('RGB', img.size, (255, 255, 255)) # type: ignore
                        if img.mode == 'RGBA':
                            background.paste(img, mask=img.split()[-1])
                        else:
                            background.paste(img)
                        img = background
                    
                    # Save processed image temporarily
                    temp_path = "./cfg/gui/resources/temp_icon.png"
                    img.save(temp_path, "PNG")
                    
                    # Load with tkinter
                    icon = tk.PhotoImage(file=temp_path)
                    self.root.iconphoto(False, icon)
                    
                    # Clean up
                    os.remove(temp_path)
                    
            else:
                print(f"Icon file not found: {icon_path}")
                
        except Exception as e:
            print(f"Could not set window icon: {e}")
        
    def setup_ui(self):
        # Create main frame
        self.main_frame = ctk.CTkFrame(self.root)
        self.main_frame.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Setup logos first
        self.setup_logos()
        self.add_header_logo()
    
        # Create menu bar
        self.create_menu_bar()
        
        # Create notebook for tabs
        self.notebook = ctk.CTkTabview(self.main_frame)
        self.notebook.pack(fill="both", expand=True, padx=10, pady=(50, 10))
        
        # Create tabs
        self.mission_tab = self.notebook.add("Mission Config")
        self.apps_tab = self.notebook.add("Applications")
        self.components_tab = self.notebook.add("Components")
        self.additional_tab = self.notebook.add("Additional Options")
        self.preview_tab = self.notebook.add("XML Preview")
        
        self.setup_mission_tab()
        self.add_tab_logos
        self.setup_apps_tab()
        self.setup_components_tab()
        self.setup_additional_tab()
        self.setup_preview_tab()
        
        # Status bar
        self.status_frame = ctk.CTkFrame(self.main_frame, height=30)
        self.status_frame.pack(fill="x", padx=10, pady=(0, 10))
        self.status_frame.pack_propagate(False)
        
        self.status_label = ctk.CTkLabel(self.status_frame, text="Ready - Open a mission configuration file to begin")
        self.status_label.pack(side="left", padx=10, pady=5)
        
        self.root.after(100, self.open_mission, True)

    def setup_additional_tab(self):
        """Setup the Additional Options tab for GUI, Orbit, and Sim configurations"""
        # Create main frame for the additional options tab
        self.additional_frame = ctk.CTkFrame(self.additional_tab)
        self.additional_frame.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Create a scrollable frame
        self.additional_scrollable = ctk.CTkScrollableFrame(self.additional_frame)
        self.additional_scrollable.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Header
        header_label = ctk.CTkLabel(self.additional_scrollable, text="Additional Spacecraft Options", 
                                font=ctk.CTkFont(size=16, weight="bold"))
        header_label.pack(pady=(5, 20))
        
        # GUI Section
        self.setup_gui_section()
        
        # Orbit Section
        self.setup_orbit_section()
        
        # Sim Section
        self.setup_sim_section()

    def setup_gui_section(self):
        """Setup the GUI configuration section"""
        # GUI Frame
        gui_frame = ctk.CTkFrame(self.additional_scrollable)
        gui_frame.pack(fill="x", pady=10, padx=5)
        
        gui_label = ctk.CTkLabel(gui_frame, text="GUI Configuration:", 
                                font=ctk.CTkFont(size=14, weight="bold"))
        gui_label.pack(anchor="w", padx=10, pady=(10, 5))
        
        # GUI Enable checkbox
        self.gui_enabled_var = tk.BooleanVar(value=True)
        self.gui_enabled_check = ctk.CTkCheckBox(gui_frame, text="Enable GUI", 
                                            variable=self.gui_enabled_var,
                                            command=self.on_gui_change)
        self.gui_enabled_check.pack(anchor="w", padx=20, pady=5)

    def setup_orbit_section(self):
        """Setup the Orbit configuration section"""
        # Orbit Frame
        orbit_frame = ctk.CTkFrame(self.additional_scrollable)
        orbit_frame.pack(fill="x", pady=10, padx=5)
        
        orbit_label = ctk.CTkLabel(orbit_frame, text="Orbit Configuration:", 
                                font=ctk.CTkFont(size=14, weight="bold"))
        orbit_label.pack(anchor="w", padx=10, pady=(10, 5))
        
        # Orbit parameters in a grid-like layout
        params_frame = ctk.CTkFrame(orbit_frame)
        params_frame.pack(fill="x", padx=10, pady=5)
        
        # Tipoff X
        tipoff_x_frame = ctk.CTkFrame(params_frame)
        tipoff_x_frame.pack(fill="x", pady=2)
        
        ctk.CTkLabel(tipoff_x_frame, text="Tipoff X:", width=100).pack(side="left", padx=10, pady=5)
        self.tipoff_x_entry = ctk.CTkEntry(tipoff_x_frame, width=100, placeholder_text="0.2")
        self.tipoff_x_entry.pack(side="left", padx=5, pady=5)
        self.tipoff_x_entry.insert(0, "0.2")
        self.tipoff_x_entry.bind('<KeyRelease>', lambda e: self.set_modified())
        
        ctk.CTkLabel(tipoff_x_frame, text="(degrees/second)").pack(side="left", padx=5, pady=5)
        
        # Tipoff Y
        tipoff_y_frame = ctk.CTkFrame(params_frame)
        tipoff_y_frame.pack(fill="x", pady=2)
        
        ctk.CTkLabel(tipoff_y_frame, text="Tipoff Y:", width=100).pack(side="left", padx=10, pady=5)
        self.tipoff_y_entry = ctk.CTkEntry(tipoff_y_frame, width=100, placeholder_text="2.0")
        self.tipoff_y_entry.pack(side="left", padx=5, pady=5)
        self.tipoff_y_entry.insert(0, "2.0")
        self.tipoff_y_entry.bind('<KeyRelease>', lambda e: self.set_modified())
        
        ctk.CTkLabel(tipoff_y_frame, text="(degrees/second)").pack(side="left", padx=5, pady=5)
        
        # Tipoff Z
        tipoff_z_frame = ctk.CTkFrame(params_frame)
        tipoff_z_frame.pack(fill="x", pady=2)
        
        ctk.CTkLabel(tipoff_z_frame, text="Tipoff Z:", width=100).pack(side="left", padx=10, pady=5)
        self.tipoff_z_entry = ctk.CTkEntry(tipoff_z_frame, width=100, placeholder_text="-2.0")
        self.tipoff_z_entry.pack(side="left", padx=5, pady=5)
        self.tipoff_z_entry.insert(0, "-2.0")
        self.tipoff_z_entry.bind('<KeyRelease>', lambda e: self.set_modified())
        
        ctk.CTkLabel(tipoff_z_frame, text="(degrees/second)").pack(side="left", padx=5, pady=5)

    def setup_sim_section(self):
        """Setup the Simulation configuration section"""
        # Sim Frame
        sim_frame = ctk.CTkFrame(self.additional_scrollable)
        sim_frame.pack(fill="x", pady=10, padx=5)
        
        sim_label = ctk.CTkLabel(sim_frame, text="Simulation Configuration:", 
                                font=ctk.CTkFont(size=14, weight="bold"))
        sim_label.pack(anchor="w", padx=10, pady=(10, 5))
        
        # Sim Truth Interface
        sim_truth_frame = ctk.CTkFrame(sim_frame)
        sim_truth_frame.pack(fill="x", padx=10, pady=5)
        
        ctk.CTkLabel(sim_truth_frame, text="Sim Truth Interface:", width=150).pack(side="left", padx=10, pady=5)
        
        self.sim_truth_var = tk.BooleanVar(value=True)
        self.sim_truth_check = ctk.CTkCheckBox(sim_truth_frame, text="Enable", 
                                            variable=self.sim_truth_var,
                                            command=self.on_sim_change)
        self.sim_truth_check.pack(side="left", padx=10, pady=5)

    # Add methods to handle changes
    def on_gui_change(self):
        """Handle GUI configuration changes"""
        self.set_modified()

    def on_sim_change(self):
        """Handle Sim configuration changes"""
        self.set_modified()
    
    # Add this method to the NOS3ConfigGUI class
    def setup_mission_tab(self):
        # Create main frame for the mission tab
        self.mission_frame = ctk.CTkFrame(self.mission_tab)
        self.mission_frame.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Create a scrollable frame for mission configuration
        self.mission_scrollable = ctk.CTkScrollableFrame(self.mission_frame)
        self.mission_scrollable.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Mission Configuration Header
        header_label = ctk.CTkLabel(self.mission_scrollable, text="NOS3 Mission Configuration", 
                                font=ctk.CTkFont(size=16, weight="bold"))
        header_label.pack(pady=(5, 20))
        
        # Mission Start Time Section
        time_frame = ctk.CTkFrame(self.mission_scrollable)
        time_frame.pack(fill="x", pady=10, padx=5)
        
        time_label = ctk.CTkLabel(time_frame, text="Mission Start Time:", 
                                font=ctk.CTkFont(size=14, weight="bold"))
        time_label.pack(anchor="w", padx=10, pady=(5, 0))
        
        time_help = ctk.CTkLabel(time_frame, text="J2000 format: seconds since Jan 1, 2000, 12:00:00 UTC")
        time_help.pack(anchor="w", padx=10, pady=(0, 5))
        
        # Time input row
        time_input_frame = ctk.CTkFrame(time_frame)
        time_input_frame.pack(fill="x", padx=10, pady=5)
        
        # Time entry
        ctk.CTkLabel(time_input_frame, text="J2000 Time:").pack(side="left", padx=(0, 5))
        self.mission_time_entry = ctk.CTkEntry(time_input_frame, width=200)
        self.mission_time_entry.pack(side="left", padx=(0, 10))
        
        # Current time button
        current_time_btn = ctk.CTkButton(time_input_frame, text="Use Current Time", 
                                    command=self.set_current_mission_time,
                                    width=130)
        current_time_btn.pack(side="left", padx=5)
        
        # Set date button
        set_date_btn = ctk.CTkButton(time_input_frame, text="Set Date...", 
                                command=self.set_mission_date,
                                width=100)
        set_date_btn.pack(side="left", padx=5)
        
        # Display time in human-readable format
        self.time_display_label = ctk.CTkLabel(time_frame, text="", wraplength=800)
        self.time_display_label.pack(anchor="w", padx=10, pady=5)
        
        # Ground Software Section
        gsw_frame = ctk.CTkFrame(self.mission_scrollable)
        gsw_frame.pack(fill="x", pady=10, padx=5)
        
        gsw_label = ctk.CTkLabel(gsw_frame, text="Ground Software:", 
                                font=ctk.CTkFont(size=14, weight="bold"))
        gsw_label.pack(anchor="w", padx=10, pady=5)
        
        # Ground software options
        self.gsw_var = tk.StringVar()
        
        # Radio buttons for ground software
        for option in self.gsw_options:
            rb = ctk.CTkRadioButton(gsw_frame, text=option, variable=self.gsw_var, value=option)
            rb.pack(anchor="w", padx=20, pady=2)
        
        # Flight Software Section
        fsw_frame = ctk.CTkFrame(self.mission_scrollable)
        fsw_frame.pack(fill="x", pady=10, padx=5)
        
        fsw_label = ctk.CTkLabel(fsw_frame, text="Flight Software:", 
                                font=ctk.CTkFont(size=14, weight="bold"))
        fsw_label.pack(anchor="w", padx=10, pady=5)
        
        # Flight software options
        self.fsw_var = tk.StringVar()
        
        # Radio buttons for flight software
        for option in self.fsw_options:
            rb = ctk.CTkRadioButton(fsw_frame, text=option, variable=self.fsw_var, value=option)
            rb.pack(anchor="w", padx=20, pady=2)
        
        # Number of Spacecraft Section
        sc_frame = ctk.CTkFrame(self.mission_scrollable)
        sc_frame.pack(fill="x", pady=10, padx=5)
        
        sc_label = ctk.CTkLabel(sc_frame, text="Number of Spacecraft: (Multiple spacecraft not fully implemented)", 
                            font=ctk.CTkFont(size=14, weight="bold"))
        sc_label.pack(anchor="w", padx=10, pady=5)
        
        # Number of spacecraft input
        sc_input_frame = ctk.CTkFrame(sc_frame)
        sc_input_frame.pack(fill="x", padx=10, pady=5)
        
        self.sc_count_var = tk.StringVar()
        sc_count_values = [str(i) for i in range(1, 11)]  # 1-10 spacecraft
        
        sc_count_dropdown = ctk.CTkComboBox(sc_input_frame, values=sc_count_values, 
                                        variable=self.sc_count_var,
                                        width=100)
        sc_count_dropdown.pack(side="left", padx=5)
        
        # Spacecraft Configuration Section
        sc_config_frame = ctk.CTkFrame(self.mission_scrollable)
        sc_config_frame.pack(fill="x", pady=10, padx=5)
        
        sc_config_label = ctk.CTkLabel(sc_config_frame, text="Spacecraft Configuration:", 
                                    font=ctk.CTkFont(size=14, weight="bold"))
        sc_config_label.pack(anchor="w", padx=10, pady=5)
        
        # Frame to hold configuration options
        self.sc_config_options_frame = ctk.CTkFrame(sc_config_frame)
        self.sc_config_options_frame.pack(fill="x", padx=10, pady=5)
        
        # Configuration options
        self.sc1_config_var = tk.StringVar()
        
        # Radio buttons for spacecraft 1 config
        self.sc1_config_label = ctk.CTkLabel(self.sc_config_options_frame, text="Spacecraft 1:")
        self.sc1_config_label.pack(anchor="w", padx=10, pady=(5, 0))
        
        for filename in self.config_filenames:
            rb = ctk.CTkRadioButton(self.sc_config_options_frame, text=filename, 
                                variable=self.sc1_config_var, value=filename)
            rb.pack(anchor="w", padx=20, pady=2)
            
        self.sc1_config_var.trace_add("write", self.on_spacecraft_config_change_wrapper)
        
        # Initialize mission configuration data
        self.mission_data = {
            "start_time": "",
            "gsw": "",
            "fsw": "",
            "num_spacecraft": "1",
            "sc1_config": "",
            "scN_config": ""  # For spacecraft N config if more than 1
        }
        
        # Button to refresh the displayed info when spacecraft count changes
        self.sc_count_var.trace_add("write", self.update_spacecraft_config_display)
        
        # Set up change tracking for mission configuration
        self.setup_mission_change_tracking()
    
    # Add a wrapper function for the trace callback
    def on_spacecraft_config_change_wrapper(self, *args):
        """Wrapper for spacecraft config change to handle trace callback format"""
        self.on_spacecraft_config_change()
    
    # Update the update_spacecraft_config_display method to include command binding
    def update_spacecraft_config_display(self, *args):
        """Update the spacecraft configuration display based on the number of spacecraft"""
        # Clear existing widgets
        for widget in self.sc_config_options_frame.winfo_children():
            widget.destroy()
        
        num_spacecraft = int(self.sc_count_var.get() if self.sc_count_var.get() else "1") # type: ignore
        
        # Add configuration for each spacecraft
        for sc_num in range(1, num_spacecraft + 1):
            sc_label = ctk.CTkLabel(self.sc_config_options_frame, 
                                text=f"Spacecraft {sc_num}:", 
                                font=ctk.CTkFont(weight="bold"))
            sc_label.pack(anchor="w", padx=10, pady=(10, 0))
            
            # Create a StringVar for this spacecraft
            sc_var_name = f"sc{sc_num}_config_var"
            if not hasattr(self, sc_var_name):
                setattr(self, sc_var_name, tk.StringVar())
            
            sc_var = getattr(self, sc_var_name)
            
            # Radio buttons for spacecraft config
            for filename in self.config_filenames:
                rb = ctk.CTkRadioButton(self.sc_config_options_frame, text=filename, 
                                    variable=sc_var, value=filename, command=self.load_xml_file(f"./cfg/spacecraft/{filename}"))
                rb.pack(anchor="w", padx=20, pady=2)
        
        # Force update of the frame
        self.sc_config_options_frame.update_idletasks()

    def set_current_mission_time(self):
        """Set the mission time to the current time in J2000 format"""
        current_time = datetime.now(timezone.utc)
        j2000_seconds = (current_time - J2000_EPOCH).total_seconds()
        
        self.mission_time_entry.delete(0, 'end')
        self.mission_time_entry.insert(0, f"{j2000_seconds:.1f}")
        self.update_time_display()

    def set_mission_date(self):
        """Open a dialog to set the mission date"""
        dialog = DateTimeDialog(self.root, "Set Mission Date")
        if dialog.result:
            # dialog.result is a datetime object
            j2000_seconds = (dialog.result - J2000_EPOCH).total_seconds()
            self.mission_time_entry.delete(0, 'end')
            self.mission_time_entry.insert(0, f"{j2000_seconds:.1f}")
            self.update_time_display()

    def update_time_display(self):
        """Update the display of the mission time in human-readable format"""
        try:
            j2000_str = self.mission_time_entry.get().strip()
            if not j2000_str:
                self.time_display_label.configure(text="")
                return
                
            j2000_seconds = float(j2000_str)
            
            # Convert J2000 seconds to datetime
            mission_time = J2000_EPOCH + timedelta(seconds=j2000_seconds)
            
            # Format the display
            formatted_date = mission_time.strftime("%d %b %Y %H:%M:%S UTC")
            
            # Calculate years since J2000 epoch
            years_since_j2000 = j2000_seconds / (365.25 * 24 * 3600)
            
            display_text = (f"Date: {formatted_date}\n"
                        f"Years since J2000 Epoch: {years_since_j2000:.2f}")
            
            self.time_display_label.configure(text=display_text)
            
        except ValueError:
            self.time_display_label.configure(text="Invalid J2000 timestamp format")

    # Update the load_xml_file method to handle both mission and spacecraft config XML files
    def load_xml_file(self, filename):
        tree = ET.parse(filename)
        root = tree.getroot()
        
        # Check if this is a mission config or spacecraft config file
        if root.tag == 'nos3-mission-cfg':
            self.load_mission_config(tree)
            self.notebook.set("Mission Config")  # Switch to mission config tab
        elif root.tag == 'sc-1-config':
            # Clear existing data
            self.apps_data = {}
            self.components_data = {}
            
            # Load applications
            apps_element = root.find('applications')
            if apps_element is not None:
                for app_element in apps_element:
                    app_name = app_element.tag
                    enable_element = app_element.find('enable')
                    enabled = True  # Default to True if not specified
                    if enable_element is not None:
                        enabled = enable_element.text.lower() == 'true' # type: ignore
                    self.apps_data[app_name] = enabled
                    
            # Load components
            components_element = root.find('components')
            if components_element is not None:
                for comp_element in components_element:
                    comp_name = comp_element.tag
                    enable_element = comp_element.find('enable')
                    enabled = True  # Default to True if not specified
                    if enable_element is not None:
                        enabled = enable_element.text.lower() == 'true' # type: ignore
                    self.components_data[comp_name] = enabled
            
            self.refresh_displays()
            # self.notebook.set("Applications")  # Switch to applications tab
        else:
            messagebox.showerror("Error", "Unknown XML format")

    # Update the save_xml_file method to handle both mission and spacecraft config files
    def save_xml_file(self, filename):
        # Determine if we're saving a mission config or spacecraft config
        is_mission_config = filename.endswith('nos3-mission.xml') or self.notebook.get() == "Mission Config"
        
        if is_mission_config:
            # Create mission config XML
            root = ET.Element('nos3-mission-cfg')
            
            # Add XML comments as in the example
            comment1 = ET.Comment(" Mission Start Time (12000 UTC) ")
            root.append(comment1)
            
            # Get the timestamp value
            time_str = self.mission_time_entry.get().strip()
            if time_str:
                try:
                    timestamp = float(time_str)
                    dt = datetime.fromtimestamp(timestamp)
                    date_str = dt.strftime("%d %b %Y")
                    comment2 = ET.Comment(f" Default time: {time_str}, {date_str} ")
                    root.append(comment2)
                except ValueError:
                    pass
            
            # Add start time
            start_time = ET.SubElement(root, 'start-time')
            start_time.text = self.mission_time_entry.get().strip()
            
            # Add ground software section
            gsw_comment = ET.Comment(" Ground Software ")
            root.append(gsw_comment)
            
            options_comment = ET.Comment(" cosmos (default), openc3, fprime, or yamcs ")
            root.append(options_comment)
            
            gsw = ET.SubElement(root, 'gsw')
            gsw.text = self.gsw_var.get() # type: ignore
            
            # Add flight software section
            fsw_comment = ET.Comment(" Flight Software ")
            root.append(fsw_comment)
            
            fsw_options_comment = ET.Comment(" cfs (default) or fprime ")
            root.append(fsw_options_comment)
            
            fsw = ET.SubElement(root, 'fsw')
            fsw.text = self.fsw_var.get() # type: ignore
            
            # Add number of spacecraft
            num_sc_comment = ET.Comment(" Number of spacecraft ")
            root.append(num_sc_comment)
            
            experimental_comment = ET.Comment(" Note this is experimental and not ready for use beyond proof of concept ")
            root.append(experimental_comment)
            
            num_sc = ET.SubElement(root, 'number-spacecraft')
            num_sc.text = self.sc_count_var.get() # type: ignore
            
            # Add spacecraft configurations
            sc_num = int(self.sc_count_var.get()) # type: ignore
            
            # Add SC1 configuration
            sc1_comment = ET.Comment(" Spacecraft 1 Configuration - options are as follows ")
            root.append(sc1_comment)
            
            for filename in self.config_filenames:
                root.append(ET.Comment(f" {filename} "))
            
            sc1_cfg = ET.SubElement(root, 'sc-1-cfg')
            selected_option = self.sc1_config_var.get() # type: ignore
            if selected_option in self.config_filenames:
                for filename in self.config_filenames:
                    if filename == selected_option:
                        sc1_cfg.text = filename
            
            # Add SCN configuration if more than one spacecraft
            if sc_num > 1:
                scn_comment = ET.Comment(" Spacecraft N Configuration ")
                root.append(scn_comment)
                
                scn_cfg_comment = ET.Comment(" <sc-N-cfg>sc-minimal-config.xml</sc-N-cfg> ")
                root.append(scn_cfg_comment)
                
                # For simplicity, we'll use the same config for all additional spacecraft
                # In a real implementation, you might want to handle each spacecraft separately
            
            # XML formatting
            xml_str = '<?xml version="1.0" encoding="utf-8"?>\n'
            rough_string = ET.tostring(root, 'utf-8')
            reparsed = minidom.parseString(rough_string)
            pretty_xml = reparsed.toprettyxml(indent="    ")
            
            # Remove the XML declaration that parseString adds since we'll add our own
            pretty_xml = '\n'.join(pretty_xml.split('\n')[1:])
            
            with open(filename, 'w', encoding='utf-8') as f:
                f.write(xml_str + pretty_xml)
        else:
            # Create spacecraft config XML
            root = ET.Element('sc-1-config')
            
            # Add applications
            apps_element = ET.SubElement(root, 'applications')
            for app_name, enabled in self.apps_data.items():
                app_element = ET.SubElement(apps_element, app_name)
                enable_element = ET.SubElement(app_element, 'enable')
                enable_element.text = 'true' if enabled else 'false'
                    
            # Add components
            components_element = ET.SubElement(root, 'components')
            for comp_name, enabled in self.components_data.items():
                comp_element = ET.SubElement(components_element, comp_name)
                enable_element = ET.SubElement(comp_element, 'enable')
                enable_element.text = 'true' if enabled else 'false'
                    
            # XML formatting
            xml_str = '<?xml version="1.0" encoding="utf-8"?>\n'
            rough_string = ET.tostring(root, 'utf-8')
            reparsed = minidom.parseString(rough_string)
            pretty_xml = reparsed.toprettyxml(indent="    ")
            
            # Remove the XML declaration that parseString adds since we'll add our own
            pretty_xml = '\n'.join(pretty_xml.split('\n')[1:])
            
            with open(filename, 'w', encoding='utf-8') as f:
                f.write(xml_str + pretty_xml)

    # Add a refresh_preview method that handles the active tab
    def refresh_preview(self):
        try:
            active_tab = self.notebook.get()
            
            if active_tab == "Mission Config":
                # Generate mission XML preview
                root = ET.Element('nos3-mission-cfg')
                
                # Add comments as in the example
                comment1 = ET.Comment(" Mission Start Time (12000 UTC) ")
                root.append(comment1)
                
                time_str = self.mission_time_entry.get().strip()
                if time_str:
                    try:
                        timestamp = float(time_str)
                        dt = datetime.fromtimestamp(timestamp)
                        date_str = dt.strftime("%d %b %Y")
                        comment2 = ET.Comment(f" Default time: {time_str}, {date_str} ")
                        root.append(comment2)
                    except ValueError:
                        pass
                
                # Add start time
                start_time = ET.SubElement(root, 'start-time')
                start_time.text = self.mission_time_entry.get().strip()
                
                # Add ground software section
                gsw_comment = ET.Comment(" Ground Software ")
                root.append(gsw_comment)
                
                options_comment = ET.Comment(" cosmos (default), openc3, fprime, or yamcs ")
                root.append(options_comment)
                
                gsw = ET.SubElement(root, 'gsw')
                gsw.text = self.gsw_var.get() # type: ignore
                
                # Add flight software section
                fsw_comment = ET.Comment(" Flight Software ")
                root.append(fsw_comment)
                
                fsw_options_comment = ET.Comment(" cfs (default) or fprime ")
                root.append(fsw_options_comment)
                
                fsw = ET.SubElement(root, 'fsw')
                fsw.text = self.fsw_var.get() # type: ignore
                
                # Add number of spacecraft
                num_sc_comment = ET.Comment(" Number of spacecraft ")
                root.append(num_sc_comment)
                
                experimental_comment = ET.Comment(" Note this is experimental and not ready for use beyond proof of concept ")
                root.append(experimental_comment)
                
                num_sc = ET.SubElement(root, 'number-spacecraft')
                num_sc.text = self.sc_count_var.get() # type: ignore
                
                # Add spacecraft configurations
                sc_num = int(self.sc_count_var.get()) # type: ignore
                
                # Add SC1 configuration
                sc1_comment = ET.Comment(" Spacecraft 1 Configuration - options are as follows ")
                root.append(sc1_comment)
                
                for filename in self.config_filenames:
                    root.append(ET.Comment(f" {filename} "))
                
                sc1_cfg = ET.SubElement(root, 'sc-1-cfg')
                selected_option = self.sc1_config_var.get() # type: ignore
                if selected_option in self.config_filenames:
                    sc1_cfg.text = selected_option
                
                # Add SCN configuration if more than one spacecraft
                if sc_num > 1:
                    scn_comment = ET.Comment(" Spacecraft N Configuration ")
                    root.append(scn_comment)
                    
                    scn_cfg_comment = ET.Comment(" <sc-N-cfg>sc-minimal-config.xml</sc-N-cfg> ")
                    root.append(scn_cfg_comment)
            else:
                # Generate spacecraft XML preview
                root = ET.Element('sc-1-config')
                
                # Add applications
                apps_element = ET.SubElement(root, 'applications')
                for app_name, enabled in self.apps_data.items():
                    app_element = ET.SubElement(apps_element, app_name)
                    enable_element = ET.SubElement(app_element, 'enable')
                    enable_element.text = 'true' if enabled else 'false'
                        
                # Add components
                components_element = ET.SubElement(root, 'components')
                for comp_name, enabled in self.components_data.items():
                    comp_element = ET.SubElement(components_element, comp_name)
                    enable_element = ET.SubElement(comp_element, 'enable')
                    enable_element.text = 'true' if enabled else 'false'
                    
                # Add GUI section
                gui_element = ET.SubElement(root, 'gui')
                gui_enable = ET.SubElement(gui_element, 'enable')
                if hasattr(self, 'gui_enabled_var'):
                    gui_enable.text = 'true' if self.gui_enabled_var.get() else 'false'
                else:
                    gui_enable.text = 'true'
                
                # Add Orbit section
                orbit_element = ET.SubElement(root, 'orbit')
                
                tipoff_x = ET.SubElement(orbit_element, 'tipoff_x')
                if hasattr(self, 'tipoff_x_entry'):
                    tipoff_x.text = self.tipoff_x_entry.get() if self.tipoff_x_entry.get() else "0.2"
                else:
                    tipoff_x.text = "0.2"
                
                tipoff_y = ET.SubElement(orbit_element, 'tipoff_y')
                if hasattr(self, 'tipoff_y_entry'):
                    tipoff_y.text = self.tipoff_y_entry.get() if self.tipoff_y_entry.get() else "2.0"
                else:
                    tipoff_y.text = "2.0"
                
                tipoff_z = ET.SubElement(orbit_element, 'tipoff_z')
                if hasattr(self, 'tipoff_z_entry'):
                    tipoff_z.text = self.tipoff_z_entry.get() if self.tipoff_z_entry.get() else "-2.0"
                else:
                    tipoff_z.text = "-2.0"
                
                # Add Sim section
                sim_element = ET.SubElement(root, 'sim')
                sim_truth = ET.SubElement(sim_element, 'sim_truth_interface')
                if hasattr(self, 'sim_truth_var'):
                    sim_truth.text = 'true' if self.sim_truth_var.get() else 'false'
                else:
                    sim_truth.text = 'true'
            
            # Format XML
            xml_str = '<?xml version="1.0" encoding="utf-8"?>\n'
            rough_string = ET.tostring(root, 'utf-8')
            reparsed = minidom.parseString(rough_string)
            pretty_xml = reparsed.toprettyxml(indent="    ")
            
            # Remove the XML declaration that parseString adds since we'll add our own
            pretty_xml = '\n'.join(pretty_xml.split('\n')[1:])
            
            self.preview_text.delete('1.0', 'end')
            self.preview_text.insert('1.0', xml_str + pretty_xml)
            
        except Exception as e:
            self.preview_text.delete('1.0', 'end')
            self.preview_text.insert('1.0', f"Error generating preview: {str(e)}")
    
    def create_menu_bar(self):
        # Create menu frame
        self.menu_frame = ctk.CTkFrame(self.main_frame, height=40)
        self.menu_frame.pack(fill="x", padx=10, pady=(10, 0))
        self.menu_frame.pack_propagate(False)
        
        # File operations
        self.new_btn = ctk.CTkButton(self.menu_frame, text="New Mission", command=self.new_mission, width=100)
        self.new_btn.pack(side="left", padx=5, pady=5)
        
        self.open_btn = ctk.CTkButton(self.menu_frame, text="Open Mission", command=self.open_mission, width=100)
        self.open_btn.pack(side="left", padx=5, pady=5)
        
        self.save_btn = ctk.CTkButton(self.menu_frame, text="Save All", command=self.save_all, width=80)
        self.save_btn.pack(side="left", padx=5, pady=5)
        
        # Current files label
        self.files_frame = ctk.CTkFrame(self.menu_frame)
        self.files_frame.pack(side="right", padx=10, pady=5)
        
        self.mission_file_label = ctk.CTkLabel(self.files_frame, text="Mission: No file loaded")
        self.mission_file_label.pack(pady=2)
        
        self.spacecraft_file_label = ctk.CTkLabel(self.files_frame, text="Spacecraft: No file loaded")
        self.spacecraft_file_label.pack(pady=2)
        
    def setup_apps_tab(self):
        # Create frame for the applications list
        self.apps_frame = ctk.CTkFrame(self.apps_tab)
        self.apps_frame.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Header
        header_label = ctk.CTkLabel(self.apps_frame, text="Application Configuration", 
                                   font=ctk.CTkFont(size=16, weight="bold"))
        header_label.pack(pady=(10, 20))
        
        # Create a scrollable frame
        self.apps_scrollable = ctk.CTkScrollableFrame(self.apps_frame)
        self.apps_scrollable.pack(fill="both", expand=True, padx=10, pady=10)
        
        # No applications loaded message
        self.no_apps_label = ctk.CTkLabel(self.apps_scrollable, text="No applications loaded. Open a configuration file.")
        self.no_apps_label.pack(pady=20)
        
        # Dictionary to keep track of app checkboxes
        self.app_checkboxes = {}
        
    def setup_components_tab(self):
        # Create frame for the components list
        self.components_frame = ctk.CTkFrame(self.components_tab)
        self.components_frame.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Header
        header_label = ctk.CTkLabel(self.components_frame, text="Component Configuration", 
                                   font=ctk.CTkFont(size=16, weight="bold"))
        header_label.pack(pady=(10, 20))
        
        # Create a scrollable frame
        self.components_scrollable = ctk.CTkScrollableFrame(self.components_frame)
        self.components_scrollable.pack(fill="both", expand=True, padx=10, pady=10)
        
        # No components loaded message
        self.no_components_label = ctk.CTkLabel(self.components_scrollable, text="No components loaded. Open a configuration file.")
        self.no_components_label.pack(pady=20)
        
        # Dictionary to keep track of component checkboxes
        self.component_checkboxes = {}
        
    def setup_preview_tab(self):
        # XML preview
        preview_frame = ctk.CTkFrame(self.preview_tab)
        preview_frame.pack(fill="both", expand=True, padx=10, pady=10)
        
        preview_label = ctk.CTkLabel(preview_frame, text="XML Preview", 
                                    font=ctk.CTkFont(size=16, weight="bold"))
        preview_label.pack(pady=(10, 5))
        
        # Create textbox for XML preview
        self.preview_text = ctk.CTkTextbox(preview_frame, font=ctk.CTkFont(family="Courier", size=12))
        self.preview_text.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Refresh button
        refresh_btn = ctk.CTkButton(preview_frame, text="Refresh Preview", command=self.refresh_preview)
        refresh_btn.pack(pady=(5, 10))
    
    def new_mission(self):
        if self.check_unsaved_changes():
            self.apps_data = {}
            self.components_data = {}
            self.mission_data = {}
            self.additional_data = {}
            self.current_mission_file = None
            self.current_spacecraft_file = None
            self.spacecraft_config_path = None
            self.modified = False
            
            # Clear mission form fields
            self.clear_mission_fields()
            self.clear_additional_fields()  # Add this line
            self.refresh_displays()
            self.update_status("New mission created - configure mission settings first")
            self.update_file_labels()
            self.notebook.set("Mission Config")
            
    def clear_additional_fields(self):
        """Clear additional options fields"""
        if hasattr(self, 'gui_enabled_var'):
            self.gui_enabled_var.set(True)
        if hasattr(self, 'tipoff_x_entry'):
            self.tipoff_x_entry.delete(0, 'end')
            self.tipoff_x_entry.insert(0, "0.2")
        if hasattr(self, 'tipoff_y_entry'):
            self.tipoff_y_entry.delete(0, 'end')
            self.tipoff_y_entry.insert(0, "2.0")
        if hasattr(self, 'tipoff_z_entry'):
            self.tipoff_z_entry.delete(0, 'end')
            self.tipoff_z_entry.insert(0, "-2.0")
        if hasattr(self, 'sim_truth_var'):
            self.sim_truth_var.set(True)
            
    def setup_mission_change_tracking(self):
        """Setup change tracking for mission configuration fields"""
        # Bind change events to track modifications
        if hasattr(self, 'mission_time_entry'):
            self.mission_time_entry.bind('<KeyRelease>', lambda e: self.set_modified())
            self.mission_time_entry.bind('<FocusOut>', lambda e: self.update_time_display())
        
        # Trace variable changes
        if hasattr(self, 'gsw_var'):
            self.gsw_var.trace_add("write", lambda *args: self.set_modified()) # type: ignore
        if hasattr(self, 'fsw_var'):
            self.fsw_var.trace_add("write", lambda *args: self.set_modified()) # type: ignore
        if hasattr(self, 'sc_count_var'):
            self.sc_count_var.trace_add("write", lambda *args: self.set_modified()) # type: ignore
        if hasattr(self, 'sc1_config_var'):
            self.sc1_config_var.trace_add("write", lambda *args: self.set_modified()) # type: ignore
    
    def open_mission(self, startup = False):
        if self.check_unsaved_changes():
            if not startup:
                filename = filedialog.askopenfilename(
                    title="Open NOS3 Mission Configuration",
                    filetypes=[("Mission XML files", "*mission*.xml"), ("XML files", "*.xml"), ("All files", "*.*")]
                )
            else:
                filename = self.default_mission_config
                
            if filename:
                try:
                    self.load_mission_file(filename)
                    self.current_mission_file = filename
                    self.modified = False
                    self.update_status(f"Loaded mission: {os.path.basename(filename)}")
                    self.update_file_labels()
                    self.notebook.set("Mission Config")
                except Exception as e:
                    messagebox.showerror("Error", f"Failed to load mission file: {str(e)}")
    
    def save_all(self):
        """Save both mission and spacecraft configuration files"""
        if not self.current_mission_file:
            # Need to save mission file first
            filename = filedialog.asksaveasfilename(
                title="Save NOS3 Mission Configuration",
                defaultextension=".xml",
                filetypes=[("Mission XML files", "*mission*.xml"), ("XML files", "*.xml"), ("All files", "*.*")]
            )
            if filename:
                self.current_mission_file = filename
            else:
                return

        try:
            # Save mission configuration
            self.save_mission_file(self.current_mission_file)
            
            # Determine spacecraft config file path from mission config
            spacecraft_config = self.get_spacecraft_config_filename()
            if spacecraft_config:
                # Get the directory of the mission file
                mission_dir = os.path.dirname(self.current_mission_file)
                
                # Handle relative path in spacecraft config
                if not os.path.isabs(spacecraft_config):
                    self.spacecraft_config_path = os.path.join(mission_dir, f"spacecraft/{spacecraft_config}")
                else:
                    self.spacecraft_config_path = spacecraft_config
                
                # Save spacecraft configuration
                self.save_spacecraft_file(self.spacecraft_config_path)
                self.current_spacecraft_file = self.spacecraft_config_path
                
                self.modified = False
                self.update_status(f"Saved mission and spacecraft configurations")
                self.update_file_labels()
            else:
                messagebox.showerror("Error", "No spacecraft configuration selected in mission config")
                
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save files: {str(e)}")
            
    def load_mission_file(self, filename):
        """Load mission configuration and associated spacecraft config"""
        tree = ET.parse(filename)
        root = tree.getroot()
        
        if root.tag != 'nos3-mission-cfg':
            raise ValueError("This is not a valid NOS3 mission configuration file")
        
        # Clear existing data
        self.apps_data = {}
        self.components_data = {}
        self.mission_data = {}
        
        # Load mission configuration
        self.load_mission_config(tree)
        
        # Try to load associated spacecraft configuration
        spacecraft_config = self.get_spacecraft_config_filename()
        if spacecraft_config:
            mission_dir = os.path.dirname(filename)
            
            # Handle relative path
            if not os.path.isabs(spacecraft_config):
                self.spacecraft_config_path = os.path.join(f"{mission_dir}/spacecraft/", spacecraft_config)
            else:
                self.spacecraft_config_path = spacecraft_config
            
            # Try to load spacecraft config
            if os.path.exists(self.spacecraft_config_path):
                try:
                    self.load_spacecraft_file(self.spacecraft_config_path)
                    self.current_spacecraft_file = self.spacecraft_config_path
                except Exception as e:
                    messagebox.showwarning("Warning", 
                        f"Could not load spacecraft configuration '{spacecraft_config}': {str(e)}\n\n"
                        "You can still edit the mission configuration, but spacecraft apps/components will be empty.")
            else:
                messagebox.showwarning("Warning", 
                    f"Spacecraft configuration file not found: {spacecraft_config}\n\n"
                    "You can still edit the mission configuration, but spacecraft apps/components will be empty.")
        
        self.refresh_displays()
        
    def load_spacecraft_file(self, filename):
        """Load spacecraft configuration from XML file"""
        tree = ET.parse(filename)
        root = tree.getroot()
        
        if root.tag != 'sc-1-config':
            raise ValueError("This is not a valid spacecraft configuration file")
        
        # Clear existing spacecraft data
        self.apps_data = {}
        self.components_data = {}
        
        # Load applications
        apps_element = root.find('applications')
        if apps_element is not None:
            for app_element in apps_element:
                app_name = app_element.tag
                enable_element = app_element.find('enable')
                enabled = True
                if enable_element is not None:
                    enabled = enable_element.text.lower() == 'true' # type: ignore
                self.apps_data[app_name] = enabled
                
        # Load components
        components_element = root.find('components')
        if components_element is not None:
            for comp_element in components_element:
                comp_name = comp_element.tag
                enable_element = comp_element.find('enable')
                enabled = True
                if enable_element is not None:
                    enabled = enable_element.text.lower() == 'true' # type: ignore
                self.components_data[comp_name] = enabled
                
        # Load additional sections
        self.load_additional_sections(root)
        
        # Refresh the additional options display
        self.refresh_additional_display()
        
    def load_additional_sections(self, root):
        """Load GUI, Orbit, and Sim sections from XML"""
        # Load GUI section
        gui_element = root.find('gui')
        if gui_element is not None:
            enable_element = gui_element.find('enable')
            if enable_element is not None:
                self.additional_data['gui_enabled'] = enable_element.text.lower() == 'true'
            else:
                self.additional_data['gui_enabled'] = True
        else:
            self.additional_data['gui_enabled'] = True
        
        # Load Orbit section
        orbit_element = root.find('orbit')
        if orbit_element is not None:
            tipoff_x = orbit_element.find('tipoff_x')
            tipoff_y = orbit_element.find('tipoff_y')
            tipoff_z = orbit_element.find('tipoff_z')
            
            self.additional_data['tipoff_x'] = tipoff_x.text if tipoff_x is not None else "0.2"
            self.additional_data['tipoff_y'] = tipoff_y.text if tipoff_y is not None else "2.0"
            self.additional_data['tipoff_z'] = tipoff_z.text if tipoff_z is not None else "-2.0"
        else:
            self.additional_data['tipoff_x'] = "0.2"
            self.additional_data['tipoff_y'] = "2.0"
            self.additional_data['tipoff_z'] = "-2.0"
        
        # Load Sim section
        sim_element = root.find('sim')
        if sim_element is not None:
            sim_truth = sim_element.find('sim_truth_interface')
            if sim_truth is not None:
                self.additional_data['sim_truth'] = sim_truth.text.lower() == 'true'
            else:
                self.additional_data['sim_truth'] = True
        else:
            self.additional_data['sim_truth'] = True

    def refresh_additional_display(self):
        """Refresh the additional options display with loaded data"""
        if hasattr(self, 'gui_enabled_var') and 'gui_enabled' in self.additional_data:
            self.gui_enabled_var.set(self.additional_data['gui_enabled'])
        
        if hasattr(self, 'tipoff_x_entry') and 'tipoff_x' in self.additional_data:
            self.tipoff_x_entry.delete(0, 'end')
            self.tipoff_x_entry.insert(0, self.additional_data['tipoff_x'])
        
        if hasattr(self, 'tipoff_y_entry') and 'tipoff_y' in self.additional_data:
            self.tipoff_y_entry.delete(0, 'end')
            self.tipoff_y_entry.insert(0, self.additional_data['tipoff_y'])
        
        if hasattr(self, 'tipoff_z_entry') and 'tipoff_z' in self.additional_data:
            self.tipoff_z_entry.delete(0, 'end')
            self.tipoff_z_entry.insert(0, self.additional_data['tipoff_z'])
        
        if hasattr(self, 'sim_truth_var') and 'sim_truth' in self.additional_data:
            self.sim_truth_var.set(self.additional_data['sim_truth'])
                
    def get_spacecraft_config_filename(self):
        """Get the spacecraft configuration filename from the selected option"""
        selected_option = self.sc1_config_var.get() if hasattr(self, 'sc1_config_var') else "" # type: ignore
        if selected_option and selected_option in self.config_filenames:
            for filename in self.config_filenames:
                    if filename == selected_option:
                        return filename
        return None
    
    def save_mission_file(self, filename):
        """Save mission configuration to XML file"""
        # Create root element with the exact format
        root = ET.Element('nos3-mission-cfg')
        
        # Add comments as in the example
        comment1 = ET.Comment(" Mission Start Time (J2000 format) ")
        root.append(comment1)
        
        time_str = self.mission_time_entry.get().strip()
        if time_str:
            try:
                j2000_seconds = float(time_str)
                mission_time = J2000_EPOCH + timedelta(seconds=j2000_seconds)
                date_str = mission_time.strftime("%d %b %Y")
                comment2 = ET.Comment(f" Default time: {time_str}, {date_str} ")
                root.append(comment2)
            except ValueError:
                pass
        
        # Add start time
        start_time = ET.SubElement(root, 'start-time')
        start_time.text = self.mission_time_entry.get().strip()
        
        # Add ground software section
        gsw_comment = ET.Comment(" Ground Software ")
        root.append(gsw_comment)
        
        options_comment = ET.Comment(" cosmos (default), openc3, fprime, or yamcs ")
        root.append(options_comment)
        
        gsw = ET.SubElement(root, 'gsw')
        gsw.text = self.gsw_var.get() # type: ignore
        
        # Add flight software section
        fsw_comment = ET.Comment(" Flight Software ")
        root.append(fsw_comment)
        
        fsw_options_comment = ET.Comment(" cfs (default) or fprime ")
        root.append(fsw_options_comment)
        
        fsw = ET.SubElement(root, 'fsw')
        fsw.text = self.fsw_var.get() # type: ignore
        
        # Add number of spacecraft
        num_sc_comment = ET.Comment(" Number of spacecraft ")
        root.append(num_sc_comment)
        
        experimental_comment = ET.Comment(" Note this is experimental and not ready for use beyond proof of concept ")
        root.append(experimental_comment)
        
        num_sc = ET.SubElement(root, 'number-spacecraft')
        num_sc.text = self.sc_count_var.get() # type: ignore
        
        # Add spacecraft configurations
        sc1_comment = ET.Comment(" Spacecraft 1 Configuration - options are as follows ")
        root.append(sc1_comment)
        
        for filename_part in self.config_filenames:
            root.append(ET.Comment(f" {filename_part} "))
        
        sc1_cfg = ET.SubElement(root, 'sc-1-cfg')
        selected_option = self.sc1_config_var.get() # type: ignore
        if selected_option in self.config_filenames:
            for filename2 in self.config_filenames:
                    if filename2 == selected_option:
                        sc1_cfg.text = f"spacecraft/{filename2}"
        
        # Format and save XML
        xml_str = '<?xml version="1.0" encoding="utf-8"?>\n'
        rough_string = ET.tostring(root, 'utf-8')
        reparsed = minidom.parseString(rough_string)
        pretty_xml = reparsed.toprettyxml(indent="    ")
        pretty_xml = '\n'.join(pretty_xml.split('\n')[1:])
        
        with open(filename, 'w', encoding='utf-8') as f:
            f.write(xml_str + pretty_xml)
            
    def save_spacecraft_file(self, filename):
        """Save spacecraft configuration to XML file"""
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(filename), exist_ok=True)
        
        # Create spacecraft config XML
        root = ET.Element('sc-1-config')
        
        # Add applications
        if self.apps_data:
            apps_element = ET.SubElement(root, 'applications')
            for app_name, enabled in self.apps_data.items():
                app_element = ET.SubElement(apps_element, app_name)
                enable_element = ET.SubElement(app_element, 'enable')
                enable_element.text = 'true' if enabled else 'false'
                
        # Add components
        if self.components_data:
            components_element = ET.SubElement(root, 'components')
            for comp_name, enabled in self.components_data.items():
                comp_element = ET.SubElement(components_element, comp_name)
                enable_element = ET.SubElement(comp_element, 'enable')
                enable_element.text = 'true' if enabled else 'false'
                
        # Add GUI section
        gui_element = ET.SubElement(root, 'gui')
        gui_enable = ET.SubElement(gui_element, 'enable')
        gui_enable.text = 'true' if self.gui_enabled_var.get() else 'false'
        
        # Add Orbit section
        orbit_element = ET.SubElement(root, 'orbit')
        
        tipoff_x = ET.SubElement(orbit_element, 'tipoff_x')
        tipoff_x.text = self.tipoff_x_entry.get()
        
        tipoff_y = ET.SubElement(orbit_element, 'tipoff_y')
        tipoff_y.text = self.tipoff_y_entry.get()
        
        tipoff_z = ET.SubElement(orbit_element, 'tipoff_z')
        tipoff_z.text = self.tipoff_z_entry.get()
        
        # Add Sim section
        sim_element = ET.SubElement(root, 'sim')
        sim_truth = ET.SubElement(sim_element, 'sim_truth_interface')
        sim_truth.text = 'true' if self.sim_truth_var.get() else 'false'
                
        # Format and save XML
        xml_str = '<?xml version="1.0" encoding="utf-8"?>\n'
        rough_string = ET.tostring(root, 'utf-8')
        reparsed = minidom.parseString(rough_string)
        pretty_xml = reparsed.toprettyxml(indent="    ")
        pretty_xml = '\n'.join(pretty_xml.split('\n')[1:])
        
        with open(filename, 'w', encoding='utf-8') as f:
            f.write(xml_str + pretty_xml)
            
    # Add/update the on_spacecraft_config_change method
    def on_spacecraft_config_change(self):
        """Handle when user changes spacecraft configuration selection"""
        
        if not hasattr(self, 'sc1_config_var') or not self.sc1_config_var.get(): # type: ignore
            return
        
        selected_config = self.sc1_config_var.get() # type: ignore
        
        if selected_config not in self.config_filenames:
            return
        
        # Clear existing spacecraft data
        self.apps_data = {}
        self.components_data = {}
        
        # Get the spacecraft config filename
        spacecraft_config_file = self.sc1_config_var.get() # type: ignore
        
        # Try to find and load the spacecraft config file
        spacecraft_path = None
        
        if self.current_mission_file:
            # Look relative to mission file
            mission_dir = os.path.dirname(self.current_mission_file)
            
            # Try different possible locations
            possible_paths = [
                os.path.join(mission_dir, "spacecraft", spacecraft_config_file),
                os.path.join(mission_dir, spacecraft_config_file),
                os.path.join(mission_dir, "..", "spacecraft", spacecraft_config_file),
            ]
            
            for path in possible_paths:
                if os.path.exists(path):
                    spacecraft_path = path
                    break
        else:
            # If no mission file is loaded, try current directory
            possible_paths = [
                os.path.join("spacecraft", spacecraft_config_file),
                spacecraft_config_file,
                os.path.join("..", "spacecraft", spacecraft_config_file),
            ]
            
            for path in possible_paths:
                if os.path.exists(path):
                    spacecraft_path = os.path.abspath(path)
                    break
        
        if spacecraft_path and os.path.exists(spacecraft_path):
            try:
                self.load_spacecraft_file(spacecraft_path)
                self.current_spacecraft_file = spacecraft_path
                self.spacecraft_config_path = spacecraft_path
                
                self.update_status(f"Loaded spacecraft config: {spacecraft_config_file}")
                
                # Refresh displays to show the loaded apps and components
                self.refresh_apps_display()
                self.refresh_components_display()
                
            except Exception as e:
                messagebox.showerror("Error", f"Failed to load spacecraft configuration '{spacecraft_config_file}':\n{str(e)}")
                self.update_status(f"Failed to load spacecraft config: {spacecraft_config_file}")
        else:
            # Config file not found - create default structure
            self.create_default_spacecraft_config(spacecraft_config_file)
            
            # Still refresh displays to show empty state
            self.refresh_apps_display()
            self.refresh_components_display()
            
            self.update_status(f"Spacecraft config '{spacecraft_config_file}' not found - will create when saving")
        
        # Update file labels
        self.update_file_labels()
        
        # Mark as modified
        self.set_modified()
        
    def create_default_spacecraft_config(self, config_filename):
        """Create default spacecraft configuration based on the selected type"""
        # Generic default
        self.apps_data = {
            "cf"  : True,
            "ds"  : True,
            "fm"  : True,
            "lc"  : True,
            "sbn" : False,
            "sc"  : True
        }
        self.components_data = {
            "adcs"     : True,
            "cam"      : False,
            "css"      : True,
            "eps"      : True,
            "fss"      : True,
            "gps"      : True,
            "imu"      : True,
            "mag"      : True,
            "mgr"      : True,
            "onair"    : False,
            "radio"    : True,
            "rw"       : True,
            "sample"   : True,
            "st"       : True,
            "syn"      : False,
            "tourqer"  : True,
            "thruster" : False
        }
        self.additional_data = {
            "gui_enabled" : True,
            "tipoff_x"    : 0.2,
            "tipoff_y"    : 2.0,
            "tipoff_z"    : -2.0,
            "sim_truth"   : True
        }
            
    # Update the load_mission_config method to properly set the spacecraft config
    def load_mission_config(self, tree):
        """Load mission configuration from XML tree"""
        root = tree.getroot()
        
        # Clear existing mission data
        self.mission_data = {
            "start_time": "",
            "gsw": "",
            "fsw": "",
            "num_spacecraft": "1",
            "sc1_config": "",
            "scN_config": ""
        }
        
        # Extract start time
        start_time_elem = root.find('start-time')
        if start_time_elem is not None and start_time_elem.text:
            self.mission_data["start_time"] = start_time_elem.text
            self.mission_time_entry.delete(0, 'end')
            self.mission_time_entry.insert(0, start_time_elem.text)
            self.update_time_display()
        
        # Extract ground software
        gsw_elem = root.find('gsw')
        if gsw_elem is not None and gsw_elem.text:
            self.mission_data["gsw"] = gsw_elem.text
            self.gsw_var.set(gsw_elem.text) # type: ignore
        
        # Extract flight software
        fsw_elem = root.find('fsw')
        if fsw_elem is not None and fsw_elem.text:
            self.mission_data["fsw"] = fsw_elem.text
            self.fsw_var.set(fsw_elem.text) # type: ignore
        
        # Extract number of spacecraft
        num_sc_elem = root.find('number-spacecraft')
        if num_sc_elem is not None and num_sc_elem.text:
            self.mission_data["num_spacecraft"] = num_sc_elem.text
            self.sc_count_var.set(num_sc_elem.text) # type: ignore
        
        # Extract spacecraft 1 configuration
        sc1_elem = root.find('sc-1-cfg')
        if sc1_elem is not None and sc1_elem.text:
            config_file = sc1_elem.text
            
            # Remove "spacecraft/" prefix if present
            if config_file.startswith("spacecraft/"):
                config_file = config_file[11:]  # Remove "spacecraft/" prefix
            
            # Find the matching option
            for filename in self.config_filenames:
                if filename == config_file:
                    self.sc1_config_var.set(filename) # type: ignore
                    self.mission_data["sc1_config"] = filename
                    break
        
        # Update display for multiple spacecraft if needed
        self.update_spacecraft_config_display()
        
        # Don't trigger spacecraft config change during loading to avoid conflicts
        # The spacecraft config will be loaded by the main load_mission_file method
        
    # Add a method to manually trigger spacecraft config loading after mission is loaded
    def load_initial_spacecraft_config(self):
        """Load initial spacecraft config after mission config is loaded"""
        if hasattr(self, 'sc1_config_var') and self.sc1_config_var.get(): # type: ignore
            # Small delay to ensure UI is ready
            self.root.after(600, self.on_spacecraft_config_change)
            
    def update_file_labels(self):
        """Update the file labels in the menu bar"""
        if self.current_mission_file:
            self.mission_file_label.configure(text=f"Mission: {os.path.basename(self.current_mission_file)}")
        else:
            self.mission_file_label.configure(text="Mission: No file loaded")
            
        if self.current_spacecraft_file:
            self.spacecraft_file_label.configure(text=f"Spacecraft: {os.path.basename(self.current_spacecraft_file)}")
        else:
            self.spacecraft_file_label.configure(text="Spacecraft: No file loaded")
        
        # Add modification indicator
        if self.modified:
            if self.current_mission_file:
                self.mission_file_label.configure(text=f"Mission: {os.path.basename(self.current_mission_file)}")
                
    def clear_mission_fields(self):
        """Clear all mission configuration fields"""
        if hasattr(self, 'mission_time_entry'):
            self.mission_time_entry.delete(0, 'end')
        if hasattr(self, 'gsw_var'):
            self.gsw_var.set("cosmos")  # Default value # type: ignore
        if hasattr(self, 'fsw_var'):
            self.fsw_var.set("cfs")     # Default value # type: ignore
        if hasattr(self, 'sc_count_var'):
            self.sc_count_var.set("1") # type: ignore
        if hasattr(self, 'sc1_config_var'):
            self.sc1_config_var.set("") # type: ignore
        if hasattr(self, 'time_display_label'):
            self.time_display_label.configure(text="")

    def set_modified(self):
        """Mark the configuration as modified"""
        self.modified = True
        self.update_file_labels()

    def check_unsaved_changes(self):
        """Check for unsaved changes and prompt user"""
        if self.modified:
            result = messagebox.askyesnocancel("Unsaved Changes", "Save changes before continuing?")
            if result is True:  # Yes
                self.save_all()
                return True
            elif result is False:  # No
                return True
            else:  # Cancel
                return False
        return True

    def update_status(self, message):
        """Update the status bar message"""
        self.status_label.configure(text=message)
            
    # App and component operations
    def add_app(self):
        dialog = SimpleNameDialog(self.root, "Add Application", "Enter application name:")
        if dialog.result:
            app_name = dialog.result
            if app_name in self.apps_data:
                messagebox.showerror("Error", f"Application '{app_name}' already exists.")
                return
            self.apps_data[app_name] = True  # Default to enabled
            self.refresh_apps_display()
            self.set_modified()
            
    def add_component(self):
        dialog = SimpleNameDialog(self.root, "Add Component", "Enter component name:")
        if dialog.result:
            comp_name = dialog.result
            if comp_name in self.components_data:
                messagebox.showerror("Error", f"Component '{comp_name}' already exists.")
                return
            self.components_data[comp_name] = True  # Default to enabled
            self.refresh_components_display()
            self.set_modified()
            
    def toggle_app_enabled(self, app_name, var):
        self.apps_data[app_name] = var.get()
        self.set_modified()
        
    def toggle_component_enabled(self, comp_name, var):
        self.components_data[comp_name] = var.get()
        self.set_modified()
        
    def delete_app(self, app_name):
        if messagebox.askyesno("Confirm Delete", f"Delete application '{app_name}'?"):
            del self.apps_data[app_name]
            self.refresh_apps_display()
            self.set_modified()
            
    def delete_component(self, comp_name):
        if messagebox.askyesno("Confirm Delete", f"Delete component '{comp_name}'?"):
            del self.components_data[comp_name]
            self.refresh_components_display()
            self.set_modified()
            
    # Display refresh methods
    def refresh_displays(self):
        self.refresh_apps_display()
        self.refresh_components_display()
        self.refresh_additional_display()
        self.refresh_preview()
        
    def refresh_apps_display(self):
        # Clear existing app widgets
        for widget in self.apps_scrollable.winfo_children():
            widget.destroy()
            
        # Clear checkbox dictionary
        self.app_checkboxes = {}
        
        if not self.apps_data:
            self.no_apps_label = ctk.CTkLabel(self.apps_scrollable, text="No applications loaded. Open a configuration file.")
            self.no_apps_label.pack(pady=20)
            return
            
        # Add header row
        header_frame = ctk.CTkFrame(self.apps_scrollable)
        header_frame.pack(fill="x", pady=(0, 5))
        
        ctk.CTkLabel(header_frame, text="Application", width=200, font=ctk.CTkFont(weight="bold")).pack(side="left", padx=10)
        ctk.CTkLabel(header_frame, text="Enabled", width=80, font=ctk.CTkFont(weight="bold")).pack(side="left", padx=10)
        ctk.CTkLabel(header_frame, text="Actions", width=80, font=ctk.CTkFont(weight="bold")).pack(side="left", padx=10)
            
        # Add apps
        for app_name, enabled in sorted(self.apps_data.items()):
            app_frame = ctk.CTkFrame(self.apps_scrollable)
            app_frame.pack(fill="x", pady=2)
            
            # App name label
            name_label = ctk.CTkLabel(app_frame, text=app_name, width=200)
            name_label.pack(side="left", padx=10, pady=5)
            
            # App enabled checkbox
            var = tk.BooleanVar(value=enabled)
            checkbox = ctk.CTkCheckBox(app_frame, text="", variable=var, onvalue=True, offvalue=False,
                                      command=lambda n=app_name, v=var: self.toggle_app_enabled(n, v))
            checkbox.pack(side="left", padx=10)
            self.app_checkboxes[app_name] = checkbox
            
            # Delete button
            delete_btn = ctk.CTkButton(app_frame, text="Delete", width=60, fg_color="#D32F2F", hover_color="#B71C1C",
                                      command=lambda n=app_name: self.delete_app(n))
            delete_btn.pack(side="left", padx=10)
            
        # Add button at the bottom
        add_btn_frame = ctk.CTkFrame(self.apps_scrollable)
        add_btn_frame.pack(fill="x", pady=10)
        
        add_btn = ctk.CTkButton(add_btn_frame, text="Add Application", command=self.add_app)
        add_btn.pack(pady=5)
            
    def refresh_components_display(self):
        # Clear existing component widgets
        for widget in self.components_scrollable.winfo_children():
            widget.destroy()
            
        # Clear checkbox dictionary
        self.component_checkboxes = {}
        
        if not self.components_data:
            self.no_components_label = ctk.CTkLabel(self.components_scrollable, text="No components loaded. Open a configuration file.")
            self.no_components_label.pack(pady=20)
            return
            
        # Add header row
        header_frame = ctk.CTkFrame(self.components_scrollable)
        header_frame.pack(fill="x", pady=(0, 5))
        
        ctk.CTkLabel(header_frame, text="Component", width=200, font=ctk.CTkFont(weight="bold")).pack(side="left", padx=10)
        ctk.CTkLabel(header_frame, text="Enabled", width=80, font=ctk.CTkFont(weight="bold")).pack(side="left", padx=10)
        ctk.CTkLabel(header_frame, text="Actions", width=80, font=ctk.CTkFont(weight="bold")).pack(side="left", padx=10)
            
        # Add components
        for comp_name, enabled in sorted(self.components_data.items()):
            comp_frame = ctk.CTkFrame(self.components_scrollable)
            comp_frame.pack(fill="x", pady=2)
            
            # Component name label
            name_label = ctk.CTkLabel(comp_frame, text=comp_name, width=200)
            name_label.pack(side="left", padx=10, pady=5)
            
            # Component enabled checkbox
            var = tk.BooleanVar(value=enabled)
            checkbox = ctk.CTkCheckBox(comp_frame, text="", variable=var, onvalue=True, offvalue=False,
                                      command=lambda n=comp_name, v=var: self.toggle_component_enabled(n, v))
            checkbox.pack(side="left", padx=10)
            self.component_checkboxes[comp_name] = checkbox
            
            # Delete button
            delete_btn = ctk.CTkButton(comp_frame, text="Delete", width=60, fg_color="#D32F2F", hover_color="#B71C1C",
                                      command=lambda n=comp_name: self.delete_component(n))
            delete_btn.pack(side="left", padx=10)
            
        # Add button at the bottom
        add_btn_frame = ctk.CTkFrame(self.components_scrollable)
        add_btn_frame.pack(fill="x", pady=10)
        
        add_btn = ctk.CTkButton(add_btn_frame, text="Add Component", command=self.add_component)
        add_btn.pack(pady=5)
        
    def run(self):
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.root.mainloop()
        
    def on_closing(self):
        if self.check_unsaved_changes():
            self.root.destroy()
            
    def setup_logos(self):
        """Setup logos in the application"""
        try:
            # Path to the logo file
            logo_path = "cfg/gui/resources/nos3_original.png"
            
            if not os.path.exists(logo_path):
                print(f"Logo file not found: {logo_path}")
                return
                
            # Open the logo image with PIL
            original_logo = Image.open(logo_path)
            
            # Create header logo (larger) using CTkImage
            header_size = (200, 80)  # Adjust size as needed
            self.header_logo_img = ctk.CTkImage(original_logo, size=header_size)
            
            # Create smaller logo for tabs using CTkImage
            tab_size = (120, 50)  # Adjust size as needed
            self.tab_logo_img = ctk.CTkImage(original_logo, size=tab_size)
            
        except Exception as e:
            print(f"Error setting up logos: {e}")
            
    def add_header_logo(self):
        """Add logo to the header of the application"""
        # Create a header frame above the menu
        self.header_frame = ctk.CTkFrame(self.main_frame, height=100)
        self.header_frame.pack(fill="x", padx=10, pady=(10, 0))
        self.header_frame.pack_propagate(False)  # Don't shrink
        
        # Add logo to the left side using CTkLabel
        self.header_logo_label = ctk.CTkLabel(self.header_frame, image=self.header_logo_img, text="")
        self.header_logo_label.pack(side="left", padx=20, pady=10)
        
        # Add title text
        title_text = "NOS³ Configuration Manager"
        self.title_label = ctk.CTkLabel(self.header_frame, text=title_text, 
                                    font=ctk.CTkFont(family="Helvetica", size=24, weight="bold"))
        self.title_label.pack(side="left", padx=20, pady=10)
        
        # Add theme toggle
        self.add_theme_toggle()

    def add_tab_logos(self):
        """Add logos to the tabs"""
        # Add logo to mission tab
        self.mission_logo_frame = ctk.CTkFrame(self.mission_scrollable)
        self.mission_logo_frame.pack(fill="x", pady=(0, 20))
        
        self.mission_logo_label = ctk.CTkLabel(self.mission_logo_frame, image=self.tab_logo_img, text="")
        self.mission_logo_label.pack(side="right", padx=10, pady=5)
        
        # Add logo to applications tab
        self.apps_logo_frame = ctk.CTkFrame(self.apps_frame)
        self.apps_logo_frame.pack(fill="x", pady=(0, 10))
        
        self.apps_logo_label = ctk.CTkLabel(self.apps_logo_frame, image=self.tab_logo_img, text="")
        self.apps_logo_label.pack(side="right", padx=10, pady=5)
        
        # Add logo to components tab
        self.components_logo_frame = ctk.CTkFrame(self.components_frame)
        self.components_logo_frame.pack(fill="x", pady=(0, 10))
        
        self.components_logo_label = ctk.CTkLabel(self.components_logo_frame, image=self.tab_logo_img, text="")
        self.components_logo_label.pack(side="right", padx=10, pady=5)
        
    def add_theme_toggle(self):
        """Add a theme toggle button to the header"""
        # Create a toggle button
        self.theme_button = ctk.CTkButton(
            self.header_frame, 
            text="🌓 Toggle Theme", 
            command=self.toggle_theme,
            width=120
        )
        self.theme_button.pack(side="right", padx=20, pady=10)

    def toggle_theme(self):
        """Toggle between light and dark themes"""
        current_theme = ctk.get_appearance_mode()
        new_theme = "Light" if current_theme == "Dark" else "Dark"
        ctk.set_appearance_mode(new_theme)
