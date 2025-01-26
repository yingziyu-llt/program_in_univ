#include <iostream>
#include <fstream>
#include <bitset>

int main() {
    std::ifstream infile("in_raw.data");
    std::ofstream outfile("in.data", std::ios::binary);

    if (!infile) {
        std::cerr << "Error opening in.data" << std::endl;
        return 1;
    }

    if (!outfile) {
        std::cerr << "Error opening input.data" << std::endl;
        return 1;
    }

    int n1, n2;
    int mat[100][100];
    infile >> n1 >> n2;
    for(int i = 1;i <= n2;i++) {
        for(int j = 1;j <= n1;j++) {
            infile >> mat[i][j];
        }
    }

    outfile.write(reinterpret_cast<const char*>(&n1), sizeof(n1));
    outfile.write(reinterpret_cast<const char*>(&n2), sizeof(n2));
    for(int i = 1;i <= n2;i++) {
        for(int j = 1;j <= n1;j++) {
            outfile.write(reinterpret_cast<const char*>(&mat[i][j]), sizeof(mat[i][j]));
        }
    }
    infile.close();
    outfile.close();

    return 0;
}