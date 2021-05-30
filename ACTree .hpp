//
//  ACTree.hpp
//  ACalgorithm
//
//  Created by Warron Yiang on 01/06/2020.
//  Copyright © 2020 Warron Yiang. All rights reserved.
//

#ifndef ACTree_hpp
#define ACTree_hpp

#include <stdio.h>
#include <algorithm>
#include <queue>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

struct Node
{
    int id;
    char x;
    bool end;
    Node *par;
    Node *next[35];
    Node *fail;
    Node()
    {
        id = -1;
        end = false;
        fail = nullptr;
        par = nullptr;
        for(int i =0 ; i < 35;i++)
        {
            next[i] = nullptr;
        }
    }
};

class ACtree
{
public:
    Node *root;
    ACtree() {root = new Node;root->fail = root;total = 0;}
    void insert(string key,int id);
    void NodeToQueue(Node *node,queue<Node*>&q);
    void BuildFailPointer();
    vector<bool> ACSearch(string key);
    int total;
};

void ACtree::insert(string key,int id)
{
    if(key.length()==0) return;
    Node *p = root;
    for(int i =0;i<key.length();i++)
    {
        int index = 0;
        if(isalpha(key[i]))
        {
            index = key[i] -'a';
        }
        else if(isdigit(key[i]))
        {
            index = (key[i] -'0') + 26;
        }
        if(p->next[index]==nullptr)
        {
            p->next[index] = new Node;
            p->next[index]->par = p;
            p->next[index]->x = key[i];
        }
        p = p->next[index];
    }
    total += 1;
//    p->end = true;
    p->id = id;
}

void ACtree::NodeToQueue(Node *node, queue<Node*> &q)
{
    if(node != nullptr)
    {
        for(int i =0;i<35;i++)
        {
            if(node->next[i]){
            q.push(node->next[i]);
            }
        }
    }
}

void ACtree::BuildFailPointer()
{
    queue<Node*> q;
    for(int i =0;i<35;i++)
    {
        if(root->next[i])
        {
            NodeToQueue(root->next[i], q);
            root->next[i]->fail = root;
        }
    }
    Node *par,*p;
    char ch;
    while(!q.empty())
    {
        p = q.front();
        ch = p->x;
        par = p->par;
        q.pop();
        NodeToQueue(p, q);
        while(1)
        {
            int index =  0;
            if(isalpha(ch))
            {
                index = ch-'a';
            }
            else if(isdigit(ch))
            {
                index = (ch-'0')+26;
            }
            if(par->fail->next[index]!=nullptr)
            {
                p->fail = par->fail->next[index];
                break;
            }
            else
            {
                if(par->fail == root)
                {
                    p->fail = root;
                    break;
                }
                else
                    par = par->fail->par;
            }
        }
    }
}

vector<bool> ACtree::ACSearch(string key)
{
    vector<bool>pair;
    pair.assign(total, false);
    if(key.length()==0) return pair;
    Node *p = root;
    int i = 0;
    while(i<key.length())
    {
       if(isalpha(key[i]) || isdigit(key[i]) || key[i] == ' ')
       {
            key[i] = tolower(key[i]);
            int index = 0;
            if(isalpha(key[i]))
            {
                index = key[i] - 'a';
            }
            else if(isdigit(key[i]))
            {
                index = (key[i] - '0') + 26;
            }
            if(p->next[index]!=nullptr)
            {
                p = p->next[index];
                if(p->id != -1)
                {
                    i++;
                    if(!isalpha(key[i]) && !isdigit(key[i]))
                    {
                        pair[p->id] = true;
                    }
                    i--;
                }
                i++;
            }
            else
            {
                if(p==root)
                {
                    i ++;
                }
                else
                {
                    p = p->fail;
                    if(p->id !=-1 )
                    {
                        i++;
                        if(!isalpha(key[i])&&!isdigit(key[i]))
                        {
                            pair[p->id] = true;
                        }
                        i--;
                    }
                }
            }
       }
        else
        {
            key[i] = ' ';
        }
    }
    while(p!=root)
    {
        p = p->fail;
        if(p->id != -1 )
        {
            pair[p->id] = true;
        }
    }
    return pair;
    
}
#endif /* ACTree_hpp */
