<p align="center">
    <img src="https://svgshare.com/i/J9L.svg" alt="Logo" width="90000" border="0">
<br />

## :information_source: About The Project

JAMES is part of a bigger project: an airborne experimental test platform I'm creating called Sparrow One. JAMES is the most low-level electronic contoller aboard. It's main purpose is to keep the craft stable in flight, and to control the power hardware. It houses the IMU and barometer among other sensors, collects measurements and controls the ESC's. JAMES takes in the desired pitch, roll, and yaw angles, the desired altitude and velocity and manipulates the engines accordingly. Control of JAMES' fligh parameters can happen either manually or automatically (by another computer aboard, called ZUES).

## :space_invader: Technitcal

### :hammer: Electronics

JAMES is ATmega328p based control board I designed and built. This repo is dedicated to the development of this boards firmware.

### :bug: Known issues and future developements
- [ ] YAW control and stabilization
- [ ] Altitude control and stabilization
- [ ] Autonomous emergency landing
- [ ] ZEUS autonomous mission capabilities (JAMES is mostly ready for this at this point, although completely autonomous flight has not been tested as ZEUS still has a long way to go.)
- [ ] Safety landing in case of an engine falure

### :construction_worker: Build

The project `(./firmware)` is Arduino based and is entirely written in C++. It can be built and compiled for use with the ATmega328p using the Arduino IDE.

### :blue_heart: Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

### :page_facing_up: License
The JAMES flight computer firmware is available under the [MIT](https://choosealicense.com/licenses/mit/) license.

### :coffee: Coffee!
[<img src="https://user-images.githubusercontent.com/1685680/61808727-4925de00-ae3c-11e9-9d60-66bef358fd8e.png" alt="drawing" width="180"/>](https://www.buymeacoffee.com/evertds "Buy me a coffee")