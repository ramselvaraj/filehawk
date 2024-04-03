CFLAGS = -Wall -pedantic -std=gnu99

all:filehawk

filehawk:
	gcc $(CFLAGS) `pkg-config --cflags --libs libnotify` filehawk.c -o build/filehawkd
