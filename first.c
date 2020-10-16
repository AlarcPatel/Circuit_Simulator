#define INPUT 0
#define OUTPUT 1
#define MAXVARLEN 10
#define NUM_GATES 6

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct Operation {
	int opNNum;

	int curNumInpts;
	int curNumOpts;
	int totalNumInpts;
	int totalNumOpts;

	int* inputs;
	int* outputs;
};

struct Variable {
	char* varName;
	int data;
	int type;
};

char GateWords[NUM_GATES][5] = { "NOT", "AND", "OR", "NAND", "NOR", "XOR" };
struct Variable* variables;
int curNumVars;
int maxVarsLen;

void CopyStr(char** a, char b[MAXVARLEN]);
int ISNUMBER(char* a, int strLen);
void BuildOp(char* str, struct Operation** operations, int* curNumOps, int* totalNumOps);
void OperationFiller(struct Operation* operation, char* curVar, int inptOpt, int varNameLen);
void GateLog(int opNum, int* inpts, int* opts, int sInpt, int sOpt);
void Operate(struct Operation* operations, int curNumOps);
int NumInptsToOutPut(char* gate);
int Pow(int a, int b);

int main(int argc, char** argv) {

	if (argc == 2) {
		char* fName = argv[1];
		FILE* fp = (FILE*)fopen(fName, "r");

		int curNumOps = 0;
		int totalNumOps = 10;
		struct Operation* operations = (struct Operation*) calloc(totalNumOps, sizeof(struct Operation));

		curNumVars = 0;
		maxVarsLen = 100;
		variables = (struct Variable*)calloc(maxVarsLen, sizeof(struct Variable));

		int lineNum = 0;
		int c = 0;
		
		int keyWorded = 0;
		char keyWord[MAXVARLEN];
		
		int kwF = 0;
		
		char curVar[MAXVARLEN];
		//int numbered = 0;

		int lastInput = 0;
		int lastOutput = 0;

		for (kwF = 0; kwF < MAXVARLEN; kwF++) {
			keyWord[kwF] = '\0';
			curVar[kwF] = '\0';
		}
		kwF = 0;
		while ((c = fgetc(fp)) != EOF) {
			if (keyWorded == 0 && c != '\n' && c != ' ') {
				keyWord[kwF] = c;
				kwF++;
				while ((c = fgetc(fp)) != '\n' && c != ' ') {
					keyWord[kwF] = c;
					kwF++;
				}
				keyWorded = 1;
				kwF = 0;
				//printf("%s", keyWord);
			}
			else {

				if (keyWorded == 1) {
					if (lineNum < 2) {
						while ((c = fgetc(fp)) != '\n' && c != EOF) {
							int cV = 0;
							if (c != ' ') {
								curVar[cV] = c;
								cV++;
							}
							while ((c = fgetc(fp)) != ' ' && c != EOF && c != '\n') {
								curVar[cV] = c;
								cV++;
							}

							if (ISNUMBER(curVar, cV) == 0) {
								//printf("%s ", curVar);
								if (curNumVars >= maxVarsLen) {
									variables = (struct Variable*) realloc(variables, maxVarsLen + 20);
									maxVarsLen = maxVarsLen + 20;
								}
								CopyStr(&(variables[curNumVars].varName), curVar);
								variables[curNumVars].data = 0;
								if (lineNum == 0) {
									variables[curNumVars].type = INPUT;
									lastInput = curNumVars;
								}
								else {
									variables[curNumVars].type = OUTPUT;
									lastOutput = curNumVars;
								}
								curNumVars++;
							}
							memset(curVar, '\0', MAXVARLEN);
							if (c == EOF || c == '\n') {
								break;
							}

						}
					}
					else {
						int mode = INPUT;
						int numToChangeMode = NumInptsToOutPut(keyWord);
						int numinputted = 0;
						int cV = 0;
						if (c != ' ') {
							curVar[cV] = c;
							cV++;
						}
						BuildOp(keyWord, &operations, &curNumOps, &totalNumOps);

						while ((c = fgetc(fp)) != EOF) {
							if (c != ' ' && c != '\n') {
								curVar[cV] = c;
								cV++;
							}
							else {
								//printf(" %s", curVar);

								numinputted++;
								OperationFiller(&operations[curNumOps - 1], curVar, mode, cV);
								if (numinputted == numToChangeMode) {
									mode = OUTPUT;
								}
								cV = 0;
								memset(curVar, '\0', MAXVARLEN);
								if (c == '\n') {
									break;
								}
							}
						}

					}
					keyWorded = 0;
					lineNum++;
					memset(keyWord, '\0', MAXVARLEN);
					//printf("finished line\n");
					//printf("Completed line: %d\n", lineNum);
				}
			}
		}

		int init = 0;
		int inptC = 0;
		for (init = 0; init < curNumVars; init++) {
			if (variables[init].type == INPUT) {
				inptC++;
			}
			variables[init].data = 0;
		}
		int h = 0;
		int v = 0;
		int powTwo = Pow(2, lastInput + 1);
		//printf("%d\n", powTwo);
		//printf("%d\n", lastInput);
		for (v = 0; v < powTwo; v++) {
			//int fx = 0;
			int i = lastInput;
			int n = v;
			while (n > 0) {
				variables[i].data = n % 2;
				//printf("%d\n", i);
				n = n / 2;
				i--;
			}
			Operate(operations, curNumOps);
			for (h = 0; h <= lastOutput; h++) {
				if (h < lastOutput) {
					printf("%d ", variables[h].data);
				}
				else {
					printf("%d", variables[h].data);
					//printf("%s", variables[h].varName);
				}
			}
			printf("\n");
		}

	}

}




void BuildOp(char* str, struct Operation** operations, int* curNumOps, int* totalNumOps) {

	int i = 0;
	for (i = 0; i < NUM_GATES; i++)
	{
		if (strcmp(GateWords[i], str) == 0) {
			if (*curNumOps >= *totalNumOps) {
				(*operations) = (struct Operation*)realloc(*operations, sizeof(struct Operation) * *totalNumOps * 2);
				*totalNumOps = *totalNumOps * 2;
			}
			((*operations)[*curNumOps]).opNNum = i;

			((*operations)[*curNumOps]).totalNumInpts = 5;
			((*operations)[*curNumOps]).inputs = (int*)calloc(5, sizeof(int));

			((*operations)[*curNumOps]).totalNumOpts = 5;
			((*operations)[*curNumOps]).outputs = (int*)calloc(5, sizeof(int));

			((*operations)[*curNumOps]).curNumInpts = 0;
			((*operations)[*curNumOps]).curNumOpts = 0;

			*curNumOps = *curNumOps + 1;
			break;
		}
	}
}

void OperationFiller(struct Operation* operation, char* curVar, int inptOpt, int varNameLen) {
	int i = 0;
	int ex = 0;
	for (i = 0; i < curNumVars; i++)
	{
		if (strcmp((variables[i]).varName, curVar) == 0) {
			ex = 1;
			break;
		}
	}

	//NOTE THIS WORKS BECAUSE THE VARIABLE 'i' is INCREMENTED AT THE END OF THE FOR LOOP IF IT FINISHES FULLY WITHOUT FINDING THE VARIABLE
	if (ex == 0) {
		if (curNumVars >= maxVarsLen) {
			variables = (struct Variable*)realloc(variables, sizeof(struct Variable) * maxVarsLen * 2);
			maxVarsLen = maxVarsLen * 2;
		}
		variables[curNumVars].varName = (char*)calloc(MAXVARLEN, sizeof(char));
		CopyStr(&(variables[curNumVars].varName), curVar);
		variables[curNumVars].type = inptOpt;
		variables[curNumVars].data = 0;
		curNumVars++;
	}

	if (inptOpt == INPUT) {
		if ((*operation).curNumInpts >= (*operation).totalNumInpts) {
			(*operation).inputs = (int*)realloc((*operation).inputs, sizeof(int) * 2 * (*operation).totalNumInpts);
			(*operation).totalNumInpts = (*operation).totalNumInpts * 2;
		}
		((*operation).inputs)[(*operation).curNumInpts] = i;
		(*operation).curNumInpts++;
		//printf("CUR OPER: %s\tCUR NUM INPTS: %d\n",GateWords[(*operation).opNum] , (*operation).curNumInpts);
	}
	else if (inptOpt == OUTPUT) {
		if ((*operation).curNumOpts >= (*operation).totalNumOpts) {
			(*operation).outputs = (int*)realloc((*operation).outputs, sizeof(int) * 2 * (*operation).totalNumOpts);
			(*operation).totalNumOpts = (*operation).totalNumOpts * 2;
		}
		((*operation).outputs)[(*operation).curNumOpts] = i;
		(*operation).curNumOpts++;
		//printf("CUR OPER: %s\tCUR NUM OPTS: %d\n", GateWords[(*operation).opNum], (*operation).curNumOpts);
	}
}

void Operate(struct Operation* operations, int curNumOps) {

	int i = 0;
	for (i = 0; i < curNumOps; i++) {
		GateLog(operations[i].opNNum, operations[i].inputs, operations[i].outputs, operations[i].curNumInpts, operations[i].curNumOpts);
	}
}

void GateLog(int opNum, int* inpts, int* opts, int sInpt, int sOpt) {
	int i = 0;
	//NOT
	if (opNum == 0) {
		//printf("NOTTING\n");
		for (i = 0; i < sInpt; i++) {
			if (variables[inpts[i]].data == 0) {
				variables[opts[0]].data = 1;
			}
			else {
				variables[opts[0]].data = 0;
			}
		}
	}
	//AND
	else if (opNum == 1) {
		//printf("AND\t");
		int a = 0;
		for (i = 0; i < sInpt; i++) {
			if (variables[inpts[i]].data == 0) {
				variables[opts[0]].data = 0;
				a = 1;
				break;
			}
		}
		if (a == 0) {
			variables[opts[0]].data = 1;
		}
		//printf("AND\n");
	}
	//OR
	else if (opNum == 2) {
		//printf("OR\t");
		//printf("ORRING\n");
		for (i = 0; i < sInpt; i++) {
			if (variables[inpts[i]].data == 1) {
				variables[(opts)[0]].data = 1;
				break;
			}
			else {
				variables[(opts)[0]].data = 0;
			}
		}
		//printf("OR\n");
	}
	//NAND
	else if (opNum == 3) {
		int k = 1;
		for (i = 0; i < sInpt; i++) {
			if (variables[inpts[i]].data == 0) {
				k = 0;
				variables[(opts)[0]].data = 1;
				break;
			}
		}
		if (k == 1) {
			variables[(opts)[0]].data = 0;
		}
	}
	//NOR
	else if (opNum == 4) {
		int k = 0;
		for (i = 0; i < sInpt; i++) {
			if (variables[inpts[i]].data == 1) {
				k = 1;
				break;
			}
		}
		variables[(opts)[0]].data = 0;
		if (k == 0) {
			variables[(opts)[0]].data = 1;
		}
	}
	//XOR
	else if (opNum == 5) {
		variables[opts[0]].data = 0;

		for (i = 0; i < sInpt; i++) {
			//printf("inpt value: %d\n", variables[inpts[i]].data);
			if (variables[opts[0]].data == 0 && variables[inpts[i]].data == 1) {
				variables[opts[0]].data = 1;
			}
			else if (variables[opts[0]].data == 1 && variables[inpts[i]].data == 1) {
				variables[opts[0]].data = 0;
				break;
			}
		}
		//printf("Outpit: %d\n", variables[opts[0]].data);
	}
}

int NumInptsToOutPut(char* gate) {
	if (strcmp(gate, GateWords[0]) == 0) {
		return 1;
	}
	else {
		return 2;
	}
}

int Pow(int a, int b) {
	if (b > 0) {
		int i = 0;
		int r = 1;
		for (i = 1; i <= b; i++) {
			r = r * a;
		}
		return r;
	}
	else {
		return 1;
	}
}

int ISNUMBER(char* a, int strLen) {
	int i = 0;
	for (i = 0; i < strLen; i++) {
		if (isdigit(a[i]) == 0) {
			return 0;
		}
	}
	return 1;
}

void CopyStr(char** a, char b[MAXVARLEN]) {
	char* temp = (char*)calloc(MAXVARLEN, sizeof(char));

	int i = 0;
	for (i = 0; i < MAXVARLEN; i++) {
		temp[i] = b[i];
	}
	free(*a);
	*a = temp;
}
