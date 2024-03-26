/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */

#include <algorithm>
#include <stdio.h>
#include <string.h>
using namespace std;

int main()
{
#ifdef llt
	freopen("1.in", "r", stdin);
	freopen("1.out", "w", stdout);
#endif
	char s[100], s2[100];
	while (~scanf("%s %s", s, s2)) {
		int len1 = strlen(s);
		char maxn = 0;
		for (int i = 0; i < len1; i++)
			maxn = max(maxn, s[i]);
		char newstr[200];
		memset(newstr, 0, 200);
		bool flag = false;
		int cnt = 0;
		for (int i = 0; i < len1; i++) {
			newstr[cnt++] = s[i];
			if (!flag && s[i] == maxn) {
				flag = true;
				for (int j = 0; j < strlen(s2); j++) {
					newstr[cnt++] = s2[j];
				}
			}
		}
		newstr[cnt] = 0;
		puts(newstr);
	}
}
