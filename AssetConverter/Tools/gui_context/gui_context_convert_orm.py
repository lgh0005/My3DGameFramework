import os
import tkinter as tk
from tkinter import filedialog, messagebox
from gui_context.gui_context_base import GUIContextBase

class GuiContextConvertORMTexture(GUIContextBase):
    def __init__(self, app_context, window):
        super().__init__(app_context, window)

    def _build_ui(self):
        
        # main label
        title_label = tk.Label(self, text="Convert AO, Roughness, Metallic to ORM Texture", font=("System", 20))
        title_label.pack(pady=(20,40))

        # AO map path context
        ao_frame = tk.Frame(self)
        ao_frame.pack(pady=10, padx=20, fill="x")
        ao_label = tk.Label(ao_frame, text="Open AO Map:")
        ao_label.pack(side="left", padx=(0,5))
        self._ao_path = tk.Entry(ao_frame)
        self._ao_path.pack(side="left", fill="x", expand=True, ipady=4)
        ao_browse = tk.Button(ao_frame, text="Browse...", command=self._browse_ao_map_file)
        ao_browse.pack(side="left", padx=5)

        # roughness map path context
        rough_frame = tk.Frame(self)
        rough_frame.pack(pady=10, padx=20, fill="x")
        rough_label = tk.Label(rough_frame, text="Open Roughness Map:")
        rough_label.pack(side="left", padx=(0,5))
        self._rough_path = tk.Entry(rough_frame)
        self._rough_path.pack(side="left", fill="x", expand=True, ipady=4)
        rough_browse = tk.Button(rough_frame, text="Browse...", command=self._browse_roughness_map_file)
        rough_browse.pack(side="left", padx=5)

        # metallic map path context
        metal_frame = tk.Frame(self)
        metal_frame.pack(pady=10, padx=20, fill="x")
        metal_label = tk.Label(metal_frame, text="Open Metallic Map:")
        metal_label.pack(side="left", padx=(0,5))
        self._metal_path = tk.Entry(metal_frame)
        self._metal_path.pack(side="left", fill="x", expand=True, ipady=4)
        metal_browse = tk.Button(metal_frame, text="Browse...", command=self._browse_metallic_map_file)
        metal_browse.pack(side="left", padx=5)

         # back and convert Button context 
        action_frame = tk.Frame(self)
        action_frame.pack(pady=20)
        back_button = tk.Button(action_frame, text="Back", command=self._clicked_back)
        back_button.pack(side="left", ipadx=10, ipady=5, padx=(0,10))
        convert_button = tk.Button(action_frame, text="Convert!", command=self._clicked_convert)
        convert_button.pack(side="left", ipadx=10, ipady=5)

    def _browse_ao_map_file(self):
        path = filedialog.askopenfilename(
            title="Select AO Map File",
            filetypes=[("Image Files", "*.png *.jpg *.jpeg *.tga *.bmp"), ("All Files", "*.*")]
        )
        if path:
            self._ao_path.delete(0, tk.END)
            self._ao_path.insert(0, path)

    def _browse_roughness_map_file(self):
        path = filedialog.askopenfilename(
            title="Select Roughness Map File",
            filetypes=[("Image Files", "*.png *.jpg *.jpeg *.tga *.bmp"), ("All Files", "*.*")]
        )
        if path:
            self._rough_path.delete(0, tk.END)
            self._rough_path.insert(0, path)

    def _browse_metallic_map_file(self):
        path = filedialog.askopenfilename(
            title="Select Metallic Map File",
            filetypes=[("Image Files", "*.png *.jpg *.jpeg *.tga *.bmp"), ("All Files", "*.*")]
        )
        if path:
            self._metal_path.delete(0, tk.END)
            self._metal_path.insert(0, path)

    def _clicked_convert(self):
        ao_path = self._ao_path.get().strip()
        rough_path = self._rough_path.get().strip()
        metal_path = self._metal_path.get().strip()

        # 유효성 검사
        if not ao_path or not os.path.isfile(ao_path):
            messagebox.showerror("Error", "Please select a valid AO map file.")
            return
        if not rough_path or not os.path.isfile(rough_path):
            messagebox.showerror("Error", "Please select a valid Roughness map file.")
            return
        if not metal_path or not os.path.isfile(metal_path):
            messagebox.showerror("Error", "Please select a valid Metallic map file.")
            return

        print(f"Converting to ORM Texture:\n  AO: {ao_path}\n  Roughness: {rough_path}\n  Metallic: {metal_path}")
        # TODO : AssetConverter.exe를 실행하여 변환 로직 수행

    def _clicked_back(self):
        self._window.set_context_by_name("main")
