#include <algorithm>
#include <iostream>
using namespace std;

struct Node {
    string name;
    int    cost, ddl;
    int    current_time;
    int    value() const {
        auto relu = []( int x ) -> int { return x < 0 ? 0 : x; };
        return relu( current_time + cost - ddl );
    }
    bool operator<( Node& rhs ) {
        Node new1 = *this, new2 = rhs;
        new1.current_time += rhs.cost;
        new2.current_time -= this->cost;
        if ( new1.value() + new2.value() > this->value() + rhs.value() || ( new1.value() + new2.value() == this->value() + rhs.value() && this->name <= rhs.name ) )
            return false;
        else {
            this->current_time = new1.current_time;
            rhs                = new2;
            return true;
        }
    }
    void init() {
        name = "";
        cost = ddl = current_time = 0;
    }
} a[ 100010 ];
int main() {
    int T;
    cin >> T;
    while ( T-- ) {
        int n;
        cin >> n;
        for ( int i = 1; i <= n; i++ ) {
            a[ i ].init();
            string name;
            int    cost, ddl;
            cin >> a[ i ].name >> a[ i ].ddl >> a[ i ].cost;
            a[ i ].current_time = a[ i - 1 ].current_time + a[ i - 1 ].cost;
        }
        for ( int i = 1; i <= n; i++ )
            for ( int j = 1; j < n; j++ )
                if ( a[ j ] < a[ j + 1 ] )
                    swap( a[ j + 1 ], a[ j ] );
        int ans = 0;
        for ( int i = 1; i <= n; i++ )
            ans += a[ i ].value();
        cout << ans << endl;
        for ( int i = 1; i <= n; i++ )
            cout << a[ i ].name << endl;
    }
    return 0;
}
