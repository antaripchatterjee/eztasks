CC=gcc
CFLAGS=-Wall -std=c99 -g
INCLUDES=./headers
BUILD_D=./build/debug
BUILD_R=./build/release
OBJECTS=./objects
AR=ar


all: main.o eztasks
	$(CC) $(CFLAGS) -I$(INCLUDES) $(OBJECTS)/main.o $(BUILD_D)/eztasks.a -o test 

install: util.o group.o task.o
	$(AR) -rcs $(BUILD_R)/eztasks.a $(OBJECTS)/util.o $(OBJECTS)/group.o $(OBJECTS)/task.o

eztasks: util.o group.o task.o
	$(AR) -rc $(BUILD_D)/eztasks.a $(OBJECTS)/util.o $(OBJECTS)/group.o $(OBJECTS)/task.o

util.o: ./src/util.c
	$(CC) $(CFLAGS) -I$(INCLUDES) -c ./src/util.c -o $(OBJECTS)/util.o

group.o: ./src/group.c
	$(CC) $(CFLAGS) -I$(INCLUDES) -c ./src/group.c -o $(OBJECTS)/group.o

task.o: ./src/task.c
	$(CC) $(CFLAGS) -I$(INCLUDES) -c ./src/task.c -o $(OBJECTS)/task.o

main.o: ./main.c
	$(CC) $(CFLAGS) -I$(INCLUDES) -c ./main.c -o $(OBJECTS)/main.o

clean:
	rm -rf $(OBJECTS)/*.o
	rm -rf $(BUILD_R)/*.a
	rm -rf $(BUILD_D)/*.a
	rm -rf $(BUILD_D)/*.dll
	rm -rf $(BUILD_R)/*.dll
	rm -f test.exe
	rm -f test