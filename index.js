// server.js
const express = require('express');
const http = require('http');
const socketIo = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

// Serve static files from the public directory
app.use(express.static('public'));

// Socket.io connection handling
io.on('connection', (socket) => {
  console.log('Client connected', socket.id);

  setInterval(() => {
    io.emit('server-event', 'You are connected to a server');
  }, 1000);

  // Handle sensor data
  socket.on('client-event', (data) => {
    console.log('Received sensor data:', data);
    const { pwm } = data
    console.log(data)
    console.log(typeof (data.pwm))
    io.emit("esp32-event", { pwm: pwm })
  });

  socket.on('esp32-client', (data) => {
    console.log('Received data from ESP32:', data);
    socket.emit('response-event', { message: 'Hello from server!' });
  });
  
  // Handle disconnection
  socket.on('disconnect', () => {
    console.log('A client disconnected');
  });
});


const PORT = process.env.PORT || 9000;
server.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});
