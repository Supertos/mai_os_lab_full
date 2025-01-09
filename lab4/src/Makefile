
all:
	# rm -f main liballocator_firstfit.so liballocator_buddy.so
	gcc -Wall -g main.c -o main -ldl -lm
	gcc -Wall -g dll/ffit.c -o liballocator_firstfit.so -shared -lm
	gcc -Wall -g dll/buddy.c -o liballocator_buddy.so -shared -lm

	