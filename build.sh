#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRC="$ROOT_DIR/c_backend/minesweeper.c"
OUT_DIR="$ROOT_DIR/c_backend"

OS_NAME="$(uname -s)"
if [[ "$OS_NAME" == "Darwin" ]]; then
  echo "Building for macOS (.dylib)..."
  clang -shared -fPIC -O2 -std=c11 "$SRC" -o "$OUT_DIR/libminesweeper.dylib"
elif [[ "$OS_NAME" == "Linux" ]]; then
  echo "Building for Linux (.so)..."
  gcc -shared -fPIC -O2 -std=c11 "$SRC" -o "$OUT_DIR/libminesweeper.so"
else
  echo "Unsupported OS: $OS_NAME (expected macOS or Linux)"
  exit 1
fi

echo "Build done."
