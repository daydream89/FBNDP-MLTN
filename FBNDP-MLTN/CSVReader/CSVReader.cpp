#include "CSVReader.h"

void CSVReader::GetFileData()
{
	ifstream FileStream("./Input/2_Node.csv");

	ReadCSVFile(FileStream);

	FileStream.close();
}

void CSVReader::ReadCSVFile(ifstream& FileStream)
{
	if (FileStream.fail())
	{
		cout << "file can't opened." << endl;
		return;
	}

	while (FileStream.good())
	{
		vector<string> Row;
		CSVReadRow(FileStream, ',', Row);
		if (Row.size() == 0)
		{
			return;
		}

		if (Row[0].find("#"))	// ignore comment
		{
			continue;
		}

		// save data
	}
}

void CSVReader::CSVReadRow(ifstream& FileStream, char Delim, vector<string>& OutRow)
{
	stringstream SStream;
	bool Inquotes = false;

	while (FileStream.good())
	{
		char Character = FileStream.get();
		if (!Inquotes && Character == '"')
		{
			Inquotes = true;
		}
		else if (Inquotes && Character == '"')
		{
			if (FileStream.peek() == '"')
				SStream << static_cast<char>(FileStream.get());
			else
				Inquotes = false;
		}
		else if (!Inquotes && Character == Delim)
		{
			OutRow.push_back(SStream.str());
			SStream.str("");
		}
		else if (!Inquotes && (Character == '\r' || Character == '\n'))
		{
			if (FileStream.peek() == '\n')
				FileStream.get();

			OutRow.push_back(SStream.str());
		}
		else
		{
			SStream << Character;
		}
	}
}