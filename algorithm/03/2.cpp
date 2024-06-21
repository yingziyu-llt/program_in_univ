/*
 * Copyright (C) 2024 Letian Lin <yingziyu-Lin@outlook.com>
 *
 * Distributed under terms of the GNU AGPLv3+ license.
 *
 * Run for our better future!
 */

#include <stdio.h>
#include <string.h>

int cor[100000];
int main()
{
#ifdef llt
	freopen("2.in", "r", stdin);
	freopen("2.out", "w", stdout);
#endif
	int n;
	scanf("%d", &n);
	for (int _i = 1; _i <= n; _i++) {
		memset(cor, 0, sizeof(cor));
		while (1) {
			int x, y;
			scanf("%d%d", &x, &y);
			if (y < 0)
				break;
			cor[y] = x;
		}
		while (1) {
			int x, y;
			scanf("%d%d", &x, &y);
			if (y < 0)
				break;
			cor[y] += x;
		}
		for (int i = 99999; i >= 0; i--)
			if (cor[i] != 0)
				printf("[ %d %d ] ", cor[i], i);
		puts("");
	}
}
