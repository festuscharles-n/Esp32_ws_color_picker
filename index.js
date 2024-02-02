const express = require('express');
const http = require('http');
const path = require('path');
const socketIo = require('socket.io');
require("dotenv").config();
const port = process.env.PORT || 4000

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

app.get('/', (req, res) => {
  res.render('index');
});

io.on('connection', (socket) => {
  console.log('A user connected');

  setInterval(() => {
    io.emit('server-event', 'You are connected to a server');
  }, 1000);

  // Handle 'status' event from ESP32
  socket.on('status', (message) => {
    console.log('Status from ESP32:', message);
  });

  socket.on('client-event', (data) => {
    console.log('Received client event:', data);

    // Extract color information from data (assuming data contains red, green, and blue properties)
    const { red, green, blue } = data;
    
    socket.emit('esp32-event', { red, green, blue });
  });

  socket.on('esp32-client', (data) => {
    console.log('Received data from ESP32:', data);

    // You can send a response back to the ESP32 if needed
    // socket.emit('response-event', { message: 'Hello from server!' });
  });

  socket.on('disconnect', () => {
    console.log('User disconnected');
  });
});

server.listen(port, () => {
  console.log(`Server running on http://localhost:${port}`);
});
