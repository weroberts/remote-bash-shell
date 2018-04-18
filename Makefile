output: chef.o cook.o
	gcc chef.o -o chef
	gcc cook.o -o cook
chef: chef.c
	gcc -c chef.c -o chef.o
cook: cook.c
	gcc -c cook.c -o cook.o
