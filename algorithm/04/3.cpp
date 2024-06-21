/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */

#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <string.h>
using namespace std;
int nxt[20010];

int main()
{
	int T;
	scanf("%d", &T);
	while (T--) {
		char s1[20010], s2[20010];
		scanf("%s %s", s1, s2);
		int len1 = strlen(s1), len2 = strlen(s2);
		int k = -1;
		nxt[0] = -1;
		for (int i = 1; i < len2; i++) {
			while (k >= 0 && s2[i] != s2[k + 1])
				k = nxt[k];
			if (s2[i] == s2[k + 1])
				k++;
			nxt[i] = k;
		}
		k = -1;
		bool flag = 0;
		for (int i = 0; i < len1; i++) {
			while (k >= 0 && s1[i] != s2[k + 1])
				k = nxt[k];
			if (s1[i] == s2[k + 1])
				k++;
			if (k == len2 - 1) {
				printf("%d\n", i - k);
				flag = 1;
				break;
			}
		}
		if (!flag)
			printf("-1\n");
	}
}
