#include "include/cryptopp/rsa.h"
using CryptoPP::RSA;
using CryptoPP::InvertibleRSAFunction;
using CryptoPP::RSAES_OAEP_SHA_Encryptor;
using CryptoPP::RSAES_OAEP_SHA_Decryptor;

#include "include/cryptopp/sha.h"
using CryptoPP::SHA512;

#include "include/cryptopp/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::PK_EncryptorFilter;
using CryptoPP::PK_DecryptorFilter;


#include "include/cryptopp/files.h"
using CryptoPP::FileSink;
using CryptoPP::FileSource;

#include "include/cryptopp/osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include "include/cryptopp/queue.h" // using for load functions 
using CryptoPP::ByteQueue;

#include "include/cryptopp/secblock.h"
using CryptoPP::SecByteBlock;

#include "include/cryptopp/cryptlib.h"
using CryptoPP::Exception;
using CryptoPP::DecodingResult;
using CryptoPP::BufferedTransformation; // using for load function

#include <string>
using std::string;
using std::wstring;

#include <exception>
using std::exception;

#include <iostream>
using std::wcout;
using std::wcin;
using std::cerr;
using std::endl;
/* Convert to hex */ 
#include "include/cryptopp/hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include <assert.h>

/* Vietnamese support */
        
/* Set _setmode()*/ 
#ifdef _WIN32
#include <io.h> 
#include <fcntl.h>
#else
#endif

/* String convert */
#include <locale>
using std::wstring_convert;
#include <codecvt>
using std::codecvt_utf8;

/* Integer convert */
#include <sstream>
using std::ostringstream;

/* Time Counter */
#include <chrono>
using namespace std::chrono;

/* Vietnames convert function def*/
wstring ConvertStringtoWstring (const std::string& str);
string ConvertWstringToString (const std::wstring& str);
wstring ConvertIntegerToWstring (const CryptoPP::Integer& t);

/*Load key from files (X.509 binary)*/
void LoadPrivateKey(const string& filename, RSA::PrivateKey& key);
void LoadPublicKey(const string& filename, RSA::PublicKey& key);
void Load(const string& filename, BufferedTransformation& bt);

/* Module Encryption and Decryption*/
void ModuleEncryption();
void ModeleDecryption();

// Input Key
void InputPublicKey(RSA::PublicKey& key);
void InputPrivateKey(RSA::PrivateKey& key);

// Input Plain text
void InputPlaintext(string& plain);
void InputPlaintextFromFile(string& plain);

// Input Cipher text
void InputCiphertext(string& cipher);
void InputCiphertextFromFile(string& cipher);

string ciphe;

int main(int argc, char* argv[])
{
    // Set mode support Vietnamese
    #ifdef __linux__
    setlocale(LC_ALL,"");
    #elif _WIN32
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stdout), _O_U16TEXT);
    #else
    #endif
    
    // Selection module encryption or decryption
    bool choose;
    wcout<<L"* Selection module encryption or decryption?"<<endl;
    wcout<<L"\t[1] Encryption"<<endl;
    wcout<<L"\t[0] Decryption"<<endl;
    wcout<<L"\tChoose : ";
    wcin>> choose;
    wcin.ignore();

    // Directional
    if (choose) ModuleEncryption();
    else ModeleDecryption();
	return 0;
}

// Module Encryption
void ModuleEncryption()
{
    try
    {
        // Generate keys
        AutoSeededRandomPool rng;
        // InvertibleRSAFunction parameters;
        // parameters.GenerateRandomWithKeySize(rng, 3072 );
        
        // Load key from files
        RSA::PublicKey publicKey;
        LoadPublicKey ("rsa-public.key", publicKey);

        ////////////////////////////////
        // // Print RSA parameters 
        // wcout << "RSA parameters:" << endl;
        // wcout << "Public modulo n=" << ConvertIntegerToWstring(publicKey.GetModulus()) << endl;
        // wcout << "Public key e=" << ConvertIntegerToWstring(publicKey.GetPublicExponent()) << endl;
        ////////////////////////////////

        // Input Plaintext
        string plain, cipher;
        InputPlaintext(plain);
        string encoded = "";
        ////////////////////////////////////////////////
        auto start = high_resolution_clock::now();

        for (int i=0;i<1000;i++)
        {
            // Setup publicKey for Encryption
            RSAES_OAEP_SHA_Encryptor e( publicKey ); // RSAES_PKCS1v15_Decrypt
            cipher.clear();
            // Create a pipelining for encryption
            StringSource( plain, true,
                new PK_EncryptorFilter( rng, e,
                    new StringSink( cipher )
                ) // PK_EncryptorFilter
            ); // StringSource
            
            // Write cipher as hexa code
            encoded.clear();
            StringSource(cipher, true, 
                new HexEncoder(new StringSink(encoded)) );
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);  
        wcout << L"* Time to ecrypt: "<< duration.count()/1000 <<L" microseconds"<< endl;

        wcout << L"* Ciphertext: " << ConvertStringtoWstring(encoded) << endl;

        // Choice to save ciphertext (as hexa code)
        wcout<<L"* Save Ciphertext? [1]:Yes or [0]:No"<<endl;
        wcout<<L"\tChoose : ";
        bool save;
        wcin>>save;
        
        if (save) StringSource ss( encoded, true /*pumpAll*/, new FileSink( "cipher-text.txt"));
    }
    catch( CryptoPP::Exception& e )
    {
        cerr << "Caught Exception..." << endl;
        cerr << e.what() << endl;
    }
}

/* Module Decryption */
void ModeleDecryption()
{
    try
    {
        // Generate keys
        AutoSeededRandomPool rng;
        // InvertibleRSAFunction parameters;
        // parameters.GenerateRandomWithKeySize(rng, 3072 );
        // Load key from files 
        RSA::PrivateKey privateKey;
        LoadPrivateKey ("rsa-private.key", privateKey);

        // // Print RSA parameters
        // wcout << "RSA parameters:" << endl;
        // wcout << "Private prime number p=" << ConvertIntegerToWstring(privateKey.GetPrime1()) << endl;
        // wcout << "Private prime number q=" << ConvertIntegerToWstring(privateKey.GetPrime2()) << endl;
        // wcout << "Secret key d=" << ConvertIntegerToWstring(privateKey.GetPrivateExponent()) << endl;
        ////////////////////////////////////////////////
        // Decryption
        string cipherHex, cipher, recovered; // ciphertext to decrypt
        cipherHex.clear();
        InputCiphertext(cipherHex);

        /* Decrypt */
        auto start = high_resolution_clock::now();

        for (int i=0;i<1000;i++)
        {
            // Hex decode the input cipher
            cipher.clear();
            StringSource(cipherHex, true, 
            new HexDecoder(new StringSink(cipher)));

            RSAES_OAEP_SHA_Decryptor d(privateKey );
            recovered.clear();
            StringSource( cipher, true,
                new PK_DecryptorFilter( rng, d,
                    new StringSink(recovered )
                ) // PK_EncryptorFilter
            ); // StringSource
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);  
        wcout << L"* Time to decrypt: "<< duration.count()/1000 <<L" microseconds"<< endl;
        wcout << "* Recover text: " << ConvertStringtoWstring(recovered) << endl;
    }
    catch( CryptoPP::Exception& e )
    {
        cerr << "Caught Exception..." << endl;
        cerr << e.what() << endl;
    }
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
        plain = ConvertWstringToString(wplain);
    }
}
// Input plaintext from file
void InputPlaintextFromFile(string& plain)
{
    FileSource file("plain-text.txt", true, new StringSink(plain));
    wcout << L"Plaintext:"<< ConvertStringtoWstring(plain) << endl;
}

// Input Cipher text
void InputCiphertext(string& cipher)
{
    bool choose;
    wcout<<L"* Input Ciphertext from Screen or File?"<<endl;
    wcout<<L"\t[1] Input from files"<<endl;
    wcout<<L"\t[0] Input from screen"<<endl;
    wcout<<L"\tChoose: ";
    wcin>>choose;
    wcin.ignore();

    if (choose) InputCiphertextFromFile(cipher);
    else //Input from screen
    {
        wstring wcipher;
        wcout << "* Input Ciphertext(HEX): ";
        fflush(stdin);
        getline(wcin,wcipher);
        cipher = ConvertWstringToString(wcipher);
    }
}

// Input Ciphertext from file
void InputCiphertextFromFile(string& cipher)
{
    FileSource file("cipher-text.txt", true, new StringSink(cipher));
    // wcout << L"Ciphertext:"<< ConvertStringtoWstring(cipher) << endl;
}

/* Input Public Key*/
void InputPublicKey(RSA::PublicKey& key)
{
    // -random;
	// -files;
	// -in code;
	// -input from screen
}

/* Input Private Key*/
void InputPrivateKey(RSA::PrivateKey& key);

/* Convert interger to wstring */
wstring ConvertIntegerToWstring (const CryptoPP::Integer& t)
{
    std::ostringstream oss;
    oss.str("");
    oss.clear();
    oss << t; // pumb t to oss
    std::string encoded(oss.str()); // to string 
    std::wstring_convert<codecvt_utf8<wchar_t>> towstring;
    return towstring.from_bytes(encoded); // string to wstring 
}

/* convert string to wstring */
wstring ConvertStringtoWstring (const std::string& str)
{
    // Khai báo đối tượng converterX thuộc lớp <wstring_convert> convert từ kiểu utf8
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;
    // Trả về kết quả chuyển đổi được từ một byte string sang wide string
    return converterX.from_bytes(str);
}

/* convert wstring to string */
string ConvertWstringToString (const std::wstring& wstr)
{
    // Khai báo đối tượng converterX thuộc lớp <wstring_convert>
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;
    // Trả về kết quả chuyển đổi được từ một wide string sang byte string
    return converterX.to_bytes(wstr);
}

void LoadPrivateKey(const string& filename, RSA::PrivateKey& key)
{
	ByteQueue queue;
	Load(filename, queue);
	key.Load(queue);	
}

void LoadPublicKey(const string& filename, RSA::PublicKey& key)
{
	ByteQueue queue;
	Load(filename, queue);
	key.Load(queue);	
}

void Load(const string& filename, BufferedTransformation& bt)
{
	FileSource file(filename.c_str(), true /*pumpAll*/);
	file.TransferTo(bt);
	bt.MessageEnd();
}
