#pragma once

void WriteVitaeEntryList(vitae_entry* EntryList, char** Dest)
{
	while(EntryList)
	{
		WriteToDest("\n\\cventry{", Dest);
		WriteToDest(EntryList->Date, Dest);
		WriteToDest("}{", Dest);
		WriteToDest(EntryList->Role, Dest);
		WriteToDest("}{", Dest);
		WriteToDest(EntryList->Company, Dest);
		WriteToDest("}{", Dest);
		WriteToDest(EntryList->City, Dest);
		WriteToDest("}{}{", Dest);
		WriteToDest(EntryList->Desc, Dest);
		WriteToDest("}", Dest);

		EntryList = EntryList->Next;
	}
}

// TODO(hugo) : Check bounds and fail if not enough
#define MAX_FILE_SIZE 1000000
file_content CreateLatexFileContent(vitae_data Data)
{
	file_content TexFileContent = GetFileContent("header.tex");
	file_content Result = {};
	Result.Content = (char*) malloc(sizeof(char) * MAX_FILE_SIZE);
	memset(Result.Content, 0, StringLength(Result.Content));
	char* CurrentWritingPoint = Result.Content;

	// TODO(hugo) : Find a way not to hardcode everything ?
	WriteToDest(TexFileContent.Content, &CurrentWritingPoint);

	WriteToDest("\n\\firstname{", &CurrentWritingPoint);
	WriteToDest(Data.AuthorFirstName, &CurrentWritingPoint);

	WriteToDest("}\n\\familyname{", &CurrentWritingPoint);
	WriteToDest(Data.AuthorLastName, &CurrentWritingPoint);

	WriteToDest("}\n\\title{", &CurrentWritingPoint);
	WriteToDest(Data.Title, &CurrentWritingPoint);

	WriteToDest("}\n\\address{", &CurrentWritingPoint);
	WriteToDest(Data.AuthorAddress, &CurrentWritingPoint);

	WriteToDest("}\n\\mobile{", &CurrentWritingPoint);
	WriteToDest(Data.AuthorPhone, &CurrentWritingPoint);

	WriteToDest("}\n\\email{", &CurrentWritingPoint);
	WriteToDest(Data.AuthorMail, &CurrentWritingPoint);

	WriteToDest("}\n\\begin{document}\n\\makecvtitle\n\\vspace*{-1.2cm}\n\n\\section{Education}\n", &CurrentWritingPoint);
	WriteVitaeEntryList(Data.EducationEntries, &CurrentWritingPoint);

	WriteToDest("\\section{Experience}\n\\subsection{Vocational}\n", &CurrentWritingPoint);
	WriteVitaeEntryList(Data.WorkExpEntries, &CurrentWritingPoint);

	WriteToDest("\\subsection{Miscellaneous}\n", &CurrentWritingPoint);
	WriteVitaeEntryList(Data.MiscEntries, &CurrentWritingPoint);

	WriteToDest("\\subsection{Projects}\n", &CurrentWritingPoint);
	WriteVitaeEntryList(Data.ProjectsEntries, &CurrentWritingPoint);

	WriteToDest("\\section{Computer Skills}\n\\cvitem{Basic}{", &CurrentWritingPoint);
	WriteToDest(Data.BasicSkills, &CurrentWritingPoint);

	WriteToDest("}\n\\cvitem{Intermediate}{", &CurrentWritingPoint);
	WriteToDest(Data.IntermediateSkills, &CurrentWritingPoint);

	WriteToDest("}\n\\cvitem{Advanced}{", &CurrentWritingPoint);
	WriteToDest(Data.AdvancedSkills, &CurrentWritingPoint);

	WriteToDest("}\n\n\\section{Languages}\n\\cvitem{}{", &CurrentWritingPoint);
	WriteToDest(Data.Languages, &CurrentWritingPoint);

	WriteToDest("}\n\n\\section{Interests}\n\\renewcommand{\\listitemssymbol}{-~}\n\n\\cvitem{}{", &CurrentWritingPoint);
	WriteToDest(Data.Hobbies, &CurrentWritingPoint);

	WriteToDest("}\n\\end{document}", &CurrentWritingPoint);

	Result.Size = StringLength(Result.Content);

	return(Result);
}

