#include "include/cryptopp/files.h"
using CryptoPP::FileSource;
using CryptoPP::FileSink;
using CryptoPP::BufferedTransformation;
#include "include/cryptopp/osrng.h"
using CryptoPP::AutoSeededRandomPool;
#include "include/cryptopp/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
int main()
{
    AutoSeededRandomPool prng1;
    CryptoPP::byte key[16];
    // memset( key, 0, sizeof(key) );
    prng1.GenerateBlock(key,sizeof(key));
    // Write key to file AES_key.key 
    StringSource ss1(key, sizeof(key), true , new FileSink( "FileKey.key"));

    // Create a random iv
    AutoSeededRandomPool prng2;
    CryptoPP::byte iv[16];
    // memset( iv, 0, sizeof(iv) );
    prng2.GenerateBlock(iv, sizeof(iv));
    // Write key to file AES_key.key 
    StringSource ss2(iv, sizeof(iv), true , new FileSink( "FileIV.key"));
    return 0;
}