#include <iostream>
#include <set>
#include <string>

using namespace std;
multiset< int > st;
set< int >      vis;
int             main() {
    int n;
    scanf( "%d", &n );
    for ( int i = 1; i <= n; i++ ) {
        string str;
        int    op;
        cin >> str >> op;
        if ( str == "add" ) {
            st.insert( op );
            vis.insert( op );
            cout << st.count( op ) << endl;
        }
        else if ( str == "del" ) {
            cout << st.count( op ) << endl;
            if ( st.count( op ) != 0 )
                st.erase( op );
        }
        else {
            if ( vis.find( op ) == vis.end() )
                cout << "0 0" << endl;
            else
                cout << "1 " << st.count( op ) << endl;
        }
    }
}
