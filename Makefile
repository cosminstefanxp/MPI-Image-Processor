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
	${MPIRUN} ${NP_ARG} 4 ./${EXECUTABLE} contrast bec10.pgm 70 180 out_bec10.pgm

contrast: $(EXECUTABLE)
	${MPIRUN} ${NP_ARG} 3 ./${EXECUTABLE} contrast test.pgm 3 11 out.pgm

contrast_back: contrast
	${MPIRUN} ${NP_ARG} 3 ./${EXECUTABLE} contrast out.pgm 0 15 out_final.pgm

sources: @echo ${SOURCES}
