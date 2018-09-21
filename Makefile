myls: myls.c
	gcc -Wall -pedantic -o myls myls.c

.PHONY: clean
clean:
	rm -f myls
