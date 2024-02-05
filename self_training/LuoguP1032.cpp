/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future!
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
#include<string>
#include<queue>
#include<iostream>
#include<map>
using namespace std;

string a,b;
string op[2][10];
int op_cnt = 1;
queue <pair<string,pair<int,int> > > q;
map <string,int> mp;

string tr(string s,int rule,int pos,int type)
{
	string ans = "";
	int xx = s.find(op[type][rule],pos);
	if(xx == -1) return ans;
	ans = s.substr(0,xx) + op[1 - type][rule] + s.substr(xx + op[type][rule].length(),s.length());
	return ans;
}

int main()
{
#ifdef llt
	freopen("LuoguP1032.in","r",stdin);
	freopen("LuoguP1032.out","w",stdout);
#endif
	cin >> a >> b;
	while(cin >> op[0][op_cnt] >> op[1][op_cnt])
		op_cnt++;
	q.push(make_pair(a,make_pair(-1,-1)));
	q.push(make_pair(b,make_pair(1,1)));
	int ans = 0;
	while(!q.empty())
	{
		string cur = q.front().first;
		int type = q.front().second.second,step = q.front().second.first;
		q.pop();
		if(step >= 6) break;
		for(int i = 1;i < op_cnt;i++)
		{
			for(int j = 0;j < (int)cur.length();j++)
			{
				string trans = tr(cur,i,j,type == 1 ? 1 : 0);
				if(trans != "")
				{
					if(mp.find(trans) == mp.end())
					{
						mp[trans] = step + type;
						q.push(make_pair(trans,make_pair(step + type,type)));
					}
					else if(mp[trans] * step < 0)
					{
						ans = abs(mp[trans] - step);
						break;
					}
				}
				else break;
			}
			if(ans != 0) break;
		}
		if(ans != 0) break;
	}
	if(ans == 0)
		printf("NO ANSWER!");
	else
		printf("%d",ans - 1);
}

