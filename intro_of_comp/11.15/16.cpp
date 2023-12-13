/*
 * Copyright (C) 2023 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 * 
 * Run for our better future
 */

#include <cmath>
#include<stdio.h>
#include<algorithm>
#include<string.h>
#include<math.h>
using namespace std;


int main()
{
#ifdef llt
	freopen("16.in","r",stdin);
	freopen("16.out","w",stdout);
#endif
	long long n;
	scanf("%lld",&n);
	int sng = n / abs(n);
	n = abs(n);
	int len = 0;
    long long temp = n;
    while (temp > 0) {
        temp /= 10;
        len++;
    }
	long long maxn = -0x7fffffff;
	for(int i = 1;i < len;i++)
		for(int j = i + 1;j < len;j++)
		{
			long long cpn = n;
			long long st1 = pow(10,i),st2 = pow(10,j - i);
			long long a = n % st1;
			n /= st1;
			long long b = n % st2;
			n /= st2;
			maxn = max(maxn,sng * n * a * b);
			n = cpn;
		}
	printf("%lld",maxn);
}

