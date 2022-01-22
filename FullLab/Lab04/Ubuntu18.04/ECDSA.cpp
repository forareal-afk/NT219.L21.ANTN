// ECDSA.KeyGen.cpp : Defines the entry point for the console application.
//

#include <assert.h>

#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#include <string>
using std::string;

#include "include/cryptopp/osrng.h"
// using CryptoPP::AutoSeededX917RNG;
using CryptoPP::AutoSeededRandomPool;

#include "include/cryptopp/aes.h"
using CryptoPP::AES;

#include "include/cryptopp/integer.h"
#include "include/cryptopp/nbtheory.h"
using CryptoPP::Integer;

#include "include/cryptopp/sha.h"
using CryptoPP::SHA1;

#include "include/cryptopp/filters.h"
using CryptoPP::StringSource;
using CryptoPP::StringSink;
using CryptoPP::ArraySink;
using CryptoPP::SignerFilter;
using CryptoPP::SignatureVerificationFilter;

#include "include/cryptopp/files.h"
using CryptoPP::FileSource;
using CryptoPP::FileSink;
using CryptoPP::byte;

#include "include/cryptopp/eccrypto.h"
using CryptoPP::ECDSA;
using CryptoPP::ECP;
using CryptoPP::DL_GroupParameters_EC;

#include "include/cryptopp/oids.h"
using CryptoPP::OID;
// Hex encode, decode
#include "include/cryptopp/hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

// Time Counter
#include <chrono>
using namespace std::chrono;

 // Funtions
bool GeneratePrivateKey( const OID& oid, ECDSA<ECP, SHA1>::PrivateKey& key );
bool GeneratePublicKey( const ECDSA<ECP, SHA1>::PrivateKey& privateKey, ECDSA<ECP, SHA1>::PublicKey& publicKey );
void SavePrivateKey( const string& filename, const ECDSA<ECP, SHA1>::PrivateKey& key );
void SavePublicKey( const string& filename, const ECDSA<ECP, SHA1>::PublicKey& key );
void LoadPrivateKey( const string& filename, ECDSA<ECP, SHA1>::PrivateKey& key );
void LoadPublicKey( const string& filename, ECDSA<ECP, SHA1>::PublicKey& key );

void PrintDomainParameters( const ECDSA<ECP, SHA1>::PrivateKey& key );
void PrintDomainParameters( const ECDSA<ECP, SHA1>::PublicKey& key );
void PrintDomainParameters( const DL_GroupParameters_EC<ECP>& params );
void PrintPrivateKey( const ECDSA<ECP, SHA1>::PrivateKey& key );
void PrintPublicKey( const ECDSA<ECP, SHA1>::PublicKey& key );

bool SignMessage( const ECDSA<ECP, SHA1>::PrivateKey& key, const string& message, string& signature );
bool VerifyMessage( const ECDSA<ECP, SHA1>::PublicKey& key, const string& message, const string& signature );

void LoadFile(string filename, string& content);
void Sign();
void Verify();

int main(int argc, char* argv[])
{
    // bool result;
    // // Private and Public keys
    // ECDSA<ECP, SHA1>::PrivateKey privateKey;
    // ECDSA<ECP, SHA1>::PublicKey publicKey;
    // /////////////////////////////////////////////
    // // Generate Keys
    // result = GeneratePrivateKey( CryptoPP::ASN1::secp256r1(), privateKey );
    // // assert( true == result );
    // // if( !result ) { return -1; }
    // result = GeneratePublicKey( privateKey, publicKey );
    // // assert( true == result );
    // // if( !result ) { return -2; }
    // /////////////////////////////////////////////
    // //Print Domain Parameters and Keys   
    // PrintDomainParameters(publicKey );
    // PrintPrivateKey( privateKey );
    // PrintPublicKey( publicKey );
    // /////////////////////////////////////////////
    // //Save key in PKCS#9 and X.509 format    
    // SavePrivateKey( "ec.private.key", privateKey );
    // SavePublicKey( "ec.public.key", publicKey );
    // /////////////////////////////////////////////
    int choose;
    cout<<"* Select [1] Sign or [0] Verify!!!\n* Input >> ";
    cin >> choose;
    cout<< std::boolalpha;
    switch (choose)
    {
    case 1:
        Sign();
        break;
    default:
        Verify();
        break;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
/* Def functions*/

void Sign()
{
    // Scratch result
    bool result = false;
    string message;
    // Load key from file
    ECDSA<ECP, SHA1>::PrivateKey privateKey;
    LoadPrivateKey( "ec.private.key", privateKey);
    // Print Private Key
    PrintPrivateKey( privateKey );
    // Load mesage from file
    FileSource Read("message.txt", true, new StringSink(message));
    string signature; // return Hex
    // Sign Message
    auto start = high_resolution_clock::now();
    for (int i = 0; i<1000;i++)
    {
        signature.clear();
        result = SignMessage(privateKey, message, signature);
    }
    auto stop = high_resolution_clock::now();
    // Time Counter
    auto duration = duration_cast<microseconds>(stop - start);  
    cout << "* Time to Sign: "<< duration.count()/1000 <<" microseconds"<< endl;
    // Write signature to screen
    cout << "* Signature: "<<signature<<endl;
    // Write signature to file
    StringSource Write(signature, true, new FileSink("signature"));
}

void Verify()
{
    // Scratch result
    bool result = false;
    // Load key from file
    ECDSA<ECP, SHA1>::PublicKey publicKey;
    LoadPublicKey( "ec.public.key", publicKey);
    // >Print Public Key
    PrintPublicKey( publicKey );
    // Load message from file
    string message;
    message.clear();
    FileSource Read1("message.txt", true, new StringSink(message));
    // Load signature from file
    string signature;
    signature.clear();
    FileSource Read2("signature", true, new StringSink(signature));
    // Write signature to screen
    cout << "* Signature: "<<signature<<endl;
    // Verify Message
    auto start = high_resolution_clock::now();
    for (int i=0;i<1000;i++)
    {
        result = VerifyMessage(publicKey, message, signature);
    }
    // Time counter
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);  
    cout << "* Time to VerifyMessage: "<< duration.count()/1000 <<" microseconds"<< endl;
    // print result
    cout << "* Verify the signature on message:" << result << endl;
}

bool GeneratePrivateKey( const OID& oid, ECDSA<ECP, SHA1>::PrivateKey& key )
{
    AutoSeededRandomPool prng;
    key.Initialize( prng, oid );
    assert( key.Validate( prng, 3 ) );
    return key.Validate( prng, 3 );
}

bool SignMessage( const ECDSA<ECP, SHA1>::PrivateKey& key, const string& message, string& signature )
{
    AutoSeededRandomPool prng;
    signature.erase();    
    StringSource( message, true,
        new SignerFilter( prng,
            ECDSA<ECP,SHA1>::Signer(key),
            new HexEncoder(new StringSink(signature))
        )
    );
    return !signature.empty();
}

bool VerifyMessage( const ECDSA<ECP, SHA1>::PublicKey& key, const string& message, const string& signature )
{
    bool result = false;
    string decode;
    decode.clear();
    // HexDecode signature
    StringSource(signature, true, new HexDecoder (new StringSink(decode)));
    // Verify
    StringSource(decode+message, true,
        new SignatureVerificationFilter(
            ECDSA<ECP,SHA1>::Verifier(key),
            new ArraySink( (byte*)&result, sizeof(result) )
        ) // SignatureVerificationFilter
    );
    return result;
}

bool GeneratePublicKey( const ECDSA<ECP, SHA1>::PrivateKey& privateKey, ECDSA<ECP, SHA1>::PublicKey& publicKey )
{
    AutoSeededRandomPool prng;
    // Sanity check
    assert( privateKey.Validate( prng, 3 ) );
    // privateKey.MakePublicKey(publicKey);
    assert( publicKey.Validate( prng, 3 ) );
    return publicKey.Validate( prng, 3 );
}

void PrintDomainParameters( const ECDSA<ECP, SHA1>::PrivateKey& key )
{
    PrintDomainParameters( key.GetGroupParameters() );
}

void PrintDomainParameters( const ECDSA<ECP, SHA1>::PublicKey& key )
{
    PrintDomainParameters( key.GetGroupParameters() );
}

void PrintDomainParameters( const DL_GroupParameters_EC<ECP>& params )
{
    cout << endl;
 
    cout << "Modulus:" << endl;
    cout << " " << params.GetCurve().GetField().GetModulus() << endl;
    
    cout << "Coefficient A:" << endl;
    cout << " " << params.GetCurve().GetA() << endl;
    
    cout << "Coefficient B:" << endl;
    cout << " " << params.GetCurve().GetB() << endl;
    
    cout << "Base Point:" << endl;
    cout << " X: " << params.GetSubgroupGenerator().x << endl; 
    cout << " Y: " << params.GetSubgroupGenerator().y << endl;
    
    cout << "Subgroup Order:" << endl;
    cout << " " << params.GetSubgroupOrder() << endl;
    
    cout << "Cofactor:" << endl;
    cout << " " << params.GetCofactor() << endl;    
}

void PrintPrivateKey( const ECDSA<ECP, SHA1>::PrivateKey& key )
{   
    cout << "* Private Exponent:" << endl;
    cout << " " << key.GetPrivateExponent() << endl; 
}

void PrintPublicKey( const ECDSA<ECP, SHA1>::PublicKey& key )
{   
    cout << "* Public Element:" << endl;
    cout << " X: " << key.GetPublicElement().x << endl; 
    cout << " Y: " << key.GetPublicElement().y << endl;
}

void SavePrivateKey( const string& filename, const ECDSA<ECP, SHA1>::PrivateKey& key )
{
    key.Save( FileSink( filename.c_str(), true /*binary*/ ).Ref() );
}

void SavePublicKey( const string& filename, const ECDSA<ECP, SHA1>::PublicKey& key )
{   
    key.Save( FileSink( filename.c_str(), true /*binary*/ ).Ref() );
}

void LoadPrivateKey( const string& filename, ECDSA<ECP, SHA1>::PrivateKey& key )
{   
    key.Load( FileSource( filename.c_str(), true /*pump all*/ ).Ref() );
}

void LoadPublicKey( const string& filename, ECDSA<ECP, SHA1>::PublicKey& key )
{
    key.Load( FileSource( filename.c_str(), true /*pump all*/ ).Ref() );
}