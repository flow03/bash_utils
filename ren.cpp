#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <getopt.h>
// #include <algorithm>	// sort, find, find_if
#include <cctype>	// isdigit, isalnum, tolower, toupper

#define _DEBUG

namespace fs = std::filesystem;
//typedef std::pair<fs::path, std::uintmax_t> couple_t;

void check_path(fs::path temp_path, fs::path &dir)
{
	if (exists(temp_path))
	{
		if (dir.empty()) dir = temp_path;
		else std::cout << "Dir already assigned " << dir << std::endl;
	}
	else std::cout << "File path " << temp_path << " is not valid" << std::endl;
}

void test_getopt(int argc, char* argv[], fs::path &dir)
{
	using namespace std;
	
	int ch;
	
	while ((ch = getopt(argc, argv, "-:p:")) != -1) {
		switch (ch)
		{
		case 'p':	// path flag
#ifdef _DEBUG
			cout << "p flag active" << endl;
#endif
			if (optarg[0] == '-')
			{
				cout << "Cannot use " << optarg << " as parameter for -" << (char)ch << endl;
				// --optind;
			}
			else check_path(optarg, dir);
			break;
		case '?':
			cout << "\"-" << (char)optopt << "\": Invalid option" << endl;
			break;
		case 1:
			check_path(optarg, dir);
			break;
		default:
			/* none */
			break;
		}
	}
}

std::vector<fs::path> vec_init(fs::path directory)
{
	std::vector<fs::path> folder;
	for (fs::directory_entry const& entry : fs::directory_iterator(directory)) 
	{
        if (entry.is_directory()) folder.push_back(entry.path());
    }
	
	return folder;
}


int main(int argc, char* argv[])
{
	fs::path dir;
	
	if (argc > 1) test_getopt(argc, argv, dir);
	
	if (!dir.empty())
	{
		std::vector<fs::path> folder = vec_init(dir);
		
#ifdef _DEBUG

		std::string folder_name = dir.filename().string();
		if (folder_name.empty()) folder_name = dir.parent_path().filename().string();
		std::cout << "\nFolder " << folder_name << " content:" << std::endl;
		
		for (fs::path p : folder)
		std::cout << p << std::endl;
#endif
	}
	else std::cout << "File path is empty" << std::endl;
	
	return 0;
}