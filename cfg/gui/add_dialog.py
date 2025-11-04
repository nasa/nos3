import customtkinter as ctk
from tkinter import messagebox


class SimpleNameDialog:
    def __init__(self, parent, title, prompt):
        self.result = None
        self.parent = parent
        
        # Create dialog window
        self.dialog = ctk.CTkToplevel(parent)
        self.dialog.title(title)
        self.dialog.geometry("400x150")
        self.dialog.transient(parent)
        
        # Center the dialog
        self.dialog.update_idletasks()
        x = (self.dialog.winfo_screenwidth() // 2) - (400 // 2)
        y = (self.dialog.winfo_screenheight() // 2) - (150 // 2)
        self.dialog.geometry(f"400x150+{x}+{y}")
        
        # Prompt
        ctk.CTkLabel(self.dialog, text=prompt).pack(pady=(20, 10))
        
        # Entry
        self.entry = ctk.CTkEntry(self.dialog, width=300)
        self.entry.pack(pady=10)
        
        # Buttons
        button_frame = ctk.CTkFrame(self.dialog)
        button_frame.pack(fill="x", pady=10)
        
        cancel_btn = ctk.CTkButton(button_frame, text="Cancel", command=self.cancel, width=100)
        cancel_btn.pack(side="right", padx=(10, 20))
        
        ok_btn = ctk.CTkButton(button_frame, text="OK", command=self.ok, width=100)
        ok_btn.pack(side="right")
        
        # Bind Enter and Escape keys
        self.dialog.bind('<Return>', lambda e: self.ok())
        self.dialog.bind('<Escape>', lambda e: self.cancel())
        
        # Schedule grab_set and focus_set to happen after dialog is fully created
        self.dialog.after(100, self._set_grab_and_focus)
        
        # Wait for dialog to close
        parent.wait_window(self.dialog)
    
    def _set_grab_and_focus(self):
        """Set grab and focus after the dialog is fully visible"""
        try:
            self.dialog.grab_set()
            self.entry.focus_set()
        except Exception as e:
            print(f"Warning: Could not set grab: {e}")
        
    def ok(self):
        value = self.entry.get().strip()
        if value:
            self.result = value
            self.dialog.destroy()
        else:
            messagebox.showerror("Error", "Please enter a name.", parent=self.dialog)
            
    def cancel(self):
        self.dialog.destroy()