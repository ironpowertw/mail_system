#ifndef MailPool_hpp
#define MailPool_hpp
#include <iostream>
#include <string>
#include <stdio.h>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "RBTree.hpp"
#include "ToolBox.hpp"
#include "Calculator_set.hpp"

using namespace std;

class MailPool;
class MailNode;

string DetermineMonth(string& al) {
	//cout << alpha << endl;
	string digi;
	string alpha = al.substr(0, 3);
	if (alpha == "Jan") digi = "01";
	else if (alpha == "Feb") digi = "02";
	else if (alpha == "Mar") digi = "03";
	else if (alpha == "Apr") digi = "04";
	else if (alpha == "May") digi = "05";
	else if (alpha == "Jun") digi = "06";
	else if (alpha == "Jul") digi = "07";
	else if (alpha == "Aug") digi = "08";
	else if (alpha == "Sep") digi = "09";
	else if (alpha == "Oct") digi = "10";
	else if (alpha == "Nov") digi = "11";
	else if (alpha == "Dec") digi = "12";

	return digi;
}

vector<double> DateBoundary(string date) {
	vector<string> time_split = split(date, '~');
	vector<double> ans;
	ans.push_back(0); ans.push_back(1E13);
	if (time_split.size() == 1) {
		ans[0] = stod(time_split[0].substr(2));
	}
	else {
		if (time_split[0].size() == 2) {
			ans[1] = stod(time_split[1]);
		}
		else {
			ans[0] = stod(time_split[0].substr(2));
			ans[1] = stod(time_split[1]);
		}
	}
	return ans;
}


class MailNode {
private:
	string _sender;
	string _receiver;
	string _subject;
	string _path;
	//string _content;

	double _date;
	int _mid;
	int _length;

public:
	MailNode() {
		_sender = "";
		_receiver = "";
		_subject = "";
		_path = "";
		_date = -1;
		_mid = -1;
		_length = 0;
	}
	~MailNode() {
		_sender.clear(); _receiver.erase(); _subject.erase();
	}
	void SetData(string& ext_path, map<int, MailNode*>& _mid_tree, map<string, set<int>>& _content_map) {
		ifstream mail(ext_path);
		string tmp;
		int topic_count = 0;
		if (mail.is_open()) {
			_path = ext_path;
			string words = "";
			map<string, set<int>>::iterator itr;

			while (getline(mail, tmp)) {
				if (topic_count == 0) {
					_sender = tmp.substr(6);
					transform(_sender.begin(), _sender.end(), _sender.begin(), ::tolower);
				}
				else if (topic_count == 1) {
					SetDate(tmp.substr(6));
				}
				else if (topic_count == 2) {
					_mid = stoi(tmp.substr(12));
					map<int, MailNode*>::iterator mitr = _mid_tree.find(_mid);
					if (mitr != _mid_tree.end()) { return; }
				}
				else if (topic_count == 3) {
					if (tmp.size() > 9) _subject = tmp.substr(9);
				}
				else if (topic_count == 4) {
					_receiver = tmp.substr(4);
					transform(_receiver.begin(), _receiver.end(), _receiver.begin(), ::tolower);
				}
				else if (topic_count >= 6) {
					for (int i = 0; i < tmp.size(); i++)
					{
						if (isdigit(tmp[i]) || isalpha(tmp[i])) {
							_length++;
							words += tolower(tmp[i]);
						}
						else {
							if (words != " " && words != "") {
								_content_map[words].insert(_mid);
								words = "";
							}
						}
					}
					if (!words.empty()) {
						_content_map[words].insert(_mid);
						words = "";
					}
				}
				topic_count++;
			}
			for (int i = 0; i < _subject.size(); i++) {
				if (isdigit(_subject[i]) || isalpha(_subject[i])) {
					words += tolower(_subject[i]);
				}
				else {
					if (words != " " && words != "") {
						_content_map[words].insert(_mid);
						words = "";
					}
				}
			}
			if (!words.empty()) {
				_content_map[words].insert(_mid);
				words = "";
			}
			mail.close();
		}
	}
	void SetDate(string date) {
		vector<string> vdata = split(date, ' ');
		if (vdata[0].size() == 1) vdata[0].insert(0, "0");
		string month = DetermineMonth(vdata[1]);
		vector<string> time = split(vdata[4], ':');
		_date = stod(vdata[2] + month + vdata[0] + time[0] + time[1]);
	}
	int GetID() {
		return _mid;
	}
	int GetLength() {
		return _length;
	}

	friend class MailPool;
};

class MailPool {
private:
	int _num_mail;
	map<int, MailNode*> _mid_tree;
	set<int> _all_mid;
	map<string, set<int>> _sender_tree;
	map<string, set<int>> _content_map;
	map<string, set<int>> _receiver_tree;
	RBTree<MailNode*, double> _date_tree;
	RBTree<MailNode*, int> _length_tree;
public:
	MailPool() :_num_mail(0) {};

	void Add(string ext_path) {
		MailNode* nmail = new MailNode;
		nmail->SetData(ext_path, _mid_tree, _content_map);
		if (nmail->_receiver.empty()) {
			delete nmail;
			printf("-\n");
			return;
		}
		_num_mail++;

		//adding new node into _mid_tree
		_mid_tree.insert(pair<int, MailNode*>(nmail->_mid, nmail));
		_all_mid.insert(nmail->_mid);
		//add new node into _sender_tree
		_sender_tree[nmail->_sender].insert(nmail->_mid);
		_receiver_tree[nmail->_receiver].insert(nmail->_mid);
		_date_tree.insert(nmail, nmail->_date);
		_length_tree.insert(nmail, nmail->_length);

		printf("%d\n", _num_mail);
	}
	void Remove(string mid) {
		int digimid = stoi(mid);
		Remove(digimid);
	}
	void Remove(int& mid) {
		map<int, MailNode*>::iterator mitr = _mid_tree.find(mid);
		if (mitr == _mid_tree.end()) {
			printf("-\n");
			return;
		}
		_num_mail--;

		MailNode* tar = mitr->second;
		SenderRemove(tar);
		ReceiverRemove(tar);
		ContentRemove(tar);
		_date_tree.deletebyVal(tar, tar->_date);
		_length_tree.deletebyVal(tar, tar->_length);
		_mid_tree.erase(mid);
		_all_mid.erase(mid);
		delete tar;
		printf("%d\n", _num_mail);
	}
	void ContentRemove(MailNode* tar) {
		ifstream mail(tar->_path);
		string tmp;
		int count = 0;
		while (getline(mail, tmp)) {
			if (count == 3 || count >= 6) {
				string words;
				for (int i = 0; i < tmp.size(); i++) {
					if (isdigit(tmp[i]) || isalpha(tmp[i])) {
						words += tolower(tmp[i]);
					}
					else {
						if (words != " " && words != "") {
							_content_map[words].erase(tar->_mid);
						}
						words = "";
					}
				}
				if (!words.empty()) {
					_content_map[words].erase(tar->_mid);
					words = "";
				}
			}
			count++;
		}
	}
	void SenderRemove(MailNode* tar) {
		_sender_tree[tar->_sender].erase(tar->_mid);
	}
	void ReceiverRemove(MailNode* tar) {
		_receiver_tree[tar->_receiver].erase(tar->_mid);
	}
	void Longest() {
		if (_length_tree.empty()) {
			printf("-\n");
			return;
		}
		RBNode<MailNode*, int>* itr = _length_tree.getLargest();
		int len_ans = itr->getKey();
		int ans = itr->getValue()->_mid;
		while (itr != nullptr) {
			if (ans > itr->getValue()->_mid) ans = itr->getValue()->_mid;
			itr = itr->getNext();
		}
		printf("%d %d\n", ans, len_ans);
	}
	void Query(string tar) {
		vector<string> command = tolower_split(tar, ' ');

		if (command.size() == 1) QueryCase1(command); //case1
		else if (command.size() == 4) QueryCase6(command); //case6
		else if (command.size() == 2) {
			if (command[0][1] == 'd') QueryCase2(command); //case2
			else QueryCase3(command);//case3
		}
		else if (command.size() == 3) {
			if (command[1][1] == 'd') QueryCase4(command); //case4
			else QueryCase5(command);  //case5
		}
	}
	void QueryWord(string tar) {
		set<int> a = _content_map[tar];
		if (a.empty()) printf("-\n");
		else {
			set<int>::iterator itr;
			for (itr = a.begin(); itr != --a.end(); itr++) printf("%d ", *itr);
			printf("%d\n", *itr);
		}
	}
	void QueryCase1(vector<string>& command) {
		set<int> Ans = ExpFound(command.back(), _content_map, _all_mid);
		if (Ans.empty()) printf("-\n");
		else {
			set<int>::iterator itr;
			for (itr = Ans.begin(); itr != --Ans.end(); itr++) printf("%d ", *itr);
			printf("%d\n", *itr);
		}
	}
	void QueryCase2(vector<string>& command) {
		vector<double> time_boundary = DateBoundary(command[0]);
		set<int> Ans;
		set<int> PreAns = ExpFound(command.back(), _content_map, _all_mid);
		_date_tree.InorderTime(Ans, PreAns, time_boundary[1], time_boundary[0], _date_tree.getRoot());
		if (Ans.empty()) printf("-\n");
		else {
			set<int>::iterator itr;
			for (itr = Ans.begin(); itr != --Ans.end(); itr++) printf("%d ", *itr);
			printf("%d\n", *itr);
		}
	}
	void QueryCase3(vector<string>& command) {
		set<int> tar;
		if (command[0][1] == 'f') {
			tar = QueryFrom(command[0].substr(3, command[0].size() - 4));
			if (tar.empty()) {
				printf("-\n"); return;
			}
		}
		else if (command[0][1] == 't') {
			tar = QueryTo(command[0].substr(3, command[0].size() - 4));
			if (tar.empty()) {
				printf("-\n"); return;
			}
		}
		set<int> PreAns = ExpFound(command.back(), _content_map, _all_mid);
		set<int> Ans;
		set_intersection(PreAns.begin(), PreAns.end(), tar.begin(), tar.end(), inserter(Ans, Ans.begin()));
		if (Ans.empty()) printf("-\n");
		else {
			set<int>::iterator itr;
			for (itr = Ans.begin(); itr != --Ans.end(); itr++) printf("%d ", *itr);
			printf("%d\n", *itr);
		}
	}
	void QueryCase4(vector<string>& command) {
		set<int> tar;
		vector<string> ft_split = split(command[0], '\"');
		vector<double> time_boundary = DateBoundary(command[1]);

		//determine from or to
		if (ft_split[0][1] == 'f') {
			tar = QueryFrom(ft_split[1]);
			if (tar.empty()) {
				printf("-\n"); return;
			}
		}
		else if (ft_split[0][1] == 't') {
			tar = QueryTo(ft_split[1]);
			if (tar.empty()) {
				printf("-\n"); return;
			}
		}
		set<int> PreAns = ExpFound(command.back(), _content_map, _all_mid);
		set<int> Ans_nodate;
		set<int> Ans;
		set_intersection(PreAns.begin(), PreAns.end(), tar.begin(), tar.end(), inserter(Ans_nodate, Ans_nodate.begin()));
		_date_tree.InorderTime(Ans, Ans_nodate, time_boundary[1], time_boundary[0], _date_tree.getRoot());
		if (Ans.empty()) printf("-\n");
		else {
			set<int>::iterator itr;
			for (itr = Ans.begin(); itr != --Ans.end(); itr++) printf("%d ", *itr);
			printf("%d\n", *itr);
		}
	}
	void QueryCase5(vector<string>& command) {

		set<int> f_tar, t_tar, tar;
		vector<string> f_split = split(command[0], '\"');
		vector<string> t_split = split(command[1], '\"');

		//determine from
		f_tar = QueryFrom(f_split[1]);
		if (f_tar.empty()) {
			printf("-\n");
			return;
		}
		t_tar = QueryTo(t_split[1]);
		if (t_tar.empty()) {
			printf("-\n");
			return;
		}
		set_intersection(f_tar.begin(), f_tar.end(), t_tar.begin(), t_tar.end(), inserter(tar, tar.begin()));
		set<int> PreAns = ExpFound(command.back(), _content_map, _all_mid);
		set<int> Ans;
		set_intersection(PreAns.begin(), PreAns.end(), tar.begin(), tar.end(), inserter(Ans, Ans.begin()));
		if (Ans.empty()) printf("-\n");
		else {
			set<int>::iterator itr;
			for (itr = Ans.begin(); itr != --Ans.end(); itr++) printf("%d ", *itr);
			printf("%d\n", *itr);
		}
	}
	void QueryCase6(vector<string>& command) {
		set<int> f_tar, t_tar, tar;
		vector<string> f_split = split(command[0], '\"');
		vector<string> t_split = split(command[1], '\"');
		vector<double> time_boundary = DateBoundary(command[2]);
		f_tar = QueryFrom(f_split[1]);
		if (f_tar.empty()) {
			printf("-\n");
			return;
		}
		t_tar = QueryTo(t_split[1]);
		if (t_tar.empty()) {
			printf("-\n");
			return;
		}
		set_intersection(f_tar.begin(), f_tar.end(), t_tar.begin(), t_tar.end(), inserter(tar, tar.begin()));
		set<int> PreAns = ExpFound(command.back(), _content_map, _all_mid);
		set<int> Ans_nodate;
		set<int> Ans;
		set_intersection(PreAns.begin(), PreAns.end(), tar.begin(), tar.end(), inserter(Ans_nodate, Ans_nodate.begin()));
		_date_tree.InorderTime(Ans, Ans_nodate, time_boundary[1], time_boundary[0], _date_tree.getRoot());
		if (Ans.empty()) printf("-\n");
		else {
			set<int>::iterator itr;
			for (itr = Ans.begin(); itr != --Ans.end(); itr++) printf("%d ", *itr);
			printf("%d\n", *itr);
		}
	}
	set<int>& QueryFrom(string key) {
		return _sender_tree[key];
	}
	set<int>& QueryTo(string key) {
		return _receiver_tree[key];
	}
};


#endif
