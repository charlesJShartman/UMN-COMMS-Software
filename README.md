# UDP Radio Service
## Initial Setup
Uses Common-Library. Clone this repo with
```sh
git clone --recursive https://github.umn.edu/UMN-CubeSat-Team/COMMS-Software_Redesign.git
```
in order to clone both repos.
Requires CMake and gpiod. If they are not installed, run
```sh
sudo apt install cmake gpiod libgpiod-dev -y
```

## Building
This repo comes with a Makefile to speed up building. To build the project, run
```sh
make
```
The programs will compile into the newly created `bin` directory

## Usage
- `bin/radio_service` is the entry point into the service
- `bin/spam_radio` is a packet generator that will send directly to the radio
