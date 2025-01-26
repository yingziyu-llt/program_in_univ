#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <fstream>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <secp256k1.h>
#include <omp.h>
#include <atomic>

using namespace std;

thread_local FILE* urandom = []{
    FILE* f = fopen("/dev/urandom", "rb");
    if (!f) {
        cerr << "Failed to open /dev/urandom" << endl;
        exit(1);
    }
    return f;
}();

string toHex(const uint8_t* data, size_t size) {
    ostringstream oss;
    oss << hex << setfill('0');
    for (size_t i = 0; i < size; ++i)
        oss << setw(2) << (int)data[i];
    return oss.str();
}

string keccak256(const uint8_t* data, size_t size) {
    thread_local EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_get_digestbyname("sha3-256");
    
    EVP_DigestInit_ex(ctx, md, nullptr);
    EVP_DigestUpdate(ctx, data, size);
    
    uint8_t hash[32];
    EVP_DigestFinal_ex(ctx, hash, nullptr);
    return toHex(hash + 12, 20); 
}

void generatePrivateKey(uint8_t* key) {
    fread(key, 1, 32, urandom);
}

string getAddress(secp256k1_context* ctx, const uint8_t* privKey) {
    secp256k1_pubkey pubkey;
    if (!secp256k1_ec_pubkey_create(ctx, &pubkey, privKey))
        return "";
    
    uint8_t pubkeySer[65];
    size_t len = 65;
    secp256k1_ec_pubkey_serialize(ctx, pubkeySer, &len, &pubkey, 
                                 SECP256K1_EC_UNCOMPRESSED);
    
    return "0x" + keccak256(pubkeySer + 1, 64);
}

void processPrefix(const string& prefix, ostream& out) {
    atomic<bool> found(false);
    string address;
    string privateKey;
    
    #pragma omp parallel num_threads(8)
    {
        thread_local secp256k1_context* ctx = 
            secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
        
        uint8_t privKey[32];
        string addr;
        
        while (!found) {
            generatePrivateKey(privKey);
            addr = getAddress(ctx, privKey);
            
            if (addr.substr(2, prefix.size()) == prefix) {
                #pragma omp critical
                {
                    if (!found) {
                        address = addr;
                        privateKey = toHex(privKey, 32);
                        found = true;
                    }
                }
            }
        }
    }
    
    out << address << '\n' << privateKey << endl;
}

int main() {
    ifstream infile("vanity.in");
    ofstream outfile("vanity.out");
    
    string prefixes[10];
    for (int i = 0; i < 10; ++i)
        infile >> prefixes[i];
    
    for (const auto& prefix : prefixes) 
        processPrefix(prefix, outfile);
    
    return 0;
}