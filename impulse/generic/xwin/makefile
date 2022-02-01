.SUFFIXES: .c .cpp .obj

TARGET =
OBJ =

LIB=-limpulse -lhotas -lskyhawk -lX11 -lm

#LIB=impulse.a hotas.a skyhawk.a -lX11 -lm

#LIB=-limpulse -lhotas -lskyhawk -lgl -lm

#LIB=impulse.a hotas.a skyhawk.a -lgl -lm



CC = gcc
CFLAGS = -I..



$(TARGET) : $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIB)



.c.o :
	$(CC) -c $*.c $(CFLAGS)
