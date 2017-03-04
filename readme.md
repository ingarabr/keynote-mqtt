# Keynote MQTT

## Server
python script to bridge MQTT commands to keynote

> ./server/keynote.py host port baseTopic

Listen to sub topic keynote with the follwoing message content

- next => next slide
- prev => previous slide

## Client

Dependencies:
- Adafruit GFX Library
- Adafruid SSD1306

Hardware setup:
- D1 -> display SCL
- D2 -> display SDA
- D6 -> button next slide
- D7 -> button previous slide



