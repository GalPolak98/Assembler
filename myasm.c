#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<stdlib.h>
#include<string.h>

void changeOutputName(char* name, char* hack);
void Error_Msg(char* str);
void assemblyToBinary(FILE* in, FILE* out);
void A_Bin_Print(unsigned int x, FILE* out);
void C_Bin_Print(char* line, FILE* out);
void removeGarbage(char* str);

/*Main function, the purpose of the code is to move from an assembly to the appropriate binary code of each command*/
int main()
{
	char name[20],*asmP, hack_name[21];
	FILE* in, * out;
	printf("Please Enter assembly file name: ");
	scanf("%s", name);
	if ((in = fopen(name, "r")) == NULL)
		Error_Msg("The input file is wrong\n");
	changeOutputName(name,hack_name); /*Name change from "NAME.asm" to "Name.hack"*/
	if ((out = fopen(hack_name, "w")) == NULL)
		Error_Msg("The output file is wrong\n");
	assemblyToBinary(in,out);
	printf("\nOutput file %s is now available.\n",hack_name);
	fclose(out);
	fclose(in);
	return 0;
}

/*changes name for the output file name will be .hack at the end instead of .asm*/
void changeOutputName(char* name, char* hack)
{
	char  * asmP;
	strcpy(hack, name);
	asmP = strstr(hack, ".asm");
	strcpy(asmP, ".hack");
}

/*Prints an error message when opening the file and exits the program.*/
void Error_Msg(char* str)
{
	printf("\n%s", str);
	exit(1);
}

/*A function that checks the type of instruction and sends it to a function with the appropriate type that is converted to binary*/
void assemblyToBinary(FILE* in, FILE* out)
{
	int i, num;
	char temp_str[300]; 
	while (fgets(temp_str, 300, in) != NULL)/*receives lines of the instructions until the end of the file*/
	{
		removeGarbage(temp_str);
		if (temp_str[0] == '(' || temp_str[0]=='\0');
		else if (temp_str[0] == '@') /*A-Instruction*/
		{
			if ('0' <= temp_str[1] && temp_str[1] <= '9') {
				for (i = 0; i < strlen(temp_str) - 1; i++) /*remove '@' from the string*/
					temp_str[i] = temp_str[i + 1];
				temp_str[strlen(temp_str) - 1] = '\0';
				num = atoi(temp_str);
				A_Bin_Print(num, out);
			}
		}
		else /*C-Instruction*/
			C_Bin_Print(temp_str, out);
	}
}

/*A-Instruction, the function prints the number in binary form to output file*/
void A_Bin_Print(unsigned int x,FILE *out)
{
	unsigned int mask = 1, temp = x, res;
	int n = 16, i;
	mask = mask << n - 1;
	for (i = 0; i < n; i++) /*Checks if the bits are 0/1 and prints respectively*/
	{
		res = temp & mask;
		fprintf(out,"%d", res != 0);
		mask = mask >> 1;
	}
	fprintf(out,"\n");
}

/*C-Instruction, the function prints the required command in binary form to output file */
void C_Bin_Print(char* line, FILE *out)
{
	/*Initializes the comp, dest, jump and their binary code*/
	char* comp_str[] = { "0","1","-1","D","A","!D","!A","-D","-A","D+1","A+1","D-1","A-1","D+A","D-A","A-D","D&A","D|A",
    "M","!M","-M","M+1","M-1","D+M","D-M","M-D","D&M","D|M" };
	char* comp_binary[] = { "0101010","0111111","0111010","0001100","0110000","0001101","0110001","0001111","0110011","0011111",
	"0110111","0001110","0110010","0000010","0010011","0000111","0000000","0010101","1110000","1110001","1110011","1110111",
	"1110010","1000010","1010011","1000111","1000000","1010101" };
	char* dest_str[] = { "null","M","D","DM","A","AM","AD","AMD" };
	char* dest_binary[] = { "000","001","010","011","100","101","110","111" };
	char* jump_str[] = { "null","JGT","JEQ","JGE","JLT","JNE","JLE","JMP" };
	char* jump_binary[] = { "000","001","010","011","100","101","110","111" };
	char temp_comp_bin[8], temp_dest_bin[4]= { '\0' }, temp_jump_bin[4] = { '\0' }, temp_st[300], * eqlP, * semicolonP, temp_line[300];
	int j;
	strcpy(temp_line, line);
	semicolonP = strstr(temp_line, ";");
	if (semicolonP != NULL)
	{
		strcpy(temp_st, semicolonP);
		/*Removes the character ';' from the temporary string and a required jump command is obtained */
		for (j = 0; j < strlen(temp_st) - 1; j++) 
		{
			temp_st[j] = temp_st[j + 1];
		}
		temp_st[strlen(temp_st) - 1] = '\0';
		for (j = 0; j < 8; j++) /*Looking for the appropriate jump command*/
		{
			if (strcmp(jump_str[j], temp_st) == 0)
				strcpy(temp_jump_bin, jump_binary[j]);
		}
		j = 0;
		while (j < strlen(temp_st)) /*Temporary array initialization*/
		{
			temp_st[j] = '\0';
			j++;
		}
		strcpy(semicolonP, ""); /*removes the part after ';' */
	}
	eqlP = strstr(temp_line, "=");
	if (eqlP == NULL)
	{
		strcpy(temp_dest_bin, dest_binary[0]); /*No character exists '=' */
		for (j = 0; j < 28; j++)/*Looking for the appropriate comp command*/
		{
			if (strcmp(comp_str[j], temp_line) == 0)
				strcpy(temp_comp_bin, comp_binary[j]);
		}
	}
	else
	{
		strcpy(temp_st, eqlP);
		/*Removes the character '=' from the temporary string*/
		for (j = 0; j < strlen(temp_st) - 1; j++)
		{
			temp_st[j] = temp_st[j + 1];
		}
		temp_st[strlen(temp_st) - 1] = '\0';
		for (j = 0; j < 28; j++) /*Looking for the appropriate comp command*/
		{
			if (strcmp(comp_str[j], temp_st) == 0)
				strcpy(temp_comp_bin, comp_binary[j]);
		}	
		strcpy(eqlP,""); /*Leaves a string with the left part of a character '=' */
		for (j = 0; j < 8; j++) /*Looking for the appropriate dest command*/
		{
			if (strcmp(dest_str[j], temp_line) == 0)
				strcpy(temp_dest_bin, dest_binary[j]);
		}
	}
	if (temp_dest_bin[0]=='\0')
		strcpy(temp_dest_bin, dest_binary[0]);
	if (temp_jump_bin[0] == '\0')
		strcpy(temp_jump_bin, jump_binary[0]);
	/*Writes the appropriate binary code into the output file*/
	fprintf(out, "111");
	fprintf(out, "%s", temp_comp_bin);
	fprintf(out, "%s", temp_dest_bin);
	fprintf(out, "%s", temp_jump_bin);
	fprintf(out, "\n");
}

/*remove all space, remarks,';',tabs and '\n' from the string */
void removeGarbage(char* str)
{
	int i,j=0;
	char strWithoutGarbage[300],*PRemark;
	if (str[strlen(str) - 1] == '\n')
		str[strlen(str) - 1] = '\0';
	PRemark = strstr(str, "//");
	if (PRemark != NULL)
		strcpy(PRemark, "");
	for (i = 0; i < strlen(str); i++) /*remove all space and tabs from the string*/
	{
		if (str[i] != ' ' &&  str[i] != '\t')
		{
			strWithoutGarbage[j] = str[i];
			j++;
		}
	}
	strWithoutGarbage[j] = '\0';
	if (strWithoutGarbage[strlen(strWithoutGarbage) - 1] == ';')
		strWithoutGarbage[strlen(strWithoutGarbage) - 1] = '\0';
		strcpy(str, strWithoutGarbage);
}