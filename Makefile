CC=g++
CFLAGS=-lGL -lGLU -lglut -lm -s -Ofast
TARGET=simulation

$(TARGET): $(TARGET).cpp
	$(CC) $(TARGET).cpp $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)

