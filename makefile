CC = gcc
CPPFLAGS = -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=500 -D_GNU_SOURCE
CFLAGS = -std=c11 -Wall -Wconversion -Werror -Wextra -pthread -Wpedantic -O2 -fstack-protector-all -fpie -g -c
LDFLAGS = -pie -Wl,-z,relro,-z,now -pthread
LDLIBS = -lrt
objects = fifosm.o lanceur.o
objects2 = fifosm.o client.o
executable = serveur
executable2 = client
bibliotheque = libfifo.a

all: $(executable) $(executable2)

clean:
	$(RM) *.o *.a *.pipe

cleanall:
	$(RM) *.o *.a *.pipe $(executable) $(executable2)

tar: cleanall
	tar -zcf "barthblond_projet.tar.gz" *.c *.h *.tex makefile

$(executable): $(objects) $(bibliotheque)
	$(CC) $(LDFLAGS)  $^ -o $@ $(LDLIBS)

$(executable2): $(objects2) $(bibliotheque)
	$(CC) $(LDFLAGS)  $^ -o $@ $(LDLIBS)

fifosm.o: fifosm.c fifosm.h
	$(CC) -c -fPIC fifosm.c
lanceur.o: lanceur.c fifosm.h lanceur.h
client.o: client.c fifosm.h client.h

$(bibliotheque): fifosm.o
	ar -rv $(bibliotheque) $^
