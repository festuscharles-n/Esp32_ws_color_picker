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

  socket.on('client-event', (data) => {
    console.log('Received sensor data:', data);
    const { pwm } = data
    io.emit("esp32-event", { pwm: pwm })
  });

  socket.on('sensor-event', (data) => {
    console.log('Received sensor data:', data);
    const { dataValue } = data
    io.emit("sensor-value", { dataValue })
  });
  
  // Smart window 
  socket.on('status-update', (status) => {
    console.log('Status Update:', status);
    // Handle the status update as needed (e.g., send notifications to users)
    socket.emit('status-update', status);
  });

  // Handle controlling stepper motor
  socket.on('control-stepper', (value) => {
    console.log('Control Stepper:', value);
    // Handle the stepper motor control command (e.g., trigger stepper movement)
  });

  // Smart Meter code
  socket.on('meter_data', (data) => {
    console.log('Sensor data received:', data);
    io.emit('meter_data', data);
  });

  //Smart Meter relay code
  socket.on('relay-control', ({ state, number }) => {
    console.log('Relay control command received:', state, number);
    io.emit('relay-control', { relayState: state, relayNumber: number });
  });

  // // Handle sensor data for color sensor shaaa...
  // socket.on('client-event', (data) => {
  //   console.log('Received sensor data:', data);
  //   const { pwm } = data
  //   console.log(data)
  //   console.log(typeof (data.pwm))
  //   io.emit("esp32-event", { pwm: pwm })
  // });

  // socket.on('esp32-client', (data) => {
  //   console.log('Received data from ESP32:', data);
  //   socket.emit('response-event', { message: 'Hello from server!' });
  // });
  
  // Handle disconnection
  socket.on('disconnect', () => {
    console.log('A client disconnected');
  });
});


const PORT = process.env.PORT || 9000;
server.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});
