#include <iostream>
#include <map>
#include <utility>
using namespace std;

map< int, int > mp;

int main() {
    int n;
    mp.insert( make_pair( 1000000000, 1 ) );
    cin >> n;
    for ( int i = 1; i <= n; i++ ) {
        int id, power;
        cin >> id >> power;
        map< int, int >::iterator xx = mp.lower_bound( power );

        int diff1 = 1000000000, diff2 = 1000000000;
        if ( xx != mp.end() )
            diff1 = xx->first - power;
        if ( xx != mp.begin() )
            diff2 = power - ( --xx )->first;
        if ( diff1 < diff2 && diff2 != 1000000000 )
            cout << id << " " << ( ++xx )->second << endl;
        else
            cout << id << " " << ( xx )->second << endl;
        mp.insert( make_pair( power, id ) );
    }
}
