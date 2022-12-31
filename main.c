#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

typedef struct label
{
	unsigned char *name;
	int addr;
} label;

int POINTER = 0;
int SIZE = 0;
int CODESSIZE = 0;
int LABELCOUNT = 0;
int LABELSTACKCOUNT = 0;

int *stack;
label *labelstack;
label *labels;

label getLabelByName(unsigned char *name) {
	for (int i = 0; i < LABELCOUNT; i++) {
		if (labels[i].name == name) {
			return labels[i];
		}
	}
	label lbl;
	lbl.addr = 0;
	return lbl;
}

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

void inc() {
	stack[SIZE-1]++;
}

void dec() {
	stack[SIZE-1]--;
}

void jmp() {
	POINTER = labelstack[LABELSTACKCOUNT-1].addr;
}

void lbl(unsigned char *addres) {
	int *temp = (int*)malloc(LABELCOUNT+1);
	array_copy(temp, labels, LABELCOUNT);
	LABELCOUNT++;
	labels = temp;

	label lbl;
	lbl.name = addres;
	lbl.addr = POINTER+1;

	labels[LABELCOUNT-1] = lbl;
}

void pushlbl(unsigned char *name) {
	int *temp = (int*)malloc(LABELSTACKCOUNT+1);
	array_copy(temp, labelstack, LABELSTACKCOUNT);
	LABELSTACKCOUNT++;
	labelstack = temp;

	label lbl = getLabelByName(name);

	labelstack[LABELSTACKCOUNT-1] = lbl;
}

void call(unsigned char addr) {
	switch (addr) {
		case 0xa0:
			putchar(stack[SIZE-1]);
			break;
	}
}

int* checkCodes(unsigned char *codes) {
	for (; POINTER < CODESSIZE; POINTER++) {
		//if (codes[POINTER] == 0xff) {continue;}
		if(codes[POINTER] == 0x0a) {
			push(codes[POINTER+1]);
			POINTER += 1;
		}
		else if(codes[POINTER] == 0x0b) {
			pop();
		}
		else if(codes[POINTER] == 0x0c) {
			inc();
		}
		else if(codes[POINTER] == 0x0d) {
			dec();
		}
		else if(codes[POINTER] == 0x0e) {
			jmp();
		}
		else if(codes[POINTER] == 0x0f) {
			int labellen = 0;
			unsigned char *name;
			while (codes[POINTER] != 0xff) {
				POINTER++;
				if (codes[POINTER] != 0xff) {
					int *temp = (int*)malloc(labellen+1);
					array_copy(temp, name, labellen);
					
					labellen++;
					name = temp;
					name[labellen-1] = codes[POINTER];
				}
			}
			lbl(name);
			free(name);
		}
		else if(codes[POINTER] == 0x1b) {

			int labellen = 0;
			unsigned char *name;
			while (codes[POINTER] != 0xff) {
				POINTER++;
				if (codes[POINTER] != 0xff) {
					int *temp = (int*)malloc(labellen+1);
					array_copy(temp, name, labellen);
					
					labellen++;
					name = temp;
					name[labellen-1] = codes[POINTER];
				}
			}
			pushlbl(name);
			free(name);
		}
		else if(codes[POINTER] == 0x1c) {
			call(codes[POINTER+1]);
			POINTER += 1;
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
    CODESSIZE = d;
   	checkCodes(codes);

   	for (int i = 0; i < SIZE; ++i)
   	{
   		printf("%x\n", stack[i]);
   	}
    return 0;
}
