thing: thing.c
	clang -Wall -Wextra -O3 thing.c -lGL -lglut -lGLEW -lpthread -o thing
