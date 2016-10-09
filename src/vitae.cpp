#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsmn.c"

#define ArrayCount(x) (sizeof((x))/(sizeof((x)[0])))
#define Assert(x) do{if(!(x)){*(int*)0=0;}}while(0)
#define InvalidCodePath Assert(!"InvalidCodePath");
#define InvalidDefaultCase default: {InvalidCodePath;} break

#define MAX_TOKEN_COUNT 1000

/*
 * TODO(hugo)
 *   - input file error display (show errors like "The author name is not given")
 *   - check memory management (freeing memory correctly ?)
 *   - enter forgotten input directly from command prompt ??
 *   - let the users chose options (file dir, color, etc...) directly from command line
 *   - more flexible interface (the user can define his own entry for example)
 *   - beware of accents (the user should not be thinking about it, so maybe post-process the tex file before compiling it)
 *   - GUI to input data
 */

// TODO(hugo) : add a union to take into account projects and education entry ?
struct vitae_entry
{
	char* Date;
	char* Role;
	char* Company;
	char* City;
	char* Desc;

	vitae_entry* Next;
};

struct vitae_data
{
	char* AuthorFirstName;
	char* AuthorLastName;
	char* Title;
	char* AuthorPhone;
	char* AuthorMail;
	char* AuthorAddress;
	char* Website;
	vitae_entry* EducationEntries;
	vitae_entry* WorkExpEntries;
	vitae_entry* MiscEntries;
	vitae_entry* ProjectsEntries;
	char* BasicSkills;
	char* IntermediateSkills;
	char* AdvancedSkills;
	char* Languages;
	char* Hobbies;
};

#include "rivten_string.h"

void AddEntry(vitae_entry** EntryList, vitae_entry* Entry)
{
	Assert(Entry);
	if(!(*EntryList))
	{
		*EntryList = Entry;
	}
	else
	{
		AddEntry(&((*EntryList)->Next), Entry);
	}
}

#include "vitae_json_parser.h"

struct file_content
{
	int Size;
	char* Content;
};

file_content GetFileContent(char* Filename)
{
	file_content Result;
	FILE* File = 0;
	fopen_s(&File, Filename, "rb");
	Assert(File != 0);
	fseek(File, 0, SEEK_END);
	Result.Size = ftell(File);
	fseek(File, 0, SEEK_SET);

	// TODO(hugo) : Check that the malloc succedeed
	Result.Content = (char*) malloc(Result.Size + 1);
	fread(Result.Content, Result.Size, 1, File);
	fclose(File);
	Result.Content[Result.Size] = 0;

	return(Result);
}

#include "vitae_pdf_creator.h"

int main(int ArgumentCount, char** Arguments)
{
	char* Filename = 0;
	if(ArgumentCount > 1)
	{
		Filename = Arguments[ArgumentCount - 1];
	}
	else
	{
		// TODO(hugo) : Log error and display help
		InvalidCodePath;
	}
	file_content JsonFileContent = GetFileContent(Filename);

	jsmn_parser Parser;
	jsmntok_t Tokens[MAX_TOKEN_COUNT];

	jsmn_init(&Parser);

	int TokenCount = jsmn_parse(&Parser, JsonFileContent.Content, JsonFileContent.Size + 1, Tokens, ArrayCount(Tokens));

	vitae_data Data = CreateVitaeData(JsonFileContent.Content, JsonFileContent.Size + 1, Tokens, TokenCount);

	file_content FileLatexContent = CreateLatexFileContent(Data);

	FILE* OutputLatexFile = fopen("output.tex", "w");
	Assert(OutputLatexFile != 0);
	fwrite(FileLatexContent.Content, sizeof(char), (size_t)FileLatexContent.Size, OutputLatexFile);
	fclose(OutputLatexFile);

	// TODO(hugo) : Get rid of system and use directly windows for that ?
	system("pdflatex -quiet -interaction=nonstopmode output.tex");
	system("call output.pdf");

	// TODO(hugo) : Delete temp tex file

	return(0);
}
