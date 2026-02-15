import os
import tkinter as tk
from tkinter import filedialog, messagebox
from gui_context.gui_context_base import GUIContextBase
from gui_process.gui_process_runner import ProcessRunner
from gui_process.gui_log_window import LogWindow

class GuiContextConvertAnimation(GUIContextBase):
    def __init__(self, app_context, window):
        self._log_win = None
        self._runner = None
        self._convert_button = None
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

        # base model Input (Reference)
        ref_frame = tk.Frame(self)
        ref_frame.pack(pady=10, padx=20, fill="x")
        ref_label = tk.Label(ref_frame, text="Base Model:") 
        ref_label.pack(side="left", padx=(0,5))
        self._ref_path = tk.Entry(ref_frame)
        self._ref_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_ref = tk.Button(ref_frame, text="Browse...", command=self._browse_ref_model)
        browse_ref.pack(side="left", padx=5)

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
        self._convert_button = tk.Button(action_frame, text="Convert!", command=self._clicked_convert)
        self._convert_button.pack(side="left", ipadx=10, ipady=5)

        # Process runner
        self._runner = ProcessRunner(self)

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

    def _browse_ref_model(self):
        path = filedialog.askopenfilename(
            title="Select Base Model File",
            filetypes=[
                ("Model Files", "*.mymodel *.fbx *.obj"), # .mymodel 우선
                ("All Files", "*.*")
            ]
        )
        if path:
            self._ref_path.delete(0, tk.END)
            self._ref_path.insert(0, path)

    def _browse_output_file(self):
        path = filedialog.askdirectory(
            title="Select Output Folder"
        )
        if path:
            self._output_path.delete(0, tk.END)
            self._output_path.insert(0, path)
    
    def _clicked_convert(self):
        input_path = self._input_path.get().strip()
        ref_path = self._ref_path.get().strip()
        output_folder = self._output_path.get().strip()

        # 1. 유효성 검사
        if not input_path or not os.path.isfile(input_path):
            messagebox.showerror("Error", "Please select a valid animation file.")
            return

        if not ref_path or not os.path.isfile(ref_path):
            messagebox.showerror("Error", "Please select a valid base model file.\n(Skeleton reference is required)")
            return

        if not output_folder or not os.path.isdir(output_folder):
            messagebox.showerror("Error", "Please select a valid output folder.")
            return

        # 2. EXE 경로 확인
        exe_path = self._window.get_data("exe_path")
        if not exe_path:
            messagebox.showerror("Error", "AssetConverter path lost! Please verify again.")
            self._window.set_context_by_name("verify")
            return

        # 3. 출력 파일명 자동 생성 (InputAnimName.myanim)
        filename_no_ext = os.path.splitext(os.path.basename(input_path))[0]
        final_output_path = os.path.join(output_folder, f"{filename_no_ext}.myanim")

        # 4. 명령 인자 조립 및 변환 로직 실행
        cmd = [exe_path, "-a", input_path, ref_path, final_output_path]

        # --- LogWindow 띄우기 (Convert 누를 때마다 새로) ---
        if self._log_win is not None and self._log_win.winfo_exists():
            self._log_win.destroy()

        parent = self.winfo_toplevel()
        self._log_win = LogWindow(parent, title="Converting Animation...")
        self._log_win.set_busy(True, "Converting...")
        self._log_win.append("[UI] Starting process...")

        # --- 비동기 실행 ---
        self._convert_button.config(state="disabled")
        self._runner.run(cmd)
        self._log_win.append("[UI] runner.run() called")
        self._log_win.append("[UI] polling...")
        self._poll_runner(final_output_path)

    def _poll_runner(self, final_output_path: str):
        done_code = None

        for kind, payload in self._runner.poll():
            if kind == "log":
                if self._log_win is not None and self._log_win.winfo_exists():
                    self._log_win.append(payload)
            elif kind == "done":
                done_code = payload

        if done_code is None:
            self.after(50, lambda: self._poll_runner(final_output_path))
            return

        # done 처리
        self._convert_button.config(state="normal")

        if self._log_win is not None and self._log_win.winfo_exists():
            self._log_win.set_busy(False, f"Done (code={done_code})")

            # 작업 완료 알림음
            self.winfo_toplevel().bell()

            if done_code == 0:
                self._log_win.show_modal_result(
                    "Success",
                    f"Animation Converted Successfully!\n\nSaved to:\n{final_output_path}",
                    ok_text="OK"
                )
                # OK 누르면 로그창도 같이 닫기
                if self._log_win.winfo_exists():
                    self._log_win.destroy()
                self._log_win = None
            else:
                self._log_win.show_modal_result(
                    "Conversion Failed",
                    f"Exit Code: {done_code}\n\n(See log above)",
                    ok_text="OK"
                )

    def _clicked_back(self):
        self._window.set_context_by_name("main")