#include <cstring>
#include <iostream>
using namespace std;

class Array2 {
	int** data;
	int n, m;

public:
	Array2() {};
	Array2(int i, int j)
	{
		data = new int*[i];
		for (int x = 0; x < i; x++)
			data[x] = new int[j];
		n = i, m = j;
	}
	Array2& operator=(const Array2& xx)
	{
		if (data != NULL)
			delete[] data;
		data = new int*[xx.n];
		for (int x = 0; x < xx.n; x++)
			data[x] = new int[xx.m];
		for (int x = 0; x < xx.n; x++)
			for (int y = 0; y < xx.m; y++)
				data[x][y] = xx.data[x][y];
		return *this;
	}
	int* operator[](int i)
	{
		return data[i];
	}
	int& operator()(int i, int j)
	{
		return data[i][j];
	}
	~Array2()
	{
		for (int x = 0; x < n; x++)
			delete[] data[x];
		delete[] data;
	}
};

int main()
{
	Array2 a(3, 4);
	int i, j;
	for (i = 0; i < 3; ++i)
		for (j = 0; j < 4; j++)
			a[i][j] = i * 4 + j;
	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 4; j++) {
			cout << a(i, j) << ",";
		}
		cout << endl;
	}
	cout << "next" << endl;
	Array2 b;
	b = a;
	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 4; j++) {
			cout << b[i][j] << ",";
		}
		cout << endl;
	}
	return 0;
}
