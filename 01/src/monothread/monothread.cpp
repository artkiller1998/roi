#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <iterator>
#include <experimental/filesystem>
#include <algorithm>
#include <fstream>
#include <boost/filesystem.hpp>
#include <chrono>
#include <boost/range/combine.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <map>

using namespace std;
namespace fs = std::experimental::filesystem;
int length = 0;

std::multimap<char, string> build_symbol_index(vector<fs::path> full_files) {
	std::multimap<char, string> symbol_index;
	string fname = "";
	for (fs::path fname_path : full_files)
	{
		fname = fname_path.filename().string();
		for (char symbol : fname)
		{
			symbol_index.insert(pair<char, string>(symbol, fname_path.string()));
		}
	}
	return symbol_index;
}

void print_symbol_index(multimap<char, string> symbol_index, string fout) {
	std::ofstream file(fout);
	file << "-- symbol_index --\n" ;
	for (const auto& pair : symbol_index) {
		file << "(" << pair.first << ":" << pair.second << ")\n";
	}
	file.close();
}

std::multimap<string, string> build_subline_index(vector<fs::path> full_files) {
	std::multimap<string, string> subline_index;
	int min_fname;
	char str[80];
	string fname = "";
	int length_min = 0;
	for (fs::path fname_path : full_files)
	{
		min_fname = INT_MAX;
		fname = fname_path.filename().string();
		fname.length() > min_fname ? 0 : min_fname = fname.length();
		length_min = length;
		if (length_min > min_fname) {
			length_min = min_fname;
		}
		int n1 = length_min - 1;
		for (auto i = fname.cbegin(); i != fname.cend() - n1; ++i) {
			sprintf(str, "%.*s", length_min, &*i);
			subline_index.insert(pair<string, string>(str, fname_path.string()));
		}
	}
	return subline_index;
}

void print_subline_index(multimap<string, string> subline_index, string fout) {
	std::ofstream file(fout);
	file << "-- subline_index --\n" ;
	for (const auto& pair : subline_index) {
		file << "(" << pair.first << ":" << pair.second << ")\n";
	}
	file.close();
}

int main(int argc, char* argv[])
{
	multimap<char, string> symbol_index;
	multimap<string, string> subline_index;
	vector<int> sizes;
	vector<float> execution_time;
	string input = "";
	string output = "";
	string config = "";
	if (argc == 9)
	{
		for (int i = 0; i < 8; i++)
		{
			if (!strcmp(argv[i], "--input"))
			{
				input = argv[i + 1];
			}
			if (!strcmp(argv[i], "--output"))
			{
				output = argv[i + 1];
			}
			if (!strcmp(argv[i], "--config"))
			{
				config = argv[i + 1];
			}
			if (!strcmp(argv[i], "--length"))
			{
				length = atoi(argv[i + 1]);
			}
		}
	}
	else
	{
		cout << "Enter correct parameters\n";
		cout << "Example: monothread --input out-file --output time_results01.csv --config parameters.cfg --length 5";
		exit(1);
	}

	std::ifstream file(config);
	string i;
	while (getline(file, i))
	{
		sizes.push_back(atoi(i.c_str()));
	}
	file.close();

	string str;
	for (int size : sizes) {
		vector<fs::path> full_files;
		std::ifstream input_file(input + to_string(size) + ".txt");
		while (getline(input_file, str)) {
			full_files.push_back(str);
		}
		input_file.close();
		auto begin = std::chrono::steady_clock::now();
		symbol_index = build_symbol_index(full_files);
		subline_index = build_subline_index(full_files);
		auto end = std::chrono::steady_clock::now();
		print_symbol_index(symbol_index, "symbol-output-monothread" + to_string(size) + ".txt");
		print_subline_index(subline_index, "subline-output-monothread" + to_string(size) + ".txt");
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
		execution_time.push_back(elapsed_ms.count());
	}

	std::ofstream file_csv(output);
	float i2;
	int i1;
	BOOST_FOREACH(boost::tie(i1, i2), boost::combine(sizes, execution_time)) {
		file_csv << i1 << ";" << i2 << "\n";
		cout << i1 << ";" << i2 << "\n";
	}
	file_csv.close();
	return 0;
}

