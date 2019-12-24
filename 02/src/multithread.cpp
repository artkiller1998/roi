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
unsigned int cores;
/*std::multimap<char, string>*/void build_symbol_index(const vector<fs::path>& full_files, multimap<char, string> &symbol_index) {
	//std::multimap<char, string> symbol_index;
	string fname = "";
	for (fs::path fname_path : full_files)
	{
		fname = fname_path.filename().string();
		for (char symbol : fname)
		{
			symbol_index.insert(pair<char, string>(symbol, fname_path.string()));
		}
	}
	//return symbol_index;
}

void print_symbol_index(const multimap<char, string> &symbol_index, const string &fout) {
	std::ofstream file(fout);
	file << "-- symbol_index --\n";
	for (const auto& pair : symbol_index) {
		file << "(" << pair.first << ":" << pair.second << ")\n";
	}
	file.close();
}

/*std::multimap<string, string>*/void build_subline_index(const vector<fs::path> &full_files, multimap<string, string> &subline_index) {
	//std::multimap<string, string> subline_index;
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
	//return subline_index;
}

void print_subline_index(const multimap<string, string> &subline_index, string fout) {
	std::ofstream file(fout);
	file << "-- subline_index --\n";
	for (const auto& pair : subline_index) {
		file << "(" << pair.first << ":" << pair.second << ")\n";
	}
	file.close();
}

void split_vector(const vector<fs::path> &full_files, vector<vector<fs::path>> &core_vectors)
{
	int i = 0;
	for (auto& path : full_files)
	{
		core_vectors[(i%cores)].push_back(path);
		++i;
	}
}

void merge_map_sym(const vector<multimap<char, string>> &core_maps_symb, multimap<char, string>  &symbol_index)
{
	for (auto& core_maps_symb_elem :core_maps_symb)
	{
		for (auto& pair : core_maps_symb_elem) {
			symbol_index.insert(pair);
			//cout << pair.first << "	" << pair.second;
		}
	}
}

void merge_map_sub(const vector<multimap<string, string>> &core_maps_subl, multimap<string, string>  &subline_index)
{
	for (auto& core_maps_subl_elem : core_maps_subl)
	{
		for (auto& pair : core_maps_subl_elem) {
			subline_index.insert(pair);
		}
	}
}

int main(int argc, char* argv[])
{
	cores = std::thread::hardware_concurrency();
	multimap<char, string> symbol_index;
	multimap<string, string> subline_index;
	vector<int> sizes;
	vector<vector<fs::path>> core_vectors;
	vector<multimap<char, string>> core_maps_symb;
	vector<multimap<string, string>> core_maps_subl;
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
		cout << "Example: multithread --input out-file --output time_results01.csv --config parameters.cfg --length 5";
		exit(1);
	}

	for (int i = 0; i < cores; i++) {
		vector<fs::path> vec;
		core_vectors.push_back(vec);
	}
	
	for (int i = 0; i < cores; i++) {
		multimap<char, string> mp;
		core_maps_symb.push_back(mp);
	}

	for (int i = 0; i < cores; i++) {
		multimap<string, string> mp;
		core_maps_subl.push_back(mp);
	}

	std::ifstream file(config);
	string i;
	while (getline(file, i))
	{
		sizes.push_back(atoi(i.c_str()));
	}
	file.close();

	std::cout << endl << cores << " concurrent threads are supported.\n\n";

	string str;
	for (int size : sizes) { //берется очередной size - количество путей
		vector<fs::path> full_files;
		std::ifstream input_file(input + to_string(size) + ".txt");
		while (getline(input_file, str)) {
			full_files.push_back(str);	//открывается файл  и пути считываются в full_files vector
		}
		input_file.close();
		split_vector(full_files, ref(core_vectors)); // вектор бьется на core/2 векторов-частей
		vector<thread> threads;						// создается вектор с потоками
		vector<thread> threads2;
		vector<thread> threads3;
		auto begin = std::chrono::steady_clock::now();
		for (int i = 0; i < cores; ++i ) {
			threads.push_back(thread(build_symbol_index, core_vectors[i], ref(core_maps_symb[i]))); }
		for (unsigned int i = 0; i < threads.size(); ++i) {
			threads.at(i).join(); }
		for (int i = 0; i < cores; ++i) {
			threads2.push_back(thread(build_subline_index, core_vectors[i], ref(core_maps_subl[i])));
		}
		for (unsigned int i = 0; i < threads2.size(); ++i) {
			threads2.at(i).join();
		}
		
		threads3.push_back(thread(merge_map_sym, core_maps_symb, ref(symbol_index)));
		threads3.push_back(thread(merge_map_sub, core_maps_subl, ref(subline_index)));
		for (unsigned int i = 0; i < threads3.size(); ++i) {
			threads3.at(i).join();
		}
	/*	merge_map(core_maps_symb, core_maps_subl, ref(symbol_index), ref(subline_index)); */
		auto end = std::chrono::steady_clock::now();
		print_symbol_index(symbol_index, "symbol-output-multithread" + to_string(size) + ".txt");
		print_subline_index(subline_index, "subline-output-multithread" + to_string(size) + ".txt");
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
		execution_time.push_back(elapsed_ms.count());
		subline_index.clear();
		symbol_index.clear();
		for (int i = 0; i < cores; i++) {
			core_maps_symb[i].clear();
		}

		for (int i = 0; i < cores; i++) {
			core_maps_subl[i].clear();
		}

		for (int i = 0; i < cores; i++) {
			core_vectors[i].clear();
		}
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



