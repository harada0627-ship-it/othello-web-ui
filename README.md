# Othello Web UI

## 構成

```text
othello_web_ui/
  ai_cli.cpp
　ai1.cpp
　board.hpp
　cell_evaluate.hpp
  server.js
  package.json
  public/
    index.html
    styles.css
    app.js
```

さらに、同じフォルダにあなたのAI用ファイルを置いてください。

```text
ai1.cpp
board.hpp
cell_evaluate.hpp
```

## 1. C++ AI CLIをコンパイル

Windows + g++ の場合:

```powershell
g++ -std=c++17 -O2 -o ai_cli.exe ai_cli.cpp
```

bash/MSYS2/Git Bash の場合、フォルダ移動は例:

```bash
cd /c/myothelloai/ai_cell_mobility/othello_web_ui
g++ -std=c++17 -O2 -o ai_cli.exe ai_cli.cpp
```

## 2. Node.js依存関係を入れる

```powershell
npm install
```

## 3. サーバー起動

```powershell
npm start
```

または

```powershell
node server.js
```

## 4. ブラウザで開く

```text
http://localhost:3000
```

## 仕組み

- ブラウザ側で盤面UI・着手処理を行う
- AI手番になると `/api/search` に現在盤面を送る
- Node.jsサーバーが `ai_cli.exe` を起動する
- `ai_cli.exe` が `search(b, depth)` を呼び、着手を返す
- ブラウザがAIの手を盤面に反映する
