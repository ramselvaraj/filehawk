CFLAGS = -Wall -pedantic -std=gnu99

all:filehawk

filehawk:
	gcc $(CFLAGS) filehawk.c -o ./build/filehawkd `pkg-config --cflags --libs libnotify`
