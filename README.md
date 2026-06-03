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

#url
https://othello-web-ui.onrender.com/


## 仕組み

- ブラウザ側で盤面UI・着手処理を行う
- AI手番になると `/api/search` に現在盤面を送る
- Node.jsサーバーが `ai_cli.exe` を起動する
- `ai_cli.exe` が `search(b, depth)` を呼び、着手を返す
- ブラウザがAIの手を盤面に反映する
