# ESP8266 Logger

This project is a data acquisition system built using an ESP8266 microcontroller board. The system incorporates an ADS1115 analog-to-digital converter, an SSD1306 display, and three buttons for user interaction. It is designed to store data both on an SD card and send it to a server for further analysis.

## Features

- Acquisition of analog sensor data using ADS1115 ADC
- Real-time display of sensor readings on SSD1306 OLED display
- User interaction through three buttons for control and configuration
- Storage of acquired data on an SD card for offline access
- Transmission of data to a remote server for centralized analysis

## Hardware Components

- ESP8266 microcontroller board
- ADS1115 analog-to-digital converter
- SSD1306 OLED display
- Three buttons for user input
- SD card module
- Server for data storage and analysis

## Software Requirements

- Arduino IDE
- ESP8266 board manager for Arduino IDE
- Libraries:
  - Adafruit ADS1X15 (for ADS1115)
  - Adafruit SSD1306 (for SSD1306 display)
  - ESP8266WiFi (for WiFi connectivity)
  - SD (for SD card storage)
  - <other libraries as required>

## Installation and Setup

1. Clone or download the project repository from GitHub.
2. Open the Arduino IDE and install the ESP8266 board manager if not already installed.
3. Connect the hardware components as per the provided circuit diagram.
4. Install the required libraries using the Arduino Library Manager.
5. Configure the WiFi credentials and server details in the code.
6. Upload the code to the ESP8266 board using the Arduino IDE.
7. Monitor the serial output to ensure successful connection to the WiFi network and server.
8. Verify the functionality by checking the sensor readings on the SSD1306 display and the data storage on the SD card and server.

## Usage

1. Power up the system and ensure that it connects to the configured WiFi network.
2. The SSD1306 display will show real-time sensor readings.
3. Use the buttons for system control or configuration changes as per the provided functionality.
4. Acquired data will be stored on the SD card in a specified format.
5. Periodically, the system will send the collected data to the server for analysis and further processing.

## Contributing

Contributions to this project are welcome. If you encounter any issues or have ideas for improvements, please submit a pull request or open an issue on the project repository.

## License

This project is licensed under the [MIT License](LICENSE). Feel free to use and modify the code as per your needs.

## Acknowledgments

- [Adafruit Industries](https://www.adafruit.com/) for their open-source libraries and hardware components.
- The open-source community for their valuable contributions and support.

## Contact

For any inquiries or questions regarding this project, please contact [fattualtech@gmail.com].