#include <iostream>
#include <list>
#include <ostream>
#include <string>
using namespace std;

template <class T1, class T2>
void Copy(T1 s, T1 e, T2 x)
{
	for (; s != e; ++s, ++x)
		*x = *s;
}

template <class T>
class myostream_iteraotr {
public:
	ostream& o;
	string op;
	T cur;
	myostream_iteraotr(ostream& o, const string c)
		: o(o)
		, op(c)
	{
	}
	myostream_iteraotr& operator++()
	{
		o << cur << op;
		return *this;
	}
	T& operator*()
	{
		return cur;
	}

	// 在此处补充你的代码
};

int main()
{
	const int SIZE = 5;
	int a[SIZE] = { 5, 21, 14, 2, 3 };
	double b[SIZE] = { 1.4, 5.56, 3.2, 98.3, 3.3 };
	list<int> lst(a, a + SIZE);
	myostream_iteraotr<int> output(cout, ",");
	Copy(lst.begin(), lst.end(), output);
	cout << endl;
	myostream_iteraotr<double> output2(cout, "--");
	Copy(b, b + SIZE, output2);
	return 0;
}
