#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZEOFARRAY(array) sizeof(array)/sizeof(array[0])

//unsigned char table[] = [
//0x1b /*NOP command*/,
//0x5a /*PUSH command*/,
//0x5b /*POP command*/,
//0xad /*ADD command*/
//];
int SIZE = 0;
int *stack;

int uchtint(unsigned char ch) {
	return (int)ch;
}

void push(int element) {
	int *temp = (int*)malloc(SIZE+1);
	for (int i = 0; i < SIZE; i++) {
		temp[i] = stack[i];
	}
	SIZE++;
	stack = temp;
	stack[SIZE-1] = element;
}

int* checkCodes(unsigned char *codes, int len) {
	for (int i = 0; i < len; i++) {
		if (codes[i] == 0x1b) {}
		else if(codes[i] == 0x5a) {
			push(uchtint(codes[i+1]));
			i++;
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
    for (int i = 0; i < SIZE; i++) {
    	printf("%d\n", stack[i]);
    }
    return 0;
}