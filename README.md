# Firmware for the UltraSwitch Project of http://www.remoteQTH.com
This is the firmware for the remoteQth.com Ultraswitch STM5500 Version
If you need help, feel free to contact DM5XX@gmx.de
Sketch is developed with IDE Version 1.8 and later (using Visual Micro)

The idea:
Inspired from the megawebswitch project, which is a really powerful way to switch 63 sources (with on/off-Switches, grouped switches, latched switching with configurable time, switching restirctions and configurable excludes - using SD-Card for configuration), the ultraswitch was born.
The learning from the megawebswitch project was, there is too much overload with configuration and possibilities, a normal user does not need.

This is the Ultraswitch Version with ETHERNET SUPPORT ONLY! The chip used is a so called "blue pill" STM32F103C8T6. Much much faster than a normal Arduino, also faster than the ESP12F. For the basic idea, look to the github.com/ultraswitch/doc for documentation of the ultraswitch. I will add the Information and layout for the ultraswitch ethernet version later.

If you want to start: Get a STM32F103C8T6 and a W5500 Ethernet board - MCP23017 and Relay board(s) like in the Ultraswitch project.
You need th Arduino IDE, but with an additional Board: https://github.com/stm32duino/BoardManagerFiles/raw/master/STM32/package_stm_index.json

To load the Code into the BluePIll, u need a USB to TTL converter. More info https://www.youtube.com/watch?v=K-jYSysmw9w


This is free software. You can redistribute it and/or modify it under the terms of Creative Commons Attribution 3.0 United States License if at least the version information says credits to remoteQTH.com :P

To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.

Tribute to OL7M!
LLAP!
