#include <iostream>
using namespace std;

template <class T>
struct GoodCopy {

	// 在此处补充你的代码
	void operator()(T* src1, T* src2, T* dst)
	{
		T tmp[100];
		for (T* i = src1; i <= src2; i++)
			tmp[i - src1] = *i;
		for (int i = 0; i < src2 - src1; i++)
			*(dst + i) = tmp[i];
	}
};

int a[200];
int b[200];
string c[200];
string d[200];

template <class T>
void Print(T s, T e)
{
	for (; s != e; ++s)
		cout << *s << ",";
	cout << endl;
}

int main()
{
	int t;
	cin >> t;
	while (t--) {
		int m;
		cin >> m;
		for (int i = 0; i < m; ++i)
			cin >> a[i];
		GoodCopy<int>()(a, a + m, b);
		Print(b, b + m);
		GoodCopy<int>()(a, a + m, a + m / 2);
		Print(a + m / 2, a + m / 2 + m);

		for (int i = 0; i < m; ++i)
			cin >> c[i];
		GoodCopy<string>()(c, c + m, d);
		Print(c, c + m);
		GoodCopy<string>()(c, c + m, c + m / 2);
		Print(c + m / 2, c + m / 2 + m);
	}
	return 0;
}
