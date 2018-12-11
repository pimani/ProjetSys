CC = gcc
CPPFLAGS = -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=500
CFLAGS = -std=c11 -Wall -Wconversion -Werror -Wextra -pthread -Wpedantic -O2 -fstack-protector-all -fpie -g
LDFLAGS = -pie -Wl,-z,relro -z,now -pthread
LDLIBS = -lrt
objects = fifosm.o
executable = fifosm

all: $(executable)

clean:
	$(RM) $(objects)

cleanAll:
	$(RM) $(objects) $(objects2) $(executable)

$(executable): $(objects)
	$(CC) $(LDFLAGS) $(objects) $(LDLIBS) -o $(executable)


fifosm.o: fifosm.c fifosm.h
