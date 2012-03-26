CC    = mpicc
CFLAGS = -c -Wall -g
MAKE  = make
LIBS  = -lm -lmpi
MPIRUN= mpirun
NP_ARG= -np

SOURCES = img_process.c
EXECUTABLE = img_process
OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(SOURCES:.c=.h) filters.h

filtru = identity

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) ${LIBS} -o $(EXECUTABLE) $(OBJECTS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(<) -o $(@)

clean:
	rm -f *.o $(EXECUTABLE)
	

contrast: $(EXECUTABLE)
	${MPIRUN} ${NP_ARG} 3 ./${EXECUTABLE} contrast test.pgm 3 11 out.pgm

filter: $(EXECUTABLE)
	${MPIRUN} ${NP_ARG} 3 ./${EXECUTABLE} filter test.pgm $(filtru) out.pgm

entropy: $(EXECUTABLE)
	${MPIRUN} ${NP_ARG} 3 ./${EXECUTABLE} entropy test.pgm 1 1 1 out_residual


 