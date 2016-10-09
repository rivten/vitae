#pragma once

char* ReadStringToken(jsmntok_t Token, char* JsonContent)
{
	Assert(Token.type == JSMN_STRING);
	char* Result = (char*)malloc(Token.end - Token.start + 1);
	strncpy(Result, JsonContent + Token.start, Token.end - Token.start);
	Result[Token.end - Token.start] = '\0';

	return(Result);
}

vitae_entry* ReadVitaeEntry(int* TokenIndex, jsmntok_t* Tokens, char* JsonContent)
{
	vitae_entry* Result = (vitae_entry*) malloc(sizeof(vitae_entry));
	*Result = {};

	for(int EntryData = 0; EntryData < 5; ++EntryData)
	{
		char* Type = ReadStringToken(Tokens[*TokenIndex], JsonContent);
		if(strcmp(Type, "date") == 0)
		{
			++*TokenIndex;
			Assert(Tokens[*TokenIndex].type == JSMN_STRING);
			Result->Date = ReadStringToken(Tokens[*TokenIndex], JsonContent);
		}
		else if(strcmp(Type, "diploma") == 0 || strcmp(Type, "role") == 0)
		{
			++*TokenIndex;
			Assert(Tokens[*TokenIndex].type == JSMN_STRING);
			Result->Role = ReadStringToken(Tokens[*TokenIndex], JsonContent);
		}
		else if(strcmp(Type, "school") == 0 || strcmp(Type, "company") == 0)
		{
			++*TokenIndex;
			Assert(Tokens[*TokenIndex].type == JSMN_STRING);
			Result->Company = ReadStringToken(Tokens[*TokenIndex], JsonContent);
		}
		else if(strcmp(Type, "city") == 0)
		{
			++*TokenIndex;
			Assert(Tokens[*TokenIndex].type == JSMN_STRING);
			Result->City = ReadStringToken(Tokens[*TokenIndex], JsonContent);
		}
		else if(strcmp(Type, "desc") == 0)
		{
			++*TokenIndex;

			if(Tokens[*TokenIndex].type == JSMN_STRING)
			{
				Result->Desc = ReadStringToken(Tokens[*TokenIndex], JsonContent);
			}
			else if(Tokens[*TokenIndex].type == JSMN_ARRAY)
			{
				int ItemCount = Tokens[*TokenIndex].size;
#define MAX_DESC_SIZE 10000
				Result->Desc = (char*) malloc(sizeof(char) * MAX_DESC_SIZE);
				char* CurrentWritingPoint = Result->Desc;
				WriteToDest("\\begin{itemize}\n", &CurrentWritingPoint);

				for(int ItemIndex = 0; ItemIndex < ItemCount; ++ItemIndex)
				{
					*TokenIndex += 2;
					WriteToDest("\\item ", &CurrentWritingPoint);
					WriteToDest(ReadStringToken(Tokens[*TokenIndex], JsonContent), &CurrentWritingPoint);
				}
				WriteToDest("\\end{itemize}\n", &CurrentWritingPoint);
			}
			else
			{
				InvalidCodePath;
			}
		}
		else
		{
			InvalidCodePath;
		}
		++*TokenIndex;
	}
	
	// TODO(hugo) : A little bit cheesy. Find out a way to better follow the index
	--*TokenIndex;

	return(Result);
}

void ReadEntryList(int* TokenIndex, jsmntok_t* Tokens, char* JsonContent, vitae_entry** EntryList)
{
	++*TokenIndex;
	jsmntok_t EntryListToken = Tokens[*TokenIndex];
	Assert(EntryListToken.type == JSMN_ARRAY);
	int EntryCount = EntryListToken.size;
	for(int EntryIndex = 0; EntryIndex < EntryCount; ++EntryIndex)
	{
		++*TokenIndex;
		jsmntok_t EntryDesc = Tokens[*TokenIndex];
		Assert(EntryDesc.type == JSMN_STRING);
		++*TokenIndex;
		jsmntok_t EntryObject = Tokens[*TokenIndex];
		Assert(EntryObject.type == JSMN_OBJECT);
		Assert(EntryObject.size == 5);
		++*TokenIndex;
		vitae_entry* Entry = ReadVitaeEntry(TokenIndex, Tokens, JsonContent);
		AddEntry(EntryList, Entry);
	}
}

// TODO(hugo) : Review memory management
vitae_data CreateVitaeData(char* JsonContent, size_t JsonSize, jsmntok_t* Tokens, int TokensCount)
{
	vitae_data Result = {};
	// NOTE(hugo) : No need to read the first token since this is an object
	// representing the whole json file
	for(int TokenIndex = 1; TokenIndex < TokensCount; ++TokenIndex)
	{
		jsmntok_t Token = Tokens[TokenIndex];
		switch(Token.type)
		{
			case JSMN_STRING:
				{
					char* Type = ReadStringToken(Token, JsonContent);
					if(strcmp(Type, "author") == 0)
					{
						++TokenIndex;
						jsmntok_t ObjectToken = Tokens[TokenIndex];
						Assert(ObjectToken.type == JSMN_OBJECT);
						Assert(ObjectToken.size == 2);

						for(int TokenArrayIndex = 0; TokenArrayIndex < ObjectToken.size; ++TokenArrayIndex)
						{
							++TokenIndex;
							jsmntok_t AuthorToken = Tokens[TokenIndex];
							Assert(AuthorToken.type == JSMN_STRING);
							char* AuthorType = ReadStringToken(AuthorToken, JsonContent);
							if(strcmp(AuthorType, "first_name") == 0)
							{
								++TokenIndex;
								Assert(Tokens[TokenIndex].type == JSMN_STRING);
								Result.AuthorFirstName = ReadStringToken(Tokens[TokenIndex], JsonContent);
							}
							else if(strcmp(AuthorType, "last_name") == 0)
							{
								++TokenIndex;
								Assert(Tokens[TokenIndex].type == JSMN_STRING);
								Result.AuthorLastName = ReadStringToken(Tokens[TokenIndex], JsonContent);
							}
						}
					}
					else if(strcmp(Type, "title") == 0)
					{
						++TokenIndex;
						Result.Title = ReadStringToken(Tokens[TokenIndex], JsonContent);
					}
					else if(strcmp(Type, "mobile") == 0)
					{
						++TokenIndex;
						Result.AuthorPhone = ReadStringToken(Tokens[TokenIndex], JsonContent);
					}
					else if(strcmp(Type, "email") == 0)
					{
						++TokenIndex;
						Result.AuthorMail = ReadStringToken(Tokens[TokenIndex], JsonContent);
					}
					else if(strcmp(Type, "address") == 0)
					{
						++TokenIndex;
						Result.AuthorAddress = ReadStringToken(Tokens[TokenIndex], JsonContent);
					}
					else if(strcmp(Type, "education") == 0)
					{
						ReadEntryList(&TokenIndex, Tokens, JsonContent, &Result.EducationEntries);
					}
					else if(strcmp(Type, "work") == 0)
					{
						ReadEntryList(&TokenIndex, Tokens, JsonContent, &Result.WorkExpEntries);
					}
					else if(strcmp(Type, "misc") == 0)
					{
						ReadEntryList(&TokenIndex, Tokens, JsonContent, &Result.MiscEntries);
					}
					else if(strcmp(Type, "projects") == 0)
					{
						ReadEntryList(&TokenIndex, Tokens, JsonContent, &Result.ProjectsEntries);
					}
					else if(strcmp(Type, "basic_skills") == 0)
					{
						++TokenIndex;
						Result.BasicSkills = ReadStringToken(Tokens[TokenIndex], JsonContent);
					}
					else if(strcmp(Type, "intermediate_skills") == 0)
					{
						++TokenIndex;
						Result.IntermediateSkills = ReadStringToken(Tokens[TokenIndex], JsonContent);
					}
					else if(strcmp(Type, "advanced_skills") == 0)
					{
						++TokenIndex;
						Result.AdvancedSkills = ReadStringToken(Tokens[TokenIndex], JsonContent);
					}
					else if(strcmp(Type, "languages") == 0)
					{
						++TokenIndex;
						Result.Languages = ReadStringToken(Tokens[TokenIndex], JsonContent);
					}
					else if(strcmp(Type, "hobbies") == 0)
					{
						++TokenIndex;
						Result.Hobbies = ReadStringToken(Tokens[TokenIndex], JsonContent);
					}
					else
					{
						InvalidCodePath;
					}
				} break;
			case JSMN_UNDEFINED:
				{
				} break;
			InvalidDefaultCase;
		}

	}

	return(Result);
}
