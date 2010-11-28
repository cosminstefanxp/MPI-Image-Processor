CC    = mpicc
CFLAGS = -c -Wall -g
MAKE  = make
LIBS  = -lm -lmpi
MPIRUN= mpirun
NP_ARG= -np

SOURCES = img_process.c
EXECUTABLE = img_process
OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(SOURCES:.c=.h)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) ${LIBS} -o $(EXECUTABLE) $(OBJECTS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(<) -o $(@)

clean:
	rm -f *.o $(EXECUTABLE)
	
run: $(EXECUTABLE)
	${MPIRUN} ${NP_ARG} 5 ./${EXECUTABLE} contrast bec10.pgm

run2: $(EXECUTABLE)
	${MPIRUN} ${NP_ARG} 3 ./${EXECUTABLE} contrast test.pgm

sources: @echo ${SOURCES}
