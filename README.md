# Logger

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Visual Studio Code](https://img.shields.io/badge/Visual%20Studio%20Code-0078d7.svg?style=for-the-badge&logo=visual-studio-code&logoColor=white)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)

![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54)
![Matplotlib](https://img.shields.io/badge/Matplotlib-%23ffffff.svg?style=for-the-badge&logo=Matplotlib&logoColor=black)
![NumPy](https://img.shields.io/badge/numpy-%23013243.svg?style=for-the-badge&logo=numpy&logoColor=white)
![SciPy](https://img.shields.io/badge/SciPy-%230C55A5.svg?style=for-the-badge&logo=scipy&logoColor=%white)


## 📝 Table of Contents
- [About](#🧐-about)
- [Getting Started](#🏁-getting-started)
- [Hardware](#🔧-hardware-required)
- [Description](#description)
- [Theoretical Notions](#theoretical-notions)
- [Code](#⌨️-code)
- [Display Mode](#🖥️-display-mode)
- [SD Mode](#💾-sd-mode)
- [Serial Mode](#💻-serial-mode)
- [Built using](#⛏️-built-using)
- [Contributing](#🫂-contributing)
- [Authors](#✍️-authors)
- [Repo Activity](#repo-activity)
- [License](#license)
- [Acknowledgments](#🎉-acknowledgments)

## 🧐 About
This project is a data acquisition system built using an ESP32 microcontroller board. The system incorporates an ADS1115 analog-to-digital converter, an SSD1306 display, and three buttons for user interaction. It is designed to store data on an SD card or send it to a pc for further analysis.

### Features

- Acquisition of analog sensor data using ADS1115 ADC
- Real-time display of sensor readings on SSD1306 OLED display
- User interaction through three buttons for control and configuration
- Storage of acquired data on an SD card for offline access
- Transmission of data to a computer for centralized analysis

## 🔧 Hardware Required

## 🏁 Getting Started



### Prerequisites

- Arduino IDE or Visual Studio Code (with PlatformIO)
- ESP32 board manager for Arduino IDE
- Libraries:
  - Adafruit ADS1X15 (for ADS1115)
  - Adafruit SSD1306 (for SSD1306 display)
  - SD (for SD card storage)
  - <other libraries as required>


### Installation

<details open>
<summary><i>Installation using Arduino IDE</i></summary>

- Clone or download the project repository from GitHub.
- Open the Arduino IDE and install the ESP32 board manager if not already installed.
- Connect the hardware components as per the provided circuit diagram.
- Install the required libraries using the Arduino Library Manager.
- Upload the code to the ESP32 board using the Arduino IDE.
- Monitor the serial output to ensure successful connection to the sensors.
- Verify the functionality by checking the sensor readings on the SSD1306 display and the data storage on the SD card.
</details>

<details open>
<summary><i>Installation using Visual Studio Code</i></summary>

- Clone or download the project repository from GitHub.
- Open VS and compile the software (changing the details on platform.ini according to your hardware).
- Connect the hardware components as per the provided circuit diagram.
- Upload the code to the ESP32 board using the upload tool.
- Monitor the serial output to ensure successful connection to the sensors.
- Verify the functionality by checking the sensor readings on the SSD1306 display and the data storage on the SD card.

</details>

## Description

## ⌨️ Code

All the code has been written using the arduino Framework, for an ESP32. This gave us significant advantages in terms of ease of writing, online documentation, and especially in terms of clock, which is considerably greater than an arduino Uno. We followed a kind of MVC model, where each component is separate from the others and deals only with the things it is responsible for, invoking higher-level primitives. This allowed us to manage different components more smoothly.

### Main

The main substantially is a method wrapper and deals, at a higher level, with handling scrolling through the various menus. Initially (in setup) it asks the controller to initialize devices (the basic ones, such as buttons, screen, ...)

```cpp
while (!initializeDevices()){
}

updateMenu();
```

After this handles menu selection

```cpp
if (stateMenu)
{
  if (goDown())
  {
    menu++;
  }
  if (goUp())
  {
    menu--;
  }
  if (select())
  {
    executeAction(); //Enters in the selected menu
  }
}
```

### Controller

The controller is the component that manages all component behavior and is directly responsible for making measurements. Besides that it initializes all the devices and the serial. The initializations that are done can be blocking or non-blocking, depending on the type of component we want to initialize: a failure of the fundamental components results in a program block inside a loop, which will not allow the execution of the remaining code, while the other components may not be initialized at all. The reference with respect to the success or failure of initialization we have it by returning a boolean value true when the method is terminated, so it does not take into account subsequent failures. The whole initial part is devoted to declaring all pins connected to the board, and in case of custom configurations they can be changed before compilation. 

After all parameters have been selected, data acquisition will proceed. This involves an initial setup phase, in which, based on selected parameters (saved as global variables in the controller) it will go to set the mode (and consequently the channels to read from), the data rate, and calculate gain and offset. Depending on the output selected it could change significantly, but we will also have an output on the serial that tells us what is happening. Finally it's initialized the measurement object, which will handle the calculations made for mean and std.

```cpp
void adcSetup()
{
  setRate(currentSampleRate); //Physically setting the sample rate with ADS function
  setChannel(); //Setting the channel, gain and other useful parameters

  K_value = calculateCoefficient();
  O_value = calculateOffset();

  Measurement measurement(currentSampleRate); //Instantiate the object
}
```

After running the setup you will access to the loggerAct, which is the method that takes care of the measurement. Here, depending on the mode that will be selected it will have different behavior, but basically what it does is:

```cpp
// The microcontroller is waiting for an interrupt from the ADC
if (!new_data)
  {
    return;
  }

// When the adc is alerted via the alert pin the code continues

if (!measurement.isArrayFull())
  {
    measurement.insertMeasurement(adcValue); // Get the misuration and stores in the array
  }

else
  {
    measurement.setArrayFull(false); // Empties the array and compute mean and std
    loggerGraphic(mode, channel, timestamp, mean); // Print on display the parameters
  }

new_data = false;
```

### View

The view component deals with screen management. Here it is initialized and passing it the necessary parameters updates the screen. Bipmap images of the various modes and associated sliders are also saved here. The view also provides functions to be used directly by the controller, such as the one used during the loggerAct, which takes care of printing media (for each second), timestamp, mode, and output.

### Model

The model is the component that manages the "measurements" class and takes care of the array. This provides various functions to calculate different values useful for measurements, such as mean and standard deviation. In fact, when we go to initialize the object an array is created, of length equal to the data rate, and we go to calculate mean and std for each second. Each time the adc returns us the value, from the controller, we go to invoke a function that is used to insert the measured value (already possibly multiplied by the gain and summed with the offset) at the tail of the array. 

```cpp
class Measurement
{
private:
    int *measurements;
    float mean;
    float std; 
    bool arrayFull;

public:
    Measurement(int len);
    ~Measurement();
    void insertMeasurement(int value);
    void calculateMean();
    void calculateStd();
    void reset();
    bool isArrayFull();
    int getLength();
};
```

## Theoretical Notions
<details open>
<summary><i>Adc</i></summary>
In this project we use the ADS1115 that is a very small, low-power, 16-bit, delta-sigma (ΔΣ) analog-to-digital converters (ADCs).
There are many ADCs available on the market for various applications: Ramping, SAR, delta-sigma, Flash and so on. 
ΔΣ ADCs are now ideal for converting analog signals over a wide range of frequencies, from DC to several megahertz. Basically, these converters consist of an oversampling modulator followed by a digital filter that together produce a high-resolution data-stream output. The ADS1115 has an adjustable gain(PGA), an interal voltage reference, a clock oscillator, an I2C interface, and also a programmable digital comparator that provides an alert on a dedicated pin. The latter played an important role in carrying out the sampling at the maximum achievable speed of 860SPS. Thanks to the mux the adc has four single-ended(to measure
each channel with respect to ground) or two differential inputs and two available conversion mode: single-shot mode(power saving mode) and continuous mode. In continuous conversion mode, the adc automatically begins a conversion of the input as soon as the previous conversion is completed and the rate is programmed by software.
To achieve high accuracy measurements it's a mandatory following several step:

- Use the Maximum Resolution in the readings
- Adjust the PGA to match the voltage range of your input signal(all specifications can be found in the datasheet)
- Use an external voltage reference instead of the integrated
- Implement appropriate filtering and signal conditioning techniques to minimize noise and Interference
- Periodically calibrate the system to compensate any drift
- Avoid common source of error such as impedance mismatches
- Select data rate based on dynamic meausurements or slow signals
- Minimize signal path lengths
</details>

<details open>
<summary><i>Input Configuration</i></summary>
The ADS1115 allow us to choose between two different input configuration: <b>Differential Terminal Configuration</b>(DIFF) and <b>Referenced Single-Ended Terminal Configuration</b>(Ground Referenced RSE).

DIFF measures the potential difference between the AIN_p and AIN_n, this is in general the recommended configuration because of errors caused by ground loops and noise injection from ground nodes.
The RSE measures the potential difference between the AI and the GND of the Adc connected by a switch within the multiplexer.

It's important to know what type of source(differential, flaoting single-ended or grounded single-ended ) is in order to make the connection.
To sum up:

- A grounded source can be connected only to differential channel to avoid ground loops.
- A floating source can be connnected to both the configuration

</details>

### Interrupt
An interrupt is a signal to the processor, emitted by hardware or software, indicating an event that needs immediate attention. It’s a way of controlling the flow of the processor’s instructions. When an interrupt signal is received, the processor halts its current operations and switches to execute the interrupt service routine (ISR), which is specifically designed to deal with the occurrence of the same interrupt.

The `IRAM_ATTR` macro is used to place functions in the instruction RAM (IRAM) instead of the flash memory. This is done for functions that need to be executed quickly, such as interrupt service routines (ISRs). If IRAM_ATTR is not defined, it’s defined as an empty macro, which means it won’t have any effect on the code.

The `new_data variable` is declared as volatile, which means it can be modified unexpectedly by something outside the current code block, such as an ISR. This keyword prevents the compiler from optimizing the code related to this variable, ensuring that the program always reads the actual value from the memory location.

The function `NewDataReadyISR()` is an ISR that’s called when new data is ready to be processed. In this function, the new_data flag is set to true. This flag will be checked in the `loggerAct()` to know when new data is available. Once the data is processed, the flag should be reset to false until the next interrupt occurs.

After, in the `adcSetup()` is declared the `attachInterrupt()` function,  used to specify a function to be called when an interrupt occurs. In this case, the function NewDataReadyISR() will be called when a falling edge is detected on the `ALERT_PIN`.

Here’s a breakdown of the parameters:

- `digitalPinToInterrupt(ALERT_PIN)` is the interrupt number. `digitalPinToInterrupt()` is a function that returns the interrupt number for a specific pin. Here, `ALERT_PIN` is the pin number where the interrupt signal is expected.
- `NewDataReadyISR` is the function to be called when the interrupt occurs. This function must take no parameters and return no value.
- `FALLING` is the mode of the interrupt. In this case, it’s FALLING, which means the interrupt will be triggered when the pin goes from HIGH to LOW.

```cpp
if (!new_data)
  {
    // No data ready
  return;
  }

// New data ready
// Follows the routing according to output mode

[...]


new_data = false;
```
### ESP32

The ESP32 is a powerful, low-cost microcontroller with integrated Wi-Fi and dual-mode Bluetooth, making it an ideal choice for a wide range of applications. It employs a Tensilica Xtensa LX6 microprocessor, which can operate at 160 or 240 MHz. This makes it significantly more powerful than many other microcontrollers on the market (for example an Arduino Uno, operating at 16MHz). It also has larger RAM and flash memory, allowing for more extensive programs.

One of the key reasons for choosing the ESP32 for this project is its cost-effectiveness. Despite its high performance and extensive features, the ESP32 is relatively cheap, making it a great choice for projects that need this power.

Despite these advantages, the ESP32 can be more complex to program than some other microcontrollers (using the ESP-IDF frameworj). That’s where the Arduino framework comes in. The Arduino framework provides a simple and consistent programming model, and it’s supported by a large, active community making it easier to find something related to this device on the scientific literature. Even though the ESP32 and Arduino are different hardware platforms, the ESP32 can be programmed using the Arduino framework, which means we can implement the existing Arduino libraries and development tools.

### I2C

The I²C (Inter-Integrated Circuit) protocol, also known as I2C or IIC, is a synchronous, multi-master/multi-slave, single-ended, serial communication bus invented in 1982 by Philips Semiconductors. It’s widely used for attaching lower-speed peripheral ICs to processors and microcontrollers in short-distance, intra-board communication1.

I²C uses only two bidirectional lines: the Serial Data Line (SDA) and the Serial Clock Line (SCL). This simplicity makes it a popular choice for connecting a wide range of devices such as sensors, EEPROMs, and display drivers. One of the strengths of I²C is the capability of a microcontroller to control a network of device chips with just two general-purpose I/O pins and software1. This makes I²C a very efficient protocol for communicating between components on a single board.

## 🖥️ Display Mode

### Usage

### Performance Evaluation


## 💾 SD Mode

### Usage

### Performance Evaluation


## 💻 Serial Mode

### Usage

### Explanation

In the data acquisition stage, data is sent to the pc, which is processed through python. The manufacturer of the module, in the datasheet indicates that the conversion time is exactly equivalent to 1/DR (worst case: 860SPS), so there will certainly be a small delay due to data processing due to the microcontroller. This, in turn, will have to be added to a time, albeit very small, due to communication via serial. We had initially opted to send the data via the `Serial.println()` function, but this route, following some testing, seemed impractical. This was because, while having a readable result directly from the serial would have made it easy and intuitive to retrieve the data, it introduced a considerable delay, far in excess of the 1/DR value, even orders of magnitude (about tenths of a second). We therefore opted for the `Serial.write()` function, but again there were problems, since the measured values are 16-bit integers, so they needed to be split in two and sent (so as to send 8 bits at a time). 

```cpp
Serial.write(0xCC);                   // Start byte
Serial.write((ADCvalue >> 8) & 0xFF); // High byte
Serial.write(ADCvalue & 0xFF);       // Low byte
```
- `Serial.write(0xCC)` sends a start byte. The Serial.write() function writes binary data to the serial port. 0xCC is the hexadecimal representation of the start byte. The microcontroller need to send this to communicate that a new set of data is starting.

- `Serial.write((ADCvalue >> 8) & 0xFF)` sends the high byte of ADCvalue. The >> 8 operation shifts the bits of ADCvalue 8 places to the right, effectively moving the high byte to the position of the low byte. The & 0xFF operation then masks the lower byte, so only the original high byte of ADCvalue remains.

- `Serial.write(ADCvalue & 0xFF)` sends the low byte of ADCvalue. The & 0xFF operation masks the high byte of ADCvalue, so only the low byte remains.

After the microcontroller sends the measured value this way, python will have to deal with reconstructing it and converting it to a readable value, like this:

```python
start_byte = ser.read(1)  # Read the start byte
    if start_byte == b'\xCC':  # Verify the start byte
        high_byte = ser.read(1)  # Read the high byte
        low_byte = ser.read(1)  # Read the low byte
        measurement = (ord(high_byte) << 8) | ord(low_byte)  # Merge the bytes
```

- `start_byte = ser.read(1)` reads one byte from the serial port and assigns it to the variable start_byte.

- `if start_byte == b'\xCC'` checks if the start byte is equal to b'\xCC', which is the byte representation of the hexadecimal number 0xCC. If the start byte is 0xCC, the code inside the if statement will be executed. This is used to verify that the data transmission is starting correctly.

- `high_byte = ser.read(1)` reads the next byte (after the start byte) from the serial port, which is the high byte of the measurement.

- `low_byte = ser.read(1)` reads the next byte from the serial port, which is the low byte of the measurement.

- `measurement = (ord(high_byte) << 8) | ord(low_byte)` merges the high byte and the low byte to reconstruct the original measurement. The `ord()` function is used to get the integer value of a byte. The `<< 8` operation shifts the bits of the high byte 8 places to the left, effectively moving it to the position of the high byte in a 16-bit number. The `|` operation is a bitwise OR, which combines the high byte and the low byte into a single 16-bit number.

For more in-depth information you can visit the python code repository directly: [Serial Mode Logger](https://github.com/mrheltic/Serial-Mode-Logger)

### Performance Evaluation

## ⛏️ Built Using
- [PlatformIO](https://platformio.org) - PlatformIO
- [Arduino Framework](https://www.arduino.cc) - Arduino Framework

## 🫂 Contributing

Contributions to this project are welcome. If you encounter any issues or have ideas for improvements, please submit a pull request or open an issue on the project repository.

## ✍️ Authors
- [@mrheltic](https://github.com/mrheltic) - Florio Vincenzo Pio
- [@Phersax](https://github.com/Phersax) - Stasi Francesco
- [@dav01luffy](https://github.com/dav01luffy) - Tonti Davide

## Repo Activity

![Alt](https://repobeats.axiom.co/api/embed/d41f5a09ed4fdf91b97787200df89b38aa1a0af6.svg "Repobeats analytics image")

## License
Details about the license.

## 🎉 Acknowledgments
Any acknowledgments (optional).