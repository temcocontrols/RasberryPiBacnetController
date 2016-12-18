#Start of the makefile

CXX = g++
LDFLAGS = -lwiringPi -pthread

VPATH = ./Source:./:./OBJ

OUTPUT_OPTION = -o OBJ/$@

CXXFLAGS = -Wall -fpermissive

Target = BAS_RPi_Port

Objects := $(notdir $(patsubst %.cpp,%.o,$(wildcard Source/*.cpp)))



all: $(Target)

$(Target): $(Objects)
	$(CXX) $(CXXFLAGS) -o $(Target) $(addprefix OBJ/,$(Objects)) $(LDFLAGS)


#Beware of -f. It skips any confirmation/errors (e.g. file does not exist)

.PHONY: clean
clean:
	rm -f $(addprefix OBJ/,$(Objects)) $(Target)