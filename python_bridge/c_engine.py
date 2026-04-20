import json
import os
from ctypes import c_char_p, c_int, c_void_p, cdll


LEVEL_CONFIG = {
    "easy": {"rows": 9, "cols": 9, "mines": 10},
    "medium": {"rows": 16, "cols": 16, "mines": 40},
    "hard": {"rows": 16, "cols": 30, "mines": 99},
}


def _find_lib_path() -> str:
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    backend = os.path.join(root, "c_backend")
    candidates = [
        os.path.join(backend, "libminesweeper.dylib"),
        os.path.join(backend, "libminesweeper.so"),
    ]
    for path in candidates:
        if os.path.exists(path):
            return path
    raise RuntimeError("找不到 C 動態函式庫，請先執行 ./build.sh")


class CEngine:
    def __init__(self) -> None:
        self._lib = cdll.LoadLibrary(_find_lib_path())
        self._lib.ms_create.argtypes = [c_int, c_int, c_int]
        self._lib.ms_create.restype = c_void_p
        self._lib.ms_destroy.argtypes = [c_void_p]
        self._lib.ms_open.argtypes = [c_void_p, c_int, c_int]
        self._lib.ms_open.restype = c_int
        self._lib.ms_toggle_flag.argtypes = [c_void_p, c_int, c_int]
        self._lib.ms_toggle_flag.restype = c_int
        self._lib.ms_state_json.argtypes = [c_void_p]
        self._lib.ms_state_json.restype = c_char_p
        self._board = c_void_p()

    def new_game(self, level: str) -> dict:
        if level not in LEVEL_CONFIG:
            raise ValueError("level 必須為 easy / medium / hard")
        cfg = LEVEL_CONFIG[level]
        self._destroy_if_needed()
        self._board = self._lib.ms_create(cfg["rows"], cfg["cols"], cfg["mines"])
        if not self._board:
            raise RuntimeError("建立棋盤失敗")
        return self.get_state()

    def open_cell(self, row: int, col: int) -> dict:
        self._ensure_board()
        self._lib.ms_open(self._board, row, col)
        return self.get_state()

    def toggle_flag(self, row: int, col: int) -> dict:
        self._ensure_board()
        self._lib.ms_toggle_flag(self._board, row, col)
        return self.get_state()

    def get_state(self) -> dict:
        self._ensure_board()
        raw = self._lib.ms_state_json(self._board)
        payload = json.loads(raw.decode("utf-8"))
        return {
            "board": payload["board"],
            "state": payload["state"],
            "stats": {
                "mines_total": payload["mines"],
                "flags_used": payload["flags"],
                "mines_left": payload["mines"] - payload["flags"],
                "rows": payload["rows"],
                "cols": payload["cols"],
            },
        }

    def _destroy_if_needed(self) -> None:
        if self._board:
            self._lib.ms_destroy(self._board)
            self._board = c_void_p()

    def _ensure_board(self) -> None:
        if not self._board:
            raise RuntimeError("尚未建立遊戲，請先呼叫 /api/new")

