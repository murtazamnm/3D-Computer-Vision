#include "MatrixReaderWriter.h"
#include <string>

MatrixReaderWriter::MatrixReaderWriter(double* data, int rowNum, int columnNum)
{
	this->data = new double[columnNum * rowNum];

	for (int i = 0; i < (columnNum * rowNum); i++)
		this->data[i] = data[i];

	this->rowNum = rowNum;
	this->columnNum = columnNum;

} // end MatrixReaderWriter(double* data, int rownum, int columnNum)

MatrixReaderWriter::~MatrixReaderWriter()
{
	if (this->data != NULL)
		delete[] data;
}

MatrixReaderWriter::MatrixReaderWriter(const char* fileName)
{
	this->data = NULL;
	load(fileName);

} // end MatrixReaderWriter(const char* fileName)

void MatrixReaderWriter::load(const char* fileName)
{
	std::ifstream datafile(fileName);
	std::string line;
	int lineCounter = 0;

	// Count lines
	if (datafile.is_open())
	{
		while (!datafile.eof())
		{
			getline(datafile, line);
			if (line.length() != 0)
				if (line.at(0) != '#')
					lineCounter++;

		} // end while

		datafile.close();

	} // end if
	else
	{
		std::cout << "Unable to open file";
	}

	int columnCounter = 0;
	std::ifstream datafile2(fileName);

	if (datafile2.is_open())
	{
		getline(datafile2, line);

		while (line.at(0) == '#')
			getline(datafile2, line);

		for (int i = line.find_first_not_of(" ,"); i <= line.find_last_not_of(" ,"); i++)
			if ((line.at(i) == ' ') || (line.at(i) == ','))
				columnCounter++;

		datafile2.close();
		columnCounter++;
	}

	rowNum = lineCounter;
	columnNum = columnCounter;

	if (data != NULL)
		delete[] data;
	data = new double[rowNum * columnNum];

	std::ifstream datafile3(fileName);

	lineCounter = 0;
	if (datafile3.is_open())
	{
		getline(datafile3, line);

		while (line.at(0) == '#')
			getline(datafile3, line);

		while ((!datafile3.eof()) && (line.length() > 0))
		{
			int index = line.find_first_not_of(" ,");

			for (int i = 0; i < columnCounter; i++)
			{
				std::string number = line.substr(index, line.find(" ", index + 1) - index + 1);
				double num = strtod((char*)&number[0], NULL);
				data[lineCounter * columnNum + i] = num;

				index = line.find(" ", index + 1);

			} // end for i

			getline(datafile3, line);
			lineCounter++;

		} //end while

		datafile3.close();
	}

} // end load(string filename)

void MatrixReaderWriter::save(const char* fileName)
{
	std::ofstream myfile;
	myfile.open(fileName);
	myfile << "#Created by C++ matrix writer.\n";

	for (int i = 0; i < (rowNum); i++)
	{
		for (int j = 0; j < columnNum; j++)
		{
			myfile << data[i * columnNum + j] << " ";

		} // end for j

		myfile << std::endl;

	} // end for i

	myfile << std::endl;
	myfile.close();

} // end save(string filename)