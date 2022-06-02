# Stuff

**Rotation Method**

a concurrent problem has been the mpu5060 has stoped working on multiple ocasions with the issue being, it stops recording values and freezes the robots movment among other problems. 
The current problem has been solved by introducing a decoupleling capacitor to take care of the electrical noice generated form the motors powerlines, that genereted a magnetic firled that would cause the electrical noice in the system. 

Using an MPU5060 with a decoupling capacitor to reduce electrical noise         


**Colorsensor**

The colorsensor works with i2c as well as the MPU5060 so in turn the colorsensor should alsom make use of a decoupeling capacitor


**General notes** 


purple: scl

