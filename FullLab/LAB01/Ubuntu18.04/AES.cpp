#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
// thêm thư viện hỗ trợ nhập xuất tiếng Việt
// #include <io.h>
#include <fcntl.h>
// thêm thư viện hỗ trợ chuyển string <-> wstring
#include <locale>
#include <codecvt>
// thêm thư viện hỗ trợ đo thời gian
#include <chrono>
//Thư viện phát hiện lỗi logic trong khâu coding
#include "assert.h"
using namespace std;

#include "include/cryptopp/osrng.h"
// using CryptoPP::AutoSeededRandomPool;
#include "include/cryptopp/cryptlib.h"
// using CryptoPP::Exception;
#include "include/cryptopp/hex.h"
// using CryptoPP::HexEncoder;
// using CryptoPP::HexDecoder;
#include "include/cryptopp/filters.h"
// using CryptoPP::StringSink;
// using CryptoPP::StringSource;
// using CryptoPP::StreamTransformationFilter;
#include "include/cryptopp/aes.h"
// using CryptoPP::AES;
#include "include/cryptopp/modes.h"
// using CryptoPP::CBC_CTS_Mode;
#include "include/cryptopp/ccm.h"
// using CryptoPP::CCM;
#include "include/cryptopp/eax.h"
// using CryptoPP::EAX;
#include "include/cryptopp/gcm.h"
// using CryptoPP::GCM;
#include "include/cryptopp/xts.h"
// using CryptoPP::XTS;
#include "include/cryptopp/secblock.h"
// using CryptoPP::SecByteBlock;
#include "include/cryptopp/files.h"
// using CryptoPP::FileSource;
// using CryptoPP::FileSink;
// using CryptoPP::BufferedTransformation;
using namespace CryptoPP;

using CryptoPP::byte;
const int TAG_SIZE = 16;
// Xây dựng hàm chuyển đổi từ kiểu string sang wstring để hỗ trợ tiếng Việt
wstring StringToWString(const string& str)
{
    // Khai báo đối tượng converterX thuộc lớp <wstring_convert> convert từ kiểu utf8
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;
    // Trả về kết quả chuyển đổi được từ một byte string sang wide string
    return converterX.from_bytes(str);
}
// Xây dựng hàm chuyển wstring sang string
string WStringToString(const wstring& wstr)
{
    // Khai báo đối tượng converterX thuộc lớp <wstring_convert>
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;
    // Trả về kết quả chuyển đổi được từ một wide string sang byte string
    return converterX.to_bytes(wstr);
}
using namespace std::chrono;

// Hàm nhập key từ màn hình
void InputKeyFormScreen(CryptoPP::byte* key, int KEYLENGTH)
{
	delete key;
	key = new CryptoPP::byte[KEYLENGTH];
	if (KEYLENGTH == 0) return;
	wstring wKey;
	wcout<<L"\tKey : "; 
	fflush(stdin);
	getline(wcin,wKey);

	string sKey = WStringToString(wKey);
	if (KEYLENGTH!= (int)sKey.length())
	{
		wcout<<L"\tVui lòng kiểm tra lại! Độ dài Key bạn chọn là "<< KEYLENGTH<<" byte!!!" <<endl;
		InputKeyFormScreen(key,KEYLENGTH);
		return;
	}

	for (int i=0; i<KEYLENGTH ;i++)
	{
		key[i]= (unsigned char) sKey[i];
	}
}
// Hàm nhập IV từ màn hình
void InputIVFormScreen(CryptoPP::byte* iv, int IVLENGTH)
{
	delete iv;
	iv = new CryptoPP::byte[IVLENGTH];
	if (IVLENGTH == 0) return;
	wstring wKey;
	wcout<<L"\tIV : "; 
	fflush(stdin);
	getline(wcin,wKey);

	string sKey = WStringToString(wKey);
	if (IVLENGTH != (int)sKey.length())
	{
		wcout<<L"\tVui lòng kiểm tra lại! Độ dài cần nhập là "<< IVLENGTH<<" byte!!!" <<endl;
		InputIVFormScreen(iv,IVLENGTH);
		return;
	}

	for (int i=0; i<IVLENGTH ;i++)
	{
		iv[i]= (unsigned char) sKey[i];
	}
}
// Hàm nhập key từ file
void InputKeyFromFiles(CryptoPP::byte* key, int KEYLENGTH)
{
	/* Reading key from file*/
	FileSource fs("FileKey.key", false);
	/*Create space  for key*/ 
	CryptoPP::ArraySink copykey(key, KEYLENGTH);
	/*Copy data from FileKey.key  to  key */ 
	fs.Detach(new Redirector(copykey));
	fs.Pump(KEYLENGTH);  // Pump first 16 bytes
}
// Hàm nhập iv từ file
void InputIVFromFiles(CryptoPP::byte* iv, int IVLENGTH)
{
	/* Reading key from file*/
	FileSource fs("FileIV.key", false);
	/*Create space  for key*/ 
	CryptoPP::ArraySink copyiv(iv, IVLENGTH);
	/*Copy data from FileIV.key  to  key */ 
	fs.Detach(new Redirector(copyiv));
	fs.Pump(IVLENGTH);  // Pump first 16 bytes
}
// Phương thức lựa chọn mode từ màn hình
void SelectMode(int& mode)
{
	wcout<<L"<0> Lựa chọn Modes of Operation"<<endl;
	wcout<<L"\t[1] XTS MODE"<<endl;
	wcout<<L"\t[2] CBC MODE"<<endl;
	wcout<<L"\t[3] CCM MODE"<<endl;
	wcout<<L"\t[4] CFB MODE"<<endl;
	wcout<<L"\t[5] CTR MODE"<<endl;
	wcout<<L"\t[6] EAX MODE"<<endl;
	wcout<<L"\t[7] ECB MODE"<<endl;
	wcout<<L"\t[8] GCM MODE"<<endl;
	wcout<<L"\t[9] OFB MODE"<<endl;
	do
	{
		wcout<<L"\tLựa chọn (1-9): ";
		wcin>>mode;
		wcin.ignore(1);
		mode = (int) mode;
	} while (mode>9 || mode<0);
}
// Phương thức nhập plaintext từ màn hình
void InputPlainText(string& plain)
{
	wstring plaintext;
	// Nhập plain text từ màn hình
	wcout<<L"<1> Nhập plaintext: ";
	fflush(stdin);
	getline(wcin,plaintext);
	plain = WStringToString(plaintext);
}
// Hàm cài đặt khóa và vector iv
void SetupKeyAndIV(CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Tạo đối tượng [prng] thuộc lớp AutoSeededRandomPool để sinh mảng ngẫu nhiên
	AutoSeededRandomPool prng;
	wcout<<L"\t<+>Lựa chọn khóa: \n\t\t[1] : Khóa ngẫu nhiên\n\t\t[2] : Nhập từ screen\n\t\t[3] : Nhập từ file\n";
	while (true)
	{
		int k;
		wcout<<L"\t\tChọn : ";
		wcin>>k;
		wcin.ignore(1);
		switch (k)
		{
		case 1: // Tạo khóa [key] là mảng byte ngẫu nhiên
			prng.GenerateBlock(key,KEYLENGTH);
			prng.GenerateBlock(iv, IVLENGTH);
			break;
		case 2:
			wcout<<L"\t<+>Nhập Key và IV \n";
			InputKeyFormScreen(key,KEYLENGTH);
			InputIVFormScreen(iv,IVLENGTH);
			break;
		case 3:
			InputKeyFromFiles(key,KEYLENGTH);
			InputIVFromFiles(iv,IVLENGTH);
			break;
		default:
			continue;
			break;
		}
		break;
	};
}
// Phương thức in khóa và key dạng Hex ra màn hình
void PrintKeyAndIVToScreen(CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	string encoded;
	//In khóa KEY và vector IV ra màn hình
	wcout << L"<3> In ra dạng HEX của Key và IV\n";
	//Làm sạch biến [encoded]
	encoded.clear();
	// chuyển [key] dạng sơ số 2 sang dạng cơ số 16 lưu ở dạng string vào biến encoded
	StringSource(key, KEYLENGTH, true, new HexEncoder(new StringSink(encoded))); 
	// Xuất Key ra màn hình
	wcout << "\t<+> Key: " << StringToWString(encoded) << endl;
	
	//Làm sạch biến [encoded]
	encoded.clear();
	// chuyển [iv] dạng sơ số 2 sang dạng cơ số 16 lưu ở dạng string vào biến encoded
	StringSource(iv, IVLENGTH, true, new HexEncoder(new StringSink(encoded))); 
	// Xuất IV ra màn hình
	wcout << "\t<+> IV: " << StringToWString(encoded) << endl;
}

/*------------------------------------------------------------------------------------------------------------*/
/*********************************\
[1]CBC_CTS_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_CBC_CTS_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc CBC_CTS_Mode< AES >::Encryption
	CBC_CTS_Mode< AES >::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key,KEYLENGTH,iv,IVLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho CBC_CTS_Mode< AES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
		StringSource s(plain, true, new StreamTransformationFilter(e, new StringSink(cipher)));
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về cipher dạng byte string
	return cipher;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_CBC_CTS_Mode(string cipher, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc CBC_CTS_Mode< AES >::Encryption
	CBC_CTS_Mode< AES >::Decryption d;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	d.SetKeyWithIV(key,KEYLENGTH,iv,IVLENGTH);

	string recovered;
	try
	{
		// Giải mã cipher (bit) lưu vào encovered (ASCII)
		StringSource s(cipher, true, new StreamTransformationFilter(d, new StringSink(recovered)));
	}
	// bắt lỗi và in ra màn hình nếu trong quá trình giải mã xuất hiện lỗi
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return recovered;
}
// Chế độ CBC_CTS_Mode
void cryption_CBC_CTS_Mode()
{
	wcout<<L"\t********CBC_CTS_Mode********\n";
/*********************************\
KHAI BÁO
\*********************************/
	// Khai báo biến cipher, encoded: lưu string đã được chuyển
	string cipher, encoded, recovered, plain;
	// Nhập KEYLENGTH;
	int KEYLENGTH;
	int IVLENGTH = AES::BLOCKSIZE;
	CryptoPP::byte* iv;
	CryptoPP::byte* key;
/*********************************\
1. Nhập plaintext
\*********************************/
	InputPlainText(plain);
/*********************************\
2. Setup Key và IV
\*********************************/
	wcout<<L"<2> Cài đặt KEY và IV:\n";
	wcout<<L"\t<+>Lựa chọn độ dài cho KEY (16 bytes hoặc 24 bytes hoặc 32 bytes) : ";
	wcin >> KEYLENGTH;

	// đặt khóa và iv
	key = new CryptoPP::byte[KEYLENGTH];
	iv = new CryptoPP::byte[IVLENGTH];
	
	SetupKeyAndIV(key,KEYLENGTH,iv,IVLENGTH);
	PrintKeyAndIVToScreen(key,KEYLENGTH,iv,IVLENGTH);
/*********************************\
4.Mã hóa PLAIN TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa plain text 10000 lần
	auto start = high_resolution_clock::now();
	// Lặp lại việc mã hóa 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		cipher = Encryption_CBC_CTS_Mode(plain,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa 10000 lần
	auto stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	auto duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian mã hóa ra màn hình
	wcout << L"<4> Thời gian mã hóa plain text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	
	// In CIPHER TEXT dạng Hexa
	// Làm sạch biến encoded
	encoded.clear();
	// Chuyển cipher(bit) thành dạng Hex vào biến encoded
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); 
	// In cipher text ra màn hình
	wcout << L"<5> Cipher text: " << StringToWString(encoded) << endl;
/*********************************\
6.Giải mã CIPHER TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa
	start = high_resolution_clock::now();
	// Lặp lại việc giải mã 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		recovered = Decryption_CBC_CTS_Mode(cipher,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa
	stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian giải mã ra màn hình
	wcout <<L"<6> Thời gian giải mã cipher text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	// In recovered text ra màn hình
	wcout << L"<7> Recovered text: " << StringToWString(recovered) << endl;
}
//Done

/*********************************\
[2]CBC_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_CBC_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc CBC_CTS_Mode< AES >::Encryption
	CBC_Mode< AES >::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key,KEYLENGTH,iv,IVLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho CBC_CTS_Mode< AES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
		StringSource s(plain, true, new StreamTransformationFilter(e, new StringSink(cipher)));
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về cipher dạng byte string
	return cipher;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_CBC_Mode(string cipher, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc CBC_Mode< AES >::Encryption
	CBC_Mode< AES >::Decryption d;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	d.SetKeyWithIV(key,KEYLENGTH,iv,IVLENGTH);

	string recovered;
	try
	{
		// Giải mã cipher (bit) lưu vào encovered (ASCII)
		StringSource s(cipher, true, new StreamTransformationFilter(d, new StringSink(recovered)));
	}
	// bắt lỗi và in ra màn hình nếu trong quá trình giải mã xuất hiện lỗi
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return recovered;
}
// Chế độ CBC_Mode
void cryption_CBC_Mode()
{
	wcout<<L"\t********CBC_Mode********\n";
/*********************************\
KHAI BÁO
\*********************************/
	// Khai báo biến cipher, encoded: lưu string đã được chuyển
	string cipher, encoded, recovered, plain;
	// Nhập KEYLENGTH;
	int KEYLENGTH;
	int IVLENGTH = AES::BLOCKSIZE;
	CryptoPP::byte* iv;
	CryptoPP::byte* key;
/*********************************\
1. Nhập plaintext
\*********************************/
	InputPlainText(plain);
/*********************************\
2. Setup Key và IV
\*********************************/
	wcout<<L"<2> Cài đặt KEY và IV:\n";
	wcout<<L"\t<+>Lựa chọn độ dài cho KEY (16 bytes hoặc 24 bytes hoặc 32 bytes) : ";
	wcin >> KEYLENGTH;

	// đặt khóa và iv
	key = new CryptoPP::byte[KEYLENGTH];
	iv = new CryptoPP::byte[IVLENGTH];
	
	SetupKeyAndIV(key,KEYLENGTH,iv,IVLENGTH);
	PrintKeyAndIVToScreen(key,KEYLENGTH,iv,IVLENGTH);
/*********************************\
4.Mã hóa PLAIN TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa plain text 100000 lần
	auto start = high_resolution_clock::now();
	// Lặp lại việc mã hóa 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		cipher = Encryption_CBC_Mode(plain,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa 10000 lần
	auto stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	auto duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian mã hóa ra màn hình
	wcout << L"<4> Thời gian mã hóa plain text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	
	// In CIPHER TEXT dạng Hexa
	// Làm sạch biến encoded
	encoded.clear();
	// Chuyển cipher(bit) thành dạng Hex vào biến encoded
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); 
	// In cipher text ra màn hình
	wcout << L"<5> Cipher text: " << StringToWString(encoded) << endl;
/*********************************\
6.Giải mã CIPHER TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa
	start = high_resolution_clock::now();
	// Lặp lại việc giải mã 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		recovered = Decryption_CBC_Mode(cipher,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa
	stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian giải mã ra màn hình
	wcout <<L"<6> Thời gian giải mã cipher text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	// In recovered text ra màn hình
	wcout << L"<7> Recovered text: " << StringToWString(recovered) << endl;
}
// Done

/*********************************\
[3]CCM_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_CCM_Mode(string pdata,string adata, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	string cipher;
	try
	{
		// Khai báo biến e thuộc CCM< AES >::Encryption
		CCM< AES,TAG_SIZE >::Encryption e;
		// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
		e.SetKeyWithIV(key, KEYLENGTH, iv, IVLENGTH);
		e.SpecifyDataLengths( adata.size(), pdata.size(), 0 );

        // Not required for GCM mode (but required for CCM mode)
        // e.SpecifyDataLengths( adata.size(), pdata.size(), 0 );

        AuthenticatedEncryptionFilter ef( e,
            new StringSink( cipher )
        ); // AuthenticatedEncryptionFilter

        // AuthenticatedEncryptionFilter::ChannelPut
        //  defines two channels: "" (empty) and "AAD"
        //   channel "" is encrypted and authenticated
        //   channel "AAD" is authenticated
        ef.ChannelPut( "AAD", (const CryptoPP::byte*)adata.data(), adata.size() );
        ef.ChannelMessageEnd("AAD");
		
        // Authenticated data *must* be pushed before
        //  Confidential/Authenticated data. Otherwise
        //  we must catch the BadState exception
        ef.ChannelPut( "", (const CryptoPP::byte*)pdata.data(), pdata.size() );
        ef.ChannelMessageEnd("");

		return cipher;
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về cipher dạng byte string
	return cipher;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_CCM_Mode(string cipher,string radata, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	string rpdata;
	try
    {
        // Break the cipher text out into it's
        //  components: Encrypted Data and MAC Value
        string enc = cipher.substr( 0, cipher.length()-TAG_SIZE );
        string mac = cipher.substr( cipher.length()-TAG_SIZE );

		CCM< AES,TAG_SIZE>::Decryption d;
        d.SetKeyWithIV( key, KEYLENGTH, iv, IVLENGTH);
		d.SpecifyDataLengths( radata.size(), enc.size(), 0 );

        // Object will not throw an exception
        //  during decryption\verification _if_
        //  verification fails.
        //AuthenticatedDecryptionFilter df( d, NULL,
        // AuthenticatedDecryptionFilter::MAC_AT_BEGIN );

        AuthenticatedDecryptionFilter df( d, NULL,
            AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
            AuthenticatedDecryptionFilter::THROW_EXCEPTION, TAG_SIZE );

        // The order of the following calls are important
        df.ChannelPut( "", (const CryptoPP::byte*)mac.data(), mac.size() );
        df.ChannelPut( "AAD", (const CryptoPP::byte*)radata.data(), radata.size() ); 
        df.ChannelPut( "", (const CryptoPP::byte*)enc.data(), enc.size() );               

        // If the object throws, it will most likely occur
        //  during ChannelMessageEnd()
        df.ChannelMessageEnd( "AAD" );
        df.ChannelMessageEnd( "" );

        // If the object does not throw, here's the only
        //  opportunity to check the data's integrity
        bool b = false;
        b = df.GetLastResult();
        assert( true == b );

        // Remove data from channel
        string retrieved;
        size_t n = (size_t)-1;

        // Plain text recovered from enc.data()
        df.SetRetrievalChannel( "" );
        n = (size_t)df.MaxRetrievable();
        retrieved.resize( n );

        if( n > 0 ) { df.Get( (CryptoPP::byte*)retrieved.data(), n ); }
		rpdata = retrieved;
    }
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return rpdata;
}
// CCM_Mode
void cryption_CCM_Mode()
{
	wcout<<L"\t********CCM_Mode********\n";
/*********************************\
KHAI BÁO
\*********************************/
	// Khai báo biến cipher, encoded: lưu string đã được chuyển
	string cipher, encoded, recovered, plain,adata;
	// Nhập KEYLENGTH;
	int KEYLENGTH;
	int IVLENGTH = 12;
	CryptoPP::byte* iv;
	CryptoPP::byte* key;
/*********************************\
1. Nhập plaintext và adata
\*********************************/
	InputPlainText(plain);
	
	wstring wadata;
	// Nhập adata từ màn hình
	wcout<<L"    Nhập adata: ";
	fflush(stdin);
	getline(wcin,wadata);
	adata = WStringToString(wadata);

/*********************************\
2. Setup Key và IV
\*********************************/
	wcout<<L"<2> Cài đặt KEY và IV:\n";
	wcout<<L"\t<+>Lựa chọn độ dài cho KEY (16 bytes hoặc 24 bytes hoặc 32 bytes) : ";
	wcin >> KEYLENGTH;

	// đặt khóa và iv
	key = new CryptoPP::byte[KEYLENGTH];
	iv = new CryptoPP::byte[IVLENGTH];
	
	SetupKeyAndIV(key,KEYLENGTH,iv,IVLENGTH);
	PrintKeyAndIVToScreen(key,KEYLENGTH,iv,IVLENGTH);
/*********************************\
4.Mã hóa PLAIN TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa plain text 100000 lần
	auto start = high_resolution_clock::now();
	// Lặp lại việc mã hóa 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		cipher = Encryption_CCM_Mode(plain,adata,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa 10000 lần
	auto stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	auto duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian mã hóa ra màn hình
	wcout << L"<4> Thời gian mã hóa plain text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	
	// In CIPHER TEXT dạng Hexa
	// Làm sạch biến encoded
	encoded.clear();
	// Chuyển cipher(bit) thành dạng Hex vào biến encoded
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); 
	// In cipher text ra màn hình
	wcout << L"<5> Cipher text: " << StringToWString(encoded) << endl;
/*********************************\
6.Giải mã CIPHER TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa
	start = high_resolution_clock::now();
	// adata được truyền qua 1 kênh 
	string radata = adata;
		// Lặp lại việc giải mã 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		recovered = Decryption_CCM_Mode(cipher,radata,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa
	stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian giải mã ra màn hình
	wcout <<L"<6> Thời gian giải mã cipher text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	// In recovered text ra màn hình
	wcout << L"<7> Recovered text: " << StringToWString(recovered) << endl;
}
// Done

/*********************************\
[4]CFB_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_CFB_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc CFB_Mode< AES >::Encryption
	CFB_Mode< AES >::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho CFB_Mode< AES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
		StringSource s(plain, true, new StreamTransformationFilter(e, new StringSink(cipher)));
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về cipher dạng byte string
	return cipher;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_CFB_Mode(string cipher, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến d thuộc CFB_Mode< AES >::Decryption
	CFB_Mode< AES >::Decryption d;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho d
	d.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string recovered;
	try
	{
		// Giải mã cipher (bit) lưu vào encovered (ASCII)
		StringSource s(cipher, true, new StreamTransformationFilter(d, new StringSink(recovered)));
	}
	// bắt lỗi và in ra màn hình nếu trong quá trình giải mã xuất hiện lỗi
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return recovered;
}
// Chế độ CFB_Mode
void cryption_CFB_Mode()
{
	wcout<<L"\t********CFB_Mode********\n";
/*********************************\
KHAI BÁO
\*********************************/
	// Khai báo biến cipher, encoded: lưu string đã được chuyển
	string cipher, encoded, recovered, plain;
	// Nhập KEYLENGTH;
	int KEYLENGTH;
	int IVLENGTH = AES::BLOCKSIZE;
	CryptoPP::byte* iv;
	CryptoPP::byte* key;
/*********************************\
1. Nhập plaintext
\*********************************/
	InputPlainText(plain);
/*********************************\
2. Setup Key và IV
\*********************************/
	wcout<<L"<2> Cài đặt KEY và IV:\n";
	wcout<<L"\t<+>Lựa chọn độ dài cho KEY (16 bytes hoặc 24 bytes hoặc 32 bytes) : ";
	wcin >> KEYLENGTH;

	// đặt khóa và iv
	key = new CryptoPP::byte[KEYLENGTH];
	iv = new CryptoPP::byte[IVLENGTH];
	
	SetupKeyAndIV(key,KEYLENGTH,iv,IVLENGTH);
	PrintKeyAndIVToScreen(key,KEYLENGTH,iv,IVLENGTH);
/*********************************\
4.Mã hóa PLAIN TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa plain text 10000 lần
	auto start = high_resolution_clock::now();
	// Lặp lại việc mã hóa 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		cipher = Encryption_CFB_Mode(plain,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa 10000 lần
	auto stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	auto duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian mã hóa ra màn hình
	wcout << L"<4> Thời gian mã hóa plain text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	
	// In CIPHER TEXT dạng Hexa
	// Làm sạch biến encoded
	encoded.clear();
	// Chuyển cipher(bit) thành dạng Hex vào biến encoded
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); 
	// In cipher text ra màn hình
	wcout << L"<5> Cipher text: " << StringToWString(encoded) << endl;
/*********************************\
6.Giải mã CIPHER TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa
	start = high_resolution_clock::now();
	// Lặp lại việc giải mã 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		recovered = Decryption_CFB_Mode(cipher,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa
	stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian giải mã ra màn hình
	wcout <<L"<6> Thời gian giải mã cipher text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	// In recovered text ra màn hình
	wcout << L"<7> Recovered text: " << StringToWString(recovered) << endl;
}

/*********************************\
[5]CTR_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_CTR_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc CTR_Mode< AES >::Encryption
	CTR_Mode< AES >::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho CTR_Mode< AES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
		StringSource s(plain, true, new StreamTransformationFilter(e, new StringSink(cipher)));
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về cipher dạng byte string
	return cipher;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_CTR_Mode(string cipher, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến d thuộc CTR_Mode< AES >::Decryption
	CTR_Mode< AES >::Decryption d;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho d
	d.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string recovered;
	try
	{
		// Giải mã cipher (bit) lưu vào encovered (ASCII)
		StringSource s(cipher, true, new StreamTransformationFilter(d, new StringSink(recovered)));
	}
	// bắt lỗi và in ra màn hình nếu trong quá trình giải mã xuất hiện lỗi
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return recovered;
}
// CTR_Mode
void cryption_CTR_Mode()
{
	wcout<<L"\t********CTR_Mode********\n";
/*********************************\
KHAI BÁO
\*********************************/
	// Khai báo biến cipher, encoded: lưu string đã được chuyển
	string cipher, encoded, recovered, plain;
	// Nhập KEYLENGTH;
	int KEYLENGTH;
	int IVLENGTH = AES::BLOCKSIZE;
	CryptoPP::byte* iv;
	CryptoPP::byte* key;
/*********************************\
1. Nhập plaintext
\*********************************/
	InputPlainText(plain);
/*********************************\
2. Setup Key và IV
\*********************************/
	wcout<<L"<2> Cài đặt KEY và IV:\n";
	wcout<<L"\t<+>Lựa chọn độ dài cho KEY (16 bytes hoặc 24 bytes hoặc 32 bytes) : ";
	wcin >> KEYLENGTH;

	// đặt khóa và iv
	key = new CryptoPP::byte[KEYLENGTH];
	iv = new CryptoPP::byte[IVLENGTH];
	
	SetupKeyAndIV(key,KEYLENGTH,iv,IVLENGTH);
	PrintKeyAndIVToScreen(key,KEYLENGTH,iv,IVLENGTH);
/*********************************\
4.Mã hóa PLAIN TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa plain text 10000 lần
	auto start = high_resolution_clock::now();
	// Lặp lại việc mã hóa 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		cipher = Encryption_CTR_Mode(plain,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa 10000 lần
	auto stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	auto duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian mã hóa ra màn hình
	wcout << L"<4> Thời gian mã hóa plain text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	
	// In CIPHER TEXT dạng Hexa
	// Làm sạch biến encoded
	encoded.clear();
	// Chuyển cipher(bit) thành dạng Hex vào biến encoded
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); 
	// In cipher text ra màn hình
	wcout << L"<5> Cipher text: " << StringToWString(encoded) << endl;
/*********************************\
6.Giải mã CIPHER TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa
	start = high_resolution_clock::now();
	// Lặp lại việc giải mã 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		recovered = Decryption_CTR_Mode(cipher,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa
	stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian giải mã ra màn hình
	wcout <<L"<6> Thời gian giải mã cipher text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	// In recovered text ra màn hình
	wcout << L"<7> Recovered text: " << StringToWString(recovered) << endl;
}
// Done

/*********************************\
[6]EAX_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string  Encryption_EAX_Mode(string pdata,string adata, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	string cipher;
	try
	{
		// Khai báo biến e thuộc GCM< AES >::Encryption
		EAX< AES >::Encryption e;
		// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
		e.SetKeyWithIV(key, KEYLENGTH, iv, IVLENGTH);

        // Not required for GCM mode (but required for CCM mode)
        // e.SpecifyDataLengths( adata.size(), pdata.size(), 0 );

        AuthenticatedEncryptionFilter ef( e,
            new StringSink( cipher ), false, TAG_SIZE
        ); // AuthenticatedEncryptionFilter

        // AuthenticatedEncryptionFilter::ChannelPut
        //  defines two channels: "" (empty) and "AAD"
        //   channel "" is encrypted and authenticated
        //   channel "AAD" is authenticated
        ef.ChannelPut( "AAD", (const CryptoPP::byte*)adata.data(), adata.size() );
        ef.ChannelMessageEnd("AAD");

        // Authenticated data *must* be pushed before
        //  Confidential/Authenticated data. Otherwise
        //  we must catch the BadState exception
        ef.ChannelPut( "", (const CryptoPP::byte*)pdata.data(), pdata.size() );
        ef.ChannelMessageEnd("");

		return cipher;
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về cipher dạng byte string
	return cipher;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_EAX_Mode(string cipher,string radata, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	string rpdata;
	try
    {
        EAX< AES >::Decryption d;
        d.SetKeyWithIV( key, KEYLENGTH, iv, IVLENGTH);

        // Break the cipher text out into it's
        //  components: Encrypted Data and MAC Value
        string enc = cipher.substr( 0, cipher.length()-TAG_SIZE );
        string mac = cipher.substr( cipher.length()-TAG_SIZE );

        // Object will not throw an exception
        //  during decryption\verification _if_
        //  verification fails.
        //AuthenticatedDecryptionFilter df( d, NULL,
        // AuthenticatedDecryptionFilter::MAC_AT_BEGIN );

        AuthenticatedDecryptionFilter df( d, NULL,
            AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
            AuthenticatedDecryptionFilter::THROW_EXCEPTION, TAG_SIZE );

        // The order of the following calls are important
        df.ChannelPut( "", (const CryptoPP::byte*)mac.data(), mac.size() );
        df.ChannelPut( "AAD", (const CryptoPP::byte*)radata.data(), radata.size() ); 
        df.ChannelPut( "", (const CryptoPP::byte*)enc.data(), enc.size() );               

        // If the object throws, it will most likely occur
        //  during ChannelMessageEnd()
        df.ChannelMessageEnd( "AAD" );
        df.ChannelMessageEnd( "" );

        // If the object does not throw, here's the only
        //  opportunity to check the data's integrity
        bool b = false;
        b = df.GetLastResult();
        assert( true == b );

        // Remove data from channel
        string retrieved;
        size_t n = (size_t)-1;

        // Plain text recovered from enc.data()
        df.SetRetrievalChannel( "" );
        n = (size_t)df.MaxRetrievable();
        retrieved.resize( n );

        if( n > 0 ) { df.Get( (CryptoPP::byte*)retrieved.data(), n ); }
		rpdata = retrieved;
    }
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return rpdata;
}
// EAX_Mode
void cryption_EAX_Mode()
{
	wcout<<L"\t********EAX_Mode********\n";
/*********************************\
KHAI BÁO
\*********************************/
	// Khai báo biến cipher, encoded: lưu string đã được chuyển
	string cipher, encoded, recovered, plain,adata;
	// Nhập KEYLENGTH;
	int KEYLENGTH;
	int IVLENGTH = 12;
	CryptoPP::byte* iv;
	CryptoPP::byte* key;
/*********************************\
1. Nhập plaintext và adata
\*********************************/
	InputPlainText(plain);
	
	wstring wadata;
	// Nhập adata từ màn hình
	wcout<<L"    Nhập adata: ";
	fflush(stdin);
	getline(wcin,wadata);
	adata = WStringToString(wadata);

/*********************************\
2. Setup Key và IV
\*********************************/
	wcout<<L"<2> Cài đặt KEY và IV:\n";
	wcout<<L"\t<+>Lựa chọn độ dài cho KEY (16 bytes hoặc 24 bytes hoặc 32 bytes) : ";
	wcin >> KEYLENGTH;

	// đặt khóa và iv
	key = new CryptoPP::byte[KEYLENGTH];
	iv = new CryptoPP::byte[IVLENGTH];
	
	SetupKeyAndIV(key,KEYLENGTH,iv,IVLENGTH);
	PrintKeyAndIVToScreen(key,KEYLENGTH,iv,IVLENGTH);
/*********************************\
4.Mã hóa PLAIN TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa plain text 10000 lần
	auto start = high_resolution_clock::now();
	// Lặp lại việc mã hóa 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		cipher = Encryption_EAX_Mode(plain,adata,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa 10000 lần
	auto stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	auto duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian mã hóa ra màn hình
	wcout << L"<4> Thời gian mã hóa plain text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	
	// In CIPHER TEXT dạng Hexa
	// Làm sạch biến encoded
	encoded.clear();
	// Chuyển cipher(bit) thành dạng Hex vào biến encoded
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); 
	// In cipher text ra màn hình
	wcout << L"<5> Cipher text: " << StringToWString(encoded) << endl;
/*********************************\
6.Giải mã CIPHER TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa
	start = high_resolution_clock::now();
	// adata được truyền qua 1 kênh 
	string radata = adata;
		// Lặp lại việc giải mã 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		recovered = Decryption_EAX_Mode(cipher,radata,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa
	stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian giải mã ra màn hình
	wcout <<L"<6> Thời gian giải mã cipher text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	// In recovered text ra màn hình
	wcout << L"<7> Recovered text: " << StringToWString(recovered) << endl;
}
// Done

/*********************************\
[7]ECB_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_ECB_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH)
{
	// Khai báo biến e thuộc ECB_Mode< AES >::Encryption
	ECB_Mode< AES >::Encryption e;
	// Đặt khóa [key] cho e
	e.SetKey(key, KEYLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho ECB_Mode< AES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
		StringSource s(plain, true, new StreamTransformationFilter(e, new StringSink(cipher)));
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về cipher dạng byte string
	return cipher;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_ECB_Mode(string cipher, CryptoPP::byte* key, int KEYLENGTH)
{
	// Khai báo biến d thuộc ECB_Mode< AES >::Decryption
	ECB_Mode< AES >::Decryption d;
	// Đặt khóa [key] cho d
	d.SetKey(key, KEYLENGTH);

	string recovered;
	try
	{
		// Giải mã cipher (bit) lưu vào encovered (ASCII)
		StringSource s(cipher, true, new StreamTransformationFilter(d, new StringSink(recovered)));
	}
	// bắt lỗi và in ra màn hình nếu trong quá trình giải mã xuất hiện lỗi
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return recovered;
}
// CTR_Mode
void cryption_ECB_Mode()
{
	wcout<<L"\t********ECB_Mode********\n";
/*********************************\
KHAI BÁO
\*********************************/
	// Khai báo biến cipher, encoded: lưu string đã được chuyển
	string cipher, encoded, recovered, plain;
	// Nhập KEYLENGTH;
	int KEYLENGTH;
	CryptoPP::byte* key;
/*********************************\
1. Nhập plaintext
\*********************************/
	InputPlainText(plain);
/*********************************\
2. Setup Key
\*********************************/
	wcout<<L"<2> Cài đặt KEY:\n";
	wcout<<L"\t<+>Lựa chọn độ dài cho KEY (16 bytes hoặc 24 bytes hoặc 32 bytes) : ";
	wcin >> KEYLENGTH;

	// đặt khóa 
	key = new CryptoPP::byte[KEYLENGTH];
	
	// Tạo đối tượng [prng] thuộc lớp AutoSeededRandomPool để sinh mảng ngẫu nhiên
	AutoSeededRandomPool prng;
	wcout<<L"\t<+>Lựa chọn khóa: \n\t\t[1] : Khóa ngẫu nhiên\n\t\t[2] : Nhập từ screen\n\t\t[3] : Nhập từ file\n";
	while (true)
	{
		int k;
		wcout<<L"\t\tChọn : ";
		wcin>>k;
		wcin.ignore(1);
		switch (k)
		{
		case 1: // Tạo khóa [key] là mảng byte ngẫu nhiên
			prng.GenerateBlock(key,KEYLENGTH);
			break;
		case 2:
			wcout<<L"\t<+>Nhập Key và IV \n";
			InputKeyFormScreen(key,KEYLENGTH);
			break;
		case 3:
			InputKeyFromFiles(key,KEYLENGTH);
			break;
		default:
			continue;
			break;
		}
		break;
	};

	//In khóa KEY và vector IV ra màn hình
	wcout << L"<3> In ra dạng HEX của Key và IV\n";
	//Làm sạch biến [encoded]
	encoded.clear();
	// chuyển [key] dạng sơ số 2 sang dạng cơ số 16 lưu ở dạng string vào biến encoded
	StringSource(key, KEYLENGTH, true, new HexEncoder(new StringSink(encoded))); 
	// Xuất Key ra màn hình
	wcout << "\t<+> Key: " << StringToWString(encoded) << endl;
/*********************************\
4.Mã hóa PLAIN TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa plain text 10000 lần
	auto start = high_resolution_clock::now();
	// Lặp lại việc mã hóa 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		cipher = Encryption_ECB_Mode(plain,key,KEYLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa 10000 lần
	auto stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	auto duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian mã hóa ra màn hình
	wcout << L"<4> Thời gian mã hóa plain text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	
	// In CIPHER TEXT dạng Hexa
	// Làm sạch biến encoded
	encoded.clear();
	// Chuyển cipher(bit) thành dạng Hex vào biến encoded
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); 
	// In cipher text ra màn hình
	wcout << L"<5> Cipher text: " << StringToWString(encoded) << endl;
/*********************************\
6.Giải mã CIPHER TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa
	start = high_resolution_clock::now();
	// Lặp lại việc giải mã 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		recovered = Decryption_ECB_Mode(cipher,key,KEYLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa
	stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian giải mã ra màn hình
	wcout <<L"<6> Thời gian giải mã cipher text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	// In recovered text ra màn hình
	wcout << L"<7> Recovered text: " << StringToWString(recovered) << endl;
}
// Done

/*********************************\
[8]GCM_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_GCM_Mode(string pdata,string adata, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	string cipher;
	try
	{
		// Khai báo biến e thuộc GCM< AES >::Encryption
		GCM< AES >::Encryption e;
		// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
		e.SetKeyWithIV(key, KEYLENGTH, iv, IVLENGTH);

        // Not required for GCM mode (but required for CCM mode)
        // e.SpecifyDataLengths( adata.size(), pdata.size(), 0 );

        AuthenticatedEncryptionFilter ef( e,
            new StringSink( cipher ), false, TAG_SIZE
        ); // AuthenticatedEncryptionFilter

        // AuthenticatedEncryptionFilter::ChannelPut
        //  defines two channels: "" (empty) and "AAD"
        //   channel "" is encrypted and authenticated
        //   channel "AAD" is authenticated
        ef.ChannelPut( "AAD", (const CryptoPP::byte*)adata.data(), adata.size() );
        ef.ChannelMessageEnd("AAD");

        // Authenticated data *must* be pushed before
        //  Confidential/Authenticated data. Otherwise
        //  we must catch the BadState exception
        ef.ChannelPut( "", (const CryptoPP::byte*)pdata.data(), pdata.size() );
        ef.ChannelMessageEnd("");

		return cipher;
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về cipher dạng byte string
	return cipher;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_GCM_Mode(string cipher,string radata, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	string rpdata;
	try
    {
        GCM< AES >::Decryption d;
        d.SetKeyWithIV( key, KEYLENGTH, iv, IVLENGTH);

        // Break the cipher text out into it's
        //  components: Encrypted Data and MAC Value
        string enc = cipher.substr( 0, cipher.length()-TAG_SIZE );
        string mac = cipher.substr( cipher.length()-TAG_SIZE );

        // Object will not throw an exception
        //  during decryption\verification _if_
        //  verification fails.
        //AuthenticatedDecryptionFilter df( d, NULL,
        // AuthenticatedDecryptionFilter::MAC_AT_BEGIN );

        AuthenticatedDecryptionFilter df( d, NULL,
            AuthenticatedDecryptionFilter::MAC_AT_BEGIN |
            AuthenticatedDecryptionFilter::THROW_EXCEPTION, TAG_SIZE );

        // The order of the following calls are important
        df.ChannelPut( "", (const CryptoPP::byte*)mac.data(), mac.size() );
        df.ChannelPut( "AAD", (const CryptoPP::byte*)radata.data(), radata.size() ); 
        df.ChannelPut( "", (const CryptoPP::byte*)enc.data(), enc.size() );               

        // If the object throws, it will most likely occur
        //  during ChannelMessageEnd()
        df.ChannelMessageEnd( "AAD" );
        df.ChannelMessageEnd( "" );

        // If the object does not throw, here's the only
        //  opportunity to check the data's integrity
        bool b = false;
        b = df.GetLastResult();
        assert( true == b );

        // Remove data from channel
        string retrieved;
        size_t n = (size_t)-1;

        // Plain text recovered from enc.data()
        df.SetRetrievalChannel( "" );
        n = (size_t)df.MaxRetrievable();
        retrieved.resize( n );

        if( n > 0 ) { df.Get( (CryptoPP::byte*)retrieved.data(), n ); }
		rpdata = retrieved;
    }
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return rpdata;
}
// GCM_Mode
void cryption_GCM_Mode()
{
	wcout<<L"\t********GCM_Mode********\n";
/*********************************\
KHAI BÁO
\*********************************/
	// Khai báo biến cipher, encoded: lưu string đã được chuyển
	string cipher, encoded, recovered, plain,adata;
	// Nhập KEYLENGTH;
	int KEYLENGTH;
	int IVLENGTH = 12;
	CryptoPP::byte* iv;
	CryptoPP::byte* key;
/*********************************\
1. Nhập plaintext và adata
\*********************************/
	InputPlainText(plain);
	
	wstring wadata;
	// Nhập adata từ màn hình
	wcout<<L"    Nhập adata: ";
	fflush(stdin);
	getline(wcin,wadata);
	adata = WStringToString(wadata);

/*********************************\
2. Setup Key và IV
\*********************************/
	wcout<<L"<2> Cài đặt KEY và IV:\n";
	wcout<<L"\t<+>Lựa chọn độ dài cho KEY (16 bytes hoặc 24 bytes hoặc 32 bytes) : ";
	wcin >> KEYLENGTH;

	// đặt khóa và iv
	key = new CryptoPP::byte[KEYLENGTH];
	iv = new CryptoPP::byte[IVLENGTH];
	
	SetupKeyAndIV(key,KEYLENGTH,iv,IVLENGTH);
	PrintKeyAndIVToScreen(key,KEYLENGTH,iv,IVLENGTH);
/*********************************\
4.Mã hóa PLAIN TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa plain text 10000 lần
	auto start = high_resolution_clock::now();
	// Lặp lại việc mã hóa 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		cipher = Encryption_GCM_Mode(plain,adata,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa 10000 lần
	auto stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	auto duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian mã hóa ra màn hình
	wcout << L"<4> Thời gian mã hóa plain text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	
	// In CIPHER TEXT dạng Hexa
	// Làm sạch biến encoded
	encoded.clear();
	// Chuyển cipher(bit) thành dạng Hex vào biến encoded
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); 
	// In cipher text ra màn hình
	wcout << L"<5> Cipher text: " << StringToWString(encoded) << endl;
/*********************************\
6.Giải mã CIPHER TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa
	start = high_resolution_clock::now();
	// adata được truyền qua 1 kênh 
	string radata = adata;
		// Lặp lại việc giải mã 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		recovered = Decryption_GCM_Mode(cipher,radata,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa
	stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian giải mã ra màn hình
	wcout <<L"<6> Thời gian giải mã cipher text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	// In recovered text ra màn hình
	wcout << L"<7> Recovered text: " << StringToWString(recovered) << endl;
}
// Done

/*********************************\
[9]OFB_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_OFB_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc OFB_Mode< AES >::Encryption
	OFB_Mode< AES >::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho OFB_Mode< AES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
		StringSource s(plain, true, new StreamTransformationFilter(e, new StringSink(cipher)));
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về cipher dạng byte string
	return cipher;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_OFB_Mode(string cipher, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến d thuộc OFB_Mode< AES >::Decryption
	OFB_Mode< AES >::Decryption d;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho d
	d.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string recovered;
	try
	{
		// Giải mã cipher (bit) lưu vào encovered (ASCII)
		StringSource s(cipher, true, new StreamTransformationFilter(d, new StringSink(recovered)));
	}
	// bắt lỗi và in ra màn hình nếu trong quá trình giải mã xuất hiện lỗi
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return recovered;
}
// OFB_Mode
void cryption_OFB_Mode()
{
	wcout<<L"\t********OFB_Mode********\n";
/*********************************\
KHAI BÁO
\*********************************/
	// Khai báo biến cipher, encoded: lưu string đã được chuyển
	string cipher, encoded, recovered, plain;
	// Nhập KEYLENGTH;
	int KEYLENGTH;
	int IVLENGTH = AES::BLOCKSIZE;
	CryptoPP::byte* iv;
	CryptoPP::byte* key;
/*********************************\
1. Nhập plaintext
\*********************************/
	InputPlainText(plain);
/*********************************\
2. Setup Key và IV
\*********************************/
	wcout<<L"<2> Cài đặt KEY và IV:\n";
	wcout<<L"\t<+>Lựa chọn độ dài cho KEY (16 bytes hoặc 24 bytes hoặc 32 bytes) : ";
	wcin >> KEYLENGTH;

	// đặt khóa và iv
	key = new CryptoPP::byte[KEYLENGTH];
	iv = new CryptoPP::byte[IVLENGTH];
	
	SetupKeyAndIV(key,KEYLENGTH,iv,IVLENGTH);
	PrintKeyAndIVToScreen(key,KEYLENGTH,iv,IVLENGTH);
/*********************************\
4.Mã hóa PLAIN TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa plain text 10000 lần
	auto start = high_resolution_clock::now();
	// Lặp lại việc mã hóa 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		cipher = Encryption_OFB_Mode(plain,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa 10000 lần
	auto stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	auto duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian mã hóa ra màn hình
	wcout << L"<4> Thời gian mã hóa plain text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	
	// In CIPHER TEXT dạng Hexa
	// Làm sạch biến encoded
	encoded.clear();
	// Chuyển cipher(bit) thành dạng Hex vào biến encoded
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); 
	// In cipher text ra màn hình
	wcout << L"<5> Cipher text: " << StringToWString(encoded) << endl;
/*********************************\
6.Giải mã CIPHER TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa
	start = high_resolution_clock::now();
	// Lặp lại việc giải mã 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		recovered = Decryption_OFB_Mode(cipher,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa
	stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian giải mã ra màn hình
	wcout <<L"<6> Thời gian giải mã cipher text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	// In recovered text ra màn hình
	wcout << L"<7> Recovered text: " << StringToWString(recovered) << endl;
}
// Done

/*********************************\
[1]XTS_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_XTS_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc XTS< AES >::Encryption
	XTS< AES >::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho XTS< AES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
		StringSource ss( plain, true, 
            new StreamTransformationFilter( e,
                new StringSink( cipher ),
                StreamTransformationFilter::NO_PADDING
            ) // StreamTransformationFilter      
        );
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về cipher dạng byte string
	return cipher;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_XTS_Mode(string cipher, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến d thuộc XTS< AES >::Decryption
	XTS< AES >::Decryption d;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho d
	d.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string recovered;
	try
	{
		// Giải mã cipher (bit) lưu vào encovered (ASCII) thông qua bộ lọc StreamTransformationFilter
        StringSource ss( cipher, true, 
            new StreamTransformationFilter( d,
                new StringSink( recovered ),
                StreamTransformationFilter::NO_PADDING
            ) // StreamTransformationFilter
        ); // StringSource  
	}
	// bắt lỗi và in ra màn hình nếu trong quá trình giải mã xuất hiện lỗi
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return recovered;
}
// XTS_MODE
void cryption_XTS_Mode()
{
	wcout<<L"\t********XTS_Mode********\n";
/*********************************\
KHAI BÁO
\*********************************/
	// Khai báo biến cipher, encoded: lưu string đã được chuyển
	string cipher, encoded, recovered, plain;
	// Nhập KEYLENGTH;
	int KEYLENGTH;
	int IVLENGTH = AES::BLOCKSIZE;
	CryptoPP::byte* iv;
	CryptoPP::byte* key;
/*********************************\
1. Nhập plaintext
\*********************************/
	InputPlainText(plain);
	//Padding
	while (plain.length() % AES::BLOCKSIZE != 0)
		plain += (char) 0;
/*********************************\
2. Setup Key và IV
\*********************************/
	wcout<<L"<2> Cài đặt KEY và IV:\n";
	wcout<<L"\t<+> Độ dài KEY mặc định của mode AES/XTS là 32\n";
	// wcin >> KEYLENGTH;
	KEYLENGTH = 32;

	// đặt khóa và iv
	key = new CryptoPP::byte[KEYLENGTH];
	iv = new CryptoPP::byte[IVLENGTH];
	
	SetupKeyAndIV(key,KEYLENGTH,iv,IVLENGTH);
	PrintKeyAndIVToScreen(key,KEYLENGTH,iv,IVLENGTH);
/*********************************\
4.Mã hóa PLAIN TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa plain text 10000 lần
	auto start = high_resolution_clock::now();
	// Lặp lại việc mã hóa 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		cipher = Encryption_XTS_Mode(plain,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa 10000 lần
	auto stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	auto duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian mã hóa ra màn hình
	wcout << L"<4> Thời gian mã hóa plain text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	
	// In CIPHER TEXT dạng Hexa
	// Làm sạch biến encoded
	encoded.clear();
	// Chuyển cipher(bit) thành dạng Hex vào biến encoded
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded))); 
	// In cipher text ra màn hình
	wcout << L"<5> Cipher text: " << StringToWString(encoded) << endl;
/*********************************\
6.Giải mã CIPHER TEXT
\*********************************/
	// Lấy mốc thời gian trước khi mã hóa
	start = high_resolution_clock::now();
	// Lặp lại việc giải mã 10000 lần để đo thời gian
	for (int i = 0;i<10000;i++)
		recovered = Decryption_XTS_Mode(cipher,key,KEYLENGTH,iv,IVLENGTH);
	// Lấy mốc thời gian khi kết thúc việc mã hóa
	stop = high_resolution_clock::now();
	// Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	duration = duration_cast<microseconds>(stop - start);
	// In giá trị của thời gian giải mã ra màn hình
	wcout <<L"<6> Thời gian giải mã cipher text 10000 lần là: "<< duration.count() <<L" microseconds"<< endl;
	// In recovered text ra màn hình
	wcout << L"<7> Recovered text: " << StringToWString(recovered) << endl;
}
// Done

/*------------------------------------------------------------------------------------------------------------*/


int main(int argc, char* argv[])
{
/*********************************\
Hỗ trợ Tiếng Việt
\*********************************/
	#ifdef __linux__
	setlocale(LC_ALL,"");
	#elif _WIN32
	// Cho phép nhập dữ liệu ở dạng utf16
	_setmode(_fileno(stdin), _O_U16TEXT);
	// Cho phép xuất dữ liệu ở dạng utf16
    _setmode(_fileno(stdout), _O_U16TEXT);
	#else
	#endif
	int mode;
	SelectMode(mode);
	switch (mode)
	{
	case 1:
		cryption_XTS_Mode();
		break;
	case 2:
		cryption_CBC_Mode();
		break;
	case 3:
		cryption_CCM_Mode();
		break;
	case 4:
		cryption_CFB_Mode();
		break;
	case 5:
		cryption_CTR_Mode();
		break;
	case 6:
		cryption_EAX_Mode();
		break;
	case 7:
		cryption_ECB_Mode();
		break;
	case 8:
		cryption_GCM_Mode();
		break;
	case 9:
		cryption_OFB_Mode();
		break;
	default:
		break;
	}
	// system("pause");
	return 0;
}