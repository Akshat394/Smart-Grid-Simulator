# Smart Grid Monitoring System.
The Smart-Grid-Monitoring-System repository is a comprehensive and innovative solution for the development and implementation of state-of-the-art smart grid monitoring systems. 
## 📜 Slave Code Overview:

### 🌐 Port and Pin Definitions:

**output:** Port P1 is defined for receiving data from ADC808.  
Channel selecting pins (add_a, add_b, add_c) determine which channel to read from.  
al enables the selected channel.  
rd and wr control reading and starting conversion.  
INTR monitors the conversion process.  
Pins (winding1, winding2, winding3, winding4) control a stepper motor.  
motor controls another motor.  

### Function Prototypes:

This code defines several functions, which are the magical spells that control the microcontroller's behavior.
🕰️ **TimerDelay() Function:**

This function casts a time-delay spell, allowing precise timing control.

⚡ **Interrupt Service Routine (rec):**

This wizardry responds to UART communication from a master device. Commands like 'V', 'P', and 'T' trigger sensor readings and data transmission.

🔌 **ADC Functions:**

adc1(), adc2(), and adc3() handle analog-to-digital conversions for different sensors and return their readings.

📡 **Data Transmission Functions:**

condition1(), condition2(), condition3(), condition2a(), and condition3a() send data back to the master device based on sensor readings.

🌡️ **Temperature and Pressure Formulas:**

Formulas for temperature and pressure conversion are applied to sensor data before transmission.

🔁 **Main Loop:**

In the main function, the code initializes necessary components and starts the timer.

🪄 **Stepper Motor Control:**

The code controls a stepper motor based on pressure conditions, rotating it to specific angles.

⚡ **Endless Magic:**

The code runs indefinitely, continuously reading sensor data and responding to master device commands, ensuring your microcontroller's capabilities are at your command.

## 📜 Master Code Overview:

📌 **Microcontroller and Ports:**
The code starts with the definition of ports for various components such as an LCD, registers, keypad, and street lights. These ports serve as gateways for data and control. 🔌

👉 **Function Prototypes:**
The code introduces function prototypes for fundamental operations like delay, LCD commands, LCD display, string manipulation, initialization, and timer delay. Functions are the building blocks of this magic! 🛠️

⏱️ **TimerDelay() Function:** 
This function conjures precise delays through timer control. In the world of embedded systems, timing is everything, and this function ensures accuracy! 🕰️

⏳ **Delay() Function:**
Delays are like spells for timing and synchronization in microcontroller projects. This function grants you control over time.

👉 **Command and Display Functions:**
These functions communicate with the LCD display, issuing commands and conjuring characters on the screen. The LCD becomes your visual storyteller! 🖥️

💡 **Initialization (init) Function:**
The code commences by initializing the LCD, welcoming the world with "Starting...". Magic begins here! ✨

🎛️ **Sensor Functions:**
These functions handle data from various sensors. Voltage, pressure, and temperature are revealed on the LCD, granting you insights into your environment. 🌡️

🔢 **Keypad Function:**
The keypad function interprets keypresses and conjures signals. It's akin to a magic wand that allows you to cast different spells by pressing keys! 🪄

📡 **UART Communication:**
The 8051 microcontroller communicates through UART, sending and receiving data. It's the microcontroller's language! 📬

💡 **Street Lights Control:**
You can control street lights with the press of a button, enhancing safety and efficiency. Let there be light! 💡

👾 **Main Function:**
The main function orchestrates it all. It's the master magician conducting the embedded symphony. 🎵
