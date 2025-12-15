import subprocess
import threading
import queue
import time

class ProcessRunner:
    def __init__(self, _tk_root=None):
        self._q = queue.Queue()
        self._proc = None

    def run(self, args: list[str]):
        def worker():
            try:
                self._q.put(("log", f"[CMD] {' '.join(args)}"))

                self._proc = subprocess.Popen(
                    args,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True,
                    bufsize=1,
                )

                while True:
                    line = self._proc.stdout.readline() if self._proc.stdout else ""
                    if line:
                        self._q.put(("log", line.rstrip()))
                    else:
                        code = self._proc.poll()
                        if code is not None:
                            break
                        time.sleep(0.02)

                code = self._proc.wait()
                self._q.put(("done", code))

            except Exception as e:
                self._q.put(("log", f"[ERROR] {e}"))
                self._q.put(("done", -1))

        threading.Thread(target=worker, daemon=True).start()

    def poll(self):
        items = []
        while True:
            try:
                items.append(self._q.get_nowait())
            except queue.Empty:
                break
        return items

    def terminate(self):
        if self._proc and self._proc.poll() is None:
            self._proc.terminate()
