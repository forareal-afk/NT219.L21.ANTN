/* Hash to string
h:{0,1}^* --> {0,1}^l, l is digest size */
#include <iostream>
using std::wcin;
using std::wcout;
using std::cerr;
using std::endl;

#include <string>
using std::string;
using std::wstring;

/* Set _setmode()*/ 
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#endif
/* Convert string*/ 
#include <locale>
using std::wstring_convert;
#include <codecvt>
using  std::codecvt_utf8;
wstring ConvertStringToWString (const std::string& str);
string ConvertWStringToString (const std::wstring& str);

#include "include/cryptopp/cryptlib.h"
#include "include/cryptopp/sha3.h"
#include "include/cryptopp/sha.h"
#include "include/cryptopp/shake.h"
#include "include/cryptopp/hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;
// input, output string
#include "include/cryptopp/filters.h"
using CryptoPP::StringSource;
using CryptoPP::StringSink;
using CryptoPP::Redirector;
// input, output file
#include "include/cryptopp/files.h"
using CryptoPP::FileSource;
using CryptoPP::FileSink;
using CryptoPP::byte;
/* Time Counter */
#include <chrono>
using namespace std::chrono;


// Input Plain text
void InputPlaintext(string& plain);
void InputPlaintextFromFile(string& plain);

//Separation all hash functions using switch case
string FunctionSHA224(string message);
string FunctionSHA256(string message);
string FunctionSHA384(string message);
string FunctionSHA512(string message);
string FunctionSHA3_224(string message);
string FunctionSHA3_256(string message);
string FunctionSHA3_384(string message);
string FunctionSHA3_512(string message);
string FunctionSHAKE128(string message, int d);
string FunctionSHAKE256(string message, int d);
// Print select hash function
void PrintSelectHashFunciton();

int main (int argc, char* argv[])
{
    #ifdef __linux__
	setlocale(LC_ALL,"");
	#elif _WIN32
	_setmode(_fileno(stdin), _O_U16TEXT);
 	_setmode(_fileno(stdout), _O_U16TEXT);
	#else
	#endif

    // Input plaintext
    std::string message;
    std::string digest;
    InputPlaintext(message);
    
    int choose;
    PrintSelectHashFunciton();
    wcin>>choose;
    int d;
    if (choose == 8 || choose == 9)
    {
        wcout<<"Output Bytes (d): ";
        wcin>>d;
    }
    auto start = high_resolution_clock::now();
    switch (choose)
    {
    case 0:
        wcout<<"*** Hash Function SHA224 ***"<<endl;
        for (int i = 0; i<1000;i++)
            digest = FunctionSHA224(message);
        break;
    case 1:
        wcout<<"*** Hash Function SHA256 ***"<<endl;
        for (int i = 0; i<1000;i++)
            digest = FunctionSHA256(message);
        break;
    case 2:
        wcout<<"*** Hash Function SHA384 ***"<<endl;
        for (int i = 0; i<1000;i++)
            digest = FunctionSHA384(message);
        break;
    case 3:
        wcout<<"*** Hash Function SHA512 ***"<<endl;
        for (int i = 0; i<1000;i++)
            digest = FunctionSHA512(message);
        break;
    case 4:
        wcout<<"*** Hash Function SHA3_224 ***"<<endl;
        for (int i = 0; i<1000;i++)
            digest = FunctionSHA3_224(message);
        break;
    case 5:
        wcout<<"*** Hash Function SHA3_256 ***"<<endl;
        for (int i = 0; i<1000;i++)
            digest = FunctionSHA3_256(message);
        break;
    case 6:
        wcout<<"*** Hash Function SHA3_384 ***"<<endl;
        for (int i = 0; i<1000;i++)
            digest = FunctionSHA3_384(message);
        break;
    case 7:
        wcout<<"*** Hash Function SHA3_512 ***"<<endl;
        for (int i = 0; i<1000;i++)
            digest = FunctionSHA3_512(message);
        break;
    case 8:
        wcout<<"*** Hash Function SHAKE128 ***"<<endl;
        for (int i = 0; i<1000;i++)
            digest = FunctionSHAKE128(message,d);
        break;
    case 9:
        wcout<<"*** Hash Function SHAKE256 ***"<<endl;
        for (int i = 0; i<1000;i++)
            digest = FunctionSHAKE256(message,d);
        break;
    default:
        wcout<<"Error!!!";
        return 0;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);  
    wcout << L"* Time to hash: "<< duration.count()/1000 <<L" microseconds"<< endl;
    // Pretty print digest
    std::wcout << "* Message: " << ConvertStringToWString(message) << std::endl;
    std::wcout << "* Digest:  " << ConvertStringToWString(digest) << std::endl;
    return 0;
}

void PrintSelectHashFunciton()
{
    wcout<<L"* Select hash functions:\n";
    wcout<<L"\t[0] : SHA224\n";
    wcout<<L"\t[1] : SHA256\n";
    wcout<<L"\t[2] : SHA384\n";
    wcout<<L"\t[3] : SHA512\n";
    wcout<<L"\t[4] : SHA3_224\n";
    wcout<<L"\t[5] : SHA3_256\n";
    wcout<<L"\t[6] : SHA3_384\n";
    wcout<<L"\t[7] : SHA3_512\n";
    wcout<<L"\t[8] : SHAKE128\n";
    wcout<<L"\t[9] : SHAKE256\n";
    wcout<<L"\tChoose : ";
}

string FunctionSHA224(string message)
{
    CryptoPP::SHA224 hash;
    // std::wcout << "Name: " << ConvertStringToWString(hash.AlgorithmName()) << std::endl;
    // std::wcout << "Digest size: " << hash.DigestSize() << std::endl;
    // std::wcout << "Block size: " << hash.BlockSize() << std::endl;
    // Compute disgest
    std::string digest;
    std::string encode;
    hash.Restart();
    hash.Update((const byte*)message.data(), message.size());
    digest.resize(hash.DigestSize());
    hash.TruncatedFinal((byte*)&digest[0], digest.size());
    encode.clear();
    StringSource(digest, true, new HexEncoder (new StringSink (encode)));
    return encode;
}
string FunctionSHA256(string message)
{
    CryptoPP::SHA256 hash;
    // std::wcout << "Name: " << ConvertStringToWString(hash.AlgorithmName()) << std::endl;
    // std::wcout << "Digest size: " << hash.DigestSize() << std::endl;
    // std::wcout << "Block size: " << hash.BlockSize() << std::endl;

    // Compute disgest
    std::string digest;
    std::string encode;
    hash.Restart();
    hash.Update((const byte*)message.data(), message.size());
    digest.resize(hash.DigestSize());
    hash.TruncatedFinal((byte*)&digest[0], digest.size());
    encode.clear();
    StringSource(digest, true, new HexEncoder (new StringSink (encode)));
    return encode;
}
string FunctionSHA384(string message)
{
    CryptoPP::SHA384 hash;
    // std::wcout << "Name: " << ConvertStringToWString(hash.AlgorithmName()) << std::endl;
    // std::wcout << "Digest size: " << hash.DigestSize() << std::endl;
    // std::wcout << "Block size: " << hash.BlockSize() << std::endl;

    // Compute disgest
    std::string digest;
    std::string encode;
    hash.Restart();
    hash.Update((const byte*)message.data(), message.size());
    digest.resize(hash.DigestSize());
    hash.TruncatedFinal((byte*)&digest[0], digest.size());
    encode.clear();
    StringSource(digest, true, new HexEncoder (new StringSink (encode)));
    return encode;
}
string FunctionSHA512(string message)
{
    CryptoPP::SHA512 hash;
    // std::wcout << "Name: " << ConvertStringToWString(hash.AlgorithmName()) << std::endl;
    // std::wcout << "Digest size: " << hash.DigestSize() << std::endl;
    // std::wcout << "Block size: " << hash.BlockSize() << std::endl;

    // Compute disgest
    std::string digest;
    std::string encode;
    hash.Restart();
    hash.Update((const byte*)message.data(), message.size());
    digest.resize(hash.DigestSize());
    hash.TruncatedFinal((byte*)&digest[0], digest.size());
    encode.clear();
    StringSource(digest, true, new HexEncoder (new StringSink (encode)));
    return encode;
}
string FunctionSHA3_224(string message)
{
    CryptoPP::SHA3_224 hash;
    // std::wcout << "Name: " << ConvertStringToWString(hash.AlgorithmName()) << std::endl;
    // std::wcout << "Digest size: " << hash.DigestSize() << std::endl;
    // std::wcout << "Block size: " << hash.BlockSize() << std::endl;

    // Compute disgest
    std::string digest;
    std::string encode;
    hash.Restart();
    hash.Update((const byte*)message.data(), message.size());
    digest.resize(hash.DigestSize());
    hash.TruncatedFinal((byte*)&digest[0], digest.size());
    encode.clear();
    StringSource(digest, true, new HexEncoder (new StringSink (encode)));
    return encode;
}
string FunctionSHA3_256(string message)
{
    CryptoPP::SHA3_256 hash;
    // std::wcout << "Name: " << ConvertStringToWString(hash.AlgorithmName()) << std::endl;
    // std::wcout << "Digest size: " << hash.DigestSize() << std::endl;
    // std::wcout << "Block size: " << hash.BlockSize() << std::endl;

    // Compute disgest
    std::string digest;
    std::string encode;
    hash.Restart();
    hash.Update((const byte*)message.data(), message.size());
    digest.resize(hash.DigestSize());
    hash.TruncatedFinal((byte*)&digest[0], digest.size());
    encode.clear();
    StringSource(digest, true, new HexEncoder (new StringSink (encode)));
    return encode;
}
string FunctionSHA3_384(string message)
{
    CryptoPP::SHA3_384 hash;
    // std::wcout << "Name: " << ConvertStringToWString(hash.AlgorithmName()) << std::endl;
    // std::wcout << "Digest size: " << hash.DigestSize() << std::endl;
    // std::wcout << "Block size: " << hash.BlockSize() << std::endl;

    // Compute disgest
    std::string digest;
    std::string encode;
    hash.Restart();
    hash.Update((const byte*)message.data(), message.size());
    digest.resize(hash.DigestSize());
    hash.TruncatedFinal((byte*)&digest[0], digest.size());
    encode.clear();
    StringSource(digest, true, new HexEncoder (new StringSink (encode)));
    return encode;
}
string FunctionSHA3_512(string message)
{
    CryptoPP::SHA3_512 hash;
    // std::wcout << "Name: " << ConvertStringToWString(hash.AlgorithmName()) << std::endl;
    // std::wcout << "Digest size: " << hash.DigestSize() << std::endl;
    // std::wcout << "Block size: " << hash.BlockSize() << std::endl;

    // Compute disgest
    std::string digest;
    std::string encode;
    hash.Restart();
    hash.Update((const byte*)message.data(), message.size());
    digest.resize(hash.DigestSize());
    hash.TruncatedFinal((byte*)&digest[0], digest.size());
    encode.clear();
    StringSource(digest, true, new HexEncoder (new StringSink (encode)));
    return encode;
}
string FunctionSHAKE128(string message, int d)
{
    CryptoPP::SHAKE128 hash;
    // std::wcout << "Name: " << ConvertStringToWString(hash.AlgorithmName()) << std::endl;
    // std::wcout << "Digest size: " << hash.DigestSize() << std::endl;
    // std::wcout << "Block size: " << hash.BlockSize() << std::endl;
    // Compute disgest
    std::string digest;
    std::string encode;
    hash.Restart();
    hash.Update((const byte*)message.data(), message.size());
    digest.resize(d);
    hash.TruncatedFinal((byte*)&digest[0], digest.size());
    encode.clear();
    StringSource(digest, true, new HexEncoder (new StringSink (encode)));
    return encode;
}
string FunctionSHAKE256(string message, int d)
{
    CryptoPP::SHAKE256 hash;
    // std::wcout << "Name: " << ConvertStringToWString(hash.AlgorithmName()) << std::endl;
    // std::wcout << "Digest size: " << hash.DigestSize() << std::endl;
    // std::wcout << "Block size: " << hash.BlockSize() << std::endl;
    // Compute disgest
    std::string digest;
    std::string encode;
    hash.Restart();
    hash.Update((const byte*)message.data(), message.size());
    digest.resize(d);
    hash.TruncatedFinal((byte*)&digest[0], digest.size());
    encode.clear();
    StringSource(digest, true, new HexEncoder (new StringSink (encode)));
    return encode;
}

/* convert string to wstring */
wstring ConvertStringToWString(const std::string& str)
{
    wstring_convert<codecvt_utf8<wchar_t>> towstring;
    return towstring.from_bytes(str);
}

/* convert wstring to string */
string ConvertWStringToString(const std::wstring& str)
{
    wstring_convert<codecvt_utf8<wchar_t>> tostring;
    return tostring.to_bytes(str);
}

// Input Plain text
void InputPlaintext(string& plain)
{
    bool choose;
    wcout<<L"* Input Plaintext from Screen or File?"<<endl;
    wcout<<L"\t[1] Input from files"<<endl;
    wcout<<L"\t[0] Input from screen"<<endl;
    wcout<<L"\tChoose : ";
    wcin>>choose;
    wcin.ignore();

    if (choose) InputPlaintextFromFile(plain);
    else //Input from screen
    {
        wstring wplain;
        wcout << "* Input Plaintext: ";
        fflush(stdin);
        getline(wcin,wplain);
        plain = ConvertWStringToString(wplain);
    }
}
// Input plaintext from file
void InputPlaintextFromFile(string& plain)
{
    FileSource file("plain-text.txt", true, new StringSink(plain));
    wcout << L"Plaintext:"<< ConvertStringToWString(plain) << endl;
}