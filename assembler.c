#include <stdio.h>
#include <stdlib.h>

// Author: Umut Karadas

short memory[2047];//2KB of memory
short AX = 0,BX = 0,CX = 0,DX = 0;

typedef struct{//string structure
	char c[50];
}string;

string str[250];


int isRegister(char str[]);
short checkAddress(int sc,int ctrl);
short* checkRegister(char str[]);
int backwards(string str[], int sc, char tag[]);
int arraySize(string arr[]);
void tokenizer(char arr[]);
int ALU();
void writeBinary(int num);

int main() {
	
	FILE *file;
	char filename[200];
	printf("The source code must be in the file location where assembler.c is located.\n");
	printf("The source code can also be accessed by typing the file path(There must be double back slash between directories.)\n");
	printf("Enter the file name(.txt): ");
	scanf("%s",filename);
	file = fopen(filename,"r");
	
	int c = getc(file);
	int counter = 0;
	char characters[900];//Characters in the text file
	printf("\nSource Code==============================\n\n");
	while(c != EOF){
		printf("%c",c);
		if(c != '\t'){
			characters[counter] = c;
			counter++;
		}
		c = getc(file);
	}
	printf("\n=========================================\n");
	fclose(file);
	
	tokenizer(characters);
	
	if(ALU() != 1){
		printf("\nRegister Contents==============\n\n");
		printf("AX(decimal) = %d\n",AX);
		printf("AX(binary) = ");
		writeBinary(AX);
		printf("\n\nBX(decimal) = %d\n",BX);
		printf("BX(binary) = ");
		writeBinary(BX);
		printf("\n\nCX(decimal) = %d\n",CX);
		printf("CX(binary) = ");
		writeBinary(CX);
		printf("\n\nDX(decimal) = %d\n",DX);
		printf("DX(binary) = ");
		writeBinary(DX);
		printf("\n\n=================================\n");
	}else{
		printf("\n!!!Progmemory Error");
	}
	return 0;
}


int isRegister(char str[]){
	if(strcmp(str,"AX") == 0 ||strcmp(str,"BX") == 0 || strcmp(str,"CX") == 0 || strcmp(str,"DX") == 0){
		return 1;
	}
	return -1;	
}

short checkAddress(int sc,int ctrl){//For address resolution
	char temp[3];
	int i,j;
	short b;
	if(str[sc].c[1] == '['){//indirect command
		i = 2;
		j = 0;
		while(str[sc].c[i] != ']'){
			temp[j] = str[sc].c[i];
			i++;
			j++;
		}
		b = atoi(temp);
		if(ctrl == 0){//return direct index
			return b;
		}
	    if(0 <= b && 2048 > b){
			b = memory[memory[b]];
			return b;//value in address
		}else{
			return -1;//wrong address
		}
	}else{//direct command
		i = 1;
		j = 0;
		while(str[sc].c[i] != ']'){
			temp[j] = str[sc].c[i];
			i++;
			j++;
		}
		b = atoi(temp);
		if(ctrl == 0){
			return b;
		}
		if(0 <= b && 2048 > b){
			printf("b2 = %d\n",b);
			b = memory[b];
			return b;//value in address
		}else{
			return -1;//wrong address
		}
	}
}

short* checkRegister(char str[]){
	short *ptr = -1;
	if(strcmp(str,"AX") == 0){
		ptr = &AX;
	}
	if(strcmp(str,"BX") == 0){
		ptr = &BX;
	}
	if(strcmp(str,"CX") == 0 ){
		ptr = &CX;
	}
	if(strcmp(str,"DX") == 0){
		ptr = &DX;
	}
	return ptr;//If there is an error, the content of the ptr is -1, otherwise, the address of the registers is returned
}

int backwards(string str[], int sc, char tag[]){//Finds the address to branch by going back in the lexical str array
	while(strcmp(str[sc].c,tag) != 0 && sc >= 0){
		sc--;
	}
	if(strcmp(str[sc].c,tag) == 0){
		return sc;
	}
	return -1;//could not find the tag to branch(error)
}

int arraySize(string arr[]){ 
	int i;
	int say = 0;
	for(i = 0; i < 200;i++){
		if(strcmp(arr[i].c,"") != 0){
			say++;
		}	
	}
	return say;	
}

void tokenizer(char arr[]){
	int k;
	int i = 0, j = 0;
	int control = 0;
	for(k = 0; k < strlen(arr); k++){
		if(arr[k] != ' ' && arr[k] != ',' && arr[k] != '\n' &&  arr[k] != ':'){
			str[i].c[j] = arr[k];
			j++;
			control = 0;
		}else if(control != 1){
			i++;
			j = 0;
			control = 1;
		}
	}
}

int ALU(){
	short *ptr;//first operand
	short a,b;//"a" is used when the first operand is memory//b 2.operand
	int sc = 0;
	int adrs;//To store the branch address
	while(sc < arraySize(str)){
		if(str[sc].c[0] == 'S'){
			adrs = backwards(str,sc,str[sc+1].c);
			if(adrs == -1){//branch address not found
				return 1;//error
			}
			if(strcmp(str[sc].c,"SS") == 0){
				if(*ptr == 0){
					sc = adrs + 1;
				}else{
					sc += 2;
				}
					
			}else if(strcmp(str[sc].c,"SSD") == 0){
				if(*ptr != 0){
					sc = adrs + 1;
				}else{
					sc += 2;
				}
			}else if(strcmp(str[sc].c,"SN") == 0){
				if(*ptr < 0){
					sc = adrs + 1;
				}else{
					sc += 2;
				}
			}else if(strcmp(str[sc].c,"SP") == 0){
				if(*ptr > 0){
					sc = adrs + 1;
				}else{
					sc += 2;
				}
			}
		}else if(isRegister(str[sc+1].c) == 1 || str[sc+1].c[0] == '['){
			if(isRegister(str[sc+1].c) == 1){
				ptr = checkRegister(str[sc+1].c);
			}else{
				a = checkAddress(sc+1,0);
				if(a == -1){
					return 1;//error
				}
				ptr = memory;
				ptr = ptr + a;
			}
			
			if(str[sc+2].c[0] == '['){//If 2.operand is address
				b = checkAddress(sc+2,1);
				if(b == -1){
					return 1;
				}
			}else if(isRegister(str[sc+2].c) == 1){
				short *tmp = checkRegister(str[sc+2].c);
				b = *tmp;
			}else if(-128 <= atoi(str[sc+2].c) && 128 > atoi(str[sc+2].c)){//If 2.operand is number
				b = atoi(str[sc+2].c);
			}
			
			if(strcmp(str[sc].c,"HRK") == 0){
				*ptr = b;
				sc += 3;
			}else if(strcmp(str[sc].c,"TOP") == 0){
				*ptr = *ptr + b;
				sc += 3;
			}else if(strcmp(str[sc].c,"CRP") == 0){
				*ptr = *ptr * b;
				sc += 3;
			}else if(strcmp(str[sc].c,"CIK") == 0){
				*ptr = *ptr - b;
				sc += 3;
			}else if(strcmp(str[sc].c,"BOL") == 0){
				*ptr = *ptr / b;
				DX = *ptr % b;
				sc += 3;
			}else if(strcmp(str[sc].c,"VE") == 0){
				*ptr = *ptr & b;
				sc += 3;
			}else if(strcmp(str[sc].c,"VEYA") == 0){
				*ptr = *ptr | b;
				sc += 3;
			}else if(strcmp(str[sc].c,"DEG") == 0){
				*ptr = *ptr ^ 255;
				sc += 2;
			}
		}else{
			sc++;//branch tag
		}
	}
	return 0;
}

void writeBinary(int num){//Negative numbers complement 2
	int b[8],i = 0,temp = -1,tnum = num;
	if(num < 0){
		num *= -1;
	}
	while (num != 0){
		b[i] = num % 2;
		num = num / 2;
		temp = i;
		i++;
	}
	for (i = 7 ; i > temp ; i--){//To fill in the blanks with 0
		b[i] = 0;
	}
	if (tnum < 0){
		int pt = 0;
		while (b[pt] != 1){
			pt++;
		}
		for (i = 7; i > pt; i--){
			if(b[i] == 1){
				printf("0");
			}else{
				printf("1");
			}
		}
		for (i = pt; i >= 0; i--){
			printf("%d",b[i]);
		}	
	}else{
		for (i = 7; i >= 0; i--){
			printf("%d",b[i]);
		}	
	}
}

