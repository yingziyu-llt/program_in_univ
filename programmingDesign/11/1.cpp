#include <iostream>
#include <list>
#include <string>
#include <vector>
using namespace std;
vector< list< int > > vec( 10010 );

int main() {
    int n;
    cin >> n;
    for ( int i = 1; i <= n; i++ ) {
        string str;
        cin >> str;
        if ( str == "new" ) {
            int op1 = 0;
            cin >> op1;
            vec[ op1 ].clear();
        }
        else if ( str == "add" ) {
            int op1, op2;
            cin >> op1 >> op2;
            vec[ op1 ].push_back( op2 );
        }
        else if ( str == "merge" ) {
            int op1, op2;
            cin >> op1 >> op2;
            if ( op1 == op2 )
                continue;
            vec[ op1 ].merge( vec[ op2 ] );
            vec[ op2 ].clear();
        }
        else if ( str == "out" ) {
            int op1;
            cin >> op1;
            vec[ op1 ].sort();
            for ( auto i : vec[ op1 ] ) {
                cout << i << " ";
            }
            cout << endl;
        }
        else {
            int op;
            cin >> op;
            vec[ op ].sort();
            vec[ op ].unique();
        }
    }
}
