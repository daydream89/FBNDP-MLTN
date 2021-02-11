#include "CSVReader.h"

FileDataList CSVReader::GetFileData(string FileName)
{
	ifstream FileStream(FileName);

	FileDataList FileData;
	ReadCSVFile(FileStream, FileData);

	FileStream.close();

	return FileData;
}

void CSVReader::ReadCSVFile(ifstream& FileStream, FileDataList& OutFileData)
{
	if (FileStream.fail())
	{
		cout << "file can't opened." << endl;
		return;
	}

	int RowCount = 0;
	while (FileStream.good())
	{
		RowCount++;

		vector<string> Row;
		CSVReadRow(FileStream, ',', Row);
		if (Row.size() == 0)
		{
			return;
		}

		if (RowCount == 1)		// ignore First Row. Not Use in this program.
		{
			continue;
		}

		if (Row[0].find('#') != string::npos)	// ignore comment
		{
			continue;
		}

		// save data
		OutFileData.emplace_back(Row);
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
			OutRow.emplace_back(SStream.str());
			SStream.str("");
		}
		else if (!Inquotes && (Character == '\r' || Character == '\n'))
		{
			if (FileStream.peek() == '\n')
				FileStream.get();

			OutRow.emplace_back(SStream.str());
			SStream.str("");
			break;
		}
		else
		{
			SStream << Character;
		}
	}
}