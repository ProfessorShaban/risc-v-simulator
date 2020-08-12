# risc-v-simulator
RISC-V assembler/simulator with GUI

===

Building on Mac, Windows or Linux:

- Download Qt
- Use Qt Creator


===

To build from command line on Linux (Debian):

Install Qt:
- sudo apt-get install build-essential libgl1-mesa-dev
- sudo apt-get install qtcreator
- sudo apt-get install qt5-default

Build RISC-V simulator:
- Get source and unzip
- cd ui
- qmake RISCV.pro
- make

Then to run: ./RISCV

Known bug: when building from command line on Linux, run/step/animate toolbar buttons are blank
