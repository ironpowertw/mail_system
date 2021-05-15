#include <iostream>
#include <set>
#include <algorithm>
#include <fstream>
#include <string>
using namespace std;

int main() {
	string filepath;
	cin >> filepath;
	ifstream file(filepath);
	string tmp;
	while (getline(file, tmp)){
		if (tmp.substr(0, 3) == "add") {
			cout << "add ../test_data/" << tmp.substr(50) << endl;
		}
		else cout << tmp << endl;
	}
	file.close();
}