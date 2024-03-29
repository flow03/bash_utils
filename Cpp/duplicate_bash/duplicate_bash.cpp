#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <getopt.h>
// #include <cstring>	// strcpy
// #include <algorithm>	// sort, find, find_if

#define _DEBUG
// #define sep fs::path::preferred_separator

namespace fs = std::filesystem;
struct flags;

struct flags
{
	flags()
	{
		c = false;
		r = false;
		t = false;
		n = false;
		s = false;
		g = false;
		d = false;
	}
	
	bool c, r, t, n, s, g, d;
};

class Duplicate
{
public:
	void check_dir(fs::path temp_path);
	void init_flags(int argc, char* argv[]);
	
	void vec_init(std::vector<fs::path> &folder, fs::path _dir);
	auto get_comp_method() const;
	char get_result_method() const;
	std::vector<std::vector<fs::path>> vec_duplicate(std::vector<fs::path> vec, bool (*comp_method)(const fs::path&, const fs::path&));
	std::vector<fs::path> vec_new_names(const std::vector<fs::path> &vec) const;
	void vec_copy(const std::vector<std::vector<fs::path>> &dup) const;
	
	void out_dir() const;
	// bool empty() const {return dir.empty();}
	fs::path get_dir() const {return dir;}
	
private:
	flags _flags;
	fs::path dir;
	fs::path dir_result;
};



void Duplicate::check_dir(fs::path temp_path)
{
	if (exists(temp_path))
	{
		if (fs::is_directory(temp_path))
		{
			if (dir.empty()) dir = temp_path;
			else if (dir_result.empty()) dir_result = temp_path;
			else 
			{
				std::cout << temp_path << ": Both pathes already exists" << std::endl;
				out_dir();
			}
			
		}
		else std::cout << "Path " << temp_path << " is not a directory" << std::endl;
	}
	else std::cout << "Path " << temp_path << " is not valid" << std::endl;
}

void Duplicate::init_flags(int argc, char* argv[])
{
	using namespace std;
	
	int ch;
	// flags f;
	
	while ((ch = getopt(argc, argv, "-:p:o:crtnsgd")) != -1) {
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
			else check_dir(optarg);
			break;
		case 'c':	// copy flag
			_flags.c = true;
			cout << "c flag active" << endl;
			break;
		case 'r':	// replace flag
			_flags.r = true;
			cout << "r flag active" << endl;
			break;
		case 't':	// text flag
			_flags.t = true;
			cout << "t flag active" << endl;
			break;
		case 'n':	// name flag
			_flags.n = true;
			cout << (char)ch << " flag active" << endl;
			break;
		case 's':	// size flag
			_flags.s = true;
			cout << (char)ch << " flag active" << endl;
			break;
		case 'g':	// add original file to results flag
			_flags.g = true;
			cout << (char)ch << " flag active" << endl;
			break;
		case 'd':	// create directory for each duplicates flag
			_flags.d = true;
			cout << (char)ch << " flag active" << endl;
			break;
		case 'o':	// out path flag
			#ifdef _DEBUG
			cout << "o flag active" << endl;
			#endif
			if (optarg[0] == '-') 
			{
				cout << "Cannot use the " << optarg << " as parameter for -" << (char)ch << endl;
				// --optind;
			}
			else if (fs::exists(optarg)) dir_result.assign(optarg);
			// try to create a new directory
			else cout << "Cannot use the " << optarg << " as result directory. It does not exist" << endl;
			//else if (fs::create_directory(optarg)) dir_result.assign(optarg);			// throw exception
			//else std::cout << "File path " << optarg << " is not valid" << std::endl;	// never go here
			break;
		case '?':	// unknown flag
			cout << "\"-" << (char)optopt << "\": Invalid option" << endl;
			break;
		case 1:
			check_dir(optarg);
			break;
		default:
			/* none */
			break;
		}
	}
	
	// return f;
}

// recursive
void Duplicate::vec_init(std::vector<fs::path> &folder, fs::path _dir)
{
	if (!_dir.empty())
	for (fs::directory_entry const& entry : fs::directory_iterator(_dir)) 
	{
        if (entry.is_regular_file())
			folder.push_back(entry.path());
		else if (entry.is_directory())
			vec_init(folder, entry.path());	// recursive
    }
}

void Duplicate::out_dir() const
{
	std::cout << "dir: " << dir.generic_string() << std::endl;
	std::cout << "dir_result: " << dir_result.generic_string() << std::endl;
}

void vec_out(const std::vector<fs::path> &vec)
{
	for (const fs::path &el : vec)
		// std::cout << el.make_preferred().string() << std::endl; // non-const
		std::cout << el.generic_string() << std::endl; // separator '/' as default
		
	// std::cout << std::endl << vec.size() << " elements in container" << std::endl;
}

void vec_out(const std::vector<std::vector<fs::path>> &dup)
{
	for (const std::vector<fs::path> &vec : dup)
	{
		vec_out(vec);
		std::cout << std::endl;
	}
}

bool comp_stem(const fs::path &path_1, const fs::path &path_2)
{
	return (path_1.stem() == path_2.stem());
}

bool comp_name(const fs::path &path_1, const fs::path &path_2)
{
	return (path_1.filename() == path_2.filename());
}

bool comp_size(const fs::path &path_1, const fs::path &path_2)
{
	return (file_size(path_1) == file_size(path_2));
}

// Функція get_comp_method повертає вказівник на функцію
// bool (*c_method(flags _flags))(const fs::path&, const fs::path&)
auto Duplicate::get_comp_method() const
{
	if (_flags.s) 
	{
		return comp_size;
		#ifdef _DEBUG
		std::cout<<"comp_method comp_size"<<std::endl;
		#endif
	}
	
	return comp_name;
}

// text > copy > rename
// не кращий приклад порівнюваного значення
char Duplicate::get_result_method() const
{
	if (_flags.t) return 't';
	else if (_flags.c) return 'c';
	else if (_flags.r) return 'r';
	
	return 't';
}

// bool (*comp_method)(const fs::path&, const fs::path&) = &comp_name;
// auto comp_method = &comp_name;

std::vector<std::vector<fs::path>> Duplicate::vec_duplicate(std::vector<fs::path> vec, 
bool (*comp_method)(const fs::path&, const fs::path&) = &comp_name)
{
	std::vector<std::vector<fs::path> > dup;
	std::vector<fs::path> res;
	
	for (auto it = vec.begin(); it!= vec.end(); ++it)
	{
		for (auto el = vec.begin(); el!= vec.end(); ++el)
		{
			if (el != it)
			{
				// change compare method
				if (comp_method(*el, *it))
				{
					// _flags.g
					if (_flags.g && res.empty()) res.push_back(*it);
					res.push_back(*el);
					el = vec.erase(el);
					--el;
				}
			}
		}
		it = vec.erase(it);
		--it;
		if (!res.empty())
		{
			dup.push_back(res);
			res.clear();
		}
	}
	
	return dup;
}

void vec_rename(std::vector<fs::path> &vec)
{
	if (vec.size() > 1)
	{
		std::string str_name;
			
		// pointer realization
		size_t i = 0;	// counter
		for (auto cur = vec.begin(); cur != vec.end(); ++cur)	// current
		{
			i=0;
			for (auto sec = vec.begin(); sec != vec.end(); ++sec)	// second
			{
				if (cur!=sec)
				{
					if (cur->filename() == sec->filename())
					{
						str_name = sec->stem().string()+"_"+std::to_string(i)+sec->extension().string();	// counter using
						sec->replace_filename(str_name);	// replace_filename
						str_name.clear();
					}
				}
				++i;
			}
		}
	}
	//else std::cout << "Invalid vector. Need two or more elements" << std::endl;
}

// reworking
std::vector<fs::path> Duplicate::vec_new_names(const std::vector<fs::path> &vec) const
{
	std::vector<fs::path> new_vec;
	fs::path prefix_name = dir_result;
	
	if (_flags.d) 
	{
		static size_t folder_count = 0;
		prefix_name /= std::to_string(folder_count);
		++folder_count;
		if (!fs::create_directory(prefix_name)) 
			std::cout << "Directory " << prefix_name.generic_string() << " already exists" << std::endl;
	}
	
	for (const fs::path &p : vec)
	{
		new_vec.push_back(prefix_name/p.filename());
	}
	
	vec_rename(new_vec);
	
	#ifdef _DEBUG
	// vec_out(new_vec);
	// std::cout << std::endl;
	#endif
	
	return new_vec;
}

// copy/rename files
void Duplicate::vec_copy(const std::vector<std::vector<fs::path>> &dup) const
{
	std::vector<fs::path> new_names;
	// const auto copyOptions = fs::copy_options::skip_existing;
	size_t count = 0;
	for (const std::vector<fs::path> &vec : dup)
	{
		// створює нові шляхи з вихідною текою; 
		// перейменовує однакові імена файлів; 
		// створює нові теки, якщо активний прапорець d
		new_names = vec_new_names(vec);
		if (vec.size() == new_names.size())
		{
			for (size_t i = 0; i < vec.size(); ++i)
			{
				if (!exists(new_names[i]))
				{
					// copy
					if (_flags.c) fs::copy_file(vec[i], new_names[i], fs::copy_options::skip_existing);
					// rename
					else if (_flags.r) fs::rename(vec[i], new_names[i]);
					++count;
				}
				else std::cout << new_names[i].generic_string() << " already exists" << std::endl;
			}
		} 
		else std::cout << "Invalid renamed vector size" << std::endl;
	}
	if (_flags.c) std::cout << count << " elements copied" << std::endl;
	else if (_flags.r) std::cout << count << " elements replaced" << std::endl;
	// count = 0;
}

int main(int argc, char* argv[])
{
	Duplicate data;
	if (argc > 1) data.init_flags(argc, argv);
	
	#ifdef _DEBUG
	// data.out_dir();
	#endif
	
	if (!data.get_dir().empty())
	{
		std::vector<fs::path> folder;
		data.vec_init(folder, data.get_dir());
		
		// Отримуємо спосіб, за яким порівнюватимуться файли
		bool (*comp_method)(const fs::path&, const fs::path&) = data.get_comp_method();
		
		// Створюємо двомірний масив з однакових елементів
		std::vector<std::vector<fs::path>> dup = data.vec_duplicate(folder, comp_method);
		
		if (!dup.empty())
		{
			std::cout<<"Vec_duplicate:"<<std::endl;
			vec_out(dup);
			
			char r_method = data.get_result_method();
			if (r_method!='t')
			{
				#ifdef _DEBUG
				if (r_method=='c') std::cout<<"Vec_copy:"<<std::endl;
				else if (r_method=='r') std::cout<<"Vec_replace:"<<std::endl;
				#endif
				// Копіюємо/переміщуємо елементи у dir_result
				data.vec_copy(dup);
			}
		}
		else std::cout << "No duplicates found" << std::endl;
		// vec_out(folder);
	}
	else std::cout << "Not enough arguments" << std::endl;
	
	return 0;
}

// ./duplicate_bash.exe -p ./test/pic1
// ./duplicate_bash.exe ./test/dup -o ./test/dup_result -gdc
