# Final Exam Project - 踩地雷 (Flask + C)

這是一個以 Flask (Python) 搭配 C 語言遊戲核心實作的踩地雷專案。  
前端透過 JavaScript 呼叫 Flask API，Flask 再用 ctypes 連接 C 動態函式庫處理遊戲邏輯。


## 功能特色

- 三種難度：easy、medium、hard
- 左鍵開格、右鍵插旗
- 即時顯示遊戲狀態（playing / win / lose）
- 顯示剩餘地雷數
- C 語言核心提供地圖、開格、插旗、勝敗判定

## 環境需求

- macOS 或 Linux
- Python 3.10+（建議）
- C 編譯器（macOS 使用 clang，Linux 使用 gcc）

## 快速開始

1) 建立虛擬環境（可選）

```bash
python3 -m venv .venv
```

2) 安裝 Python 套件

```bash
./.venv/bin/pip install -r requirements.txt
```

若未使用虛擬環境，也可改用：

```bash
pip install -r requirements.txt
```

3) 編譯 C 動態函式庫

```bash
./build.sh
```

4) 啟動伺服器

```bash
./.venv/bin/python app.py
```

若未使用虛擬環境，也可改用：

```bash
python app.py
```

5) 開啟網頁  
在瀏覽器打開：`http://127.0.0.1:5000`

## 專案結構

```text
project/
├─ app.py
├─ build.sh
├─ requirements.txt
├─ c_backend/
│  ├─ minesweeper.c
│  └─ libminesweeper.*      # 編譯後輸出（.dylib / .so）
├─ python_bridge/
│  └─ c_engine.py
├─ templates/
│  └─ index.html
├─ static/
│  ├─ game.js
│  └─ style.css
├─ FINAL_PROJECT_PLAN.md
└─ SPEC_CHECKLIST.md
```

## API 簡介

- `POST /api/new`：建立新遊戲（傳入 `level`）
- `POST /api/open`：開啟格子（傳入 `row`, `col`）
- `POST /api/flag`：切換旗標（傳入 `row`, `col`）
- `GET /api/state`：取得目前盤面與狀態

回傳資料包含：

- `board`：目前盤面資料
- `state`：遊戲狀態
- `stats`：地雷與旗標統計

## 常見問題

### 找不到 C 動態函式庫

若出現「找不到 C 動態函式庫，請先執行 ./build.sh」，請確認：

- 已執行 `./build.sh`
- `c_backend/` 底下有 `libminesweeper.dylib`（macOS）或 `libminesweeper.so`（Linux）

### 權限不足無法執行 build.sh

先給執行權限再重試：

```bash
chmod +x build.sh
./build.sh
```
