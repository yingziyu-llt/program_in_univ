#include <iostream>
#include <set>
int main() {
    int n;
    std::cin >> n;
    while ( n-- ) {
        std::multiset< int > st;

        int s, a;
        std::cin >> s >> a;
        for ( int i = 1; i <= a; i++ ) {
            int t;
            std::cin >> t;
            st.insert( t );
        }
        int b, cnt = 0;
        std::cin >> b;
        for ( int i = 1; i <= b; i++ ) {
            int tt;
            std::cin >> tt;
            if ( st.find( s - tt ) != st.end() )
                cnt += st.count( s - tt );
        }
        std::cout << cnt << std::endl;
    }
}
