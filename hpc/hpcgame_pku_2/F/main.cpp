#include <fstream>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cmath>

using namespace std;

struct SparseRow {
    unordered_map<int, int> cols; // 列索引到非零值的映射
    int rhs;                      // 方程右侧常数项

    SparseRow() : rhs(0) {}
};



bool sparseGaussianElimination(vector<SparseRow>& aug, vector<int>& solution, int mod) {
    if (mod <= 0) return false;
    
    int num_vars = 0;
    for (auto& row : aug) {
        for (auto& pair : row.cols) {
            num_vars = max(num_vars, pair.first + 1);
        }
    }
    
    int current_row = 0, current_col = 0;
    const int num_rows = aug.size();
    
    while (current_row < num_rows && current_col < num_vars) {
        int pivot_row = -1;
        for (int r = current_row; r < num_rows; ++r) {
            if (aug[r].cols.count(current_col) && aug[r].cols[current_col] != 0) {
                pivot_row = r;
                break;
            }
        }
        
        if (pivot_row == -1) {
            bool conflict = false;
            for (int r = current_row; r < num_rows; ++r) {
                if (aug[r].rhs % mod != 0) {
                    conflict = true;
                    break;
                }
            }
            if (conflict) return false;
            ++current_col;
            continue;
        }
        
        if (pivot_row != current_row) {
            swap(aug[current_row], aug[pivot_row]);
        }
        
        int val = aug[current_row].cols[current_col];        
        
        int inv_val = val;
        if (inv_val == -1) return false;
        
        for (auto& pair : aug[current_row].cols) {
            pair.second = (pair.second * inv_val) % mod;
            if (pair.second < 0) pair.second += mod;
        }
        aug[current_row].rhs = (aug[current_row].rhs * inv_val) % mod;
        if (aug[current_row].rhs < 0) aug[current_row].rhs += mod;
        
        for (int r = 0; r < num_rows; ++r) {
            if (r == current_row) continue;
            if (aug[r].cols.count(current_col)) {
                int factor = aug[r].cols[current_col];
                aug[r].cols.erase(current_col);
                
                aug[r].rhs = (aug[r].rhs - factor * aug[current_row].rhs) % mod;
                if (aug[r].rhs < 0) aug[r].rhs += mod;
                
                for (auto& pair : aug[current_row].cols) {
                    int col = pair.first;
                    int v = pair.second;
                    if (col == current_col) continue;
                    
                    int& target = aug[r].cols[col];
                    target = (target - factor * v) % mod;
                    if (target < 0) target += mod;
                    if (target == 0) aug[r].cols.erase(col);
                }
            }
        }
        
        ++current_row;
        ++current_col;
    }
    
    for (int r = current_row; r < num_rows; ++r) {
        if (aug[r].rhs % mod != 0) return false;
    }
    
    if (current_col < num_vars) return false;
    
    solution.assign(num_vars, 0);
    for (int r = current_row - 1; r >= 0; --r) {
        int pivot_col = -1;
        for (int c = 0; c < num_vars; ++c) {
            if (aug[r].cols.count(c) && aug[r].cols[c] != 0) {
                pivot_col = c;
                break;
            }
        }
        if (pivot_col == -1) continue;
        
        int sum = aug[r].rhs;
        for (auto& pair : aug[r].cols) {
            int col = pair.first;
            if (col == pivot_col) continue;
            sum = (sum - pair.second * solution[col]) % mod;
        }
        sum = (sum % mod + mod) % mod;
        solution[pivot_col] = sum;
    }
    
    return true;
}

int main() {
    ifstream in("in.data", ios::binary);
    if (!in) return 1;

    int32_t n1, n2;
    in.read(reinterpret_cast<char*>(&n1), sizeof(n1));
    in.read(reinterpret_cast<char*>(&n2), sizeof(n2));

    vector<int32_t> data(n1 * n2);
    in.read(reinterpret_cast<char*>(data.data()), n1 * n2 * sizeof(int32_t));
    in.close();

    vector<vector<int32_t>> m(n2, vector<int32_t>(n1));
    for (int i = 0; i < n2; ++i)
        for (int j = 0; j < n1; ++j)
            m[i][j] = data[i * n1 + j];

    vector<vector<int32_t>> im(n2, vector<int32_t>(n1, -1));
    int ci_total = 0;
    for (int i = 0; i < n2; ++i)
        for (int j = 0; j < n1; ++j)
            if (m[i][j] != 0)
                im[i][j] = ci_total++;

    vector<SparseRow> aug(ci_total);
    const vector<pair<int, int>> nl = {{0,1}, {0,-1}, {1,0}, {-1,0}, {0,0}};

    for (int i = 0; i < n2; ++i) {
        for (int j = 0; j < n1; ++j) {
            const int ci = im[i][j];
            if (ci == -1) continue;

            aug[ci].rhs = (3 - m[i][j]) % 3;
            if (aug[ci].rhs < 0) aug[ci].rhs += 3;

            for (const auto& delta : nl) {
                int i_ = i + delta.first;
                int j_ = j + delta.second;
                if (i_ < 0 || i_ >= n2 || j_ < 0 || j_ >= n1) continue;

                const int ci_ = im[i_][j_];
                if (ci_ != -1) {
                    aug[ci].cols[ci_] = (aug[ci].cols[ci_] + 1) % 3;
                    if (aug[ci].cols[ci_] == 0) {
                        aug[ci].cols.erase(ci_);
                    }
                }
            }
        }
    }

    vector<int> solution;
    if (!sparseGaussianElimination(aug, solution, 3))
        return 1;

    vector<vector<int32_t>> x(n2, vector<int32_t>(n1, 0));
    for (int i = 0; i < n2; ++i)
        for (int j = 0; j < n1; ++j)
            if (im[i][j] != -1)
                x[i][j] = solution[im[i][j]];

    ofstream out("out.data", ios::binary);
    for (const auto& row : x)
        out.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(int32_t));
    out.close();

    return 0;
}