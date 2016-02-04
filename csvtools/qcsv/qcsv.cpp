// qcsv.cpp : Defines the entry point for the console application.
//
// This is the windows wrapper for the Posix compliant source code.
// Ah... windows... why are they not compliant with Real OS's?

// #include "stdafx.h"
#include "windows.h"

int main(int argc, char *argv[]);

int _tmain(int argc, TCHAR* argv[])
{
	return main(argc, (char **) argv);
}

