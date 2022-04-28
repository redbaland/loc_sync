#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <stdlib.h>

using namespace std;
namespace fs = std::filesystem;

void removeExtra(const fs::path& src, const fs::path& dst) {
	vector<fs::path> toDel;
	if (!fs::exists(dst))
		create_directories(dst);
	for (auto dir_entry : fs::recursive_directory_iterator(dst)) {
		fs::path check = src;
		check /= dir_entry.path().string().substr(dst.string().size() + 1);
		if (!fs::exists(check) || fs::status(check).type() != fs::status(dir_entry).type())
			toDel.push_back(dir_entry.path());
	}
	for (int i = 0; i < toDel.size(); i++)
		cout << toDel[i];
	while (!toDel.empty()) {
		if (fs::exists(toDel.back()) && fs::is_directory(toDel.back()))
			fs::remove_all(toDel.back());
		else if (fs::exists(toDel.back()))
			fs::remove(toDel.back());
		toDel.pop_back();
	}
}

void copyMain(const fs::path& src, const fs::path& dst) {
	if (!fs::exists(dst))
		create_directories(dst);
	for (auto dir_entry : fs::recursive_directory_iterator(src)) {
		fs::path check = dst;
		check /= dir_entry.path().string().substr(src.string().size() + 1);
		if (fs::exists(check) && fs::status(check).type() == fs::status(dir_entry).type()) {
			if (dir_entry.is_regular_file() && fs::file_size(check) == fs::file_size(dir_entry))
				continue;
			else if (dir_entry.is_directory())
				continue;
		}
		const auto copyOptions = fs::copy_options::overwrite_existing | fs::copy_options::recursive;
		fs::copy(dir_entry.path(), check, copyOptions);
	}
}

void syncContent(const fs::path& src, const fs::path& dst){
	removeExtra(src, dst);
	copyMain(src, dst);
}

void locSync(const vector<fs::path>& paths) {
	cout << "iteration..." << endl;
	for (int i = 1; i < paths.size(); ++i) {
		syncContent(paths[0], paths[i]);
	}
}

void readPaths(const string cfg, vector<fs::path>& paths) {
	ifstream file(cfg);
	if (file.fail())
		throw std::invalid_argument("Can't read: " + cfg);
	for (string i; getline(file, i); paths.push_back(i));
	file.close();
	for (int i = 0; i < paths.size(); ++i) {
		paths[i] = fs::weakly_canonical(paths[i]);
		cout << paths[i] << endl;
		if (i == 0 && fs::status(paths[i]).type() != fs::file_type::directory)
			throw std::invalid_argument("Isn't a directory \nWrong path: " + paths[0].string());
		else if (i > 0 && paths[i] == paths[0])
			throw std::invalid_argument("Main path isn't unique \n");
	}

}

int main(int argc, char* argv[]) {
	string cfg = "paths.cfg";
	if (argc != 1)
		cfg = argv[1];
	vector<fs::path> paths;
	try{
		readPaths(cfg, paths);
	}
	catch (invalid_argument err)
	{
		cout << err.what() << endl;
		cout << "Try another config ? Y/n \n";
		char c;
		cin >> c;
		if (c == 'Y' || c == 'y') {
			cout << "Enter file name \n";
			cin >> cfg;
			readPaths(cfg, paths);
		}
		else 
			return -1;
	}
	while (true) {
		locSync(paths);
		usleep(2000000);
	}
	return 0;
}
