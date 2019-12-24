#define _CRT_SECURE_NO_WARNINGS
#include <QMap>
#include <qapplication.h>
#include <qtconcurrentmap.h>
#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include <fstream>
#include <boost/filesystem.hpp>
#include <chrono>
#include <boost/range/combine.hpp>
#include <boost/foreach.hpp>


using namespace std;
namespace fs = std::experimental::filesystem;
typedef QMultiMap<char, string> qm_sym;
typedef QMultiMap<string, string> qm_sub;
int length = 0;

qm_sym build_symbol_index(const fs::path& fname_path) {
	qm_sym symbol_index;
	string fname = "";
	fname = fname_path.filename().string();
	for (char symbol : fname)
	{
		symbol_index.insert(symbol, fname_path.string());
	}
	return symbol_index;
}

void print_symbol_index(const qm_sym& symbol_index, const string& fout) {
	std::ofstream file(fout);
	file << "-- symbol_index --\n";
	QMapIterator<char, string> i(symbol_index);
	while (i.hasNext()) {
		i.next();
		file << "(" << i.key() << ": " << i.value() << ")\n";
	}
	file.close();
}

qm_sub build_subline_index(const fs::path& fname_path) {
	qm_sub subline_index;
	int min_fname;
	char str[80];
	string fname = "";
	int length_min = 0;
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
		subline_index.insert(str, fname_path.string());
	}
	return subline_index;
}

void print_subline_index(const qm_sub& subline_index, const string& fout) {
	std::ofstream file(fout);
	file << "-- subline_index --\n";
	QMapIterator<string, string> i(subline_index);
	while (i.hasNext()) {
		i.next();
		file << "(" <<  i.key() << ": " << i.value() << ")\n";
	}
	file.close();
}

void merge_map_sym(qm_sym  &symbol_index, const qm_sym &core_maps_symb)
{
	QMapIterator<char, string> i(core_maps_symb);
	while (i.hasNext()) {
		i.next();
		symbol_index.insert( i.key() , i.value());
	}
}

void merge_map_sub(qm_sub  &subline_index , const qm_sub &core_maps_subl)
{
	QMapIterator<string, string> i(core_maps_subl);
	while (i.hasNext()) {
		i.next();
		subline_index.insert(i.key(), i.value());
	}
}

int main(int argc, char* argv[])
{
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
		cout << "enter correct parameters\n";
		cout << "example: mapreduce --input out-file --output time_results01.csv --config parameters.cfg --length 5";
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
	for (int size : sizes) { //берется очередной size - количество путей
		vector<fs::path> full_files;
		std::ifstream input_file(input + to_string(size) + ".txt");
		while (getline(input_file, str)) {
			full_files.push_back(str);	//открывается файл  и пути считываются в full_files vector
		}
		input_file.close();

		auto begin = std::chrono::steady_clock::now();
		
		qm_sub res_sub = QtConcurrent::mappedReduced(full_files, build_subline_index, merge_map_sub);
		qm_sym res_sym = QtConcurrent::mappedReduced(full_files, build_symbol_index, merge_map_sym);
		auto end = std::chrono::steady_clock::now();
		print_symbol_index(res_sym, "symbol-output-mapreduced" + to_string(size) + ".txt");
		print_subline_index(res_sub, "subline-output-mapreduced" + to_string(size) + ".txt");
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



