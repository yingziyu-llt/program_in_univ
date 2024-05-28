2#include <iostream>
#include <stack>
#include <string>
std::stack< bool > num;
std::stack< int >  op;
using namespace std;
int main() {
    string s;
    int    cnt = 0;
    while ( getline( cin, s ) ) {
        cnt++;
        for ( int i = 0; i < s.length(); i++ ) {
            bool x;
            if ( s[ i ] == ' ' )
                continue;
            else if ( s[ i ] == '|' ) {
                while ( !op.empty() && op.top() == 1 ) {
                    bool x = num.top();
                    num.pop();
                    bool y = num.top();
                    num.pop();
                    op.pop();
                    num.push( x && y );
                }
                op.push( 0 );
            }
            else if ( s[ i ] == '&' ) {
                op.push( 1 );
            }
            else if ( s[ i ] == '!' )
                op.push( 2 );
            else if ( s[ i ] == '(' )
                op.push( -1 );
            else if ( s[ i ] == 'V' ) {
                x = true;
                while ( !op.empty() && op.top() == 2 )
                    x = !x, op.pop();
                num.push( x );
            }
            else if ( s[ i ] == 'F' ) {
                x = false;
                while ( !op.empty() && op.top() == 2 )
                    x = !x, op.pop();
                num.push( x );
            }
            else if ( s[ i ] == ')' ) {
                x = num.top();
                num.pop();
                while ( op.top() != -1 ) {
                    if ( op.top() == 0 )
                        x = x || num.top();
                    if ( op.top() == 1 )
                        x = x && num.top();
                    num.pop();
                    op.pop();
                }
                op.pop();
                while ( !op.empty() && op.top() == 2 )
                    x = !x, op.pop();
                num.push( x );
            }
        }
        bool ans = num.top();
        num.pop();
        while ( !op.empty() ) {
            if ( op.top() == 0 )
                ans = ans || num.top();
            if ( op.top() == 1 )
                ans = ans && num.top();
            num.pop();
            op.pop();
        }
        printf( "Expression %d: %s\n", cnt, ans ? "V" : "F" );
    }
}
