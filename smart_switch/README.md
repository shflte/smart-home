## Overview
- A pump switch that can be controlled both physically using a button and through Apple Home.
- The LED indicates whether the power is on or off.

## Components
- **Button**
	- Physically toggles the pump state.
	- A button module typically comes with `IN`, `VCC` (Voltage Common Collector), and `GND` pins and often has built-in debounce functionality.
	- `IN` connects to a GPIO pin used to monitor whether the button is pressed.
	- `VCC` and `GND` connect to 3.3V/5V and GND, respectively.
- **EEPROM (Electrically-Erasable Programmable ROM)**
	- Built into the NodeMCU, no additional wiring required.
- **LED**
	- Indicates the pump state.
- **Relay**
	- Controls whether the pump is powered on or off.
	- Ensure the relay can handle the voltage and current of the pump motor.
- **NodeMCU**
- **Breadboard**
- **Pump's Original Switch**
	- Do not remove the pump’s original switch; leave it in an open circuit state. Instead, connect wires from both terminals of the switch for external control. This ensures the original switch remains functional in case the custom setup fails.

## Control Logic
```pseudo
startup:
	init Wi-Fi & MQTT
	read pump state from EEPROM

when button_pressed or message_arrived:
	status := ~status  // toggle the pump status
	update EEPROM pump status
	publish new pump status

	control relay, LED according to the status
```
