# risc-v-simulator
RISC-V assembler/simulator with GUI

===

Building on Mac, Windows or Linux:

- Download Qt
- Use Qt Creator


===

To build from command line on Linux (Debian):

Install Qt:
- sudo apt-get update
- sudo apt-get install build-essential libgl1-mesa-dev
- sudo apt-get install qtcreator
- sudo apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools


Build RISC-V simulator:
- Get source and unzip
- cd ui
- qmake RISCV.pro
- make

Then to run: ./RISCV
