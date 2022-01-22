// Linux help: http://www.cryptopp.com/wiki/Linux

// Debug:
// g++ -g -ggdb -O0 -Wall -Wextra -Wno-unused -Wno-type-limits -I. -I/usr/include/cryptopp cryptopp-key-gen.cpp -o cryptopp-key-gen.exe -lcryptopp

// Release:
// g++ -O2 -Wall -Wextra -Wno-unused -Wno-type-limits -I. -I/usr/include/cryptopp cryptopp-key-gen.cpp -o cryptopp-key-gen.exe -lcryptopp && strip --strip-all cryptopp-key-gen.exe

/*Compute in Interger */
#include "include/cryptopp/integer.h"
#include "include/cryptopp/modarith.h"
#include "include/cryptopp/nbtheory.h"
#include <iomanip>

#include <iostream>
using std::wcout;
using std::wcin;
using std::cerr;
using std::endl;

#include <string>
using std::string;
using std::wstring;

#include <stdexcept>
using std::runtime_error;

#include "include/cryptopp/queue.h"
using CryptoPP::ByteQueue;

#include "include/cryptopp/files.h"
using CryptoPP::FileSource;
using CryptoPP::FileSink;

#include "include/cryptopp/dsa.h"
using CryptoPP::DSA;

#include "include/cryptopp/rsa.h"
using CryptoPP::RSA;

#include "include/cryptopp/base64.h"
using CryptoPP::Base64Encoder;
using CryptoPP::Base64Decoder;

#include "include/cryptopp/cryptlib.h"
using CryptoPP::PrivateKey;
using CryptoPP::PublicKey;
using CryptoPP::BufferedTransformation;

#include "include/cryptopp/osrng.h"
using CryptoPP::AutoSeededRandomPool;

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
using  std::codecvt_utf8;

/* Integer convert */
#include <sstream>
using std::ostringstream;

/* Vietnames convert function def*/
wstring ConvertStringToWstring (const std::string& str);
string ConvertWstringToString (const std::wstring& str);
wstring ConvertIntegerToWstring (const CryptoPP::Integer& t);


void SavePrivateKey(const string& filename, const PrivateKey& key);
void SavePublicKey(const string& filename, const PublicKey& key);

void SaveBase64PrivateKey(const string& filename, const PrivateKey& key);
void SaveBase64PublicKey(const string& filename, const PublicKey& key);

void SaveBase64(const string& filename, const BufferedTransformation& bt);
void Save(const string& filename, const BufferedTransformation& bt);

void LoadPrivateKey(const string& filename, PrivateKey& key);
void LoadPublicKey(const string& filename, PublicKey& key);

void LoadBase64PrivateKey(const string& filename, PrivateKey& key);
void LoadBase64PublicKey(const string& filename, PublicKey& key);

void LoadBase64(const string& filename, BufferedTransformation& bt);
void Load(const string& filename, BufferedTransformation& bt);

int main(int argc, char** argv)
{
	/*Set mode support Vietnamese*/
	#ifdef __linux__
	setlocale(LC_ALL,"");
	#elif _WIN32
	_setmode(_fileno(stdin), _O_U16TEXT);
 	_setmode(_fileno(stdout), _O_U16TEXT);
	#else
	#endif
	AutoSeededRandomPool rnd;

	try
	{
		RSA::PrivateKey rsaPrivate;
		rsaPrivate.GenerateRandomWithKeySize(rnd, 3072);
		RSA::PublicKey rsaPublic(rsaPrivate);
		SavePrivateKey("rsa-private.key", rsaPrivate);
		SavePublicKey("rsa-public.key", rsaPublic);
		
		/* Pretty print n,p,q,e,d */
		wcout << "Modunlo  n= " << ConvertIntegerToWstring(rsaPublic.GetModulus()) << endl;
		wcout << "Public key  e= " << ConvertIntegerToWstring(rsaPublic.GetPublicExponent()) << endl;
		wcout << "Private prime number  p= " << ConvertIntegerToWstring(rsaPrivate.GetPrime1()) << endl;
		wcout << "Private prime number  q= " << ConvertIntegerToWstring(rsaPrivate.GetPrime2()) << endl;
		wcout << "Private key  d= " << ConvertIntegerToWstring(rsaPrivate.GetPrivateExponent()) << endl;
		
		/* Test Vietnamese*/
		// std::wstring test;
		// wcout << L"Test Tiếng Việt" << endl;
		// wcout << L"Input message:";
		// getline(wcin, test);
		// wcout << test << endl;

		/* Check the keys */
		CryptoPP::Integer n,p,q,e,d;
		n=rsaPublic.GetModulus();
		p=rsaPrivate.GetPrime1();
		q=rsaPrivate.GetPrime2();
		CryptoPP::ModularArithmetic ma(n);
		wcout << L"Check RSA keys"<<endl;
		wcout << "p*q % n=" << ConvertIntegerToWstring(ma.Multiply(p, q)) << endl;
		wcout << "p*q % n=" << ConvertIntegerToWstring(a_times_b_mod_c(p,q,n)) << endl;

		// ////////////////////////////////////////////////////////////////////////////////////
		// DSA::PrivateKey dsaPrivate;
		// dsaPrivate.GenerateRandomWithKeySize(rnd, 3072);

		// DSA::PublicKey dsaPublic;
		// dsaPrivate.MakePublicKey(dsaPublic);

		// SavePrivateKey("dsa-private.key", dsaPrivate);
		// SavePublicKey("dsa-public.key", dsaPublic);

		// // ////////////////////////////////////////////////////////////////////////////////////
		// RSA::PrivateKey r1, r2;
		// r1.GenerateRandomWithKeySize(rnd, 3072);

		// SavePrivateKey("rsa-roundtrip.key", r1);
		// LoadPrivateKey("rsa-roundtrip.key", r2);

		// r1.Validate(rnd, 3);
		// r2.Validate(rnd, 3);

		// if(r1.GetModulus() != r2.GetModulus() ||
		//    r1.GetPublicExponent() != r2.GetPublicExponent() ||
		//    r1.GetPrivateExponent() != r2.GetPrivateExponent())
		// {
		// 	throw runtime_error("key data did not round trip");
		// }
		
		// ////////////////////////////////////////////////////////////////////////////////////

		wcout << "Successfully generated and saved RSA and DSA keys" << endl;
	}

	catch(CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		return -2;
	}

	catch(std::exception& e)
	{
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}

void SavePrivateKey(const string& filename, const PrivateKey& key)
{
	ByteQueue queue;
	key.Save(queue);
	Save(filename, queue);
}

void SavePublicKey(const string& filename, const PublicKey& key)
{
	ByteQueue queue;
	key.Save(queue);
	Save(filename, queue);
}

void Save(const string& filename, const BufferedTransformation& bt)
{
	FileSink file(filename.c_str());
	bt.CopyTo(file);
	file.MessageEnd();
}

void SaveBase64PrivateKey(const string& filename, const PrivateKey& key)
{
	ByteQueue queue;
	key.Save(queue);
	SaveBase64(filename, queue);
}

void SaveBase64PublicKey(const string& filename, const PublicKey& key)
{
	ByteQueue queue;
	key.Save(queue);
	SaveBase64(filename, queue);
}

void SaveBase64(const string& filename, const BufferedTransformation& bt)
{
	// http://www.cryptopp.com/docs/ref/class_base64_encoder.html
	Base64Encoder encoder;
	bt.CopyTo(encoder);
	encoder.MessageEnd();
	Save(filename, encoder);
}

void LoadPrivateKey(const string& filename, PrivateKey& key)
{
	ByteQueue queue;
	Load(filename, queue);
	key.Load(queue);	
}

void LoadPublicKey(const string& filename, PublicKey& key)
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

void LoadBase64PrivateKey(const string& filename, PrivateKey& key)
{
	throw runtime_error("Not implemented");
}

void LoadBase64PublicKey(const string& filename, PublicKey& key)
{
	throw runtime_error("Not implemented");
}

void LoadBase64(const string& filename, BufferedTransformation& bt)
{
	throw runtime_error("Not implemented");
}

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
wstring ConvertStringToWstring (const std::string& str)
{
    wstring_convert<codecvt_utf8<wchar_t>> towstring;
    return towstring.from_bytes(str);
}

/* convert wstring to string */
string ConvertWstringToString (const std::wstring& str)
{
    wstring_convert<codecvt_utf8<wchar_t>> tostring;
    return tostring.to_bytes(str);
}