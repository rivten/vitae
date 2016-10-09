#pragma once

void CatStrings(size_t SourceACount, char* SourceA,
				size_t SourceBCount, char* SourceB,
				size_t DestCount, char* Dest)
{
	for(int Index = 0; Index < SourceACount; ++Index)
	{
		*Dest = *SourceA;
		++Dest;
		++SourceA;
	}
	for(int Index = 0; Index < SourceBCount; ++Index)
	{
		*Dest = *SourceB;
		++Dest;
		++SourceB;
	}

	*Dest = 0;
	++Dest;
}

void AppendString(size_t SourceCount, char* Source,
		size_t DestCount, char* Dest)
{
	CatStrings(SourceCount, Source, 0, 0, DestCount, Dest);
}

int StringLength(char* String)
{
	int Count = 0;
	while(*String)
	{
		++Count;
		++String;
	}

	return(Count);
}

void WriteToDest(char* String, char** Dest)
{
	if(String)
	{
		size_t Length = StringLength(String);
		AppendString(Length, String, 0, *Dest);
		*Dest += Length;
	}
}


