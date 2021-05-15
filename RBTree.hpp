
#ifndef RBTree_hpp
#define RBTree_hpp

#include <iostream>
#include <queue>
#include <string>
//#include "ACTree.hpp"
//#include "Calculator.hpp"

using namespace std;

enum COLOR{RED, BLACK};

template <typename Elem, typename KEY>
class RBTree;
template <typename Elem, typename KEY>
class RBNode;

template <typename Elem, typename KEY>
class RBNode {
private:
	Elem val;
	KEY key;
	COLOR color;
	RBNode* left, * right, * parent;
	RBNode* next;
public:
	RBNode(Elem tval, KEY tkey){
		val = tval;
		key = tkey;
		parent = left = right = NULL;
		next = nullptr;
		color = RED;
	}

	RBNode* uncle(){
		if (parent == NULL or parent->parent == NULL)
			return NULL;
		if (parent->isOnLeft())
			return parent->parent->right;
		else
			return parent->parent->left;
	}

	RBNode* sibling(){
		if (parent == NULL)
			return NULL;
		if (isOnLeft())
			return parent->right;
		return parent->left;
	}

	RBNode* getNext(){
		return next;
	}

	void movedDown(RBNode* nParent){
		if (parent != NULL)
		{
			if (isOnLeft()) {
				parent->left = nParent;
			}
			else
			{
				parent->right = nParent;
			}
		}
		nParent->parent = parent;
		parent = nParent;
	}
	
	bool isOnLeft(){
		return this == parent->left;
	}

	bool hasRedChild(){
		return (left != NULL and left->color == RED) or (right != NULL and right->color == RED);
	}

	Elem getValue(){
		return val;
	}
	
	KEY getKey(){
		return key;
	}

	friend class RBTree<Elem, KEY>;
};

template <typename Elem, typename KEY>
class RBTree {
private:
	RBNode<Elem, KEY>* root;

	void leftRotate(RBNode<Elem, KEY>* x){
		RBNode<Elem, KEY>* Par = x->right;
		if (x == root)
			root = Par;
		x->movedDown(Par);
		x->right = Par->left;
		if (Par->left != NULL)
			Par->left->parent = x;
		Par->left = x;
	}

	void rightRotate(RBNode<Elem, KEY>* x){
		RBNode<Elem, KEY>* par = x->left;
		if (x == root)
			root = par;
		x->movedDown(par);
		x->left = par->right;
		if (par->right != NULL)
			par->right->parent = x;
		par->right = x;
	}

	void swapColors(RBNode<Elem, KEY>* x, RBNode<Elem, KEY>* y){
		COLOR temp;
		temp = x->color;
		while (x != nullptr) {
			x->color = y->color;
			x = x->next;
		}
		while (y != nullptr) {
			y->color = temp;
			y = y->next;
		}
	}

	void swapValues(RBNode<Elem, KEY>* x, RBNode<Elem, KEY>* y){
		Elem Etemp;
		KEY Ktemp;
		RBNode<Elem, KEY>* Ntemp;
		Etemp = x->val;
		Ktemp = x->key;
		Ntemp = x->next;
		x->val = y->val;
		x->key = y->key;
		x->next = y->next;
		y->val = Etemp;
		y->key = Ktemp;
		y->next = Ntemp;
	}

	void fixRedRed(RBNode<Elem, KEY>* x){
		if (x == root)
		{
			x->color = BLACK;
			return;
		}
		RBNode<Elem, KEY>* parent = x->parent, * grandparent = parent->parent, * uncle = x->uncle();

		if (parent->color != BLACK)
		{
			if (uncle != NULL && uncle->color == RED)
			{
				parent->color = BLACK;
				uncle->color = BLACK;
				grandparent->color = RED;
				fixRedRed(grandparent);
			}
			else
			{
				if (parent->isOnLeft())
				{
					if (x->isOnLeft())
					{
						swapColors(parent, grandparent);
					}
					else
					{
						leftRotate(parent);
						swapColors(x, grandparent);
					}
					rightRotate(grandparent);
				}
				else
				{
					if (x->isOnLeft())
					{
						rightRotate(parent);
						swapColors(x, grandparent);
					}
					else
					{
						swapColors(parent, grandparent);
					}
					leftRotate(grandparent);
				}
			}
		}
	}
	
	RBNode<Elem, KEY>* successor(RBNode<Elem, KEY>* x){
		RBNode<Elem, KEY>* temp = x;
		while (temp->left != NULL)
			temp = temp->left;
		return temp;
	}
	
	RBNode<Elem, KEY>* BSTreplace(RBNode<Elem, KEY>* x){
		if (x->left != NULL and x->right != NULL)
			return successor(x->right);
		if (x->left == NULL and x->right == NULL)
			return NULL;
		if (x->left != NULL)
			return x->left;
		else
			return x->right;
	}

	void deleteNode(RBNode<Elem, KEY>* x){
		RBNode<Elem, KEY>* u = BSTreplace(x);
		bool uvBlack = ((u == NULL or u->color == BLACK) and (x->color == BLACK));
		RBNode<Elem, KEY>* par = x->parent;
		if (u == NULL)
		{
			if (x == root)
			{
				root = NULL;
			}
			else
			{
				if (uvBlack)
				{
					fixDoubleBlack(x);
				}
				else
				{
					if (x->sibling() != NULL)
						x->sibling()->color = RED;
				}
				if (x->isOnLeft())
				{
					par->left = NULL;
				}
				else
				{
					par->right = NULL;
				}
			}
			delete x;
			return;
		}
		if (x->left == NULL or x->right == NULL)
		{
			if (x == root)
			{
				swapValues(x, u);
				x->left = x->right = NULL;
				delete u;
			}
			else
			{
				if (x->isOnLeft())
				{
					par->left = u;
				}
				else
				{
					par->right = u;
				}
				delete x;
				u->parent = par;
				if (uvBlack)
				{
					fixDoubleBlack(u);
				}
				else
				{
					u->color = BLACK;
				}
			}
			return;
		}
		swapValues(u, x);
		deleteNode(u);
	}

	void deleteHead(RBNode<Elem, KEY>* x){
		RBNode<Elem, KEY>* tar = x->next;
		if (x != root) {
			if (x->parent->left == x) x->parent->left = tar;
			else x->parent->right = tar;
		}
		else {
			root = x->next;
		}
		if (x->left != nullptr) x->left->parent = tar;
		if (x->right != nullptr) x->right->parent = tar;

		tar->parent = x->parent;
		tar->left = x->left;
		tar->right = x->right;
		tar->color = x->color;
		
		x = nullptr;
	}

	void deleteBody(RBNode<Elem, KEY>* x){
		RBNode<Elem, KEY>* tar = x->next;
		x->next = tar->next;
		tar = nullptr;
	}

	void fixDoubleBlack(RBNode<Elem, KEY>* x){
		if (x == root)
			return;
		RBNode<Elem, KEY>* sibling = x->sibling(), * parent = x->parent;
		if (sibling == NULL)
		{
			fixDoubleBlack(parent);
		}
		else
		{
			if (sibling->color == RED)
			{
				parent->color = RED;
				sibling->color = BLACK;
				if (sibling->isOnLeft())
				{
					rightRotate(parent);
				}
				else
				{
					leftRotate(parent);
				}
				fixDoubleBlack(x);
			}
			else
			{
				if (sibling->hasRedChild())
				{
					if (sibling->left != NULL and sibling->left->color == RED)
					{
						if (sibling->isOnLeft())
						{
							sibling->left->color = sibling->color;
							sibling->color = parent->color;
							rightRotate(parent);
						}
						else
						{
							sibling->left->color = parent->color;
							rightRotate(sibling);
							leftRotate(parent);
						}
					}
					else
					{
						if (sibling->isOnLeft())
						{
							sibling->right->color = parent->color;
							leftRotate(sibling);
							rightRotate(parent);
						}
						else
						{
							sibling->right->color = sibling->color;
							sibling->color = parent->color;
							leftRotate(parent);
						}
					}
					parent->color = BLACK;
				}
				else
				{
					sibling->color = RED;
					if (parent->color == BLACK)
						fixDoubleBlack(parent);
					else
						parent->color = BLACK;
				}
			}
		}
	}
	//void levelOrder(RBNode*);
	//void inorder(RBNode*);
	//void inorderSearch(RBNode*, string);
public:
	RBTree() { root = NULL; }

	RBNode<Elem, KEY>* getRoot(){
		return root;
	}

	RBNode<Elem, KEY>* search(Elem tar, KEY n){
		RBNode<Elem, KEY>* temp = root;
		while (temp != nullptr)
		{
			if (n < temp->key)
			{
				if (temp->left == nullptr)
					break;
				else
					temp = temp->left;
			}
			else if (n == temp->key)
				break;
			else
			{
				if (temp->right == nullptr)
					break;
				else
					temp = temp->right;
			}
		}
		while (temp != nullptr) {
			if (temp->val == tar) break;
			temp = temp->next;
		}
		return temp;
	}

	RBNode<Elem, KEY>* search(KEY n){
		RBNode<Elem, KEY>* temp = root;
		while (temp != nullptr)
		{
			if (n < temp->key)
			{
				if (temp->left == nullptr)
					break;
				else
					temp = temp->left;
			}
			else if (n == temp->key)
				break;
			else
			{
				if (temp->right == nullptr)
					break;
				else
					temp = temp->right;
			}
		}
		return temp;
	}
	void insert(Elem n, KEY k){
		RBNode<Elem, KEY>* newNode = new RBNode<Elem, KEY>(n, k);
		if (root == NULL)
		{
			newNode->color = BLACK;
			root = newNode;
		}
		else
		{
			RBNode<Elem, KEY>* temp = search(k);
			if (temp->key == k) {
				while (temp->next != nullptr) {
					temp = temp->next;
				}
				temp->next = newNode;
				return;
			}
			newNode->parent = temp;
			if (k < temp->key)
				temp->left = newNode;
			else
				temp->right = newNode;

			fixRedRed(newNode);
		}
	}

	void deletebyVal(Elem n, KEY k){
		if (root == NULL) return;

		RBNode<Elem, KEY>* v = search(k);
		if (v->key != k)
		{
			cout << "KEY not found: " << k << endl;
			cout << "Node not found: " << n->GetID() << endl;
			return;
		}
		if (v->val == n) {
			if (v->next != nullptr) deleteHead(v);
			else deleteNode(v);
		}
		else {
			RBNode<Elem, KEY>* vp = nullptr;
			while (v != nullptr) {               
				if (v->val == n) break;
				vp = v;
				v = v->next;
			}
			if (v == nullptr) {               
				cout << "KEY found: " << k << endl;
				cout << "Node not found: " << n->GetID() << endl;
			}
			else deleteBody(vp);
		}
	}
	
	 void InorderTime(set<int>& Ans, set<int>& PreAns, double& higher, double& lower, RBNode<Elem, KEY>* x){
		if (x != nullptr) {
			if (x->key > lower) InorderTime(Ans, PreAns, higher, lower, x->left);
			if (x->key >= lower && x->key <= higher) {
				RBNode<Elem, KEY>* itr = x;
				while (itr != nullptr) {
					set<int>::iterator it = PreAns.find(itr->val->GetID());
					if (it != PreAns.end()) Ans.insert(itr->val->GetID());
					itr = itr->next;
				}
			}
			if (x->key < higher) InorderTime(Ans, PreAns, higher, lower, x->right);
		}
	}
	
	RBNode<Elem, KEY>* getLargest(){
		RBNode<Elem, KEY>* itr = root;
    	while (itr->right != nullptr) itr = itr->right;
    	return itr;
	}
	
	bool empty(){
		if (root == nullptr) return true;
    	else return false;
	}

};
#endif
