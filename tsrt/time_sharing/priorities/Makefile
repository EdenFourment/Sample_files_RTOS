
CC=arm-linux-gcc

TARGET?=

WARN= -Wall



all: $(TARGET)

$(TARGET) : $(TARGET).c
	    $(CC) $(TARGET).c  -o $(TARGET) $(WARN)  
		 
clean:
		-rm $(TARGET)
