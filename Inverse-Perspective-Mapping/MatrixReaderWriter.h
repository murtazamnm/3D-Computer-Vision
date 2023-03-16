#pragma once

#include <fstream>
#include <iostream>
#include <stdlib.h>

class MatrixReaderWriter
{
public:
	double* data;
	int rowNum;
	int columnNum;

	MatrixReaderWriter(double* data, int rownum, int columnNum);
	MatrixReaderWriter(const char* fileName);

	~MatrixReaderWriter();

	void load(const char* fileName);
	void save(const char* fileName);
};