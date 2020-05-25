CC=gcc
CFLAGS=`sdl2-config --cflags` -O3 -std=c99
LINK=`sdl2-config --libs` -lSDL2_image
SRC= main.c game.c sdl_driver.c 
OBJ=$(SRC:.c=.o)

%.o:	%.c
	$(CC) -c $(CFLAGS) $(SRC) $<

gamesdl:	$(OBJ)
	$(CC) $(LINK) $(OBJ) -o $@

clean:
	rm *.o gamesdl
