import os, subprocess
import tkinter as tk
from tkinter import filedialog, messagebox
from gui_context.gui_context_base import GUIContextBase

class GuiContextVerifyAssetConverter(GUIContextBase):
    def __init__(self, app_context, window):
        super().__init__(app_context, window)

    def _build_ui(self):

        # title
        title_label = tk.Label(self, text="Please verify your AssetConverter!", font=("System", 20))
        title_label.pack(pady=(20,40))

        # exe path entry
        exe_frame = tk.Frame(self)
        exe_frame.pack(pady=10, padx=20, fill="x")
        exe_label = tk.Label(exe_frame, text="AssetConverter Path:")
        exe_label.pack(side="left", padx=(0,5))
        self._exe_path = tk.Entry(exe_frame)
        self._exe_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_button = tk.Button(exe_frame, text="Browse...", command=self._browse_exe_file)
        browse_button.pack(side="left", padx=5)

        # verify and close button
        action_frame = tk.Frame(self)
        action_frame.pack(pady=20)
        verify_button = tk.Button(action_frame, text="Verify", command=self._verify_exe)
        verify_button.pack(side="left", ipadx=10, ipady=5, padx=(0,10))
        close_button = tk.Button(action_frame, text="Close", command=self._clicked_close)
        close_button.pack(side="left", ipadx=10, ipady=5)

    def _browse_exe_file(self):
        path = filedialog.askopenfilename(
            title="Select AssetConverter.exe",
            filetypes=[("Executable Files", "*.exe"), ("All Files", "*.*")]
        )
        if path:
            self._exe_path.delete(0, tk.END)
            self._exe_path.insert(0, path)

    def _verify_exe(self):
        exe_path = self._exe_path.get().strip()
        if not exe_path or not os.path.isfile(exe_path):
            messagebox.showerror("Error", "AssetConverter.exe not found!")
            return

        try:
            result = subprocess.run(
                [exe_path, "--check"],
                capture_output=True,
                text=True,
                timeout=1
            )
            if "AssetConverter" in result.stdout:
                self._window.set_data("exe_path", exe_path)
                messagebox.showinfo("Success", "AssetConverter.exe verified successfully!")
                self._window.set_context_by_name("main")
            else:
                messagebox.showerror("Error", f"Invalid executable.\nOutput:\n{result.stdout}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to run executable.\n{e}")

    def _clicked_close(self):
        self._window.destroy()