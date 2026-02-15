import tkinter as tk
from gui_context.gui_context_exe_verify import GuiContextVerifyAssetConverter
from gui_context.gui_context_main import GuiContextConvertMain
from gui_context.gui_context_convert_animation import GuiContextConvertAnimation
from gui_context.gui_context_convert_model import GuiContextConvertModel
from gui_context.gui_context_convert_orm import GuiContextConvertORMTexture
from gui_context.gui_context_convert_ktx import GuiContextConvertTextureKTX

class Window:

    title: str = "My Asset Converter"
    width: int = 500
    height: int = 400
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
            "verify": GuiContextVerifyAssetConverter,
            "main": GuiContextConvertMain,
            "animation": GuiContextConvertAnimation,
            "model": GuiContextConvertModel,
            "orm": GuiContextConvertORMTexture,
            "ktx": GuiContextConvertTextureKTX
        }
        self._app_data = {
            "exe_path": "",       
            "last_input_dir": "", 
            "last_output_dir": "" 
        }

    def _set_context(self, context_cls):
        if self._current_context:
            self._current_context.destroy()

        self._current_context = context_cls(self._root, self)
        self._current_context.pack(fill="both", expand=True)

    def get_data(self, key):
        return self._app_data.get(key)

    def set_data(self, key, value):
        self._app_data[key] = value

    def set_context_by_name(self, name: str):
        context_cls = self._context_classes[name]
        self._set_context(context_cls)

    def destroy(self):
        self._root.destroy()

    def run(self, context_cls=GuiContextVerifyAssetConverter):
        self._set_context(context_cls)
        self._root.mainloop()
