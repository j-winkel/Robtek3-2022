# Notes for robot

**HANDIN DATE:** 
June 3rd. 2022. : 03-06-2022.
-

**Rotation Method**

a concurrent problem has been the mpu5060 has stoped working on multiple ocasions with the issue being, it stops recording values and freezes the robots movment among other problems. 
The current problem has been solved by introducing a decoupleling capacitor to take care of the electrical noice generated form the motors powerlines, that genereted a magnetic firled that would cause the electrical noice in the system. 

Using an MPU5060 with a decoupling capacitor to reduce electrical noise.         


**Colorsensor**
- The colorsensor works with i2c as well as the MPU5060 so in turn the colorsensor should alsom make use of a decoupeling capacitor.


**General notes** 
- Section for additional notes and points for robot.


**Track description**
- There will be around 12 different colors.
- The different colors will be the size of a normal A4 paper. 
- The track will have white in between the colors. 
 
**Arduino Wifi**
- Install required drivers and the WIFININA libiary. 
- Install board type (megaAVR). 
- pin 10 on the motorsheilld should be connected to pin 11 on the arduino wifi as pin 10 is not able to use PWM and it is needed for the motor control. 


**TODO**
-

- Get the Arduino Wifi module. (and change out the ESP32 dev board).
- Sketch and draw a 3D modelled frame for the car.
- - Determin best way of placing components of the car frame. 
- - Make gearing for robot to slow it down to better track movment trough gyroscope MPU6050 (gearing between 1:2 and 1:4).
- - Determin how much we want to print/cut out for the car (wheels, axcels, gearing, ...). 
- Complete implementation of robot movement.
- - Posibly implement webserver movement ie. stop/start and maybe controls. (could be sick to implement smartphone trackpad, or simialr)
- Line sensors for tracking boundaries of robot (code is partially complete and needs to be reworked into the code).
- Graphics: Implement way of determining the specefic color of the (ground/card), and accuratley put it into a grid formation for display, as well as report back the positions in text. 
- Power supply: choose suitable power supply to power car and additional components. 
- 

---

**[Tinkercad link](https://www.tinkercad.com/things/cVaP56cp1Xu-magnificent-jarv-wluff/edit?sharecode=f-WMT-jDCFX4ljCWVwpv9Y9nf8SZGSp0SBOLLC0KjG4)** 


