import os, subprocess
import tkinter as tk
from tkinter import filedialog, messagebox
from gui_context.gui_context_base import GUIContextBase

class GuiContextConvertModel(GUIContextBase):
    def __init__(self, app_context, window):
        super().__init__(app_context, window)

    def _build_ui(self):
        
        # main label
        title_label = tk.Label(self, text="Convert model to .mymodel", font=("System", 20))
        title_label.pack(pady=(20,40))

        # open model file path context
        input_frame = tk.Frame(self)
        input_frame.pack(pady=10, padx=20, fill="x")
        input_label = tk.Label(input_frame, text="Open Model:")
        input_label.pack(side="left", padx=(0,5))
        self._input_path = tk.Entry(input_frame)
        self._input_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_input = tk.Button(input_frame, text="Browse...", command=self._browse_input_file)
        browse_input.pack(side="left", padx=5)

        # output path context
        output_frame = tk.Frame(self)
        output_frame.pack(pady=10, padx=20, fill="x")
        output_label = tk.Label(output_frame, text="Output Folder:")
        output_label.pack(side="left", padx=(0,5))
        self._output_path = tk.Entry(output_frame)
        self._output_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_output = tk.Button(output_frame, text="Browse...", command=self._browse_output_folder)
        browse_output.pack(side="left", padx=5)

        # extract orm texture context
        opt_frame = tk.Frame(self)
        opt_frame.pack(pady=(5, 0), padx=20, fill="x")
        self._extract_orm = tk.BooleanVar(value=False)
        invert_cb = tk.Checkbutton(
            opt_frame,
            text="Extract ORM texture from model (from AO, Roughness, Metallic)",
            variable=self._extract_orm
        )
        invert_cb.pack(side="left")

        # back and convert Button context 
        action_frame = tk.Frame(self)
        action_frame.pack(pady=20)
        back_button = tk.Button(action_frame, text="Back", command=self._clicked_back)
        back_button.pack(side="left", ipadx=10, ipady=5, padx=(0,10))
        convert_button = tk.Button(action_frame, text="Convert!", command=self._clicked_convert)
        convert_button.pack(side="left", ipadx=10, ipady=5)

    def _browse_input_file(self):
        path = filedialog.askopenfilename(
            title="Select Model File",
            filetypes=[
                ("Model Files", "*.fbx *.obj *.dae *.gltf *.glb"),
                ("All Files", "*.*")
            ]
        )
        if path:
            self._input_path.delete(0, tk.END)
            self._input_path.insert(0, path)

    def _browse_output_folder(self):
        path = filedialog.askdirectory(title="Select Output Folder")
        if path:
            self._output_path.delete(0, tk.END)
            self._output_path.insert(0, path)

    def _clicked_convert(self):
        input_path = self._input_path.get().strip()
        output_folder = self._output_path.get().strip()

        # 입력 경로 체크
        if not input_path or not os.path.isfile(input_path):
            messagebox.showerror("Error", "Please select a valid input model file.")
            return

        # 출력 경로 체크
        if not output_folder or not os.path.isdir(output_folder):
            messagebox.showerror("Error", "Please select a valid output folder.")
            return
        
        # 부모(Window)에게서 exe 경로를 받아옴
        exe_path = self._window.get_data("exe_path")
        if not exe_path:
            messagebox.showerror("Error", "AssetConverter path lost! Please verify again.")
            self._window.set_context_by_name("verify")
            return
        
        # 명령 인자 조립 및 변환 로직 실행
        file_name_no_ext = os.path.splitext(os.path.basename(input_path))[0]
        final_output_path = os.path.join(output_folder, f"{file_name_no_ext}.mymodel")
        cmd = [exe_path, "-m", input_path, final_output_path]
        if self._extract_orm.get(): cmd.append("--extract-orm")
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
            )

            if result.returncode == 0:
                messagebox.showinfo("Conversion Success", f"Conversion Completed!\n\nSaved to:\n{output_folder}")
                self._window.set_context_by_name("main")
            else:
                err_msg = result.stderr if result.stderr else result.stdout
                messagebox.showerror("Conversion Failed", f"Error Code: {result.returncode}\n\nLog:\n{err_msg}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to run executable.\n{e}")

    def _clicked_back(self):
        self._window.set_context_by_name("main")