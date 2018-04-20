# NCAS Lidar Scheduler
This is software to generate schedules for the Halo Photonics Stream 
Line lidar. Currently the software can be used to build schedules with 
multiple stare, RHI and VAD manouvres. As demand requires further 
manouvres will be added.

# Usage
Usage is pretty straightforward. When you run the code you can add the 
first manouvre by selecting the manouvre from the drop down and hitting 
the Add button. Once you have created some manouvres you can move them 
up and down the schedule, delete them and add new manouvres anywhere in 
the schedule using any of the apropriate buttons.

You may save the schedule using the Save or Save As... menu items which 
will save the current schedule in a format that can be reloaded using 
the Open menu item.

When you have a schedule that you wish to use with the lidar use the 
File->Export menu item to export the schedule in a format that the Lidar 
software can load.

# Building
The software currently only includes a Visual Studio build system for 
windows. The code should be cross platform so builing on other OSs 
should be possible if you know what you are doing.

This software uses the wxWidgets library for GUI elements so you must 
install this before building. See www.wxwidgets.org for instructions. 
You will need version 3.0 or newer.

To build simply open the .sln file with Visual Studio. Select either x64 
or x86 platform from the dropdown at the top then select Build->Build 
Solution.

# To Do
Validation: I need to add checks that the numbers entered are actually 
numbers and that they are within range.

Tabbing: The tab order is all messed up if users use tab to try to move 
between boxes.
