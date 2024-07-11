# Per-port Power Control USB HUB
![Preview](/Assets/hub-small.jpg)

## Key Features
* 3-port USB hub fully compatiable with USB High-Speed(480Hz), Full-Speed(12Hz) and Low-Speed(1.5MHz) devices
* Supports per-port power control, current limitation and thermal shutdown protection
* No special driver required, reuses the driver from the widely known CH340 USB-to-serial chip
* Supports 5V 2A external USB-C power supply
* Reverse current protection along with resetable fuse
* Relay: SRD-05VDC-SL-C 10A 30VDC, 10A 250VAC
* Customized settings stored in built-in flash memory

## Schematic & Layout (Designed w/ KiCAD 8.0)
![Schematic](/Assets/schematic.svg)  
![Schematic](/Assets/layout.png)  

## 3D Printed Case (Designed w/ Fusion 360)
![3D Printed Case](/Assets/3d_printed_case.gif) 

## Basic Control Commands (Group Control)
Turn On:  ```A0 01 01 A2```
  
Turn Off: ```A0 01 00 A1```
  
*These commands are designed to mimic the command of **[USB RELAY](https://www.smart-prototyping.com/USB-Relay-1-Channel)** on purpose*  
![USB RELAY](/Assets/usb-relay.jpg)  

## Advanced Control Commands

### Per-port control
Foramt: ```F0 {AA} {BB} {ZZ} ```  

* ```{AA}``` Port Address:  
  | Port | Address |
  |:----:|:-------:|
  | USB1 |   0x01  |
  | USB2 |   0x02  |
  | USB3 |   0x03  |
  | Relay|   0x04  |

*  ```{BB}``` On/Off:  
    * ON: ```0x01```  
    * OFF: ```0x00```  

* ```{ZZ}``` Checksum:  
 ```ZZ = (0xF0 + {AA} + {BB}) & 0xFF```

Example:  
* Turn on USB1: ```F0 01 01 F2```  
* Turn off Relay: ```F0 04 00 F4```  

### Write Device Settings
Format:  
* Command: ```AB CD EF 67 {X1 X2 X3 X4} ZZ```:

    *  ```{X1 X2 X3 X4}``` Device Settings:  
        |          |                       |
        |:--------:|:---------------------:|
        | ```X1``` |   USB1 Port Settings  |
        | ```X2``` |   USB2 Port Settings  |
        | ```X3``` |   USB3 Port Settings  |
        | ```X4``` |  Relay Port Settings  |
        ```c
        typedef struct DeviceSettings
        {
            PortSettings_t usb1;
            PortSettings_t usb2;
            PortSettings_t usb3;
            PortSettings_t relay;
        } __attribute__((packed)) DeviceSettings_t;
        ```
        * Port Settings:  
            |  Bit Field |  Setting  |  Description |
            |:--------:|:---------------------:|:------------------|
            | b0 |   Port Default State  | Default state of port while power on  |
            | b1 |   *Group Control       | 0: Disable group control (ignores **Basic Control Commands**)<br>1: Enable group control
            | b2 |   *Polarity  | Polarity of the port during group control<br>0: Default polarity<br>1: Reversed polarity
            | b3 ~ b7 |  Reserved  |  Reserved for future use   |

            **The **Group Control** and the **Polarity** only affets the **Basic Control Commands***
            ```c
            typedef struct PortSettings
            {
                uint8_t defaultState :1;
                uint8_t groupControl :1;
                uint8_t polarity :1;
                uint8_t reserved :5;
            } __attribute__((packed)) PortSettings_t;
            ```
    * ```{ZZ}``` Checksum:  
    ```ZZ = (0xAB + 0xCD + 0xEF + 0x67 + {X1} {X2} {X3} {X4}) & 0xFF```

* Response:  
    ```SUCCESS``` or ```FAILED```

Example:  
* Defaut Devcie Settings  
    TX: ```AB CD EF 67 07 07 07 02 E5```  
    RX: ```SUCCESS```  

    This command sets the default state of USB1, USB2 and USB3 to On, the default state of Relay to Off.  
    The "Turn on" command ```A0 01 01 A2``` turns on the relay but turns off USB1, USB2 and USB3.  
    The "Turn off" command ```A0 01 00 A1``` turns off the relay but turns on USB1, USB2 and USB3. 

