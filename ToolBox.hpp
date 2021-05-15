//
//  ToolBox.hpp
//  useful toolbox for multiple use
//
//  Created by Chiaming Liang on 01/06/2020.
//  Copyright © 2020 Chiaming Liang. All rights reserved.
//

#ifndef ToolBox_hpp
#define ToolBox_hpp
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

vector<string> split(const string& tar, const char& delim) {
    vector<string> ans;
    if (tar == "") { return ans; }
    stringstream tarp(tar);
    string token;
    while (getline(tarp, token, delim)) {
        ans.push_back(token);
    }
    return ans;
}

vector<string> tolower_split(const string& tar, const char& delim) {
    vector<string> ans;
    if (tar == "") { return ans; }
    stringstream tarp(tar);
    string token;
    while (getline(tarp, token, delim)) {
        transform(token.begin(), token.end(), token.begin(), ::tolower);
        ans.push_back(token);
    }
    return ans;
}

vector<string> expression_split(const string& tar) {
    vector<string> ans;
    for (int i = 0; i < tar.size(); i++) {
        if (!isalpha(tar[i]) && !isdigit(tar[i])) {
            if (ans.empty()) ans.push_back("");
            if (!ans.back().empty()) ans.push_back("");
        }
        else {
            if (ans.empty()) ans.push_back("");
            ans.back() = ans.back() + tar[i];
        }
    }
    if (ans.back().empty()) ans.pop_back();
    return ans;
}
#endif