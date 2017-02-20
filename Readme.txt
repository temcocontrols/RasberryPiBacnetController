/******************************************************************************
 *                     RASPBERRY PI BACNET CONTROLLER
 *****************************************************************************/
 
### Procedure to compile ###

- Unzip the project and copy it to home or any directory of your choice in RPi
- Browse to RasberryPiBacnetController directory
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


### BAS_RPi_Port project revision ###