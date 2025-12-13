import tkinter as tk

class GUIContextBase(tk.Frame):
    def __init__(self, parent : tk.Widget, window):
        super().__init__(parent)
        self._window = window
        self._build_ui()

    def _build_ui(self):
        raise NotImplementedError