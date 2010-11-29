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
	
contrast_big: $(EXECUTABLE)
	${MPIRUN} ${NP_ARG} 4 ./${EXECUTABLE} contrast bec10.pgm

contrast: $(EXECUTABLE)
	${MPIRUN} ${NP_ARG} 3 ./${EXECUTABLE} contrast test.pgm 2 12 out.pgm

sources: @echo ${SOURCES}
