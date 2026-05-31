// server.js
// Node.js backend for Othello web UI.
// It serves the browser UI and calls ai_cli.exe for AI search.

const express = require("express");
const path = require("path");
const { spawn } = require("child_process");

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.json({ limit: "1mb" }));
app.use(express.static(path.join(__dirname, "public")));

const AI_EXE = process.platform === "win32"
  ? path.join(__dirname, "ai_cli.exe")
  : path.join(__dirname, "ai_cli");

app.post("/api/search", (req, res) => {
  const { board, player, depth } = req.body;

  if (!Array.isArray(board) || board.length !== 64) {
    return res.status(400).json({ error: "board must be an array of 64 integers" });
  }

  const p = Number(player);
  const d = Number(depth || 7);

  if (!(p === 0 || p === 1)) {
    return res.status(400).json({ error: "player must be 0 or 1" });
  }

  const input = `${p} ${d}\n${board.map(Number).join(" ")}\n`;

  const child = spawn(AI_EXE, [], {
    cwd: __dirname,
    stdio: ["pipe", "pipe", "pipe"],
  });

  let stdout = "";
  let stderr = "";

  const timeout = setTimeout(() => {
    child.kill();
    res.status(500).json({ error: "AI timeout" });
  }, 60000);

  child.stdout.on("data", (data) => {
    stdout += data.toString();
  });

  child.stderr.on("data", (data) => {
    stderr += data.toString();
  });

  child.on("error", (err) => {
    clearTimeout(timeout);
    res.status(500).json({
      error: "failed to start ai_cli",
      detail: err.message,
    });
  });

  child.on("close", () => {
    clearTimeout(timeout);

    const text = stdout.trim().split(/\s+/)[0];
    const move = Number(text);

    if (!Number.isInteger(move)) {
      return res.status(500).json({
        error: "AI returned invalid output",
        stdout,
        stderr,
      });
    }

    res.json({ move });
  });

  child.stdin.write(input);
  child.stdin.end();
});

app.listen(PORT, () => {
  console.log(`Othello web UI: http://localhost:${PORT}`);
});
