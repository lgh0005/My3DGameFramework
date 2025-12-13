import os
import tkinter as tk
from tkinter import filedialog, messagebox
from gui_context.gui_context_base import GUIContextBase

class GuiContextConvertAnimation(GUIContextBase):
    def __init__(self, app_context, window):
        super().__init__(app_context, window)

    def _build_ui(self):
        
        # main label
        title_label = tk.Label(self, text="Convert animation to .myanim", font=("System", 20))
        title_label.pack(pady=(20,40))

        # open animation file path context
        input_frame = tk.Frame(self)
        input_frame.pack(pady=10, padx=20, fill="x")
        input_label = tk.Label(input_frame, text="Open Animation:")
        input_label.pack(side="left", padx=(0,5))
        self._input_path = tk.Entry(input_frame)
        self._input_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_button = tk.Button(input_frame, text="Browse...", command=self._browse_input_file)
        browse_button.pack(side="left", padx=5)

        # output path context
        output_frame = tk.Frame(self)
        output_frame.pack(pady=10, padx=20, fill="x")
        output_label = tk.Label(output_frame, text="Output Path:")
        output_label.pack(side="left", padx=(0,5))
        self._output_path = tk.Entry(output_frame)
        self._output_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_output = tk.Button(output_frame, text="Browse...", command=self._browse_output_file)
        browse_output.pack(side="left", padx=5)

        # back and convert Button context 
        action_frame = tk.Frame(self)
        action_frame.pack(pady=20)
        back_button = tk.Button(action_frame, text="Back", command=self._clicked_back)
        back_button.pack(side="left", ipadx=10, ipady=5, padx=(0, 10))
        convert_button = tk.Button(action_frame, text="Convert!", command=self._clicked_convert)
        convert_button.pack(side="left", ipadx=10, ipady=5)

    def _browse_input_file(self):
        path = filedialog.askopenfilename(
            title="Select Animation File",
            filetypes=[
                ("Animation Files", "*.fbx *.dae *.gltf *.glb *.bvh"),
                ("All Files", "*.*")
            ]
        )
        if path:
            self._input_path.delete(0, tk.END)
            self._input_path.insert(0, path)

    def _browse_output_file(self):
        path = filedialog.askdirectory(
            title="Select Output Folder"
        )
        if path:
            self._output_path.delete(0, tk.END)
            self._output_path.insert(0, path)
    
    def _clicked_convert(self):
        input_path = self._input_path.get().strip()
        output_path = self._output_path.get().strip()

        # 입력 경로 검증
        if not input_path:
            messagebox.showwarning("Warning", "Please select an input animation file.")
            return

        if not os.path.isfile(input_path):
            messagebox.showerror("Error", f"Input file does not exist:\n{input_path}")
            return

        # 출력 경로 검증
        if not output_path:
            messagebox.showwarning("Warning", "Please select an output file path.")
            return

        if os.path.isdir(output_path):
            # 출력 경로가 폴더라면 유효
            pass
        else:
            # 파일 경로일 경우 상위 폴더 존재 여부 확인
            output_dir = os.path.dirname(output_path)
            if not os.path.isdir(output_dir):
                messagebox.showerror("Error", f"Output directory does not exist:\n{output_dir}")
                return

        print(f"Converting animation:\n  Input: {input_path}\n  Output: {output_path}")
        # TODO : AssetConverter.exe를 실행하여 변환 로직 수행


    def _clicked_back(self):
        self._window.set_context_by_name("main")