#include <iostream>
#include <map>
#include <queue>
#include <vector>

using namespace std;
map< int, int > mp;

class Greater {
    int get_factor( int x ) {
        if ( mp.find( x ) != mp.end() )
            return mp[ x ];
        int tot = 0;
        int xx  = x;
        for ( int i = 2; i <= x && i < xx; i++ ) {
            if ( x % i == 0 ) {
                x /= i;
                if ( x % i )
                    tot++;
                i--;
            }
        }
        mp.insert( make_pair( x, tot ) );
        return tot;
    }

public:
    bool operator()( int a, int b ) {
        int fa = get_factor( a ), fb = get_factor( b );
        if ( fa > fb )
            return 1;
        else if ( fa < fb )
            return 0;
        else
            return a > b;
    }
};

class Less {
    int get_factor( int x ) {
        if ( mp.find( x ) != mp.end() )
            return mp[ x ];
        int tot = 0;
        int xx  = x;
        for ( int i = 2; i <= x && i < xx; i++ ) {
            if ( x % i == 0 ) {
                x /= i;
                if ( x % i )
                    tot++;
                i--;
            }
        }
        mp.insert( make_pair( x, tot ) );
        return tot;
    }

public:
    bool operator()( int a, int b ) {
        int fa = get_factor( a ), fb = get_factor( b );
        if ( fa > fb )
            return 0;
        else if ( fa < fb )
            return 1;
        else
            return a < b;
    }
};

priority_queue< int, vector< int >, Less >    smaller;
priority_queue< int, vector< int >, Greater > bigger;

int main() {
    int n;
    scanf( "%d", &n );
    for ( int i = 1; i <= n; i++ ) {
        for ( int j = 1; j <= 10; j++ ) {
            int x;
            cin >> x;
            smaller.push( x );
            bigger.push( x );
        }
        cout << smaller.top() << " " << bigger.top() << endl;
        smaller.pop(), bigger.pop();
    }
}
