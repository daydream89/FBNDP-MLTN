#pragma once

#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>

#include "../Data/DataTypes.h"

using namespace std;

class CSVReader
{
public:
	FileDataList GetFileData(string FileName);

private:
	void ReadCSVFile(ifstream& FileStream, FileDataList& OutFileData);
	void CSVReadRow(ifstream& FileStream, char delim, vector<string>& OutRow);
};