#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "MailPool_mapset.hpp"
#include "ToolBox.hpp"
using namespace std;

int main() {
	MailPool tongpool;

	string inp;
	while (getline(cin, inp)) {
		//if (input.empty()) break;
		string input = inp.substr(0, 3);
		if (input == "add") tongpool.Add(inp.substr(4));
		else if (input == "rem") tongpool.Remove(inp.substr(7));
		else if (input == "que") tongpool.Query(inp.substr(6));
		//else if (input == "que") printf("-\n");
		else if (input == "lon") tongpool.Longest();
		//else if (input == "lon") printf("-\n");
		else if (input == "fin") tongpool.QueryWord(inp.substr(4));
	}
}