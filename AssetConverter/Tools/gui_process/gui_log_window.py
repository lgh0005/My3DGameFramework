import tkinter as tk
from tkinter import ttk

class LogWindow(tk.Toplevel):
    def __init__(self, parent, title="Working..."):
        super().__init__(parent)
        self.title(title)
        self.geometry("400x300")
        self.resizable(True, True)

        self._status = tk.Label(self, text="Idle")
        self._status.pack(anchor="w", padx=10, pady=(10, 0))

        self._bar = ttk.Progressbar(self, mode="indeterminate")
        self._bar.pack(fill="x", padx=10, pady=(6, 10))

        box = tk.Frame(self)
        box.pack(fill="both", expand=True, padx=10, pady=(0, 10))

        self._text = tk.Text(box, state="disabled")
        self._text.pack(side="left", fill="both", expand=True)

        sb = tk.Scrollbar(box, command=self._text.yview)
        sb.pack(side="right", fill="y")
        self._text.config(yscrollcommand=sb.set)

        # 작업 중 임의로 닫지 못하게(원하면 제거)
        self.protocol("WM_DELETE_WINDOW", lambda: None)

    def set_busy(self, busy: bool, msg: str):
        self._status.config(text=msg)
        if busy:
            self._bar.start(10)
        else:
            self._bar.stop()

    def append(self, line: str):
        self._text.configure(state="normal")
        self._text.insert(tk.END, line + "\n")
        self._text.see(tk.END)
        self._text.configure(state="disabled")

    def show_modal_result(self, title: str, message: str, *, ok_text: str = "OK") -> None:
        """LogWindow 위에 모달 서브 다이얼로그를 띄우고 OK까지 기다림."""
        dlg = tk.Toplevel(self)
        dlg.title(title)
        dlg.resizable(False, False)
        dlg.transient(self)      # 부모 위에 붙임
        dlg.grab_set()           # 모달로 만들기 (중요)
        dlg.focus_set()

        # 내용
        body = tk.Frame(dlg, padx=14, pady=12)
        body.pack(fill="both", expand=True)

        lbl = tk.Label(body, text=message, justify="left", wraplength=420)
        lbl.pack(anchor="w")

        # 버튼
        btns = tk.Frame(body)
        btns.pack(anchor="e", pady=(12, 0))

        def close():
            dlg.grab_release()
            dlg.destroy()

        ok = tk.Button(btns, text=ok_text, width=10, command=close)
        ok.pack()

        # 창 닫기(X)도 OK로 처리
        dlg.protocol("WM_DELETE_WINDOW", close)

        # 가운데 배치(간단 버전)
        dlg.update_idletasks()
        x = self.winfo_rootx() + (self.winfo_width() // 2) - (dlg.winfo_width() // 2)
        y = self.winfo_rooty() + (self.winfo_height() // 2) - (dlg.winfo_height() // 2)
        dlg.geometry(f"+{x}+{y}")

        # OK 누를 때까지 대기
        self.wait_window(dlg)
