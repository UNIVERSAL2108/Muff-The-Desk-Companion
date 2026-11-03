 DeskMate 🤖

Your little desk companion that reacts to your presence.

DeskMate is an ESP32-powered interactive desk companion that uses an OLED display and an ultrasonic distance sensor to detect nearby people and respond with expressive facial animations.

 ✨ Features

- Smart Proximity Detection:** Uses the HC-SR04 ultrasonic sensor to detect nearby people.
- Interactive OLED Display:** Displays an expressive face when someone approaches.
- Automatic Response:** Changes its facial expression based on detected proximity.
- ESP32 Powered:** Built using the ESP32 DevKit V1.

 🛠️ Hardware Components

- ESP32 DevKit V1
- 0.96-inch I2C OLED Display (128×64)
- HC-SR04 Ultrasonic Distance Sensor
- 1.4 kΩ and 2.2 kΩ resistors (voltage divider)
- Breadboard and jumper wires

 💻 Technologies & Libraries

- Arduino IDE
- C/C++ (Arduino Framework)
- Adafruit GFX Library
- Adafruit SSD1306 Library

 ⚙️ How It Works

1. The HC-SR04 ultrasonic sensor continuously measures the distance of nearby objects.
2. When someone approaches within approximately 30 cm, DeskMate detects their presence.
3. The OLED display responds by showing a happy facial expression.

# 🚀 Project Goal

DeskMate is a hands-on exploration of embedded systems, sensor integration, microcontroller programming, and OLED-based interfaces.

Built with curiosity, code, and a little personality.

 🔮 Future Improvements

- Digital clock with Wi-Fi time synchronization.
- More facial expressions and animations.
- Additional interactive features.
