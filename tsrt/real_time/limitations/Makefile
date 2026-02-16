
CC=arm-linux-gcc

TARGET?=

WARN= -Wall -lrt



all: $(TARGET)

$(TARGET) : $(TARGET).c
	    $(CC) $(TARGET).c  -o $(TARGET) $(WARN)  
		 
clean:
		-rm $(TARGET)
