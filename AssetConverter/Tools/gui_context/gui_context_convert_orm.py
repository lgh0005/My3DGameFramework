import os, subprocess
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

        # 4. [추가됨] Output Folder context
        output_frame = tk.Frame(self)
        output_frame.pack(pady=10, padx=20, fill="x")
        output_label = tk.Label(output_frame, text="Output Folder:")
        output_label.pack(side="left", padx=(0,5))
        self._output_path = tk.Entry(output_frame)
        self._output_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_output = tk.Button(output_frame, text="Browse...", command=self._browse_output_folder)
        browse_output.pack(side="left", padx=5)

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

    def _browse_output_folder(self):
        path = filedialog.askdirectory(title="Select Output Folder")
        if path:
            self._output_path.delete(0, tk.END)
            self._output_path.insert(0, path)

    def _clicked_convert(self):
        ao_path = self._ao_path.get().strip()
        rough_path = self._rough_path.get().strip()
        metal_path = self._metal_path.get().strip()
        output_folder = self._output_path.get().strip()

        # 1. 입력 파일 유효성 검사
        if not all(os.path.isfile(p) for p in [ao_path, rough_path, metal_path]):
            messagebox.showerror("Error", "Please select valid files for all texture inputs.")
            return

        # 2. 출력 폴더 유효성 검사
        if not output_folder or not os.path.isdir(output_folder):
            messagebox.showerror("Error", "Please select a valid output folder.")
            return

        # 3. EXE 경로 확인
        exe_path = self._window.get_data("exe_path")
        if not exe_path:
            messagebox.showerror("Error", "AssetConverter path lost! Please verify again.")
            self._window.set_context_by_name("verify")
            return

        # 4. 출력 파일명 생성 (BaseName_ORM.png)
        # 예: Wood_AO.png -> Wood_ORM.png
        # 파일명에서 _AO, _Roughness 등을 제거하고 싶다면 추가 로직이 필요하지만,
        # 일단은 Base Name 뒤에 붙입니다.
        base_name = os.path.splitext(os.path.basename(ao_path))[0]
        final_output_path = os.path.join(output_folder, f"{base_name}_ORM.png")

        # 5. 명령 인자 조립
        # C++ ArgumentParser: exe --orm <ao> <rough> <metal> <out>
        cmd = [exe_path, "--orm", ao_path, rough_path, metal_path, final_output_path]
        try:
            # 6. 실행
            result = subprocess.run(
                cmd, 
                capture_output=True, 
                text=True
            )
            
            # 7. 결과 처리
            if result.returncode == 0:
                messagebox.showinfo("Success", f"ORM Texture Created Successfully!\n\nSaved to:\n{final_output_path}")
            else:
                err_msg = result.stderr if result.stderr else result.stdout
                messagebox.showerror("Conversion Failed", f"Error Code: {result.returncode}\n\nLog:\n{err_msg}")

        except Exception as e:
            messagebox.showerror("Critical Error", f"Failed to run executable.\n{e}")

    def _clicked_back(self):
        self._window.set_context_by_name("main")
