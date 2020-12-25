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
	// vector<vector<string>> ���� return ���ֵ��� ����.
	void GetFileData();

private:
	void ReadCSVFile(ifstream& FileStream);
	void CSVReadRow(ifstream& FileStream, char delim, vector<string>& OutRow);
};