#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <utility>
using namespace std;

multimap< int, int >   mp;
map< int, set< int > > keymap;

int main() {
    int n;
    mp.insert( make_pair( 1000000000, 1 ) );
    set< int > s;
    s.insert( 1 );
    keymap.insert( make_pair( 1000000000, s ) );
    cin >> n;
    for ( int i = 1; i <= n; i++ ) {
        int id, power;
        cin >> id >> power;
        multimap< int, int >::iterator xx = mp.lower_bound( power );

        int diff1 = 1000000000, diff2 = 1000000000;
        int id1 = -1, id2 = -1;
        if ( xx != mp.end() )
            diff1 = xx->first - power, id1 = *keymap[ xx->first ].begin();
        if ( xx != mp.begin() ) {
            xx    = mp.lower_bound( ( --xx )->first );
            id2   = *keymap[ xx->first ].begin();
            diff2 = power - xx->first;
        }
        if ( diff1 < diff2 )
            cout << id << " " << id1 << endl;
        else if ( diff1 > diff2 )
            cout << id << " " << id2 << endl;
        else if ( diff1 == diff2 )
            cout << id << " " << min( id1, id2 ) << endl;

        mp.insert( make_pair( power, id ) );
        if ( keymap.find( power ) == keymap.end() ) {
            set< int > tmp;
            tmp.insert( id );
            keymap.insert( make_pair( power, tmp ) );
        }
        else {
            keymap[ power ].insert( id );
        }
    }
}
