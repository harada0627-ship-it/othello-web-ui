const BLACK = 0;
const WHITE = 1;
const VACANT = 2;

const boardEl = document.getElementById("board");
const statusEl = document.getElementById("status");
const blackCountEl = document.getElementById("blackCount");
const whiteCountEl = document.getElementById("whiteCount");
const historyEl = document.getElementById("history");
const humanColorEl = document.getElementById("humanColor");
const depthEl = document.getElementById("depth");
const newGameBtn = document.getElementById("newGame");
const passBtn = document.getElementById("passBtn");

const DIRS = [
  [-1, -1], [-1, 0], [-1, 1],
  [0, -1],           [0, 1],
  [1, -1],  [1, 0],  [1, 1],
];

let board = Array(64).fill(VACANT);
let turn = BLACK;
let humanColor = BLACK;
let aiColor = WHITE;
let history = [];
let thinking = false;
let lastMove = -1;

function opponent(c) {
  return c === BLACK ? WHITE : BLACK;
}

function inside(r, c) {
  return r >= 0 && r < 8 && c >= 0 && c < 8;
}

function flipsForMove(pos, color) {
  if (board[pos] !== VACANT) return [];

  const [r0, c0] = [Math.floor(pos / 8), pos % 8];
  const opp = opponent(color);
  const flips = [];

  for (const [dr, dc] of DIRS) {
    let r = r0 + dr;
    let c = c0 + dc;
    const temp = [];

    while (inside(r, c) && board[r * 8 + c] === opp) {
      temp.push(r * 8 + c);
      r += dr;
      c += dc;
    }

    if (inside(r, c) && board[r * 8 + c] === color && temp.length > 0) {
      flips.push(...temp);
    }
  }

  return flips;
}

function legalMoves(color) {
  const moves = [];
  for (let i = 0; i < 64; i++) {
    if (flipsForMove(i, color).length > 0) moves.push(i);
  }
  return moves;
}

function applyMove(pos, color) {
  const flips = flipsForMove(pos, color);
  if (flips.length === 0) return false;

  board[pos] = color;
  for (const p of flips) board[p] = color;

  lastMove = pos;
  history.push({ color, pos });
  turn = opponent(color);
  return true;
}

function newGame() {
  board = Array(64).fill(VACANT);
  board[27] = WHITE;
  board[28] = BLACK;
  board[35] = BLACK;
  board[36] = WHITE;

  humanColor = Number(humanColorEl.value);
  aiColor = opponent(humanColor);
  turn = BLACK;
  history = [];
  thinking = false;
  lastMove = -1;

  render();

  if (turn === aiColor) {
    setTimeout(requestAiMove, 300);
  }
}

function render() {
  boardEl.innerHTML = "";
  const legal = turn === humanColor && !thinking ? new Set(legalMoves(turn)) : new Set();

  for (let i = 0; i < 64; i++) {
    const cell = document.createElement("div");
    cell.className = "cell";
    if (i === lastMove) cell.classList.add("last-move");
    cell.addEventListener("click", () => onCellClick(i));

    if (legal.has(i)) {
      const hint = document.createElement("div");
      hint.className = "legal";
      cell.appendChild(hint);
    }

    if (board[i] !== VACANT) {
      const stone = document.createElement("div");
      stone.className = `stone ${board[i] === BLACK ? "black" : "white"}`;
      cell.appendChild(stone);
    }

    boardEl.appendChild(cell);
  }

  updateStatus();
  updateHistory();
}

function updateStatus() {
  const b = board.filter(x => x === BLACK).length;
  const w = board.filter(x => x === WHITE).length;

  blackCountEl.textContent = b;
  whiteCountEl.textContent = w;

  const turnText = turn === BLACK ? "黒" : "白";
  const humanText = humanColor === BLACK ? "黒" : "白";
  const aiText = aiColor === BLACK ? "黒" : "白";

  const legal = legalMoves(turn).length;

  statusEl.textContent =
    `${thinking ? "AI思考中..." : `手番: ${turnText}`}\n` +
    `あなた: ${humanText} / AI: ${aiText}\n` +
    `合法手数: ${legal}\n` +
    `黒: ${b}  白: ${w}`;
}

function updateHistory() {
  const record = history.map(h => {
    const r = Math.floor(h.pos / 8);
    const c = h.pos % 8;
    return `${String.fromCharCode(97 + c)}${r + 1}`;
  }).join("");

  historyEl.textContent = record;
}

function onCellClick(pos) {
  if (thinking) return;
  if (turn !== humanColor) return;

  if (applyMove(pos, turn)) {
    afterMove();
  }
}

function passTurn() {
  if (thinking) return;
  if (legalMoves(turn).length > 0) {
    alert("合法手があるためパスできません。");
    return;
  }
  history.push({ color: turn, pos: -1 });
  turn = opponent(turn);
  afterMove();
}

function isGameOver() {
  return legalMoves(BLACK).length === 0 && legalMoves(WHITE).length === 0;
}

function afterMove() {
  render();

  if (isGameOver()) {
    showGameOver();
    return;
  }

  if (legalMoves(turn).length === 0) {
    history.push({ color: turn, pos: -1 });
    turn = opponent(turn);
    render();

    if (isGameOver()) {
      showGameOver();
      return;
    }
  }

  if (turn === aiColor) {
    setTimeout(requestAiMove, 250);
  }
}

function showGameOver() {
  const b = board.filter(x => x === BLACK).length;
  const w = board.filter(x => x === WHITE).length;
  let msg = `終局: 黒 ${b} - 白 ${w}`;
  if (b > w) msg += "\n黒勝ち";
  else if (w > b) msg += "\n白勝ち";
  else msg += "\n引き分け";
  setTimeout(() => alert(msg), 50);
}

async function requestAiMove() {
  if (thinking) return;

  const legal = legalMoves(turn);
  if (legal.length === 0) {
    afterMove();
    return;
  }

  thinking = true;
  render();

  const depth = Math.max(1, Number(depthEl.value || 7));

  try {
    const res = await fetch("/api/search", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ board, player: turn, depth }),
    });

    const data = await res.json();

    let move = Number(data.move);
    if (!legal.includes(move)) {
      move = legal[0];
    }

    applyMove(move, turn);
  } catch (e) {
    alert(`AI呼び出しエラー: ${e.message}`);
  } finally {
    thinking = false;
    afterMove();
  }
}

newGameBtn.addEventListener("click", newGame);
passBtn.addEventListener("click", passTurn);
humanColorEl.addEventListener("change", newGame);

newGame();
