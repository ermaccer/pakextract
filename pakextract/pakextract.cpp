// pakextract.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <filesystem>
#include <memory>
#include "FileFunctions.h"

struct pak_header {
	int   header;
	int	  unk;
	int   files;
};

struct pak_entry {
	char name[_MAX_PATH];
	int  size;
	int  offset;
	int  unknown;
	int  hash;
};

int main(int argc, char* argv[])
{
	if (argc == 1) {
		std::cout << "Manhunt.PAKExtract - extract Manhunt .pak file\n"
			<< "Usage: paktool <file> (drag'n'drop works)\n";
		return 1;
	}

	if (argv[1])
	{
		std::ifstream pFile(argv[1], std::ifstream::binary);

		if (!pFile)
		{
			std::cout << "ERROR: Could not open " << argv[1] << "!" << std::endl;
			return 1;
		}

		pak_header pak;

		pFile.read((char*)&pak, sizeof(pak_header));

		if (!(pak.header == 'KPHM'))
		{
			std::cout << "ERROR: " << argv[1] << " is not a Manhunt .pak archive!" << std::endl;
			return 1;
		}

		std::vector<pak_entry> Files;

		for (int i = 0; i < pak.files; i++)
		{
			pak_entry file;
			pFile.read((char*)&file, sizeof(pak_entry));
			Files.push_back(file);
		}

		for (unsigned int i = 0; i < Files.size(); i++)
		{
			std::cout << "Processing: " << Files[i].name << std::endl;
			std::string name = Files[i].name;
			if (checkSlash(name))
				std::filesystem::create_directories(splitString(name, false));

			std::unique_ptr<char[]> dataBuff = std::make_unique<char[]>(Files[i].size);
			pFile.seekg(Files[i].offset, pFile.beg);
			pFile.read(dataBuff.get(), Files[i].size);

			std::ofstream oFile(name, std::ofstream::binary);

			for (int a = 0; a < Files[i].size ; a++)
				dataBuff[a] ^= 0x7F;

			
			oFile.write(dataBuff.get(), Files[i].size);
		}

		std::cout << "Finished." << std::endl;
	}
}
