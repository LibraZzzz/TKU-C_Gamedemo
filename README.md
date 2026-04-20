# 踩地雷(試做)

以「Flask (Python) + C 核心 + ctypes 整合」做的踩地雷範例專案。

參考範例 repo：
`https://github.com/ryanjih/cgame1142demo.git`

## 專案結構

```text
TKU-C_Gamedemo/
├─ backend/
│  ├─ minesweeper.h
│  ├─ minesweeper.c
│  ├─ queue.h
│  ├─ queue.c
│  └─ Makefile
├─ frontend/
│  ├─ app.py
│  ├─ templates/
│  │  └─ index.html
│  └─ static/
│     ├─ style.css
│     └─ game.js
└─ requirements.txt
```

## 如何執行

1) 編譯 C 動態函式庫

```bash
cd backend
make
```

2) 安裝 Python 套件

```bash
pip install -r requirements.txt
```

3) 啟動 Flask

```bash
cd frontend
python app.py
```

4) 開啟瀏覽器

`http://127.0.0.1:5000`

## 已完成功能

- 可自訂 rows / cols / mines 建立新局
- 左鍵翻格、右鍵插旗
- 遇到 0 時以 BFS 自動展開
- 踩到地雷顯示失敗，全部安全格翻開顯示勝利
