# Knoblomat

The "Knoblomat" allows to play the simple game of "Rock - Paper - Scissors" agains a machine.
Nine LEDs and three push puttons are used to display the user and machine choices and the values.

~~~TEXT
----------------------------------------------
|                                            |
|                                            |
|      O              O             O        |
|     LED7           LED8          LED9      |
|                                            |
|                                            |
|                                            |
|      O              O             O        |
|     LED4           LED5          LED6      |
|                                            |
|                                            |
|                                            |
|      O              O             O        |
|     LED1           LED2          LED3      |
|                                            |
|                                            |
|                                            |
|      O              O             O        |
|   BUTTON1        BUTTON2       BUTTON3     |
|                                            |
|                                            |
----------------------------------------------
~~~

The LEDs indicate the user selection (push buttons) as follows:

~~~TEXT
LED1     Rock     (user selection)
LED2     Scissors (user selection)
LED3     Paper    (user selection)

LED4     Rock     (machine selection)
LED5     Scissors (machine selection)
LED6     Paper    (machine selection)

LED7     Winning  (user won, machine lost)
LED8     Tied     (no winner)
LED9     Loosing  (user lost, machine won)
~~~

The push buttons are used to select the users choice.

~~~TEXT
SWITCH1  Rock     (reset)
SWITCH2  Scissors (reset)
SWITCH3  Paper    (reset)
~~~

After startup the system is in the STARTUP state and three rows of
LEDs are turned on, and the system goes to the WAITING state.
The game is played by pushing a button (users choice) three times
switching the system state in the following order:

~~~TEXT
Click 1 => INIT     (all user LEDs are turned on)
Click 2 => READY    (all machine LEDs are turned on)
Click 3 => DONE     (the selections and values are displayed)
Click 4 => WAITING  (all LEDs are turned off)
~~~

Note that the third click determines the outcome of the game (user selection).
After 15 seconds inactivity (no button is pushed) the system state is reset to the WAITING state.

# Hardware

A single ESP32 - a small, complete, and breadboard-friendly board using the ESP-WROOM-32 module.
It is based on the ESP32 microcontroller that boasts Wifi, Bluetooth, Ethernet and Low Power support all in a single chip.

## Specifications

| Technical                  | Specifications           |
|----------------------------|--------------------------|
| Microcontroller            | Dual-core CPU Xtensa LX6 |
| Operating Voltage          | 3.3V                     |
| Input Voltage              | 7-12V                    |
| Digital I/O Pins (DIO)     | 25                       |
| Analog Input Pins (ADC)    | 6                        |
| Analog Outputs Pins (DAC)  | 2                        |
| UARTs                      | 3                        |
| SPIs                       | 2                        |
| I2Cs                       | 3                        |
| Flash Memory               | 4 MB                     |
| RTC SRAM                   | 16 KB                    |
| SRAM                       | 520 KB                   |
| ROM                        | 448 KB                   |
| Clock Speed                | 240 Mhz                  |
| Wi-Fi                      | IEEE 802.11 b/g/n/e/i    |
| PWM Output                 | 16                       |
| Power Consumption          | 80 mA                    |
| PCB Size (L x W x H)       | 5 x 3.7 x 1.2cm          |
| Weight                     | 13 g                     |

# Software
An Arduino compatible project implements all the code running on the board.
A async web server is used to provide a web based interface to the application.
The actual user interface is implemented using Bootstrap, jQuery and Javascript.
               