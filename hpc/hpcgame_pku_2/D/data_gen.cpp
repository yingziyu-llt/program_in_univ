#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

const int ARRAY_SIZE = 65536;

void generate_random_data(const std::string& filename) {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    outfile << ARRAY_SIZE << std::endl << ARRAY_SIZE << std::endl;
    srand(static_cast<unsigned>(time(0)));

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        outfile << rand() << " ";
    }
    outfile << std::endl;

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        outfile << rand() << " ";
    }
    outfile << std::endl;

    outfile.close();
}

int main() {
    generate_random_data("input.dat");
    return 0;
}