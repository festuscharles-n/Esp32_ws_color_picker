const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');
require("dotenv").config();
const port = process.env.PORT || 4000

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

let connectedClients = new Set();

app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

app.get('/', (req, res) => {
  res.render('index');
});

wss.on('connection', (ws) => {
  console.log('WebSocket client connected');
  connectedClients.add(ws);

  ws.on('message', (message) => {
    console.log('Received message:', message);
 
    // Broadcast sensor data to all connected clients
    wss.clients.forEach((client) => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(message);
      }
    });
  });

  ws.on('close', () => { 
    console.log('WebSocket client disconnected');
    connectedClients.delete(ws);
  });
});
 
// Handle LED control messages
app.post('/control', express.json(), (req, res) => {
  const { red, green, blue } = req.body;
  console.log('Received LED control:', { red, green, blue });

  // Broadcast LED control to all connected clients
  connectedClients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(JSON.stringify({ red, green, blue }));
    }
  }); 

  res.sendStatus(200);
}); 

server.listen(port, () => {
  console.log(`Server running on http://localhost:${port}`);
});
