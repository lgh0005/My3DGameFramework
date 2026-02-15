import os
import tkinter as tk
from tkinter import filedialog, messagebox
from gui_context.gui_context_base import GUIContextBase
from gui_process.gui_process_runner import ProcessRunner
from gui_process.gui_log_window import LogWindow


class GuiContextConvertORMTexture(GUIContextBase):
    def __init__(self, app_context, window):
        self._log_win = None
        self._runner = None
        self._convert_button = None
        super().__init__(app_context, window)

    def _build_ui(self):
        # main label
        title_label = tk.Label(self, text="Convert AO, Roughness, Metallic to ORM Texture", font=("System", 20))
        title_label.pack(pady=(20, 40))

        # AO map path context
        ao_frame = tk.Frame(self)
        ao_frame.pack(pady=10, padx=20, fill="x")
        ao_label = tk.Label(ao_frame, text="Open AO Map:")
        ao_label.pack(side="left", padx=(0, 5))
        self._ao_path = tk.Entry(ao_frame)
        self._ao_path.pack(side="left", fill="x", expand=True, ipady=4)
        ao_browse = tk.Button(ao_frame, text="Browse...", command=self._browse_ao_map_file)
        ao_browse.pack(side="left", padx=5)

        # roughness map path context
        rough_frame = tk.Frame(self)
        rough_frame.pack(pady=10, padx=20, fill="x")
        rough_label = tk.Label(rough_frame, text="Open Roughness Map:")
        rough_label.pack(side="left", padx=(0, 5))
        self._rough_path = tk.Entry(rough_frame)
        self._rough_path.pack(side="left", fill="x", expand=True, ipady=4)
        rough_browse = tk.Button(rough_frame, text="Browse...", command=self._browse_roughness_map_file)
        rough_browse.pack(side="left", padx=5)

        # metallic map path context
        metal_frame = tk.Frame(self)
        metal_frame.pack(pady=10, padx=20, fill="x")
        metal_label = tk.Label(metal_frame, text="Open Metallic Map:")
        metal_label.pack(side="left", padx=(0, 5))
        self._metal_path = tk.Entry(metal_frame)
        self._metal_path.pack(side="left", fill="x", expand=True, ipady=4)
        metal_browse = tk.Button(metal_frame, text="Browse...", command=self._browse_metallic_map_file)
        metal_browse.pack(side="left", padx=5)

        # output folder context
        output_frame = tk.Frame(self)
        output_frame.pack(pady=10, padx=20, fill="x")
        output_label = tk.Label(output_frame, text="Output Folder:")
        output_label.pack(side="left", padx=(0, 5))
        self._output_path = tk.Entry(output_frame)
        self._output_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_output = tk.Button(output_frame, text="Browse...", command=self._browse_output_folder)
        browse_output.pack(side="left", padx=5)

        # invert roughness option checkbox
        opt_frame = tk.Frame(self)
        opt_frame.pack(pady=(5, 0), padx=20, fill="x")
        self._invert_rough = tk.BooleanVar(value=False)
        invert_cb = tk.Checkbutton(
            opt_frame,
            text="Input is Glossiness (invert to Roughness)",
            variable=self._invert_rough
        )
        invert_cb.pack(side="left", padx=(0, 15))

        # 5. Flip Y Option
        flip_opt_frame = tk.Frame(self)
        flip_opt_frame.pack(pady=(5, 10), padx=20, fill="x")
        self._flip_y = tk.BooleanVar(value=False)
        flip_y_cb = tk.Checkbutton(
            flip_opt_frame,
            text="Flip Y (Texture Coordinates)",
            variable=self._flip_y
        )
        flip_y_cb.pack(side="left")

        # back and convert Button context
        action_frame = tk.Frame(self)
        action_frame.pack(pady=10)
        back_button = tk.Button(action_frame, text="Back", command=self._clicked_back)
        back_button.pack(side="left", ipadx=10, ipady=5, padx=(0, 10))
        self._convert_button = tk.Button(action_frame, text="Convert!", command=self._clicked_convert)
        self._convert_button.pack(side="left", ipadx=10, ipady=5)

        # Process runner
        self._runner = ProcessRunner(self)

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
        if not (self._is_valid_optional_file(ao_path) and
                self._is_valid_optional_file(rough_path) and
                self._is_valid_optional_file(metal_path)):
            messagebox.showerror("Error", "If you set a texture path, it must be a valid file.\n(Empty is allowed.)")
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

        # 4. 최소 1개는 있어야 한다
        if not any([ao_path, rough_path, metal_path]):
            messagebox.showerror("Error", "Please select at least one texture (AO/Roughness/Metallic).")
            return

        # 5. 출력 파일명 생성 (BaseName_ORM.png)
        ref_path = ao_path or rough_path or metal_path
        base_name = os.path.splitext(os.path.basename(ref_path))[0]
        final_output_path = os.path.join(output_folder, f"{base_name}_ORM.png")

        # 6. 명령 인자 조립
        ao_arg = ao_path if ao_path else "none"
        rough_arg = rough_path if rough_path else "none"
        metal_arg = metal_path if metal_path else "none"

        cmd = [exe_path, "--orm", ao_arg, rough_arg, metal_arg, final_output_path]
        if self._invert_rough.get(): cmd.append("--invert-roughness")
        if self._flip_y.get(): cmd.append("--flip-y")

        # --- LogWindow 띄우기 (Convert 누를 때마다 새로) ---
        if self._log_win is not None and self._log_win.winfo_exists():
            self._log_win.destroy()

        parent = self.winfo_toplevel()
        self._log_win = LogWindow(parent, title="Converting ORM...")
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
                    f"ORM Texture Created Successfully!\n\nSaved to:\n{final_output_path}",
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

    def _is_valid_optional_file(self, p: str) -> bool:
        p = (p or "").strip()
        return (p == "") or os.path.isfile(p)
