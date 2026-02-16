
CC=arm-linux-gcc

TARGET?=

WARN= -Wall

LIBS= -lgpiod

all: $(TARGET)

$(TARGET) : $(TARGET).c
	    $(CC) $(TARGET).c  -o $(TARGET) $(WARN) $(LIBS) 
		 
clean:
		-rm $(TARGET)
