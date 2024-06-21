/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;
string s1, s2;

class Node {
public:
	shared_ptr<Node> lson, rson;
	char data;
	Node(char c)
		: data(c)
	{
	}
};

shared_ptr<Node> find_root(int f1, int e1, int f2, int e2)
{
	if (e2 < f2 || e1 < f1)
		return nullptr;
	int c = s1[f1];
	shared_ptr<Node> curr(new Node(c));
	if (e2 <= f2 || e1 <= f1)
		return curr;
	for (int i = f2; i <= e2; i++) {
		if (s2[i] == c) {
			curr->lson = find_root(f1 + 1, f1 + i - f2, f2, i - 1);
			curr->rson = find_root(f1 + 1 + i - f2, e1, i + 1, e2);
			break;
		}
	}
	return curr;
}

void cur(shared_ptr<Node> root)
{
	if (root->lson != nullptr)
		cur(root->lson);
	if (root->rson != nullptr)
		cur(root->rson);
	printf("%c", root->data);
	return;
}

int main()
{
	while (cin >> s1 >> s2) {
		auto root = find_root(0, s1.length() - 1, 0, s2.length() - 1);
		cur(root);
		puts("");
	}
	return 0;
}
