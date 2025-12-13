import os, subprocess
import tkinter as tk
from tkinter import filedialog, messagebox
from gui_context.gui_context_base import GUIContextBase

class GuiContextConvertMain(GUIContextBase):
    def __init__(self, app_context, window):
        super().__init__(app_context, window)

    def _build_ui(self):

        # main label
        title_label = tk.Label(self, text="Welcome to My3DGameFramework Asset Converter!", font=("System", 20))
        title_label.pack(pady=(20,40))

        # Buttons
        button_texts = [
            "Convert animation to .myanim",
            "Convert model to .mymodel",
            "Convert ao, roughness, metallic to ORM texture"]
        max_button_width = max(len(text) for text in button_texts)
        
        for text, command in zip(button_texts, [self._clicked_convert_animation,
                                                self._clicked_convert_model,
                                                self._clicked_convert_orm]):
            btn = tk.Button(self, text=text, command=command, width=max_button_width)
            btn.pack(pady=10, ipadx=10, ipady=5)

    def _browse_exe_file(self):
        path = filedialog.askopenfilename(
            title="Select AssetConverter.exe",
            filetypes=[("Executable Files", "*.exe"), ("All Files", "*.*")]
        )
        if path:
            self._exe_path.delete(0, tk.END)
            self._exe_path.insert(0, path)

    def _clicked_convert_animation(self):
        self._window.set_context_by_name("animation")

    def _clicked_convert_model(self):
        self._window.set_context_by_name("model")

    def _clicked_convert_orm(self):
        self._window.set_context_by_name("orm")