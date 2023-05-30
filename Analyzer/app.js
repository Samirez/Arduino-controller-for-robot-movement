const fs = require('node:fs');

const logs = fs.readFileSync('logs.txt', 'utf8').split('\n');

const interval = 100; // ms

// Distances are in cm
const position = {
    x: 0,
    y: 0,
};

// Robot angle in radians
let theta = 0;

const encoderDiameter = 2.5;
const encoderCircumference = encoderDiameter * Math.PI;
const encoderTicksPerRevolution = 20;
let lastCommand = null;

let leftWheelTicks = 0;
let rightWheelTicks = 0;

let windowStartTime = null;
let ticksInWindow = 0;

// Read lines one by one
for (let i = 0; i < logs.length - 1; i++) {
    const line = logs[i];
    const [type, timestamp, ...remaining] = line.split(',');
    if (type === 'received') {
        // console.log('received', timestamp);

        const [time, event, direction] = remaining;

        if (windowStartTime === null) {
            windowStartTime = time;
        }

        if (event === 'left') {
            if (direction === 'forward') {
                leftWheelTicks++;
            } else {
                leftWheelTicks--;
            }

            ticksInWindow++;
        } else if (event === 'right') {
            if (direction === 'forward') {
                rightWheelTicks++;
            } else {
                rightWheelTicks--;
            }

            ticksInWindow++;
        }

        if (time - windowStartTime > interval) {
            // Update position and angle based on ticks in given window
            const leftWheelDistance = (leftWheelTicks / encoderTicksPerRevolution) * encoderCircumference;
            const rightWheelDistance = (rightWheelTicks / encoderTicksPerRevolution) * encoderCircumference;

            const distance = (leftWheelDistance + rightWheelDistance) / 2;

            position.x += distance * Math.cos(theta);
            position.y += distance * Math.sin(theta);

            theta += (rightWheelDistance - leftWheelDistance) / encoderDiameter;

            // Reset window
            windowStartTime = time;
            ticksInWindow = 0;
            leftWheelTicks = 0;
            rightWheelTicks = 0;
        }
    } else {
        const [command] = remaining;

        console.log('sent', timestamp, command);

        if (lastCommand !== null) {
            break;
        }

        lastCommand = command;
    }
}
