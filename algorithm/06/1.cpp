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

int dep[100010];
int main()
{
#ifdef llt
	freopen("1.in", "r", stdin);
	freopen("1.out", "w", stdout);
#endif
	int n;
	scanf("%d", &n);
	int ans = 0;
	for (int i = 1; i <= n; i++) {
		int lson, rson;
		scanf("%d%d", &lson, &rson);
		if (lson != -1)
			dep[lson] = dep[i] + 1;
		if (rson != -1)
			dep[rson] = dep[i] + 1;
		ans = max(ans, dep[i]);
	}
	printf("%d", ans + 1);
}
