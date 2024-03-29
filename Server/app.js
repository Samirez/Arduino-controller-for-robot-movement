const dgram = require('node:dgram');
const socket = dgram.createSocket('udp4');
const readline = require('node:readline');
const fs = require('node:fs');

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

socket.on('error', (err) => {
  console.error(`server error:\n${err.stack}`);
  socket.close();
});

socket.on('message', (msg, rinfo) => {
  console.log(`server got: \"${msg}\" from ${rinfo.address}:${rinfo.port}`);

  const data = msg.toString().split(';');
  const now = Date.now();
  
  let append = '';

  for (let i = 0; i < data.length; i++) {
    append += "received," + now + ',' + data[i] + '\n';
  }

  fs.appendFileSync('logs.txt', append);
});

socket.on('listening', () => {
  const address = socket.address();
  console.log(`server listening ${address.address}:${address.port}`);
});

socket.bind(10000);

rl.on('line', (input) => {
  socket.send(input, 10000, '172.20.10.3', (err) => {
    if (err) {
      console.log(err);
    } else {
      fs.appendFileSync('logs.txt', "sent," + Date.now() + ',' + input + '\n');
    }
  });
});
