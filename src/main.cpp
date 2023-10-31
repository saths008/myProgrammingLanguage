#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>
#include <any>
#include "scanner.hpp"
using std::cout, std::endl, std::string;

string readFile(string filePath)
{
	std::ifstream MyReadFile(filePath);
	std::string wholeFile;
	std::string line;
	while (std::getline(MyReadFile, line))
	{
		wholeFile = wholeFile + line + "\\n";
	}
	return wholeFile;
}

int main(int argc, char *argv[])
{
	cout << "Number of Arguments: " << argc << endl;
	for (int i = 0; i < argc; i++)
	{
		char *ptr = argv[i];
		cout << "Argument " << i << " :" << ptr << endl;
	}
	if (argc > 1 && std::filesystem::exists(argv[1]))
	{
		string fileRead = readFile(argv[1]);
		std::unique_ptr<string> fileReadPtr = std::make_unique<string>(fileRead);
		Scanner scanner(*fileReadPtr);
		cout << "Constructed Scanner: " << scanner << endl;
		scanner.scanFile();
		cout << "After scanning Scanner: " << scanner << endl;
	}
	return 0;
}