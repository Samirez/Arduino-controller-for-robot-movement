const dgram = require('node:dgram');
const socket = dgram.createSocket('udp4');
const readline = require('node:readline');

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

  if (msg.toString().startsWith('ready')) {
    socket.send('0', rinfo.port, rinfo.address, (err) => {
      if (err) {
        console.log(err);
      }
    });
  }
});

socket.on('listening', () => {
  const address = socket.address();
  console.log(`server listening ${address.address}:${address.port}`);
});

socket.bind(10000);

rl.on('line', (input) => {
  socket.send(input, 10000, '192.168.50.221', (err) => {
    if (err) {
      console.log(err);
    }
  });
});
