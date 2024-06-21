#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>

using namespace std;

typedef pair< int, int > pii;

struct State {
    int  a, b, c;
    bool operator==( const State& s ) const {
        return a == s.a && b == s.b && c == s.c;
    }
};

struct Hash {
    size_t operator()( const State& s ) const {
        return s.a + 1000 * s.b + 1000000 * s.c;
    }
};

unordered_map< State, pair< pii, State >, Hash > previous_state;

int main() {
    int a, b, c;
    cin >> a >> b >> c;
    queue< State > q;
    q.push( { 0, 0, 0 } );
    State last_state;
    previous_state.insert( { { 0, 0, 0 }, { { 0, 0 }, { 0, 0 } } } );
    while ( !q.empty() ) {
        State s = q.front();
        q.pop();
        if ( s.a == c || s.b == c ) {
            last_state = s;
            break;
        }
        State new_state = s;
        // fill second = -1
        new_state.a = a;
        if ( previous_state.find( new_state ) == previous_state.end() ) {
            previous_state[ new_state ] = { { 1, -1 }, s };
            q.push( new_state );
        }
        new_state   = s;
        new_state.b = b;
        if ( previous_state.find( new_state ) == previous_state.end() ) {
            previous_state[ new_state ] = { { 2, -1 }, s };
            q.push( new_state );
        }
        // drop second = -2
        new_state   = s;
        new_state.a = 0;
        if ( previous_state.find( new_state ) == previous_state.end() ) {
            previous_state[ new_state ] = { { 1, -2 }, s };
            q.push( new_state );
        }
        new_state   = s;
        new_state.b = 0;
        if ( previous_state.find( new_state ) == previous_state.end() ) {
            previous_state[ new_state ] = { { 2, -2 }, s };
            q.push( new_state );
        }
        // pour
        // 1->2
        new_state   = s;
        new_state.b = min( b, s.a + s.b );
        new_state.a = s.a - new_state.b + s.b;
        if ( previous_state.find( new_state ) == previous_state.end() ) {
            previous_state[ new_state ] = { { 1, 2 }, s };
            q.push( new_state );
        }
        // 2->1
        new_state   = s;
        new_state.a = min( a, s.b + s.a );
        new_state.b = s.b - new_state.a + s.a;
        if ( previous_state.find( new_state ) == previous_state.end() ) {
            previous_state[ new_state ] = { { 2, 1 }, s };
            q.push( new_state );
        }
    }

    vector< string > ans;
    if ( last_state.a != c && last_state.b != c ) {
        cout << "impossible";
        return 0;
    }

    for ( State s = last_state; s.a > 0 || s.b > 0 || s.c > 0; s = previous_state[ s ].second ) {
        pii action = previous_state[ s ].first;
        if ( action.second == -1 )
            ans.push_back( "FILL(" + to_string( action.first ) + ")" );
        else if ( action.second == -2 )
            ans.push_back( "DROP(" + to_string( action.first ) + ")" );
        else
            ans.push_back( "POUR(" + to_string( action.first ) + "," + to_string( action.second ) + ")" );
    }
    cout << ans.size() << endl;
    for ( int i = ans.size() - 1; i >= 0; --i )
        cout << ans[ i ] << endl;
}
