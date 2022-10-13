all: clean vmsim fifo optimal lru
	gcc -o vmsim vmsim.o fifo.o optimal.o lru.o
vmsim:
	gcc -c vmsim.c
fifo:
	gcc -c fifo.c
optimal:
	gcc -c optimal.c
lru:
	gcc -c lru.c
clean:
	rm -f vmsim