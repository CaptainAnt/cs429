#include <stdlib.h>
#include <stdio.h>

#if 0
   #define debugPrint(a) printf a
#else
   #define debugPrint(a) (void)0
#endif

#define AOK 1
#define HLT 2
#define ADR 3
#define INS 4

FILE *fp;

typedef char* string;
typedef unsigned int y86;
typedef unsigned char byte;
typedef unsigned int byte4;

y86 smallest = 0xFFFFFFFF;
y86 largest = 0x0;

byte4 reg[8];	//program registers
const string reg_name[8] = {"\%EAX", "\%ECX", "\%EDX", "\%EBX", "\%ESP", "\%EBP", "\%ESI", "\%EDI"};
unsigned char ZF =0 , SF = 0 , OF = 0;	//condition codes
y86 PC; 	//program counter (duh)
unsigned char stat = AOK; //program status

byte **meml1[1024] = {NULL};

void error_exit(const string message){
    fprintf(stderr, "%s.\n", message);
    exit(-1);
}

void setmem(y86 a, byte val){
	int top = (a >> 22) & 0x3FF;
	byte **meml2 = meml1[top];
	if(meml2 == NULL){
		meml1[top] = (byte**)calloc(1024,sizeof(byte*));
		meml2 = meml1[top];
		if(meml2 == NULL)
			error_exit("Ran out of L2 memory");
	}
	int mid = (a >> 12) & 0x3FF;
	byte *meml3 = meml2[mid];
	if(meml3 ==NULL){
		meml2[mid] = (byte*)calloc(4096,sizeof(byte*));
		meml3 = meml2[mid];
		if(meml3 == NULL)
			error_exit("Ran out of L3 memory");
	}
	int bot = a & 0xFFF;
	meml3[bot] = val;
	debugPrint(("store 0x%02X at 0x0x%08X\n", val, a));
	if(a<smallest){
		smallest = a;
		debugPrint(("new smallest address: 0x0x%08X\n", a));
	}
	if(a>largest){
		largest = a;
		debugPrint(("new largest address: 0x0x%08X\n", a));
	}
}

void setmem4(y86 a, byte4 v){
	setmem(a+3, ((v >> 24) & 0xFF));
	setmem(a+2, ((v >> 16) & 0xFF));
	setmem(a+1, ((v >> 8) & 0xFF));
	setmem(a, (v & 0xFF));
}

byte getmem(y86 a){
	byte val = 0;
	int top = (a >> 22) & 0x3FF;
	byte **meml2 = meml1[top];
	if(meml2 != NULL){
		int mid = (a >> 12) & 0x3FF;
		byte *meml3 = meml2[mid];
		if(meml3 != NULL){
			int bot = a & 0xFFF;
			val = meml3[bot];
		}
	}
	debugPrint(("load 0x%02X from 0x0x%08X\n", val, a));
	return val;
}

byte4 get4(y86 a){
	byte4 ret = 0;
	int i;
	for(i = 3; i >=0; i--){
		byte val = getmem(a+i);
		ret = (ret << 8) | val;
	}
	debugPrint(("load4 0x0x%08X from 0x0x%08X\n", ret, a));
	return ret;
}

int next2byte(FILE *f){
	int c1,c2;
	c1 = getc(fp);
	if(c1 == EOF) return -1;
	c2 = getc(fp);
	if(c2 == EOF) return -2;
	return ((c1 << 8) | c2);
}

void readfile(FILE* fp){
	y86 load;
	debugPrint(("reading yb file\n"));
	if(fp == NULL)
		error_exit("The file is not a y86 object file");
		
	int check = next2byte(fp);
	if(check != 0x7962){
		debugPrint(("magic number was 0x%04X\n", check));
		error_exit("The file is not a y86 object file");
	}
	int size;
	PC = load = next2byte(fp);
	while(load!=-1){
		if(load == -2)
			error_exit("The y86 file is corrupted");
		size = next2byte(fp);
		if(size<0)
			error_exit("The y86 file is corrupted");
		debugPrint(("load %d bytes at 0x0x%08X\n", size, load));
		while(size>0){
			int c = getc(fp);
			if (c == EOF)
				error_exit("The y86 file is corrupted");
			setmem(load,c);
			load++;
			size--;
		}
		load = next2byte(fp);
	}
}

void halt(void){
	stat = HLT;
	printf("0x%08X: halt \n",PC);
}

void nop(void){
	printf("0x%08X: nop\n",PC);
	PC++;
}

void rrmovl(void){
	//0x00000089: rrmovl %EBP,%ESP                     (%EBP -> 0x000000F4, 0x000000F4 -> %ESP)
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else{
		reg[b] = reg[a];
		printf("0x%08X: rrmovl %s,%s                     (%s -> 0x%08X, 0x%08X -> %s)\n", PC, reg_name[a], reg_name[b], reg_name[a], reg[a], reg[a], reg_name[b]);
	}
	PC += 2;
}

void irmovl(void){
	//0x0000006A: irmovl 0x00000001,%EDI               (0x00000001 -> %EDI)
	byte regs = getmem(PC+1);
	byte b = regs & 0xF;
	if(b > 7){
		stat = ADR;
	}
	else{
		byte4 v = get4(PC+2);
		reg[b] = v;
		printf("0x%08X: irmovl 0x%08X,%s               (0x%08X -> %s)\n", PC, v, reg_name[b], v, reg_name[b]);
	}
	PC += 6;
}

void rmmovl(void){
	//0x00000055: rmmovl %ESI,0x00000000(%ECX)         (%ECX -> 0x000000B4, %ESI -> 0xFFFFFFFF, 0xFFFFFFFF -> [0x000000B4])
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else{
		byte4 A = reg[a];
		byte4 B = reg[b];
		byte4 C = get4(PC+2);
		C += B;
		setmem4(C,A);
		printf("0x%08X: rmmovl %s,0x%08X(%s)         (%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> [0x%08X])\n", PC, reg_name[a], C-B, reg_name[b], reg_name[b], C, reg_name[a], reg[a], reg[a], C);
	}
	PC += 6;
}

void mrmovl(void){
	//> 0x00000034: mrmovl 0x00000008(%EBP),%EBX         (%EBP -> 0x000002C0, [0x000002C8] -> 0x00000090, 0x00000090 -> %EBX)
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else{
		byte4 B = reg[b];
		byte4 C = get4(PC+2);
		byte4 E = B+C;
		byte4 M = get4(E);
		reg[a] = M;
		printf("0x%08X: mrmovl 0x%08X(%s),%s         (%s -> 0x%08X, [0x%08X] -> 0x%08X, 0x%08X -> %s)\n", PC, C, reg_name[b], reg_name[a], reg_name[b], B, E, M, M, reg_name[a]);
	}
	PC += 6;
}

void call(void){
	//0x00000024: call 0x0000002A                      (%ESP -> 0x000000FC, 0x00000029 -> [0x000000F8], 0x000000F8 -> %ESP, 0x0000002A -> PC)
	byte4 C = get4(PC+1);
	byte4 P = PC + 5;
	byte4 B = reg[4];
	byte4 E = B-4;
	setmem4(E,P);
	reg[4] = E;
	printf("0x%08X: call 0x%08X                      (%s -> 0x%08X, 0x%08X -> [0x%08X], 0x%08X -> %s, 0x%08X -> PC)\n", PC, C, reg_name[4], B, P, E, E, reg_name[4], C);
	PC = C;
}

void ret(void){
	//0x0000008D: ret                                  (%ESP -> 0x000000F8, [0x000000F8] -> 0x00000029, 0x000000FC -> %ESP, 0x00000029 -> PC)
	byte4 A = reg[4];
	byte4 E = A + 4;
	byte4 M = get4(A);
	reg[4] = E;
	printf("0x%08X: ret                                  (%s -> 0x%08X, [0x%08X] -> 0x%08X, 0x%08X -> %s, 0x%08X -> PC)\n", PC, reg_name[4], A, A, M, E, reg_name[4], M, reg_name[4], E);
	PC = M; 	
}

void pushl(void){
	//0x0000001A: pushl %EAX                           (%EAX -> 0x000000B4, %ESP -> 0x00000104, 0x000000B4 -> [0x00000100], 0x00000100 -> %ESP)
	byte regs = getmem(PC+1);
	byte a;
	a = (regs >> 4) & 0xF;
	if(a>7){
		stat = ADR;
	}
	else{
		byte4 A = reg[a];
		byte4 B = reg[4];
		byte4 E  = B - 4;
		setmem4(E,A);
		reg[4] = E;
		printf("0x%08X: pushl %s                           (%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> [0x%08X], 0x%08X -> %s)\n", PC, reg_name[a], reg_name[a], A, reg_name[4], B, A, E, E, reg_name[4]);
	}
	PC += 2;
}

void popl(void){
	//0x00000083: popl %EDI                            (%ESP -> 0x000000E8, [0x000000E8] -> 0x00000000, 0x000000EC -> %ESP, 0x00000000 -> %EDI)
	byte regs = getmem(PC+1);
	byte a;
	a = (regs >> 4) & 0xF;
	if(a>7){
		stat = ADR;
	}
	else{
		byte4 A = reg[4];
		byte4 B = reg[4];
		byte4 E = B + 4;
		byte4 M = get4(A);
		reg[4] = E;
		reg[a] = M;
		printf("0x%08X: popl %s                            (%s -> 0x%08X, [0x%08X] -> 0x%08X, 0x%08X -> %s, 0x%08X -> %s)\n", PC, reg_name[a], reg_name[4], A, A, M, E, reg_name[4], M, reg_name[a]);
	}
	PC += 2;
}

void cmovle(void){
//	0x00000065: cmovg %EDI,%ESI                      (ZF->0, SF->0, OF->0, %EDI -> 0x000000C0, 0x000000C0 -> %ESI)
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else if(ZF == 1 || SF != OF){
		reg[b] = reg[a];
		printf("0x%08X: cmovle %s %s                      (ZF->%d, SF->%d, 0F->%d, %s -> 0x%08X, 0x%08X -> %s)\n", PC, reg_name[a], reg_name[b], ZF, SF, OF, reg_name[a], reg[a], reg[a], reg_name[b]);
	}
	else{
		printf("0x%08X: cmovle %s %s                      (ZF->%d, SF->%d, 0F->%d, no move)\n", PC, reg_name[a], reg_name[b], ZF, SF, OF);
	}
	PC += 2;
}

void cmovl(void){
	//	0x00000065: cmovg %EDI,%ESI                      (ZF->0, SF->0, OF->0, %EDI -> 0x000000C0, 0x000000C0 -> %ESI)
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else if(SF != OF){
		reg[b] = reg[a];
		printf("0x%08X: cmovl %s %s                      (SF->%d, 0F->%d, %s -> 0x%08X, 0x%08X -> %s)\n", PC, reg_name[a], reg_name[b], SF, OF, reg_name[a], reg[a], reg[a], reg_name[b]);
	}
	else{
		printf("0x%08X: cmovl %s %s                      (SF->%d, 0F->%d, no move)\n", PC, reg_name[a], reg_name[b], SF, OF);
	}
	PC += 2;
}

void cmove(void){
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else if(ZF == 1){
		reg[b] = reg[a];
		printf("0x%08X: cmove %s %s                      (ZF->%d, %s -> 0x%08X, 0x%08X -> %s)\n", PC, reg_name[a], reg_name[b], ZF, reg_name[a], reg[a], reg[a], reg_name[b]);
	}
	else{
		printf("0x%08X: cmove %s %s                      (ZF->%d, no move)\n", PC, reg_name[a], reg_name[b], ZF);
	}
	PC += 2;
}

void cmovne(void){
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else if(ZF == 0){
		reg[b] = reg[a];
		printf("0x%08X: cmovne %s %s                      (ZF->%d, %s -> 0x%08X, 0x%08X -> %s)\n", PC, reg_name[a], reg_name[b], ZF, reg_name[a], reg[a], reg[a], reg_name[b]);
	}
	else{
		printf("0x%08X: cmovne %s %s                      (ZF->%d, no move)\n", PC, reg_name[a], reg_name[b], ZF);
	}
	PC += 2;
}

void cmovge(void){
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else if(SF == OF){
		reg[b] = reg[a];
		printf("0x%08X: cmovge %s %s                      (SF->%d, 0F->%d, %s -> 0x%08X, 0x%08X -> %s)\n", PC, reg_name[a], reg_name[b], SF, OF, reg_name[a], reg[a], reg[a], reg_name[b]);
	}
	else{
		printf("0x%08X: cmovge %s %s                      (SF->%d, 0F->%d, no move)\n", PC, reg_name[a], reg_name[b], SF, OF);
	}
	PC += 2;
}

void cmovg(void){
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else if(SF == OF && ZF == OF){
		reg[b] = reg[a];
		printf("0x%08X: cmovg %s %s                      (ZF->%d, SF->%d, 0F->%d, %s -> 0x%08X, 0x%08X -> %s)\n", PC, reg_name[a], reg_name[b], ZF, SF, OF, reg_name[a], reg[a], reg[a], reg_name[b]);
	}
	else{
		printf("0x%08X: cmovg %s %s                      (ZF->%d, SF->%d, 0F->%d, no move)\n", PC, reg_name[a], reg_name[b], ZF, SF, OF);
	}
	PC += 2;
}

void addl(void){
	//0x00000077: addl %EBX,%EDX                       (%EBX -> 0xFFFFFFFF, %EDX -> 0x00000002, 0x00000001 -> %EDX, ... -> CC)
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else{
		byte4 A = reg[a];
		byte4 B = reg[b];
		byte4 E = A + B;
		if (E == 0) ZF = 1;
		else ZF = 0;
		if (B >> 31 == A >>31 && B >> 31 != E >> 31) OF = 1;
		else OF = 0;
		SF = E >> 31;
		printf("0x%08X: addl %s,%s                       (%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> %s,", PC, reg_name[a], reg_name[b], reg_name[a], A, reg_name[b], B, E, reg_name[b]);
		printf(" %s%s%s -> CC)\n", ZF != 0 ? "Z" : ".", SF != 0 ? "S" : ".", OF != 0 ? "0" : ".");
		reg[b] = E;
	}
	PC += 2;
}

void subl(void){
	//0x00000077: addl %EBX,%EDX                       (%EBX -> 0xFFFFFFFF, %EDX -> 0x00000002, 0x00000001 -> %EDX, ... -> CC)
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else{
		byte4 A = reg[a];
		byte4 B = reg[b];
		byte4 E = B - A;
		if (E == 0) ZF = 1;
		else ZF = 0;
		if (B >> 31 != A >>31 && B >> 31 != E >> 31) OF = 1;
		else OF = 0;
		SF = E >> 31;
		printf("0x%08X: subl %s,%s                       (%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> %s,", PC, reg_name[a], reg_name[b], reg_name[a], A, reg_name[b], B, E, reg_name[b]);
		printf(" %s%s%s -> CC)\n", ZF != 0 ? "Z" : ".", SF != 0 ? "S" : ".", OF != 0 ? "O" : ".");
		reg[b] = E;
	}
	PC += 2;
}

void andl(void){
	//0x00000077: addl %EBX,%EDX                       (%EBX -> 0xFFFFFFFF, %EDX -> 0x00000002, 0x00000001 -> %EDX, ... -> CC)
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else{
		byte4 A = reg[a];
		byte4 B = reg[b];
		byte4 E = A & B;
		if (E == 0) ZF = 1;
		else ZF = 0;
		OF = 0;
		SF = E >> 31;
		printf("0x%08X: andl %s,%s                       (%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> %s,", PC, reg_name[a], reg_name[b], reg_name[a], A, reg_name[b], B, E, reg_name[b]);
		printf(" %s%s%s -> CC)\n", ZF != 0 ? "Z" : ".", SF != 0 ? "S" : ".", OF != 0 ? "O" : ".");
		reg[b] = E;
	}
	PC += 2;
}

void xorl(void){
	//0x00000077: addl %EBX,%EDX                       (%EBX -> 0xFFFFFFFF, %EDX -> 0x00000002, 0x00000001 -> %EDX, ... -> CC)
	byte regs = getmem(PC+1);
	byte a,b;
	a = (regs >> 4) & 0xF;
	b = regs & 0xF;
	if(a>7 || b > 7){
		stat = ADR;
	}
	else{
		byte4 A = reg[a];
		byte4 B = reg[b];
		byte4 E = B ^ A;
		if (E == 0) ZF = 1;
		else ZF = 0;
		OF = 0;
		SF = E >> 31;
		printf("0x%08X: xorl %s,%s                       (%s -> 0x%08X, %s -> 0x%08X, 0x%08X -> %s,", PC, reg_name[a], reg_name[b], reg_name[a], A, reg_name[b], B, E, reg_name[b]);
		printf(" %s%s%s -> CC)\n", ZF != 0 ? "Z" : ".", SF != 0 ? "S" : ".", OF != 0 ? "O" : ".");
		reg[b] = E;
	}
	PC += 2;
}

void jmp(void){
	//0x0000000C: jmp 0x00000024                       (0x00000024 -> PC)
	byte4 C = get4(PC+1);
	printf("0x%08X: jmp 0x%08X                       (0x%08X -> PC)\n", PC, C, C);
	PC = C;
}

void jle(void){
	//0x00000079: jne 0x00000057                       (ZF->1, no jump)
	byte4 C = get4(PC+1);
	if(ZF == 1 || SF != OF){
		printf("0x%08X: jle 0x%08X                       (ZF->%d, SF->%d, OF->%d, 0x%08X -> PC)\n", PC, C, ZF, SF, OF, C);
		PC = C;
	}
	else{ 
		printf("0x%08X: jle 0x%08X                       (ZF->%d, SF->%d, OF->%d, no jump)\n", PC, C, ZF, SF, OF);
		PC += 5;
	}
}

void jl(void){
	byte4 C = get4(PC+1);
	if(SF != OF){
		printf("0x%08X: jl 0x%08X                       (SF->%d, OF->%d, 0x%08X -> PC)\n", PC, C, SF, OF, C);
		PC = C;
	}
	else{ 
		printf("0x%08X: jl 0x%08X                       (SF->%d, OF->%d, no jump)\n", PC, C, SF, OF);
		PC += 5;
	}
}

void je(void){
	byte4 C = get4(PC+1);
	if(ZF == 1){
		printf("0x%08X: je 0x%08X                        (ZF->%d, 0x%08X -> PC)\n", PC, C, ZF, C);
		PC = C;
	}
	else{ 
		printf("0x%08X: je 0x%08X                        (ZF->%d, no jump)\n", PC, C, ZF);
		PC += 5;
	}
}

void jne(void){
	byte4 C = get4(PC+1);
	if(ZF == 0){
		printf("0x%08X: jne 0x%08X                       (ZF->%d, 0x%08X -> PC)\n", PC, C, ZF, C);
		PC = C;
	}
	else{ 
		printf("0x%08X: jne 0x%08X                       (ZF->%d, no jump)\n", PC, C, ZF);
		PC += 5;
	}
}

void jge(void){
	byte4 C = get4(PC+1);
	if(SF == OF){
		printf("0x%08X: jge 0x%08X                       (SF->%d, OF->%d, 0x%08X -> PC)\n", PC, C, SF, OF, C);
		PC = C;
	}
	else{ 
		printf("0x%08X: jge 0x%08X                       (SF->%d, OF->%d, no jump)\n", PC, C, SF, OF);
		PC += 5;
	}
}

void jg(void){
	byte4 C = get4(PC+1);
	if(ZF == 0 && SF == OF){
		printf("0x%08X: jg 0x%08X                        (ZF->%d, SF->%d, OF->%d, 0x%08X -> PC)\n", PC, C, ZF, SF, OF, C);
		PC = C;
	}
	else{ 
		printf("0x%08X: jg 0x%08X                        (ZF->%d, SF->%d, OF->%d, no jump)\n", PC, C, ZF, SF, OF);
		PC += 5;
	}
}

void invalid(void){
	byte i = getmem(PC);
	stat = INS;
	printf("0x%08X: 0x%02X -- invalid opcode)\n", PC, i);
}

void (*decode_op(byte adr))(void)
{
    switch (adr){
		case 0x00: return *halt;		//done
		case 0x10: return *nop;			//done
		case 0x20: return *rrmovl;		//done
		case 0x30: return *irmovl;		//done
		case 0x40: return *rmmovl;		//done
		case 0x50: return *mrmovl;		//done
		case 0x80: return *call;		//done
		case 0x90: return *ret;			//done
		case 0xA0: return *pushl;		//done
		case 0xB0: return *popl;		//done
		case 0x21: return *cmovle;		//done
		case 0x22: return *cmovl;		//done
		case 0x23: return *cmove;		//done
		case 0x24: return *cmovne;		//done
		case 0x25: return *cmovge;		//done
		case 0x26: return *cmovg;		//done
		case 0x60: return *addl;		//done
		case 0x61: return *subl;		//done
		case 0x62: return *andl;		//done
		case 0x63: return *xorl;		//done
		case 0x70: return *jmp;			//done
		case 0x71: return *jle;			//done
		case 0x72: return *jl;			//done
		case 0x73: return *je;			//done
		case 0x74: return *jne;			//done
		case 0x75: return *jge;			//done
		case 0x76: return *jg;			//done
		default : return *invalid;		//done
	}
}

void execute(void){
	while(stat == AOK){
		byte ins = getmem(PC);
		decode_op(ins)();
	}
	
}

main(int arg1, char *arg2[]){
	while(--arg1 > 0)
		fp = fopen(*++arg2, "r");
	readfile(fp);
	
	unsigned int i;
	for (i = smallest; i <= largest; i++)
		{
			byte a = getmem(i);
			byte4 n = get4(i);
			//fprintf(stdout, "%04X %02X 0x%08X %10d %s\n", i, a, n, n, opcode(a);
		}
	debugPrint(("PC start = %X\n",PC));
	execute();	
}