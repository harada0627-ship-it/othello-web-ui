import subprocess
import tkinter as tk
from tkinter import messagebox

BLACK = 0
WHITE = 1
VACANT = 2

AI_PATH = "./ai_cli.exe"

CELL_SIZE = 64
BOARD_SIZE = CELL_SIZE * 8

def initial_board():
    cells = [VACANT] * 64
    cells[27] = WHITE
    cells[28] = BLACK
    cells[35] = BLACK
    cells[36] = WHITE
    return cells

def opponent(p):
    return WHITE if p == BLACK else BLACK

def inside(r, c):
    return 0 <= r < 8 and 0 <= c < 8

def flips_for_move(cells, player, move):
    if cells[move] != VACANT:
        return []

    r, c = divmod(move, 8)
    opp = opponent(player)
    result = []
    dirs = [(-1,-1), (-1,0), (-1,1),
            (0,-1),           (0,1),
            (1,-1),  (1,0),  (1,1)]

    for dr, dc in dirs:
        rr, cc = r + dr, c + dc
        temp = []

        while inside(rr, cc):
            idx = rr * 8 + cc

            if cells[idx] == opp:
                temp.append(idx)
            elif cells[idx] == player:
                if temp:
                    result.extend(temp)
                break
            else:
                break

            rr += dr
            cc += dc

    return result

def legal_moves(cells, player):
    return [i for i in range(64) if flips_for_move(cells, player, i)]

def apply_move(cells, player, move):
    flips = flips_for_move(cells, player, move)
    if not flips:
        return False

    cells[move] = player
    for i in flips:
        cells[i] = player
    return True

def index_to_coord(i):
    r, c = divmod(i, 8)
    return chr(ord("a") + c) + str(r + 1)

def call_ai(cells, player, depth):
    input_text = f"{player} {depth}\n" + " ".join(map(str, cells)) + "\n"

    result = subprocess.run(
        [AI_PATH],
        input=input_text,
        text=True,
        capture_output=True
    )

    if result.returncode != 0:
        raise RuntimeError(result.stderr.strip() or "AI実行に失敗しました")

    try:
        return int(result.stdout.strip())
    except ValueError:
        raise RuntimeError(f"AI出力が不正です: {result.stdout!r}")

class OthelloApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Othello AI GUI")

        self.cells = initial_board()
        self.human = BLACK
        self.ai = WHITE
        self.player = BLACK
        self.pass_count = 0
        self.game_started = False

        top = tk.Frame(root)
        top.pack(pady=8)

        tk.Label(top, text="あなたの色").pack(side=tk.LEFT)
        self.color_var = tk.StringVar(value="black")
        tk.Radiobutton(top, text="黒", variable=self.color_var, value="black").pack(side=tk.LEFT)
        tk.Radiobutton(top, text="白", variable=self.color_var, value="white").pack(side=tk.LEFT)

        tk.Label(top, text="探索深さ").pack(side=tk.LEFT, padx=(16, 0))
        self.depth_var = tk.StringVar(value="5")
        tk.Entry(top, textvariable=self.depth_var, width=5).pack(side=tk.LEFT)

        tk.Button(top, text="開始 / リセット", command=self.start_game).pack(side=tk.LEFT, padx=12)

        self.status = tk.Label(root, text="色と探索深さを選んで開始してください", font=("Arial", 12))
        self.status.pack(pady=4)

        self.canvas = tk.Canvas(root, width=BOARD_SIZE, height=BOARD_SIZE, bg="darkgreen")
        self.canvas.pack(padx=10, pady=10)
        self.canvas.bind("<Button-1>", self.on_click)

        self.draw_board()

    def start_game(self):
        self.cells = initial_board()
        self.human = BLACK if self.color_var.get() == "black" else WHITE
        self.ai = opponent(self.human)
        self.player = BLACK
        self.pass_count = 0
        self.game_started = True
        self.draw_board()
        self.update_status()

        if self.player == self.ai:
            self.root.after(300, self.ai_turn)

    def current_depth(self):
        try:
            d = int(self.depth_var.get())
            return max(1, d)
        except ValueError:
            return 5

    def draw_board(self):
        self.canvas.delete("all")

        for r in range(8):
            for c in range(8):
                x1 = c * CELL_SIZE
                y1 = r * CELL_SIZE
                x2 = x1 + CELL_SIZE
                y2 = y1 + CELL_SIZE
                self.canvas.create_rectangle(x1, y1, x2, y2, fill="darkgreen", outline="black")

                idx = r * 8 + c
                v = self.cells[idx]

                if v != VACANT:
                    color = "black" if v == BLACK else "white"
                    self.canvas.create_oval(
                        x1 + 7, y1 + 7, x2 - 7, y2 - 7,
                        fill=color, outline="black"
                    )

        if self.game_started and self.player == self.human:
            for m in legal_moves(self.cells, self.human):
                r, c = divmod(m, 8)
                x = c * CELL_SIZE + CELL_SIZE // 2
                y = r * CELL_SIZE + CELL_SIZE // 2
                self.canvas.create_oval(
                    x - 8, y - 8, x + 8, y + 8,
                    fill="yellow", outline=""
                )

    def update_status(self):
        black_count = self.cells.count(BLACK)
        white_count = self.cells.count(WHITE)

        if self.pass_count >= 2:
            if black_count > white_count:
                winner = "黒の勝ち"
            elif white_count > black_count:
                winner = "白の勝ち"
            else:
                winner = "引き分け"
            self.status.config(text=f"終了  黒:{black_count} 白:{white_count}  {winner}")
            self.game_started = False
            return

        turn = "黒" if self.player == BLACK else "白"
        side = "あなた" if self.player == self.human else "AI"
        self.status.config(text=f"{side}の番（{turn}）  黒:{black_count} 白:{white_count}")

    def advance_turn(self):
        self.player = opponent(self.player)
        self.handle_passes()
        self.draw_board()
        self.update_status()

        if self.game_started and self.player == self.ai:
            self.root.after(300, self.ai_turn)

    def handle_passes(self):
        while self.pass_count < 2 and not legal_moves(self.cells, self.player):
            self.pass_count += 1
            self.player = opponent(self.player)

        if legal_moves(self.cells, self.player):
            self.pass_count = 0

    def on_click(self, event):
        if not self.game_started or self.player != self.human:
            return

        c = event.x // CELL_SIZE
        r = event.y // CELL_SIZE
        move = r * 8 + c

        if move not in legal_moves(self.cells, self.human):
            return

        apply_move(self.cells, self.human, move)
        self.advance_turn()

    def ai_turn(self):
        if not self.game_started or self.player != self.ai:
            return

        moves = legal_moves(self.cells, self.ai)
        if not moves:
            self.advance_turn()
            return

        self.status.config(text="AI思考中...")
        self.root.update_idletasks()

        try:
            move = call_ai(self.cells, self.ai, self.current_depth())
        except Exception as e:
            messagebox.showerror("AIエラー", str(e))
            self.game_started = False
            return

        if move not in moves:
            messagebox.showerror("AIエラー", f"AIが不正な手を返しました: {move}")
            self.game_started = False
            return

        apply_move(self.cells, self.ai, move)
        self.advance_turn()

def main():
    root = tk.Tk()
    app = OthelloApp(root)
    root.mainloop()

if __name__ == "__main__":
    main()
