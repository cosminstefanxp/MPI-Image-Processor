CC    = mpicc
CFLAGS = -c -Wall
MAKE  = make
LIBS  = -lm -lmpi
MPIRUN= mpirun
NP_ARG= -np

SOURCES = img_process.c
EXECUTABLE = img_process
OBJECTS = $(SOURCES:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) ${LIBS} -o $(EXECUTABLE) $(OBJECTS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(<) -o $(@)

clean:
	rm -f *.o $(EXECUTABLE)
	
run:
	${MPIRUN} ${NP_ARG} 4 ./${EXECUTABLE} 100

sources: @echo ${SOURCES}