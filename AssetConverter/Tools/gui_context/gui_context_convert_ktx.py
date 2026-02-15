import os
import tkinter as tk
from tkinter import filedialog, messagebox, ttk
from gui_context.gui_context_base import GUIContextBase
from gui_process.gui_process_runner import ProcessRunner
from gui_process.gui_log_window import LogWindow


class GuiContextConvertTextureKTX(GUIContextBase):
    def __init__(self, app_context, window):
        self._log_win = None
        self._runner = None
        self._convert_button = None
        self._formats = ["BC7", "BC3", "BC1", "BC6H", "RGBA8"]
        self._color_spaces = ["sRGB", "Linear"]
        super().__init__(app_context, window)

    def _build_ui(self):

        # 1. Main Label
        title_label = tk.Label(self, text="Convert texture to .ktx", font=("System", 20))
        title_label.pack(pady=(20, 40))

        # 2. Input Texture Path
        input_frame = tk.Frame(self)
        input_frame.pack(pady=10, padx=20, fill="x")
        input_label = tk.Label(input_frame, text="Open Texture:")
        input_label.pack(side="left", padx=(0, 5))
        self._input_path = tk.Entry(input_frame)
        self._input_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_input = tk.Button(input_frame, text="Browse...", command=self._browse_input_file)
        browse_input.pack(side="left", padx=5)

        # 3. Output Folder Path
        output_frame = tk.Frame(self)
        output_frame.pack(pady=10, padx=20, fill="x")
        output_label = tk.Label(output_frame, text="Output Folder:")
        output_label.pack(side="left", padx=(0, 5))
        self._output_path = tk.Entry(output_frame)
        self._output_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_output = tk.Button(output_frame, text="Browse...", command=self._browse_output_folder)
        browse_output.pack(side="left", padx=5)

        # 4. Texture Format Selection (Combobox)
        format_frame = tk.Frame(self)
        format_frame.pack(pady=10, padx=20, fill="x")
        format_label = tk.Label(format_frame, text="Texture Format:")
        format_label.pack(side="left", padx=(0, 5))
        self._format_combo = ttk.Combobox(format_frame, values=self._formats, state="readonly")
        self._format_combo.set("BC7") # 기본값 설정
        self._format_combo.pack(side="left", ipady=2)

        # 5 Colour Space Selection (Combobox)
        space_frame = tk.Frame(self)
        space_frame.pack(pady=10, padx=20, fill="x")
        space_label = tk.Label(space_frame, text="Color Space:")
        space_label.pack(side="left", padx=(0, 5))
        self._space_combo = ttk.Combobox(space_frame, values=self._color_spaces, state="readonly")
        self._space_combo.set("sRGB")
        self._space_combo.pack(side="left", ipady=2)

        # 6. Flip Y
        opt_frame = tk.Frame(self)
        opt_frame.pack(pady=10, padx=20, fill="x")
        self._flip_y = tk.BooleanVar(value=False)
        flip_y_cb = tk.Checkbutton(
            opt_frame, 
            text="Flip Y (Texture Coordinates)", 
            variable=self._flip_y
        )
        flip_y_cb.pack(side="left")

        # 7. Buttons (Back & Convert)
        action_frame = tk.Frame(self)
        action_frame.pack(pady=20)
        back_button = tk.Button(action_frame, text="Back", command=self._clicked_back)
        back_button.pack(side="left", ipadx=10, ipady=5, padx=(0, 10))
        self._convert_button = tk.Button(action_frame, text="Convert!", command=self._clicked_convert)
        self._convert_button.pack(side="left", ipadx=10, ipady=5)

        # Process runner 초기화
        self._runner = ProcessRunner(self)

    def _browse_input_file(self):
        path = filedialog.askopenfilename(
            title="Select Texture Image File",
            filetypes=[
                ("All Supported", "*.png *.jpg *.jpeg *.tga *.bmp *.hdr"),
                ("LDR Images", "*.png *.jpg *.jpeg *.tga *.bmp"),
                ("HDR Images", "*.hdr"),
                ("All Files", "*.*")
            ]
        )
        if path:
            self._input_path.delete(0, tk.END)
            self._input_path.insert(0, path)

            # 확장자 자동 감지 및 포맷 추천
            ext = os.path.splitext(path)[1].lower()
            if ext == ".hdr":
                self._format_combo.set("BC6H")
                self._space_combo.set("Linear")
                self._space_combo.config(state="disabled") 
            else:
                self._format_combo.set("BC7")
                self._space_combo.config(state="readonly")
                self._space_combo.set("sRGB")

    def _browse_output_folder(self):
        path = filedialog.askdirectory(title="Select Output Folder")
        if path:
            self._output_path.delete(0, tk.END)
            self._output_path.insert(0, path)

    def _clicked_convert(self):
        input_path = self._input_path.get().strip()
        output_folder = self._output_path.get().strip()
        format_str = self._format_combo.get()
        space_str = self._space_combo.get()

        # 유효성 검사: HDR 파일인데 LDR 전용 압축을 선택한 경우 경고
        ext = os.path.splitext(input_path)[1].lower()
        ldr_only_formats = ["BC1", "BC3", "BC7"]
        if ext == ".hdr" and format_str in ldr_only_formats:
            if not messagebox.askyesno("Warning", 
                f"You are trying to save an HDR file as {format_str}.\n"
                "This will cause significant data loss (clamping to 0-1 range).\n"
                "Continue anyway?"):
                return

        # 유효성 검사
        if not input_path or not os.path.isfile(input_path):
            messagebox.showerror("Error", "Please select a valid input texture file.")
            return

        if not output_folder or not os.path.isdir(output_folder):
            messagebox.showerror("Error", "Please select a valid output folder.")
            return

        # exe 경로 확인
        exe_path = self._window.get_data("exe_path")
        if not exe_path:
            messagebox.showerror("Error", "AssetConverter path lost! Please verify again.")
            self._window.set_context_by_name("verify")
            return

        # 출력 파일명 생성 (BaseName.ktx)
        file_name_no_ext = os.path.splitext(os.path.basename(input_path))[0]
        final_output_path = os.path.join(output_folder, f"{file_name_no_ext}.ktx")

        # 명령 인자 조립 (--ktx <in> <out> <format> <space>)
        cmd = [exe_path, "--ktx", input_path, final_output_path, format_str, space_str]
        if self._flip_y.get(): cmd.append("--flip-y")

        # LogWindow 띄우기
        if self._log_win is not None and self._log_win.winfo_exists():
            self._log_win.destroy()

        parent = self.winfo_toplevel()
        self._log_win = LogWindow(parent, title="Converting Texture to KTX...")
        self._log_win.set_busy(True, "Compressing...")
        self._log_win.append(f"[UI] Command: {' '.join(cmd)}")

        # 비동기 실행
        self._convert_button.config(state="disabled")
        self._runner.run(cmd)
        self._poll_runner(final_output_path)

    def _poll_runner(self, final_output_path: str):
        done_code = None

        for kind, payload in self._runner.poll():
            if kind == "log":
                if self._log_win and self._log_win.winfo_exists():
                    self._log_win.append(payload)
            elif kind == "done":
                done_code = payload

        if done_code is None:
            self.after(50, lambda: self._poll_runner(final_output_path))
            return

        # 프로세스 종료 처리
        self._convert_button.config(state="normal")
        if self._log_win and self._log_win.winfo_exists():
            self._log_win.set_busy(False, f"Done (code={done_code})")
            self.winfo_toplevel().bell()

            if done_code == 0:
                self._log_win.show_modal_result(
                    "Success",
                    f"Texture Converted Successfully!\n\nSaved to:\n{final_output_path}",
                    ok_text="OK"
                )
                if self._log_win.winfo_exists():
                    self._log_win.destroy()
                self._log_win = None
            else:
                self._log_win.show_modal_result(
                    "Conversion Failed",
                    f"Exit Code: {done_code}\n\nCheck the log for details.",
                    ok_text="OK"
                )

    def _clicked_back(self):
        self._window.set_context_by_name("main")