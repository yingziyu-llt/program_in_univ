/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */

#include <future>
#include <iostream>
#include <memory>
using namespace std;

class Node {
public:
	shared_ptr<Node> lson, rson;
	int data;
	Node(int t)
		: data(t)
		, lson(nullptr)
		, rson(nullptr)
	{
	}
};

void insert(int t, shared_ptr<Node> root)
{
	if (t == root->data)
		return;
	if (t > root->data) {
		if (root->rson == nullptr)
			root->rson.reset(new Node(t));
		else
			insert(t, root->rson);
	}
	if (t < root->data) {
		if (root->lson == nullptr)
			root->lson.reset(new Node(t));
		else
			insert(t, root->lson);
	}
}

void print(shared_ptr<Node> root)
{
	cout << root->data << ' ';
	if (root->lson != nullptr)
		print(root->lson);
	if (root->rson != nullptr)
		print(root->rson);
}

int main()
{
	int xx;
	cin >> xx;
	shared_ptr<Node> root(new Node(xx));

	while (cin >> xx) {
		insert(xx, root);
	}
	print(root);
}
