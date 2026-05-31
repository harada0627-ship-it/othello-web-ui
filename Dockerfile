FROM node:20-bookworm

WORKDIR /app

RUN apt-get update \
    && apt-get install -y --no-install-recommends g++ ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY package*.json ./
RUN npm install --omit=dev

COPY . .

RUN g++ -std=c++17 -O2 -o ai_cli ai_cli.cpp
RUN chmod +x ai_cli

EXPOSE 3000

CMD ["node", "server.js"]