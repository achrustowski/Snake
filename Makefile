CC = cc
CFLAGS = -g -Wall
IFLAGS = -I/opt/homebrew/Cellar/sdl3/3.2.8/include -I/opt/homebrew/Cellar/sdl3_ttf/3.2.2/include
LDFLAGS = -L/opt/homebrew/Cellar/sdl3/3.2.8/lib/ -L/opt/homebrew/Cellar/sdl3_ttf/3.2.2/lib

build:
	${CC} ${CFLAGS} *.c ${IFLAGS} ${LDFLAGS} -lSDL3 -lSDL3_ttf -o snake

run:
	./snake

clean:
	rm snake
