
### Procedure to compile ###

- Unzip the project and copy it to home or any directory of your choice in RPi
- Browse to src directory
- enter command 'sudo make' to compile code
- enter command 'sudo make clean' to clean code


### To Run project ###

- Execute below command after compiling project
	./BAS_RPi_Port
- The 'BAS_RPi_Port' linux executable file is located at '../BAS_RPi_Port/src/' path


### Prerequisites ###

- Raspberry pi should have g++ compiler

To install g++ compiler, enter below command:
sudo apt-get install build-essential


### TBD/Pending ###

- Configure the firmware to 18.2 ticks/second



### BAS_RPi_Port project revision ###

********************************** Rev: 0.01 **********************************
- Created baseline makefile project

********************************** Rev: 0.02 **********************************
- Ported tick task (int8_task_switch()) to Linux tick task (SIGALRM).
- Configured 100 ticks/second. But DOS default ticks are 18.2 ticks/second. 

********************************** Rev: 0.03 **********************************
- Ported RS232.cpp to linux using wiringpi library

********************************** Rev: 0.04 **********************************
- Modified:   
	baseclas.h
	pc8250.h
	ptp.cpp
	ptp.h
	recdef.h
	rs485.h
	serial.cpp
	serial.h
	t3000.cpp
	t3000hlp.h
	
********************************** Rev: 0.05 **********************************
- New makefile created to suit latest project folder structure
- Some files renamed to lower-case since g++ rejects upper-case file extension
- X.cpp commented