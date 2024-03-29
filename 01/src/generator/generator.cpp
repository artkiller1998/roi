/* ---------- Example using generator: generator --config parameters.cfg --output out-file --path "C:/Program Files"  -----------------*/
#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <boost/filesystem.hpp>
#include <vector>
#include <iterator>
#include <experimental/filesystem>
#include <algorithm>
#include <fstream>
#include <boost/range/iterator_range.hpp>


using namespace std;
namespace fs = std::experimental::filesystem;
std::vector<string> all_files;
vector <int> sizes;

template<typename T>
void pop_front(std::vector<T>& vec)
{
	assert(!vec.empty());
	vec.front() = std::move(vec.back());
	vec.pop_back();
}

class IndexBuilder
{
public:
	IndexBuilder(boost::filesystem::path p);
	~IndexBuilder();
	std::vector<fs::path> get_subdirs();
private:
	boost::filesystem::path full_path;
	std::vector<fs::path> subdirs;
	std::vector<string> files;
	std::vector<fs::path> build_subdirs();
	std::vector<string> build_files();
};

IndexBuilder::IndexBuilder(boost::filesystem::path p) {
	full_path = p;
	subdirs = this->build_subdirs();
	files = this->build_files();
}

IndexBuilder::~IndexBuilder() {
	full_path = "";
	subdirs.clear();
}

std::vector<fs::path> IndexBuilder::build_subdirs() {
	fs::recursive_directory_iterator begin(full_path.c_str());
	fs::recursive_directory_iterator end;
	std::vector<fs::path> subdirs;
	std::copy_if(begin, end, std::back_inserter<>(subdirs), [](const fs::path full_path) {
		return fs::is_directory(full_path); });
	return subdirs;
}

std::vector<string> IndexBuilder::build_files() {
	std::vector<string> files;
	fs::directory_iterator end_itr;
	for (fs::directory_iterator itr(full_path.c_str()); itr != end_itr; ++itr)
	{
		if (is_regular_file(itr->path())) {
			all_files.push_back(itr->path().string());
		}
	}
	return files;
}

std::vector<fs::path> IndexBuilder::get_subdirs() { return subdirs; }

int main(int argc, char* argv[])
{
	boost::filesystem::path full_path = "";
	string config = "";
	string output = "";

	if (argc == 7)
	{
		for (int i = 0; i < 6; i++)
		{
			if (!strcmp(argv[i], "--config"))
			{
				config = argv[i + 1];
			}
			if (!strcmp(argv[i], "--output"))
			{
				output = argv[i + 1];
			}
			if (!strcmp(argv[i], "--path"))
			{
				full_path = argv[i + 1];
			}
		}
	}
	else
	{
		cout << "Enter correct parameters\n";
		cout << "Example: generator --config parameters.cfg --output out-file --path \"C:/Program Files\"";
		exit(1);
	}

	if (!is_directory(full_path)) {
		cout << "Path is not a directory";
		exit(1);
	}

	std::ifstream file(config);
	string i;
	while (getline(file, i))
	{
		sizes.push_back(atoi(i.c_str()));
	}
	file.close();

	IndexBuilder index((boost::filesystem::path)full_path);
	std::vector<fs::path> temp = index.get_subdirs();
	index.~IndexBuilder();
	while (temp.size() != 0)
	{
		IndexBuilder index((boost::filesystem::path)temp.front());
		pop_front(temp);
		index.~IndexBuilder();
	}
	cout << "\n\n\n";
	int count = 0;
	for (int size : sizes)
	{
		count = size;
		std::ofstream outfile(output + to_string(size) + ".txt");
		for (string elem : all_files)
		{
			count--;
			outfile << elem;
			if (count == 0 || elem == all_files.back())
				break;
			outfile << '\n';
		}
		outfile.close();
	}
	return 0;
}

