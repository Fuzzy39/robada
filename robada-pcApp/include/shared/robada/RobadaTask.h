#pragma once

// this is a big singleton, you initialize it, it spawns a thread.
// It will be a command queue-type thing. It runs a command if available,
// reads from robada's console, if that existed, and polls the gamepad each iteration.

// Need to figure out a good way to do async. That'll be the other headers in this, the actual commands.
// Probably will have a state machine of sorts for robada control modes. That could be represented by classes, actually.
