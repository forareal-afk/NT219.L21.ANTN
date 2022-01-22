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
#include "include/cryptopp/des.h"
// using CryptoPP::DES;
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
		wcout<<L"\tVui lòng kiểm tra lại! Độ dài DES key là "<< KEYLENGTH<<" byte!!!" <<endl;
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
	wcout<<L"\t[1] OFB MODE"<<endl;
	wcout<<L"\t[2] CBC MODE"<<endl;
	wcout<<L"\t[3] CFB MODE"<<endl;
	wcout<<L"\t[4] CTR MODE"<<endl;
	wcout<<L"\t[5] ECB MODE"<<endl;
	do
	{
		wcout<<L"\tLựa chọn (1-5): ";
		wcin>>mode;
		wcin.ignore(1);
		mode = (int) mode;
	} while (mode>5 || mode<1);
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
[2]CBC_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_CBC_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc CBC_CTS_Mode< DES >::Encryption
	CBC_Mode< DES >::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key,KEYLENGTH,iv,IVLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho CBC_CTS_Mode< DES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
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
	// Khai báo biến e thuộc CBC_Mode< DES >::Encryption
	CBC_Mode< DES >::Decryption d;
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
	int IVLENGTH = DES::BLOCKSIZE;
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
	wcout<<L"\t<+>Độ dài cho DES KEY là 8 bytes.\n";
	KEYLENGTH = 8;


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
[3]CFB_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_CFB_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc CFB_Mode< DES >::Encryption
	CFB_Mode< DES >::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho CFB_Mode< DES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
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
	// Khai báo biến d thuộc CFB_Mode< DES >::Decryption
	CFB_Mode< DES >::Decryption d;
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
	int IVLENGTH = DES::BLOCKSIZE;
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
	wcout<<L"\t<+>Độ dài cho DES KEY là 8 bytes.\n";
	KEYLENGTH = 8;

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
[4]CTR_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_CTR_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc CTR_Mode< DES >::Encryption
	CTR_Mode< DES >::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho CTR_Mode< DES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
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
	// Khai báo biến d thuộc CTR_Mode< DES >::Decryption
	CTR_Mode< DES >::Decryption d;
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
	int IVLENGTH = DES::BLOCKSIZE;
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
	wcout<<L"\t<+>Độ dài cho DES KEY là 8 bytes.\n";
	KEYLENGTH = 8;


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
[5]ECB_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_ECB_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH)
{
	// Khai báo biến e thuộc ECB_Mode< DES >::Encryption
	ECB_Mode< DES >::Encryption e;
	// Đặt khóa [key] cho e
	e.SetKey(key, KEYLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho ECB_Mode< DES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
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
	// Khai báo biến d thuộc ECB_Mode< DES >::Decryption
	ECB_Mode< DES >::Decryption d;
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
	wcout<<L"<2> Cài đặt KEY và IV:\n";
	wcout<<L"\t<+>Độ dài cho DES KEY là 8 bytes.\n";
	KEYLENGTH = 8;

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
[1]OFB_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_OFB_Mode(string plain, CryptoPP::byte* key, int KEYLENGTH, CryptoPP::byte* iv, int IVLENGTH)
{
	// Khai báo biến e thuộc OFB_Mode< DES >::Encryption
	OFB_Mode< DES >::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key, KEYLENGTH, iv,IVLENGTH);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho OFB_Mode< DES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
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
	// Khai báo biến d thuộc OFB_Mode< DES >::Decryption
	OFB_Mode< DES >::Decryption d;
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
	int IVLENGTH = DES::BLOCKSIZE;
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
	wcout<<L"\t<+>Độ dài cho DES KEY là 8 bytes.\n";
	KEYLENGTH = 8;


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
		cryption_OFB_Mode();
		break;
	case 2:
		cryption_CBC_Mode();
		break;
	case 3:
		cryption_CFB_Mode();
		break;
	case 4:
		cryption_CTR_Mode();
		break;
	case 5:
		cryption_ECB_Mode();
		break;
	default:
		break;
	}
	// system("pause");
	return 0;
}