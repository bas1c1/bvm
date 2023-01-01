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

enum {
	/* UNDEF INSTR */
	UNDEF   = 0x1a,

	/* NULL INSTR */
	ZERO    = 0xff,

	/* MAIN INSTR */
	PUSH    = 0x0a,
	POP     = 0x0b,
	INC     = 0x0c,
	DEC     = 0x0d,
	JMP     = 0x0e,
	LBL     = 0x0f,
	PUSHLBL = 0x1b,
	CALL    = 0x1c,

	/* CALL INSTR */
	PUTC    = 0xa0,
	GETC    = 0xa1,
	SIZEOF  = 0xa2,
	PUTI    = 0xa3,
	PUTUC   = 0xa4,

	/* COND INSTR */
	JE      = 0xb0,
	JG      = 0xb1,
	JL      = 0xb2,
	JNE     = 0xb3,
	JLE     = 0xb4,
	JGE     = 0xb5,

	/* BIN  INSTR */
	ADD     = 0xc0,
	SUB     = 0xc1,
	MUL     = 0xc2,
	DIV     = 0xc3,
	MOD     = 0xc4,
	AND     = 0xc5,
	OR      = 0xc6,
	XOR     = 0xc7,
	SHR     = 0xc8,
	SHL     = 0xc9,
};

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

void cond(unsigned char addr) {
	switch (addr) {
		case JE:
			if (stack[SIZE-1] == stack[SIZE-2]) jmp();
			break;
		case JG:
			if (stack[SIZE-1] > stack[SIZE-2]) jmp();
			break;
		case JL:
			if (stack[SIZE-1] < stack[SIZE-2]) jmp();
			break;
		case JNE:
			if (stack[SIZE-1] != stack[SIZE-2]) jmp();
			break;
		case JLE:
			if (stack[SIZE-1] <= stack[SIZE-2]) jmp();
			break;
		case JGE:
			if (stack[SIZE-1] >= stack[SIZE-2]) jmp();
			break;
	}
}

void bin(unsigned char addr) {
	switch (addr) {
		case ADD:
			push(stack[SIZE-1] + stack[SIZE-2]);
			break;
		case SUB:
			push(stack[SIZE-1] - stack[SIZE-2]);
			break;
		case DIV:
			push(stack[SIZE-1] / stack[SIZE-2]);
			break;
		case MUL:
			push(stack[SIZE-1] * stack[SIZE-2]);
			break;
		case MOD:
			push(stack[SIZE-1] % stack[SIZE-2]);
			break;
		case AND:
			push(stack[SIZE-1] & stack[SIZE-2]);
			break;
		case OR:
			push(stack[SIZE-1] | stack[SIZE-2]);
			break;
		case XOR:
			push(stack[SIZE-1] ^ stack[SIZE-2]);
			break;
		case SHR:
			push(stack[SIZE-1] >> stack[SIZE-2]);
			break;
		case SHL:
			push(stack[SIZE-1] << stack[SIZE-2]);
			break;
	}
}

void call(unsigned char addr) {
	switch (addr) {
		case PUTC:
			putchar(stack[SIZE-1]);
			break;
		case PUTI:
			printf("%d", stack[SIZE-1]); 
			break;
		case PUTUC:
			printf("%x", stack[SIZE-1]); 
			break;
		case GETC:
			push(getchar());
			break;
		case SIZEOF:
			push(sizeof stack[SIZE-1]);
			break;
	}
}

int* checkCodes(unsigned char *codes) {
	for (; POINTER < CODESSIZE; POINTER++) {
		//if (codes[POINTER] == 0xff) {continue;}
		if(codes[POINTER] == PUSH) {
			push(codes[POINTER+1]);
			POINTER += 1;
		}
		else if(codes[POINTER] == POP) {
			pop();
		}
		else if(codes[POINTER] == INC) {
			inc();
		}
		else if(codes[POINTER] == DEC) {
			dec();
		}
		else if(codes[POINTER] == JMP) {
			jmp();
		}
		else if(codes[POINTER] == JE || codes[POINTER] == JG || codes[POINTER] == JL ||
				codes[POINTER] == JNE || codes[POINTER] == JLE || codes[POINTER] == JGE) {
			cond(codes[POINTER]);
		}
		else if(codes[POINTER] == ADD || codes[POINTER] == SUB || codes[POINTER] == MUL ||
				codes[POINTER] == DIV || codes[POINTER] == AND || codes[POINTER] == OR  ||
				codes[POINTER] == XOR || codes[POINTER] == SHR || codes[POINTER] == SHL) {
			bin(codes[POINTER]);
		}
		else if(codes[POINTER] == LBL) {
			int labellen = 0;
			unsigned char *name;
			while (codes[POINTER] != ZERO) {
				POINTER++;
				if (codes[POINTER] != ZERO) {
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
		else if(codes[POINTER] == PUSHLBL) {

			int labellen = 0;
			unsigned char *name;
			while (codes[POINTER] != ZERO) {
				POINTER++;
				if (codes[POINTER] != ZERO) {
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
		else if(codes[POINTER] == CALL) {
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
    return 0;
}
