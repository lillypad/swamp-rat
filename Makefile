all:
	mkdir -p bin/
	gcc src/main.c -o bin/swamp-rat
	gcc src/stub.c -o bin/stub
	objcopy bin/swamp-rat --add-section rodata=bin/stub
	cp bin/swamp-rat swamp-rat
clean:
	rm -f swamp-rat
	rm -rf bin/
