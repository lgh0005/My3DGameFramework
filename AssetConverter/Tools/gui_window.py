import tkinter as tk

from gui_context.gui_context_main import GuiContextConvertMain
from gui_context.gui_context_convert_animation import GuiContextConvertAnimation
from gui_context.gui_context_convert_model import GuiContextConvertModel
from gui_context.gui_context_convert_orm import GuiContextConvertORMTexture

class Window:

    title: str = "My Asset Converter"
    width: int = 500
    height: int = 300
    resizeable: bool = False

    def __init__(self):
        self._root = tk.Tk()
        self._root.title(Window.title)
        self._root.geometry(f"{Window.width}x{Window.height}")
        self._root.resizable(Window.resizeable, Window.resizeable)
        self._root.minsize(Window.width, Window.height)
        self._root.maxsize(Window.width, Window.height)
        self._current_context = None
        self._context_classes = {
            "main": GuiContextConvertMain,
            "animation": GuiContextConvertAnimation,
            "model": GuiContextConvertModel,
            "orm": GuiContextConvertORMTexture
        }

    def _set_context(self, context_cls):
        if self._current_context:
            self._current_context.destroy()

        self._current_context = context_cls(self._root, self)
        self._current_context.pack(fill="both", expand=True)

    def set_context_by_name(self, name: str):
        context_cls = self._context_classes[name]
        self._set_context(context_cls)

    def run(self, context_cls=GuiContextConvertMain):
        self._set_context(context_cls)
        self._root.mainloop()