CC = gcc
FLAGS = `pkg-config --cflags gtk+-3.0` -fsanitize=address
TARGET = main
SRCS = main.c logicanalyzer/*.c
LINKS = `pkg-config --cflags --libs gtk+-3.0` -lm -lpthread
MINGW =  x86_64-w64-mingw32-gcc
WINTARGET = main.exe

LA_FLAGS = 
LA_TARGET = logicAnalyzer
LA_SRCS = logicAnalyzer.c
LA_LINKS = -lm

all:
	$(CC) $(FLAGS) -o $(TARGET) $(SRCS) $(LINKS)
	./$(TARGET)

windows:
	$(MINGW) $(FLAGS) -o $(WINTARGET) $(SRCS) $(LINKS)

la:
	$(CC) $(LA_FLAGS) -o $(LA_TARGET) $(LA_SRCS) $(LA_LINKS)
	./$(LA_TARGET)
	
