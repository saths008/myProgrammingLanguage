#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>
#include <any>
#include "scanner.hpp"
using std::cout, std::endl, std::string;
void run(string program)
{
	Scanner scanner(program);
	scanner.scanFile();
}
string readFile(string filePath)
{
	std::ifstream MyReadFile(filePath);
	std::string wholeFile;
	std::string line;
	while (std::getline(MyReadFile, line))
	{
		wholeFile = wholeFile + line + "\n";
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
		run(fileRead);
	}
	else if (argc == 1)
	{
		std::string line;
		std::cout << "Welcome to the REPL! \n";
		while (true)
		{
			std::cout << "> ";
			std::getline(std::cin, line);

			if (line.empty())
			{
				break;
			}
			std::cout << "You entered: " << line << std::endl;
			run(line);
		}
	}
	return 0;
}