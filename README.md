
# DoorsOS

## Overview
This project is part of the EEET2490 course at RMIT University, focusing on bare metal development of an operating system to enhance practical skills in embedded OS through the implementation of various features like a command-line interpreter (CLI), terminal formatting using ANSI codes, and handling variable arguments with a standard printf function.

## Features
- **Welcome Message:** A simple welcome message during boot up using [ASCII Art Generator](https://onlineasciitools.com/convert-text-to-ascii-art), you can replace it with your own design by editting the Command Line Interpreter (CLI).
- **Command Line Interpreter (CLI):** A simple CLI that supports:
  - Auto-completion using the TAB key.
  - Command history navigable with `_` and `+` keys.
  - Commands such as `help`, `clear`, and `setcolor` for basic interactions.
  - Support UART related settings such as `setbaud`, `setdatabits`, `setstopbits`, `setparity`, and `setflowcontrol`.
- **ANSI Terminal Formatting:** Utilize ANSI escape sequences to set text and background colors. Helpful references:
  - [ANSI Escape Codes](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)
  - [Terminal Colors](https://chrisyeh96.github.io/2020/03/28/terminal-colors.html)

## Hardware Support
- The software is designed to run on a Raspberry Pi 3/4, and functionality has been tested with QEMU emulation and actual hardware. Board information can be verified using instructions from [Raspberry Pi Board Version](https://www.raspberrypi-spy.co.uk/2012/09/checking-your-raspberry-pi-board-version/).

## Getting Started
1. **Clone the repository:**
   ```bash
   git clone https://github.com/Railroad-Wrecker/EEET2490-Embedded-System-DoorsOS.git
   ```
2. **Navigate to the project directory:**
   ```bash
   cd ./EEET2490-Embedded-System-Assignment-2/ 
   ```
3. **Build the project:**
   ```bash
   make all
   ```
4. **Run the OS on your Raspberry Pi or through QEMU emulation.**
   ```bash
   For Pi4, change GPIO setting to RPI4, QEMU Sumulation only support Pi3
   ```

## UART Configuration
Enhanced UART driver to support various configurations:
- Baud rates: 9600, 19200, 38400, 57600, 115200 bps.
- Data bits: 5, 6, 7, 8.
- Stop bits: 1 or 2.
- Parity: None, Even, Odd.
- Handshaking: CTS/RTS.

## Contributors
- Luong Nguyen

## License
This project is licensed under the MIT License - see the LICENSE file for details
