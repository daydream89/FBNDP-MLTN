#pragma once

#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class CSVReader
{
public:
	// vector<vector<string>> 으로 return 해주도록 하자.
	void GetFileData();

private:
	void ReadCSVFile(ifstream& FileStream);
	void CSVReadRow(ifstream& FileStream, char delim, vector<string>& OutRow);
};