
#ifndef Calculatorset_hpp
#define Calculatorset_hpp

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <algorithm>
using namespace std;

class op {
public:
	op(const char& t) {
		symb = t;
		prec = 0;
	}
	op(const char* t, const int& p) {
		symb = t;
		prec = p;
	}
	op(const char& t, const int& p) {
		symb = ""; symb += t;
		prec = p;
	}
	void operator=(const op& b) {
		symb = b.symb;
		prec = b.prec;
	}
	bool operator<(const op& b) const {
		if (prec > b.prec) { return true; }
		return false;
	}
	bool operator>(const op& b) const {
		if (prec < b.prec) { return true; }
		return false;
	}
	bool operator==(const op& b) const {
		if (prec == b.prec) { return true; }
		return false;
	}
	bool operator==(const int& b) const {
		if (prec == b) { return true; }
		return false;
	}
	bool operator==(const char* b) const {
		if (symb == b) { return true; }
		return false;
	}

	string symb;
	int prec;
};

set<int> eval(set<int>& a, set<int>& b, const op& opr) {
	set<int> ans;
	if (opr == "&") { 
		set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(ans, ans.begin()));
	}
	if (opr == "|") {
		set_union(a.begin(), a.end(), b.begin(), b.end(), inserter(ans, ans.begin()));
	}
	if (opr == "!") {
		set_difference(b.begin(), b.end(), a.begin(), a.end(),  inserter(ans, ans.begin()));
	}
	return ans;
}
op PrecImplent(int& i, const string& infixexp, stack<op>& post) {
	if (infixexp[i] == '!') { op opr(infixexp[i], 2); return opr; }
	else if (infixexp[i] == '&') { op opr(infixexp[i], 7); return opr; }
	else if (infixexp[i] == '|') { op opr(infixexp[i], 9); return opr; }
}
bool ToBool(op& tar) {
	if (stoi(tar.symb) == 1) return true;
	else return false;
}

vector<op> IntoPost(string& infixexp) {
	stack<op> post;
	stack<int> pmark;												//mark of the existing parentheses
	vector<op> postfixexp;
	bool dswitch = true;											//if needed to create a new <op> for number

	pmark.push(0);													//view the whole input as a parentheses
	int bool_num = 0;
	for (int i = 0; i < infixexp.length(); i++) {					//process each character
		if (isdigit(infixexp[i]) || isalpha(infixexp[i])) {
			if (dswitch) {											//create new <op>
				op d(infixexp[i]);
				postfixexp.push_back(d);
				bool_num++;
				dswitch = false;
			}
			else { postfixexp.back().symb += infixexp[i]; }
		}
		else {
			dswitch = true;											//turn on dswitch (encounter operator)
			if (infixexp[i] == '(') pmark.push(post.size());
			else if (infixexp[i] == ')') {
				while (post.size() > pmark.top()) {
					postfixexp.push_back(post.top());
					post.pop();
				}
				pmark.pop();
			}
			else {
				op opr = PrecImplent(i, infixexp, post);			//implemt precedence
				if (post.size() == pmark.top()) {
					post.push(opr);
				}
				else if (post.size() > pmark.top()) {
					if (opr > post.top()) {
						post.push(opr);
					}
					else if (opr == 2 && post.top() == 2) {
						post.push(opr);
					}
					else if (opr == post.top()) {
						postfixexp.push_back(post.top());
						post.pop(); post.push(opr);
					}
					else if (opr < post.top()) {
						while (post.size() > pmark.top()) {
							if (post.top() < opr) { break; }
							postfixexp.push_back(post.top());
							post.pop();
						}
						post.push(opr);
					}
				}
			}
		}
	}
	while (!post.empty()) {
		postfixexp.push_back(post.top());
		post.pop();
	}
	//for (int i = 0; i < postfixexp.size(); i++) {
	//	cout << postfixexp[i].symb << " ";
	//}
	//cout << endl;
	return postfixexp;
}
set<int> PosttoAns(vector<op>& postfixexp, map<string, set<int>>& _content_map, set<int> _all_mid) {
	stack<set<int>> found;
	for (int i = 0; i < postfixexp.size(); i++) {
		if (postfixexp[i] == 0) {
			set<int> tmp;
			map<string, set<int>>::iterator itr = _content_map.find(postfixexp[i].symb);
			if (itr == _content_map.end()) found.push(tmp);
			else found.push(_content_map[postfixexp[i].symb]);
		}
		else {
			if (postfixexp[i] == 2) {
				set<int> c(eval(found.top(), _all_mid, postfixexp[i]));
				found.pop(); found.push(c);
			}
			else {
				set<int> b = found.top(); found.pop();
				set<int> a = found.top(); found.pop();
				found.push(eval(a, b, postfixexp[i]));
			}
		}
	}
	return found.top();
}
set<int> ExpFound(string& expression, map<string, set<int>>& _content_map, set<int>& _all_mid) {
	vector<op> post = IntoPost(expression);
	return PosttoAns(post, _content_map, _all_mid);
}
#endif