/******************************************************************************
 *                     RASPBERRY PI BACNET CONTROLLER
 *****************************************************************************/
 
### Procedure to compile ###

- Clone the project.
- cd to RasberryPiBacnetController directory
- enter command 'sudo make' to compile code
- enter command 'sudo make clean' to clean code


### To Run project ###

- Execute below command after compiling project
	./BAS_RPi_Port
- The 'BAS_RPi_Port' linux executable file is located at '../RasberryPiBacnetController/' path


### Prerequisites ###

- Raspberry pi should have g++ compiler

To install g++ compiler, enter below command:
sudo apt-get install build-essential


### GLOSSARY ###

APDU - The application Protocol Data Unit. The APDU is made up of an APCI block and an ASDU. 
APCI - (Application Protocol Control Information) contains message control information.
ASDU - (Application Specific Data Unit) contains information to be processed by the receiving station.
Want-Points - A point being requested from a network. A list of want-points is created in each controller when a point in another controller is referenced in a program, system group, or trend log.
Points - Any hardware or software object configured in a digital controller. A point can be an input, output, variable, schedule, log, or PID controller loop.


### Project Approach and Status ###

The project involves the porting of legacy DOS code to RasberryPi. 

The project has below major parts:
1] BACnet stack and application
2] Small scheduling wrapper called MT Kernel
3] DOS API
4] BGI graphics
5] x86 assembly code

My approach:
1] BACnet Stack and Application
This code/module needs no porting since its written in C and is easily portable to any platform.
I have kept this module unchanged.

2] MT Kernel
MT Kernel is a custom scheduling wrapper over DOS OS APIs. This is critical module since it decides the timing and scheduling for BACnet MSTP and PTP.
I am using ucos2 wrapper for scheduling. The logic within MT Kernel APIs is replaced by ucos2 APIs.
This is Work in Progress.

My second thought on this module was to replace MT Kernel APIs with POSIX APIs.

3] DOS APIs
This too is critical and API varies from reading keyboard input to OS APIs.
Replaced few of the APIs with POSIX. Some of API like bioskey etc are re-created.

4] BGI Graphics
This can be replaced by QT, openGL or SDL.

I thought was to keep it as is. And use libgraph or similar BGI Linux Library.

5] x86 Assembly Code
The x86 assembly code snippets are used at many places and needs to replaced by C logic.

Done nothing on this module.