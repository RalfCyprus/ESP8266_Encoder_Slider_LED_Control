ESP8266 LED Dimming Project with Rotary Encoder and Smartphone Control
This project allows you to control 12V LED ceiling spotlights using both a rotary encoder and a smartphone. It’s based on an ESP8266-12 and designed for an off-grid container on my farm, powered by a 12V battery.
Features
Dual control: Dimming via rotary encoder or smartphone slider
Heartbeat LED: Blue LED blinks when connected to the router
Push-button toggle: Switch LEDs on/off with memory of the last brightness
Light-dependent feedback: LED brightness of indicators adapts to ambient light
No internet required: Works with a NAT router as a local access point
Failsafe: Encoder control works even without router connection
Hardware Used
ESP8266-12
12V LED ceiling spotlights (max. 9W each)
Rotary encoder with push-button
Blue and yellow indicator LEDs
Light sensor (analog input)
N-channel FET (no heatsink needed for 1–2 LEDs)
5V regulator for ESP8266 (no heatsink needed)
NAT router (for local Wi-Fi communication)
12V battery (power supply)
How It Works
1. Startup:
When the ESP is powered on, it attempts to connect to the router.
If successful: the blue LED begins blinking in a heartbeat pattern.
If unsuccessful: the ESP still works with the rotary encoder, but smartphone control is unavailable.
2. LED Control:
Rotary encoder or smartphone slider increases/decreases brightness.
Yellow LED lights up when brightness is increased.
Push-button:
First press dims LEDs to 0
Second press restores the previous brightness level
3. Light Sensor Feedback:
In bright ambient light: blue and yellow LEDs glow at full intensity
In darkness: indicator LEDs dim automatically
4. Connection Loss:
If the router goes offline during operation:
LED brightness and encoder control continue to work
Blue LED stops blinking
When the router becomes available again:
ESP reconnects automatically
Blue LED resumes heartbeat blinking
5. Power Loss:
On reboot, the ESP does not retain the last brightness level
LEDs remain off until reactivated manually
Wiring Overview
Blue and yellow LED connections
FET wiring
Light sensor to analog input
Push-button for toggle control
5V regulator between 12V battery and ESP8266
See images in the docs/ folder or project description for wiring diagrams.
Setup Instructions
1. Enter your Wi-Fi credentials in the sketch
2. Set a static IP address for the ESP
You can find your router’s IP by connecting your smartphone to it and checking the network details


