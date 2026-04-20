const boardEl = document.getElementById("board");
const statusEl = document.getElementById("status");
const rowsEl = document.getElementById("rows");
const colsEl = document.getElementById("cols");
const minesEl = document.getElementById("mines");
const newGameBtn = document.getElementById("newGame");

let game = null;

function getCell(r, c) {
  return game.cells.find((x) => x.r === r && x.c === c);
}

function render() {
  if (!game) return;
  boardEl.style.gridTemplateColumns = `repeat(${game.cols}, 30px)`;
  boardEl.innerHTML = "";

  for (let r = 0; r < game.rows; r++) {
    for (let c = 0; c < game.cols; c++) {
      const cell = getCell(r, c);
      const btn = document.createElement("button");
      btn.className = "cell";
      if (cell.revealed) btn.classList.add("revealed");
      if (cell.revealed && cell.mine) btn.classList.add("mine");

      if (cell.revealed) {
        if (cell.mine) btn.textContent = "💣";
        else if (cell.n > 0) btn.textContent = String(cell.n);
      } else if (cell.flagged) {
        btn.textContent = "🚩";
      }

      btn.addEventListener("click", async () => {
        if (game.gameOver) return;
        const res = await fetch("/api/reveal", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ r, c }),
        });
        game = await res.json();
        updateStatus();
        render();
      });

      btn.addEventListener("contextmenu", async (e) => {
        e.preventDefault();
        if (game.gameOver) return;
        const res = await fetch("/api/flag", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ r, c }),
        });
        game = await res.json();
        updateStatus();
        render();
      });

      boardEl.appendChild(btn);
    }
  }
}

function updateStatus() {
  if (!game) {
    statusEl.textContent = "按「新局」開始";
    return;
  }
  if (game.gameOver === 1) statusEl.textContent = "你踩到地雷，遊戲結束。";
  else if (game.gameOver === 2) statusEl.textContent = "恭喜通關！";
  else statusEl.textContent = "遊戲進行中...";
}

newGameBtn.addEventListener("click", async () => {
  const rows = Number(rowsEl.value);
  const cols = Number(colsEl.value);
  const mines = Number(minesEl.value);

  const res = await fetch("/api/new", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ rows, cols, mines }),
  });
  game = await res.json();
  if (game.error) {
    statusEl.textContent = game.error;
    return;
  }
  updateStatus();
  render();
});
