CC = gcc
CPPFLAGS = -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=500 -D_GNU_SOURCE
CFLAGS = -std=c11 -Wall -Wconversion -Werror -Wextra -pthread -Wpedantic -O2 -fstack-protector-all -fpie -g
LDFLAGS = -pie -Wl,-z,relro -z,now -pthread
LDLIBS = -lrt
objects = fifosm.o lanceur.o
objects2 = fifosm.o client.o
executable = serveur
executable2 = client

all: $(executable) $(executable2)

clean:
	$(RM) $(objects)

cleanAll:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(LDFLAGS) $(objects) $(LDLIBS) -o $(executable)

$(executable2): $(objects2)
	$(CC) $(LDFLAGS) $(objects2) $(LDLIBS) -o $(executable2)

fifosm.o: fifosm.c fifosm.h
lanceur.o: lanceur.c fifosm.h lanceur.h
client.o: client.c fifosm.h
