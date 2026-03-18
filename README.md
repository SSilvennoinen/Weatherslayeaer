Weatherslayeaer – IoT Weather Display
Weatherslayeaer is a playful IoT weather board built with an ESP32, an LED matrix, animated NeoPixel lights, and a 3D‑printed frame. The device fetches real‑time weather data from the OpenWeather API and displays the current time, temperature, weather condition, and a humorous weather‑based quote. The LED strip around the frame changes color depending on the temperature, giving the device a lively and atmospheric look.

Features
Real‑time weather data fetched from OpenWeather

LED matrix display showing:

Time

Temperature

Weather description

A scrolling humorous quote

Animated NeoPixel LED strip with color gradients based on temperature

3D‑printed frame for wall mounting

Custom 3D‑printed nameplate (“Weatherslayeaer”)

WiFi‑connected ESP32 for continuous updates

How It Works
Weather Fetching
The ESP32 connects to WiFi and retrieves weather data in JSON format.
We parse:

main.temp

weather[0].description

Funny Weather Quotes
The function getReminder() uses a series of if statements to detect keywords like rain, snow, fog, clear sky, etc.
Depending on the weather, the device might say things like:

“You WILL get wet.”

“Winter is coming!”

“Hold onto your snacks.”

“Don’t lick metal.”

“Blue sky unlocked.”

These messages make the device feel more personal and fun.

LED Gradient Animation
The NeoPixel strip uses a sine‑wave animation to smoothly blend between two colors.
Temperature ranges have their own color themes:

Below 0°C → icy blues

Below 10°C → grey–yellow

Below 20°C → pink–green

Below 25°C → warm orange tones

Above 25°C → hot reds
Components Used
ESP32 Dev Module

8×32 LED matrix display

39‑LED NeoPixel strip

3D‑printed frame and nameplate

OpenWeather API

LiquidCrystal_I2C library

ArduinoJson

ezTime

Adafruit NeoPixel

Code
The full source code is available in this repository: https://github.com/SSilvennoinen/Weatherslayeaer
