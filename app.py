from flask import Flask, jsonify, render_template, request

from python_bridge.c_engine import CEngine

app = Flask(__name__)
engine = CEngine()


@app.get("/")
def index():
    return render_template("index.html")


@app.post("/api/new")
def api_new():
    data = request.get_json(force=True, silent=True) or {}
    level = data.get("level", "easy")
    try:
        return jsonify(engine.new_game(level))
    except Exception as exc:
        return jsonify({"error": str(exc)}), 400


@app.post("/api/open")
def api_open():
    data = request.get_json(force=True, silent=True) or {}
    row = int(data.get("row", -1))
    col = int(data.get("col", -1))
    try:
        return jsonify(engine.open_cell(row, col))
    except Exception as exc:
        return jsonify({"error": str(exc)}), 400


@app.post("/api/flag")
def api_flag():
    data = request.get_json(force=True, silent=True) or {}
    row = int(data.get("row", -1))
    col = int(data.get("col", -1))
    try:
        return jsonify(engine.toggle_flag(row, col))
    except Exception as exc:
        return jsonify({"error": str(exc)}), 400


@app.get("/api/state")
def api_state():
    try:
        return jsonify(engine.get_state())
    except Exception as exc:
        return jsonify({"error": str(exc)}), 400


if __name__ == "__main__":
    app.run(debug=True)
