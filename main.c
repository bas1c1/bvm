#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

//#define RAM_SIZE 32768

//unsigned char table[] = [
//0x1b /*NOP command*/,
//0x5a /*PUSH command*/,
//0x5b /*POP command*/
//];

int SIZE = 0;
int SIZEVIDMEM = 0;
unsigned char *stack;
//uint16_t RAM[RAM_SIZE];

void array_copy(int * dst, const int * src, size_t size) {
    while ( size-- )
        *dst++ = *src++;
}

void push(unsigned char element) {
	int *temp = (int*)malloc(SIZE+1);
	array_copy(temp, stack, SIZE);
	
	SIZE++;
	stack = temp;
	stack[SIZE-1] = element;
}

void pop() {
	int *temp = (int*)malloc(SIZE-1);
	array_copy(temp, stack, SIZE-1);
	SIZE--;
	stack = temp;
}

int* checkCodes(unsigned char *codes, int len) {
	for (int i = 0; i < len; i++) {
		if (codes[i] == 0x1b) {}
		else if(codes[i] == 0x5a) {
			push(codes[i+1]);
			i += 1;
			continue;
		}
		else if(codes[i] == 0x5b) {
			pop();
			continue;
		}

	}
	
	return stack;
}

void main(int argc, char const *argv[]) {
	int fd;
	int d;
    unsigned char c;
    unsigned char *codes;

    sscanf(argv[2], "%d", &d);
    codes = (unsigned char*)malloc(d);

    fd = open(argv[1], O_RDONLY);
    for (int i = 0; i < d; i++) {
    	read(fd, &c, sizeof(c));
    	codes[i] = c;
    }
    close(fd);
   	checkCodes(codes, d);
    return 0;
}
