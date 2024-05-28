#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void work( string name, int dep ) {
    for ( int i = 1; i <= dep; i++ )
        cout << "|     ";
    cout << name << endl;
    vector< string > vec;
    for ( ;; ) {
        string input;
        cin >> input;
        if ( input == "]" )
            break;
        if ( input[ 0 ] == 'f' )
            vec.push_back( input );
        if ( input[ 0 ] == 'd' )
            work( input, dep + 1 );
    }
    sort( vec.begin(), vec.end() );
    for ( int i = 0; i < vec.size(); i++ ) {
        for ( int i = 1; i <= dep; i++ )
            cout << "|     ";
        cout << vec[ i ] << endl;
    }
}

int main() {
    int cnt = 0;
    while ( 1 ) {
        vector< string > vec;
        cnt++;
        string input;
        cin >> input;
        if ( input == "#" )
            break;
        cout << "DATA SET " << cnt << ":" << endl << "ROOT" << endl;

        for ( ;; ) {
            if ( input == "*" )
                break;
            if ( input[ 0 ] == 'f' )
                vec.push_back( input );
            if ( input[ 0 ] == 'd' )
                work( input, 1 );
            cin >> input;
        }
        sort( vec.begin(), vec.end() );
        for ( int i = 0; i < vec.size(); i++ ) {
            cout << vec[ i ] << endl;
        }
        cout << endl;
    }
}
