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
using namespace std;

class BigNumber
{
	public:
		int data[10001];
		int len;
		long long pow_10[11]; 
		int fast_pow_10(int x) // a^x
		{
			int res = 1;
			for(int i = 1;i <= x;i++)
				res *= 10;
			return res;
		}
		void init(int n)
		{
			pow_10[0] = 1;
			memset(data,0,sizeof(data));
			for(int i = 0;i <= 9;i++)
				data[i] = n / (int)fast_pow_10(i) % 10;
			len = 10;
		}
		void init(char *s) // init Bignumber from a string (count from 0)
		{
			memset(data,0,sizeof(data));
			for(int i = 0;i < (int)strlen(s);i++)
				data[strlen(s) - i - 1] = s[i] - '0';
			len = strlen(s) - 1;
		}
		void operator= (const int a)           {  init(a); }
		bool operator> (const BigNumber a)
		{
			if(len > a.len) return true;
			if(len < a.len) return false;
			for(int i = len;i >= 0;i++)
			{
				if(data[i] == a.data[i]) continue;
				if(data[i] > a.data[i]) return true;
				return false;
			}
			return false;
		}
		bool operator< (const BigNumber a)
		{
			if(len < a.len) return true;
			if(len > a.len) return false;
			for(int i = len;i >= 0;i++)
			{
				if(data[i] == a.data[i]) continue;
				if(data[i] < a.data[i]) return true;
				return false;
			}
			return false;
	
		}
		BigNumber operator+ (const BigNumber& a)
		{
			BigNumber res;
			res.init(0);
			for(int i = 0;i <= max(this -> len,a.len);i++)
				res.data[i] += this -> data[i] + a.data[i],data[i + 1] += res.data[i] / 10,res.data[i] %= 10;
			res.len = res.data[max(this->len,a.len) + 1] > 0 ? max(this->len,a.len) + 1 : max(this->len,a.len);
			return res;
		}
		BigNumber operator- (const BigNumber& a)
		{
			BigNumber res;
			res = 0;
			for(int i = 0;i <= len;i++)
			{
				res.data[i] = data[i];
				if(res.data[i] < a.data[i])
					res.data[i] += 10,res.data[i + 1] -= 1;
				res.data[i] -= a.data[i];
			}
			res.len = res.data[len] != 0 ? len : len - 1;
			return res;
		}
		BigNumber operator* (const BigNumber& a)
		{
			BigNumber res;
			res.init(0);
			for(int i = 0;i <= len;i++)
				for(int j = 0;j <= a.len;j++)
					res.data[i + j] += data[i] * a.data[j],res.data[i + j + 1] += res.data[i + j] / 10,res.data[i + j] %= 10;
			res.len = 10000;
			while(res.data[res.len] == 0)
				res.len--;
			return res;
		}
		BigNumber operator/ (const int a)
		{
			BigNumber res;
			res.init(0);
			int ret = 0;
			for(int i = len;i >= 0;i--)
			{
				ret = ret * 10 + data[i];
				res.data[i] = ret / a;
				ret %= a;
			}
			for(int i = len;i >= 0;i--)
				if(res.data[i] != 0)
				{
					res.len = i;
					return res;
				}
			return res;
		}
		BigNumber operator% (const int a)
		{
			BigNumber res;
			res = 0;
			for(int i = 0;i <= len;i++)
				res.data[i] = data[i];
			res.len = len;
			BigNumber a_cp;
			a_cp = a;
			return res - res / a * a_cp;
		}
};

BigNumber ksm(int x)
{
	BigNumber res,two;
	two.init(2);
	if(x <= 30)
		res.init(1 << x);
	else if(x & 1)
		res = two * ksm(x / 2) * ksm(x / 2); 
	else
		res = ksm(x / 2) * ksm(x / 2);
	return res;
}

int main()
{
#ifdef llt
	freopen("10.in","r",stdin);
	freopen("10.out","w",stdout);
#endif
	char s[110];
	memset(s,0,sizeof(s));
	scanf("%s",s);
	BigNumber a;
	a.init(s);
	BigNumber res = a / 13;
	while(res.len > 0 && res.data[res.len] == 0)
		res.len--;
	for(int i = res.len;i >= 0;i--)
		printf("%d",res.data[i]);
	puts("");
	res = a % 13;
	while(res.len > 0 && res.data[res.len] == 0)
		res.len--;
	for(int i = res.len;i >= 0;i--)
		printf("%d",res.data[i]);
}

