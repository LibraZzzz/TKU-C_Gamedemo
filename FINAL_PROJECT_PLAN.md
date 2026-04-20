# FINAL_PROJECT_PLAN

## Goal
- Build a Minesweeper web app with Flask frontend and C backend game engine.

## Scope
- Difficulty levels: easy, medium, hard.
- Core interactions: open cell, toggle flag, game state update.
- Flask APIs for new game, open, flag, and state.
- C engine compiled into shared library and called via ctypes.

## Architecture
- `app.py`: Flask routes and API responses.
- `python_bridge/c_engine.py`: ctypes bridge and response shaping.
- `c_backend/minesweeper.c`: board data, mine placement, open/flag logic, win/lose state.
- `templates/index.html` + `static/game.js/style.css`: UI rendering and interaction.

## Delivery Checklist
- Build script for macOS/Linux (`build.sh`).
- `requirements.txt` for Python dependency install.
- README with run instructions and troubleshooting.
