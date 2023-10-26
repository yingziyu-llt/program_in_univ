/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include<stdio.h>
#include<algorithm>
#include<string>
#include<map>
#include<iostream>
using namespace std;

map <string,int> mp;

string split_val(string s2)
{
	int iter = 0;
	string s3 = "";
	for(iter;iter < s2.length();iter++)
	{
		if(s2[iter] == '(')
			break;
		s3 = s3 + s2[iter];
	}
	return s3;
}
int main()
{
#ifdef llt
	freopen("LuoguP9680.in","r",stdin);
	freopen("LuoguP9680.out","w",stdout);
#endif
	int L;
	scanf("%d",&L);
	while(L--)
	{
		string s1;
		cin >> s1;
		if(s1 == "string")
		{
			string s2;
			cin >> s2;
			string name = split_val(s2);

		}
		else
		{
		}
	}
}

