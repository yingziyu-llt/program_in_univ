/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
#include<stack>
using namespace std;

char s[100];
stack <long long> st;

long long to_int()
{
	long long len = strlen(s);
	long long a = 0;
	for(int i = 0;i < len;i++)
		a = a * 10 + s[i] - '0';
	return a;
}

int main()
{
#ifdef llt
	freopen("1.in","r",stdin);
	freopen("1.out","w",stdout);
#endif
	while(~scanf("%s",s))
	{
		bool flag = false;
		while(1)
		{
			if(!flag)
			{
				if(s[0] >= '0' && s[0] <= '9')
				{
					long long xx = to_int();
					st.push(xx);
				}
				else if(s[0] == '+')
				{
					long long a = st.top();
					st.pop();
					long long b = st.top();
					st.pop();
					st.push(a + b);
				}
				else if(s[0] == '-')
				{
					long long a = st.top();
					st.pop();
					long long b = st.top();
					st.pop();
					st.push(b - a);
				}
				else if(s[0] == '*')
				{
					long long a = st.top();
					st.pop();
					long long b = st.top();
					st.pop();
					st.push(a * b);
				}
				else if(s[0] == '/')
				{
					long long a = st.top();
					st.pop();
					long long b = st.top();
					st.pop();
					if(a == 0)
					{
						puts("NaN");
						flag = true;
					}
					else
						st.push(b / a);
				}
				else if(s[0] == '@')
					break;
			}
			scanf("%s",s);
			if(s[0] == '@') break;
		}
		if(!flag)
			printf("%lld\n",st.top());
		fflush(stdout);
		while(!st.empty())
			st.pop();
	}  
}

