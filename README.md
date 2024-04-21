
# EEET2490 Embedded System: OS and Interfacing Assignment

## Overview
This project is part of the EEET2490 course at RMIT University, focusing on bare metal development of an operating system to enhance practical skills in embedded OS through the implementation of various features like a command-line interpreter (CLI), terminal formatting using ANSI codes, and handling variable arguments with a standard printf function.

## Features
- **Welcome Message:** Displays a customizable welcome message in ASCII art on boot up. Use tools like [ASCII Art Generator](https://onlineasciitools.com/convert-text-to-ascii-art) to create your own designs.
- **Command Line Interpreter (CLI):** A simple CLI that supports:
  - Auto-completion using the TAB key.
  - Command history navigable with `_` and `+` keys.
  - Commands such as `help`, `clear`, and `setcolor` for basic interactions.
- **ANSI Terminal Formatting:** Utilize ANSI escape sequences to set text and background colors. Helpful references:
  - [ANSI Escape Codes](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)
  - [Terminal Colors](https://chrisyeh96.github.io/2020/03/28/terminal-colors.html)

## Hardware Support
- The software is designed to run on a Raspberry Pi, and functionality has been tested with QEMU emulation and actual hardware. Board information can be verified using instructions from [Raspberry Pi Board Version](https://www.raspberrypi-spy.co.uk/2012/09/checking-your-raspberry-pi-board-version/).

## Getting Started
1. **Clone the repository:**
   ```bash
   git clone
   ```
2. **Navigate to the project directory:**
   ```bash
   cd
   ```
3. **Build the project:**
   ```bash
   make all
   ```
4. **Run the OS on your Raspberry Pi or through QEMU emulation.**

## UART Configuration
Enhanced UART driver to support various configurations:
- Baud rates: 9600, 19200, 38400, 57600, 115200 bps.
- Data bits: 5, 6, 7, 8.
- Stop bits: 1 or 2.
- Parity: None, Even, Odd.
- Handshaking: CTS/RTS.

## Reflection and Learning
This project also includes a reflection on the practical experiences and learning outcomes related to bare metal OS development, linking these experiences with the skills demanded by job roles in embedded software engineering.

## Additional Resources
- **TinkerCad Circuits:** For experimenting with microcontrollers and sensors.
  [Explore TinkerCad Circuits](https://www.tinkercad.com/learn/circuits)

## Contributors
- Luong Nguyen

## License
This project is licensed under the MIT License.
