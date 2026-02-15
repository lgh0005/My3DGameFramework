import os
import tkinter as tk
from tkinter import filedialog, messagebox, ttk
from gui_context.gui_context_base import GUIContextBase
from gui_process.gui_process_runner import ProcessRunner
from gui_process.gui_log_window import LogWindow

class GuiContextConvertCubeMapKTX(GUIContextBase):
    def __init__(self, app_context, window):
        self._log_win = None
        self._runner = None
        self._convert_button = None
        
        # OpenGL/KTX 표준 순서에 맞춘 직관적인 영어 라벨
        self._face_labels = [
            "Right Side (+X)", 
            "Left Side (-X)", 
            "Top Side (+Y)", 
            "Bottom Side (-Y)", 
            "Back Side (+Z)", 
            "Front Side (-Z)"
        ]
        
        self._face_entries = []
        self._face_flip_xs = [] # Flip X 체크박스 변수 리스트
        self._face_flip_ys = [] # Flip Y 체크박스 변수 리스트
        
        super().__init__(app_context, window)

    def _build_ui(self):
        # 1. Main Label
        title_label = tk.Label(self, text="Convert 6 Images to KTX CubeMap", font=("System", 20))
        title_label.pack(pady=(20, 20))

        # 2. Global Option: sRGB (필수 인자로 전달됨)
        opt_frame = tk.Frame(self)
        opt_frame.pack(fill="x", padx=20, pady=5)
        self._is_srgb = tk.BooleanVar(value=True) # 스카이박스는 sRGB가 기본
        srgb_cb = tk.Checkbutton(opt_frame, text="Use sRGB Color Space (Recommended for Skybox)", 
                                 variable=self._is_srgb)
        srgb_cb.pack(side="left")

        # 3. 6 Faces Input Rows
        input_container = tk.Frame(self)
        input_container.pack(pady=10, padx=20, fill="x")

        for i in range(6):
            row = tk.Frame(input_container)
            row.pack(fill="x", pady=2)

            # 방향 라벨
            lbl = tk.Label(row, text=self._face_labels[i], width=18, anchor="w")
            lbl.pack(side="left")

            # 경로 입력창
            ent = tk.Entry(row)
            ent.pack(side="left", fill="x", expand=True, padx=5, ipady=3)
            self._face_entries.append(ent)

            # 파일 찾기 버튼
            btn = tk.Button(row, text="Browse...", command=lambda idx=i: self._browse_face_file(idx))
            btn.pack(side="left", padx=5)

            # Flip X & Y 체크박스
            fx_var = tk.BooleanVar(value=False)
            fy_var = tk.BooleanVar(value=False)
            
            chk_x = tk.Checkbutton(row, text="Flip X", variable=fx_var)
            chk_x.pack(side="left", padx=2)
            chk_y = tk.Checkbutton(row, text="Flip Y", variable=fy_var)
            chk_y.pack(side="left", padx=2)
            
            self._face_flip_xs.append(fx_var)
            self._face_flip_ys.append(fy_var)

        # 4. Output File Path
        output_frame = tk.Frame(self)
        output_frame.pack(pady=10, padx=20, fill="x")
        output_label = tk.Label(output_frame, text="Output (.ktx):", width=18, anchor="w")
        output_label.pack(side="left")
        self._output_path = tk.Entry(output_frame)
        self._output_path.pack(side="left", fill="x", expand=True, ipady=4)
        browse_output = tk.Button(output_frame, text="Browse...", command=self._browse_output_file)
        browse_output.pack(side="left", padx=5)

        # 5. Buttons (Back & Convert) - 표준 스타일
        action_frame = tk.Frame(self)
        action_frame.pack(pady=5)
        back_button = tk.Button(action_frame, text="Back", command=self._clicked_back)
        back_button.pack(side="left", ipadx=10, ipady=5, padx=(0, 10))
        self._convert_button = tk.Button(action_frame, text="Convert!", command=self._clicked_convert)
        self._convert_button.pack(side="left", ipadx=10, ipady=5)

        self._runner = ProcessRunner(self)

    def _browse_face_file(self, index):
        path = filedialog.askopenfilename(
            title=f"Select {self._face_labels[index]} Image",
            filetypes=[("Image Files", "*.png *.jpg *.jpeg *.tga *.bmp"), ("All Files", "*.*")]
        )
        if path:
            self._face_entries[index].delete(0, tk.END)
            self._face_entries[index].insert(0, path)
            
            # 첫 번째 파일(Right Side) 선택 시 출력 경로 자동 제안
            if index == 0 and not self._output_path.get().strip():
                base_dir = os.path.dirname(path)
                base_name = os.path.splitext(os.path.basename(path))[0]
                self._output_path.insert(0, os.path.join(base_dir, f"{base_name}_CubeMap.ktx"))

    def _browse_output_file(self):
        path = filedialog.asksaveasfilename(
            title="Save KTX CubeMap",
            defaultextension=".ktx",
            filetypes=[("KTX Texture", "*.ktx")]
        )
        if path:
            self._output_path.delete(0, tk.END)
            self._output_path.insert(0, path)

    def _clicked_convert(self):
        face_paths = [ent.get().strip() for ent in self._face_entries]
        output_path = self._output_path.get().strip()

        # 유효성 검사
        if any(not p or not os.path.isfile(p) for p in face_paths):
            messagebox.showerror("Error", "Please select valid image files for all 6 faces.")
            return
        if not output_path:
            messagebox.showerror("Error", "Please specify an output file path.")
            return

        exe_path = self._window.get_data("exe_path")
        if not exe_path:
            messagebox.showerror("Error", "AssetConverter path lost!")
            return

        # 총 22개의 인자를 생성
        # AssetConverter.exe --cubemap <p1 fx1 fy1> ... <p6 fx6 fy6> <out> <sRGB>
        cmd = [exe_path, "--cubemap"]
        
        # 1. 6개의 면에 대해 Triplet(Path, FlipX, FlipY) 추가 (18개 인자)
        for i in range(6):
            cmd.append(face_paths[i])
            cmd.append("1" if self._face_flip_xs[i].get() else "0")
            cmd.append("1" if self._face_flip_ys[i].get() else "0")
        
        # 2. 출력 경로 추가 (20번째 인자)
        cmd.append(output_path)
        
        # 3. sRGB 여부 추가 (21번째 인자, 필수)
        cmd.append("1" if self._is_srgb.get() else "0")

        # 실행 로그창 초기화
        if self._log_win is not None and self._log_win.winfo_exists():
            self._log_win.destroy()

        parent = self.winfo_toplevel()
        self._log_win = LogWindow(parent, title="Generating KTX CubeMap...")
        self._log_win.set_busy(True, "Stitching 6 faces into KTX...")
        
        self._convert_button.config(state="disabled")
        self._runner.run(cmd)
        self._poll_runner(output_path)

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

        self._convert_button.config(state="normal")
        if self._log_win and self._log_win.winfo_exists():
            self._log_win.set_busy(False, f"Done (code={done_code})")
            self.winfo_toplevel().bell()

            if done_code == 0:
                self._log_win.show_modal_result("Success", f"CubeMap Created Successfully!\n\n{final_output_path}")
                if self._log_win.winfo_exists(): self._log_win.destroy()
                self._log_win = None
            else:
                self._log_win.show_modal_result("Failed", f"Exit Code: {done_code}\nCheck log for details.")

    def _clicked_back(self):
        self._window.set_context_by_name("main")