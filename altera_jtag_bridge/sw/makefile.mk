###############################################################################
## Makefile
###############################################################################

# Target
TARGET     ?= test

# Options
CFLAGS      = -g
LDFLAGS     = 
#LIBS        = -lftdi
LIBS        = -ljtag_atlantic

# Source Files
#OBJ = ftdi_hw.o $(TARGET).o
OBJ = altera_jtag_hw.o $(TARGET).o

###############################################################################
# Rules
###############################################################################
all: $(TARGET)
    
clean:
	-rm *.o $(TARGET)

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

%.o : %.cpp
	g++ -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJ)
	g++ $(LDFLAGS) $(OBJ) $(LIBS) -o $@
