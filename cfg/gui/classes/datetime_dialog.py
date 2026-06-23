import customtkinter as ctk
import tkinter as tk
from tkinter import messagebox
from datetime import datetime, timezone

class DateTimeDialog:
    def __init__(self, parent, title):
        self.result = None
        
        # Create dialog window
        self.dialog = ctk.CTkToplevel(parent)
        self.dialog.title(title)
        self.dialog.geometry("500x350")
        self.dialog.transient(parent)
        
        # Center the dialog
        self.dialog.update_idletasks()
        x = (self.dialog.winfo_screenwidth() // 2) - (500 // 2)
        y = (self.dialog.winfo_screenheight() // 2) - (350 // 2)
        self.dialog.geometry(f"500x350+{x}+{y}")
        
        # Setup widgets
        self.setup_dialog()
        
        # Schedule grab_set and focus_set to happen after dialog is fully created
        self.dialog.after(100, self._set_grab_and_focus)
        
        # Wait for dialog to close
        parent.wait_window(self.dialog)
    
    def _set_grab_and_focus(self):
        """Set grab and focus after the dialog is fully visible"""
        try:
            self.dialog.grab_set()
            self.year_entry.focus_set()
        except Exception as e:
            print(f"Warning: Could not set grab: {e}")
        
    def setup_dialog(self):
        # Main frame
        main_frame = ctk.CTkFrame(self.dialog)
        main_frame.pack(fill="both", expand=True, padx=20, pady=20)
        
        # Title label
        ctk.CTkLabel(main_frame, text="Set Mission Date and Time", 
                   font=ctk.CTkFont(size=16, weight="bold")).pack(pady=(0, 20))
        
        # Date section
        date_frame = ctk.CTkFrame(main_frame)
        date_frame.pack(fill="x", pady=10)
        
        ctk.CTkLabel(date_frame, text="Date:").pack(side="left", padx=10)
        
        # Year
        ctk.CTkLabel(date_frame, text="Year:").pack(side="left", padx=(10, 0))
        self.year_entry = ctk.CTkEntry(date_frame, width=70)
        self.year_entry.pack(side="left", padx=(0, 10))
        self.year_entry.insert(0, str(datetime.now().year))
        
        # Month
        ctk.CTkLabel(date_frame, text="Month:").pack(side="left", padx=(10, 0))
        
        months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
        self.month_var = tk.StringVar()
        self.month_var.set(months[datetime.now().month - 1])
        self.month_combo = ctk.CTkComboBox(date_frame, values=months, variable=self.month_var, width=70)
        self.month_combo.pack(side="left", padx=(0, 10))
        
        # Day
        ctk.CTkLabel(date_frame, text="Day:").pack(side="left", padx=(10, 0))
        self.day_entry = ctk.CTkEntry(date_frame, width=50)
        self.day_entry.pack(side="left", padx=(0, 10))
        self.day_entry.insert(0, str(datetime.now().day))
        
        # Time section
        time_frame = ctk.CTkFrame(main_frame)
        time_frame.pack(fill="x", pady=10)
        
        ctk.CTkLabel(time_frame, text="Time (UTC):").pack(side="left", padx=10)
        
        # Hour
        ctk.CTkLabel(time_frame, text="Hour:").pack(side="left", padx=(10, 0))
        self.hour_entry = ctk.CTkEntry(time_frame, width=50)
        self.hour_entry.pack(side="left", padx=(0, 10))
        self.hour_entry.insert(0, "12")
        
        # Minute
        ctk.CTkLabel(time_frame, text="Min:").pack(side="left", padx=(10, 0))
        self.minute_entry = ctk.CTkEntry(time_frame, width=50)
        self.minute_entry.pack(side="left", padx=(0, 10))
        self.minute_entry.insert(0, "00")
        
        # Second
        ctk.CTkLabel(time_frame, text="Sec:").pack(side="left", padx=(10, 0))
        self.second_entry = ctk.CTkEntry(time_frame, width=50)
        self.second_entry.pack(side="left", padx=(0, 10))
        self.second_entry.insert(0, "00")
        
        # Current time button
        current_btn = ctk.CTkButton(main_frame, text="Use Current Date/Time", 
                                  command=self.use_current_time)
        current_btn.pack(pady=10)
        
        # Preview section
        preview_frame = ctk.CTkFrame(main_frame)
        preview_frame.pack(fill="x", pady=10)
        
        self.preview_label = ctk.CTkLabel(preview_frame, text="", wraplength=450)
        self.preview_label.pack(pady=10)
        
        # Update preview initially
        self.update_preview()
        
        # Bind events to update preview
        self.year_entry.bind('<KeyRelease>', lambda e: self.update_preview())
        self.day_entry.bind('<KeyRelease>', lambda e: self.update_preview())
        self.hour_entry.bind('<KeyRelease>', lambda e: self.update_preview())
        self.minute_entry.bind('<KeyRelease>', lambda e: self.update_preview())
        self.second_entry.bind('<KeyRelease>', lambda e: self.update_preview())
        self.month_var.trace_add("write", lambda *args: self.update_preview())
        
        # Buttons
        button_frame = ctk.CTkFrame(main_frame)
        button_frame.pack(fill="x", pady=(20, 0))
        
        cancel_btn = ctk.CTkButton(button_frame, text="Cancel", command=self.cancel, width=100)
        cancel_btn.pack(side="right", padx=(10, 0))
        
        ok_btn = ctk.CTkButton(button_frame, text="OK", command=self.ok, width=100)
        ok_btn.pack(side="right")
        
        # Bind Enter and Escape keys
        self.dialog.bind('<Return>', lambda e: self.ok())
        self.dialog.bind('<Escape>', lambda e: self.cancel())
    
    def use_current_time(self):
        """Fill in current date and time"""
        now = datetime.now(timezone.utc)
        
        self.year_entry.delete(0, 'end')
        self.year_entry.insert(0, str(now.year))
        
        months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
        self.month_var.set(months[now.month - 1])
        
        self.day_entry.delete(0, 'end')
        self.day_entry.insert(0, str(now.day))
        
        self.hour_entry.delete(0, 'end')
        self.hour_entry.insert(0, f"{now.hour:02d}")
        
        self.minute_entry.delete(0, 'end')
        self.minute_entry.insert(0, f"{now.minute:02d}")
        
        self.second_entry.delete(0, 'end')
        self.second_entry.insert(0, f"{now.second:02d}")
        
        self.update_preview()
    
    def update_preview(self):
        """Update the preview of the selected date/time"""
        try:
            year = int(self.year_entry.get())
            
            months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
            month_name = self.month_var.get()
            month = months.index(month_name) + 1
            
            day = int(self.day_entry.get())
            hour = int(self.hour_entry.get())
            minute = int(self.minute_entry.get())
            second = int(self.second_entry.get())
            
            # Create datetime object
            dt = datetime(year, month, day, hour, minute, second, tzinfo=timezone.utc)
            
            # Update preview
            preview_text = f"Date: {dt.strftime('%d %b %Y %H:%M:%S UTC')}"
            self.preview_label.configure(text=preview_text)
            
        except (ValueError, IndexError):
            self.preview_label.configure(text="Invalid date/time")
    
    def ok(self):
        try:
            year = int(self.year_entry.get())
            
            months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
            month_name = self.month_var.get()
            month = months.index(month_name) + 1
            
            day = int(self.day_entry.get())
            hour = int(self.hour_entry.get())
            minute = int(self.minute_entry.get())
            second = int(self.second_entry.get())
            
            # Validate ranges
            if not (1 <= month <= 12):
                raise ValueError("Invalid month")
            if not (1 <= day <= 31):
                raise ValueError("Invalid day")
            if not (0 <= hour <= 23):
                raise ValueError("Invalid hour")
            if not (0 <= minute <= 59):
                raise ValueError("Invalid minute")
            if not (0 <= second <= 59):
                raise ValueError("Invalid second")
            
            # Create datetime object
            dt = datetime(year, month, day, hour, minute, second, tzinfo=timezone.utc)
            
            self.result = dt
            self.dialog.destroy()
            
        except (ValueError, IndexError) as e:
            messagebox.showerror("Error", f"Invalid date/time: {str(e)}", parent=self.dialog)
    
    def cancel(self):
        self.dialog.destroy()