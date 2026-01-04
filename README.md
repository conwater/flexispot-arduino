# flexispot-arduino
Arduino code for controlling a Flexispot E7 standing desk (control box CB38M2L(IB)-1). The pins used and commands available are listed below.

## Pin Map
| **GPIO** | **Name** | **T568A** | **T568B** | 
| :------- | :------- | :-------- | :-------- |
| 11       | TX       | O         | G         |
| 12       | Pin 20   | B         | B         |
| 13       | RX       | B/W       | B/W       |
| GND      | GND      | BR/W      | BR/W      |

## Commands
| **Command** | **Description** |
| :---------- | :-------------- |
| `sit`       | Move to sitting height |
| `stand`     | Move to standing height |
| `goto XX.X` | Move to XX.X inches |

*note that the desk is bound between 24.5 and 48.5 inches, and will not move within 0.5 inches of its present height (to prevent under/overshoot)*

## Acknowledgements
[iMicknl / LoctekMotion_IoT](https://github.com/iMicknl/LoctekMotion_IoT) for publishing research on the pin naming and commands list- this was extremely useful, and I couldn't have made this without it!
