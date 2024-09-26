/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

#define MAXSIZE 500
#define MAXLINELENGTH 1000
#define MAXFILES 6

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
	char label[7];
	char location;
	unsigned int offset;
};

struct RelocationTableEntry {
	unsigned int offset;
	char inst[7];
	char label[7];
	unsigned int file;
};

struct FileData {
	unsigned int textSize;
	unsigned int dataSize;
	unsigned int symbolTableSize;
	unsigned int relocationTableSize;
	unsigned int textStartingLine; // in final executable
	unsigned int dataStartingLine; // in final executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
	unsigned int textSize;
	unsigned int dataSize;
	unsigned int symbolTableSize;
	unsigned int relocationTableSize;
	int text[MAXSIZE*MAXFILES];
	int data[MAXSIZE*MAXFILES];
	SymbolTableEntry symbolTable[MAXSIZE*MAXFILES];
	RelocationTableEntry relocTable[MAXSIZE*MAXFILES];
};

int textOffset(FileData *files, int textNum, int lineNum, int totalSize);
int dataOffset(FileData *files, int textNum, int lineNum, int totalSize);
void CheckDup(int size, FileData *files, char Array[MAXLINELENGTH][7]);

int main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr; 
	unsigned int i, j;

	if (argc <= 2) {
		printf("error: usage: %s <obj file> ... <output-exe-file>\n",
				argv[0]);
		exit(1);
	}

	outFileString = argv[argc - 1];

	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	FileData files[MAXFILES];

  // read in all files and combine into a "master" file
	for (i = 0; i < argc - 2; i++) {
		inFileString = argv[i+1];

		inFilePtr = fopen(inFileString, "r");
		printf("opening %s\n", inFileString);

		if (inFilePtr == NULL) {
			printf("error in opening %s\n", inFileString);
			exit(1);
		}

		char line[MAXLINELENGTH];
		unsigned int textSize, dataSize, symbolTableSize, relocationTableSize;

		// parse first line of file
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
				&textSize, &dataSize, &symbolTableSize, &relocationTableSize);

		files[i].textSize = textSize;
		files[i].dataSize = dataSize;
		files[i].symbolTableSize = symbolTableSize;
		files[i].relocationTableSize = relocationTableSize;

		// read in text section
		int instr;
		for (j = 0; j < textSize; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = strtol(line, NULL, 0);
			files[i].text[j] = instr;
		}

		// read in data section
		int data;
		for (j = 0; j < dataSize; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = strtol(line, NULL, 0);
			files[i].data[j] = data;
		}

		// read in the symbol table
		char label[7];
		char type;
		unsigned int addr;
		for (j = 0; j < symbolTableSize; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
					label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
			if (!strcmp("Stack", label) && type != 'U'){
				printf("Crash on defined Stack\n");
                exit(1);
            }
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < relocationTableSize; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
					&addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file	= i;
		}
		fclose(inFilePtr);
	} // end reading files

	// *** INSERT YOUR CODE BELOW ***
	
	//in this case argc - 2 = 2
	int totalSize = argc - 2;
	
	char deflabelArray[MAXLINELENGTH][7];
	char undeflabelArray[MAXLINELENGTH][7];
	int defarrSize = 0;
	int undefarrSize = 0;
	//error checking for duplicated defind global lables
	for(int i = 0; i < totalSize; i++) {
		for(int k = 0; k < files[i].symbolTableSize; k++) {
			if(files[i].symbolTable[k].location == 'U') {
				strcpy(undeflabelArray[undefarrSize], files[i].symbolTable[k].label);
				undefarrSize++;
			} else {
				strcpy(deflabelArray[defarrSize], files[i].symbolTable[k].label);
				defarrSize++;
			}
		}
	}
	CheckDup(defarrSize, files, deflabelArray);
	
	//check for labels that are never definedf
	bool undefCheck = true;
	for(int i = 0; i < undefarrSize; i++) {
		undefCheck = true;
		for(int k = 0; k < defarrSize; k++) {
			if(!strcmp(undeflabelArray[i], deflabelArray[k])) {
				undefCheck = false;
			}
		}
		//printf("%s\n", undeflabelArray[i]);
		if((undefCheck == true) && (strcmp(undeflabelArray[i], "Stack"))) {
			printf("Crash on never defined label\n");
			exit(1);
		}
	}
	
	for(int i = 0; i < totalSize; i++) {
		printf("%d\n", i);
		printf("%s\n", "icheck");
		int relocNum = 0;
		for(int t = 0; t < files[i].textSize; t++) {
			
			printf("%d\n", t);
			printf("%s\n", "tcheck");
			printf("%s\n", files[i].relocTable[relocNum].label);
			printf("%d\n", files[i].relocTable[relocNum].offset);
			printf("%s\n", files[i].relocTable[relocNum].inst);
			if((files[i].relocTable[relocNum].offset == t) && (!strcmp(files[i].relocTable[relocNum].inst, "lw") || !strcmp(files[i].relocTable[relocNum].inst, "sw"))) {
				
				int addToMac = textOffset(files, i, relocNum, totalSize);
				fprintf(outFilePtr, "%d\n", files[i].text[t] + addToMac);
				relocNum++;
			} else {
				fprintf(outFilePtr, "%d\n", files[i].text[t]);
			}
		}
		
	}
	printf("data begins\n");
	for(int i = 0; i < totalSize; i++) {
		// printf("%d\n", i);
		// printf("%s\n", "icheck");
		int relocNum = 0;
		for(int textrelSize = 0; textrelSize < files[i].relocationTableSize; textrelSize++) {
			printf("%d\n", relocNum);
			printf("%s\n", files[i].relocTable[textrelSize].inst);
			if(strcmp(files[i].relocTable[textrelSize].inst, ".fill")) {
				relocNum++;
			}
		}
		for(int d = 0; d < files[i].dataSize; d++) {
			// printf("%d\n", d);
			// printf("%s\n", "dcheck");
			// printf("%d\n", relocNum);
			// printf("%s\n", files[i].relocTable[relocNum].label);
			// printf("%d\n", files[i].relocTable[relocNum].offset);
			// printf("%s\n", files[i].relocTable[relocNum].inst);
			if((files[i].relocTable[relocNum].offset == d) && (!strcmp(files[i].relocTable[relocNum].inst, ".fill"))) {
				int dataToMac = dataOffset(files, i, relocNum, totalSize);
				fprintf(outFilePtr, "%d\n", files[i].data[d] + dataToMac);
				relocNum++;
			} else {
				fprintf(outFilePtr, "%d\n", files[i].data[d]);
			}
		}
		
	}
	




	//    Begin the linking process
	//    Happy coding!!!
	
} // main

	
	void CheckDup(int size, FileData *files, char Array[MAXLINELENGTH][7]) {
		printf("work83497\n");
		//printf("%d\n", size + 1);
		for(int i = 0; i < size; i++) {
			//printf("work\n");
			for(int k = i + 1; k < size; k++) {
				if(strlen(Array[i]) && strlen(Array[k])) {
					//printf("%s\n", Array[i]);
					//printf("%s\n", Array[k]);
					if(!strcmp(Array[i], Array[k])){
						printf("Crash on same label\n");
						exit(1);
					}
				}
			}
		}
	}

	int getLastFourBitsAsDecimal(int number) {
		int lastFourBits = number & 0xF;
		return lastFourBits;
	}

	//textOffset should take in the textNum, lineNum
	//goal: check if its a local or global label, 
	//if its local then add length of the other text files 
	//and preceeding data to the offset
	
	int textOffset(FileData *files, int textNum, int lineNum, int totalSize) {
		int offsettoAdd = 0;
		printf("%s\n", files[textNum].relocTable[lineNum].label);
		//check if global label, check if stack
		if(!strcmp(files[textNum].relocTable[lineNum].label, "Stack")) {
			for(int outText = 0; outText < totalSize; outText++) {
				offsettoAdd += files[outText].textSize;
			}
			for(int outData = 0; outData < totalSize; outData++) {
				offsettoAdd += files[outData].dataSize;
			}
		} else if(isupper(files[textNum].relocTable[lineNum].label[0])) {
			bool undefLab = false;
			int saveK = 0;
			//printf("work\n");

			//look through symbol table to find label
			for(int k = 0; k < files[textNum].symbolTableSize; k++) {
				if(!strcmp(files[textNum].symbolTable[k].label, files[textNum].relocTable[lineNum].label)) {
					printf("loading label below\n");
					printf("%s\n", files[textNum].symbolTable[k].label);
					saveK = k;

					//look to see if its undefined
					if(files[textNum].symbolTable[k].location == 'U') {
						undefLab = true;
						printf("workssdfsd\n");
						printf("%s\n", files[textNum].symbolTable[k].label);
					}
					
				}
			}
			if(undefLab) {
				//look through every single symbol table to find utilized label
				for(int symfilNum = 0; symfilNum < totalSize; symfilNum++) {
					for(int symIndex = 0; symIndex < files[symfilNum].symbolTableSize; symIndex++) {
						
						// printf("%s\n", files[symfilNum].relocTable[symIndex].label);
						// printf("%s\n", &files[symfilNum].symbolTable[symIndex].location);
						if(!strcmp(files[textNum].relocTable[lineNum].label, files[symfilNum].symbolTable[symIndex].label) && (files[symfilNum].symbolTable[symIndex].location == 'T')) {
							for(int outText = 0; outText < symfilNum; outText++) {
								offsettoAdd += files[outText].textSize;
							}
							offsettoAdd += files[symfilNum].symbolTable[symIndex].offset;
						} else if(!strcmp(files[textNum].relocTable[lineNum].label, files[symfilNum].symbolTable[symIndex].label) && (files[symfilNum].symbolTable[symIndex].location == 'D')) {
							printf("work1\n");
							for(int outText = 0; outText < totalSize; outText++) {	
								offsettoAdd += files[outText].textSize;
							}
							printf("%d\n", offsettoAdd);
							for(int outData = 0; outData < symfilNum; outData++) {
								offsettoAdd += files[outData].dataSize;
							}
							printf("%d\n", offsettoAdd);
							offsettoAdd += files[symfilNum].symbolTable[symIndex].offset;
						} 
					}
				}
			} else {
				if(files[textNum].symbolTable[saveK].location == 'D') {
					for(int outText = 0; outText < totalSize; outText++) {
						if(outText != textNum) {
							offsettoAdd += files[outText].textSize;
						}
					}

					for(int outData = 0; outData < textNum; outData++) {
						offsettoAdd += files[outData].dataSize;
					}
				} else {
					for(int outText = 0; outText < textNum; outText++) {
						offsettoAdd += files[outText].textSize;	
					}
				}
			}
			
		} else {
			//its local
			printf("its local time\n");
			int testerNum = getLastFourBitsAsDecimal(files[textNum].text[files[textNum].relocTable[lineNum].offset]);
			printf("%d\n", files[textNum].text[files[textNum].relocTable[lineNum].offset]);
			printf("%d\n", testerNum);
			
			int space = files[textNum].textSize;
			
			// for(int randi = 0; randi < textNum + 1; randi++) {
			// 	space += files[randi].textSize;
			// }
			printf("%d\n", space);
			if(testerNum >= space) {
				for(int outText = 0; outText < totalSize; outText++) {
					if(outText != textNum) {
						offsettoAdd += files[outText].textSize;
					}
				}

				for(int outData = 0; outData < textNum; outData++) {
					offsettoAdd += files[outData].dataSize;
				}
			} else {
				for(int outText = 0; outText < textNum; outText++) {
					offsettoAdd += files[outText].textSize;
				}
			}
			
		}
		printf("%d\n", offsettoAdd);
		return offsettoAdd;
	}



	int dataOffset(FileData *files, int dataNum, int lineNum, int totalSize) {
		int offsettoAdd = 0;
		printf("data in work\n");
		printf("%s\n", files[dataNum].relocTable[lineNum].label);
		if(!strcmp(files[dataNum].relocTable[lineNum].label, "Stack")) {
			for(int outText = 0; outText < totalSize; outText++) {
				offsettoAdd += files[outText].textSize;
			}
			for(int outData = 0; outData < totalSize; outData++) {
				offsettoAdd += files[outData].dataSize;
			}
		} else if(isupper(files[dataNum].relocTable[lineNum].label[0])) {
			bool undefLab = false;
			int saveK = 0;
			printf("work\n");
			for(int k = 0; k < files[dataNum].symbolTableSize; k++) {
				if(!strcmp(files[dataNum].symbolTable[k].label, files[dataNum].relocTable[lineNum].label)) {
					//printf("%s\n", files[dataNum].symbolTable[k].label);
					saveK = k;
					
					if(files[dataNum].symbolTable[k].location == 'U') {
						undefLab = true;
					}
					
				}
			}
			if(undefLab) {
				for(int symfilNum = 0; symfilNum < totalSize; symfilNum++) {
					for(int symIndex = 0; symIndex < files[symfilNum].symbolTableSize; symIndex++) {
						
						// printf("%s\n", files[symfilNum].relocTable[symIndex].label);
						// printf("%s\n", &files[symfilNum].symbolTable[symIndex].location);
						if(!strcmp(files[dataNum].relocTable[lineNum].label, files[symfilNum].symbolTable[symIndex].label) && (files[symfilNum].symbolTable[symIndex].location == 'T')) {
							for(int outText = 0; outText < symfilNum; outText++) {
								offsettoAdd += files[outText].textSize;
							}
							offsettoAdd += files[symfilNum].symbolTable[symIndex].offset;
						} else if(!strcmp(files[dataNum].relocTable[lineNum].label, files[symfilNum].symbolTable[symIndex].label) && (files[symfilNum].symbolTable[symIndex].location == 'D')) {
							printf("work1\n");
							for(int outText = 0; outText < totalSize; outText++) {	
								offsettoAdd += files[outText].textSize;
							}
							printf("%d\n", offsettoAdd);
							for(int outData = 0; outData < symfilNum; outData++) {
								offsettoAdd += files[outData].dataSize;
							}
							printf("%d\n", offsettoAdd);
							offsettoAdd += files[symfilNum].symbolTable[symIndex].offset;
						}
					}
				}
			} else {
				if(files[dataNum].symbolTable[saveK].location == 'T') {
					for(int outText = 0; outText < dataNum; outText++) {	
						offsettoAdd += files[outText].textSize;
					}
				} else {
					for(int outText = 0; outText < totalSize; outText++) {
						if(outText != dataNum) {
							offsettoAdd += files[outText].textSize;
						}
					}
					for(int outData = 0; outData < dataNum; outData++) {
						offsettoAdd += files[outData].dataSize;
					}
				}
				
			}
			
		} else {
			//its data local
			printf("look at the data now\n");
			printf("%s\n", files[dataNum].relocTable[lineNum].label);
			printf("%d\n", files[dataNum].data[files[dataNum].relocTable[lineNum].offset]);
			printf("%d\n", files[dataNum].textSize);
			if(files[dataNum].data[files[dataNum].relocTable[lineNum].offset] < files[dataNum].textSize) {
				for(int outText = 0; outText < dataNum; outText++) {	
					offsettoAdd += files[outText].textSize;
				}
			} else {
				for(int outText = 0; outText < totalSize; outText++) {
					if(outText != dataNum) {
						offsettoAdd += files[outText].textSize;
					}
				}
				for(int outData = 0; outData < dataNum; outData++) {
					offsettoAdd += files[outData].dataSize;
				}
			}
			
		}
		printf("%d\n", offsettoAdd);
		return offsettoAdd;
	}

	
