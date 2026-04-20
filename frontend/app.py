import json
import os
from ctypes import c_char_p, c_int, c_void_p, cdll

from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
BACKEND_DIR = os.path.join(ROOT, "backend")

LIB_CANDIDATES = [
    os.path.join(BACKEND_DIR, "minesweeper.dll"),
    os.path.join(BACKEND_DIR, "libminesweeper.so"),
    os.path.join(BACKEND_DIR, "libminesweeper.dylib"),
]

lib_path = next((p for p in LIB_CANDIDATES if os.path.exists(p)), None)
if lib_path is None:
    raise RuntimeError("找不到 C 動態函式庫，請先到 backend 編譯。")

lib = cdll.LoadLibrary(lib_path)

lib.create_board.argtypes = [c_int, c_int, c_int]
lib.create_board.restype = c_void_p
lib.destroy_board.argtypes = [c_void_p]
lib.reveal_cell.argtypes = [c_void_p, c_int, c_int]
lib.reveal_cell.restype = c_int
lib.toggle_flag.argtypes = [c_void_p, c_int, c_int]
lib.toggle_flag.restype = c_int
lib.get_state_json.argtypes = [c_void_p]
lib.get_state_json.restype = c_char_p

board_ptr = c_void_p()


def board_state():
    raw = lib.get_state_json(board_ptr)
    return json.loads(raw.decode("utf-8"))


@app.route("/")
def index():
    return render_template("index.html")


@app.post("/api/new")
def new_game():
    global board_ptr

    payload = request.get_json(force=True)
    rows = int(payload.get("rows", 9))
    cols = int(payload.get("cols", 9))
    mines = int(payload.get("mines", 10))

    if board_ptr:
        lib.destroy_board(board_ptr)
    board_ptr = lib.create_board(rows, cols, mines)
    if not board_ptr:
        return jsonify({"error": "建立棋盤失敗，請檢查 rows/cols/mines"}), 400
    return jsonify(board_state())


@app.post("/api/reveal")
def reveal():
    payload = request.get_json(force=True)
    r = int(payload.get("r", -1))
    c = int(payload.get("c", -1))
    if not board_ptr:
        return jsonify({"error": "請先 new game"}), 400
    lib.reveal_cell(board_ptr, r, c)
    return jsonify(board_state())


@app.post("/api/flag")
def flag():
    payload = request.get_json(force=True)
    r = int(payload.get("r", -1))
    c = int(payload.get("c", -1))
    if not board_ptr:
        return jsonify({"error": "請先 new game"}), 400
    lib.toggle_flag(board_ptr, r, c)
    return jsonify(board_state())


if __name__ == "__main__":
    app.run(debug=True)
