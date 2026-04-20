const boardEl = document.getElementById("board");
const statusEl = document.getElementById("status");
const minesLeftEl = document.getElementById("mines-left");
const levelEl = document.getElementById("level");
const newBtn = document.getElementById("new-btn");

let game = null;

function cellText(cell) {
  if (!cell.open) return cell.flag ? "🚩" : "";
  if (cell.mine) return "💣";
  if (cell.n > 0) return String(cell.n);
  return "";
}

function render() {
  if (!game) return;
  const rows = game.stats.rows;
  const cols = game.stats.cols;
  boardEl.style.gridTemplateColumns = `repeat(${cols}, 28px)`;
  boardEl.innerHTML = "";

  for (let r = 0; r < rows; r++) {
    for (let c = 0; c < cols; c++) {
      const cell = game.board[r][c];
      const btn = document.createElement("button");
      btn.className = "cell";
      if (cell.open) btn.classList.add("open");
      if (cell.open && cell.mine) btn.classList.add("mine");
      btn.textContent = cellText(cell);

      btn.addEventListener("click", async () => {
        if (game.state !== "playing") return;
        await postAndRender("/api/open", { row: r, col: c });
      });

      btn.addEventListener("contextmenu", async (e) => {
        e.preventDefault();
        if (game.state !== "playing") return;
        await postAndRender("/api/flag", { row: r, col: c });
      });

      boardEl.appendChild(btn);
    }
  }
}

function updateHeader() {
  if (!game) {
    statusEl.textContent = "state: waiting";
    minesLeftEl.textContent = "剩餘地雷: -";
    return;
  }
  statusEl.textContent = `state: ${game.state}`;
  minesLeftEl.textContent = `剩餘地雷: ${game.stats.mines_left}`;
}

async function postAndRender(url, payload) {
  const res = await fetch(url, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload),
  });
  game = await res.json();
  if (game.error) {
    alert(game.error);
    return;
  }
  updateHeader();
  render();
}

newBtn.addEventListener("click", async () => {
  await postAndRender("/api/new", { level: levelEl.value });
});
