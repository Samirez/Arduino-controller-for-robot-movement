const dgram = require('node:dgram');
const socket = dgram.createSocket('udp4');

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
