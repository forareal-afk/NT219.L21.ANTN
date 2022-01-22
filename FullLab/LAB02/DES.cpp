/*************************************************\
- Nguyễn Ngọc Trưởng . MSSV 19522440
Tài liệu tham khảo: 
+ http://page.math.tu-berlin.de/~kant/teaching/hess/krypto-ws2006/des.htm [1]
+ http://nguyenquanicd.blogspot.com/2017/08/background-thuat-toan-ma-hoa-va-giai-ma.html [2]
\*************************************************/
#include <iostream>
#include <string>
#include <map>
// thêm thư viện hỗ trợ nhập xuất tiếng Việt
// #include <io.h>
#include <fcntl.h>
// thêm thư viện hỗ trợ chuyển string <-> wstring
#include <locale>
#include <codecvt>
// Định nghĩa kiểu dữ liệu binary
using namespace std;
typedef string BINARY;
typedef string HEX;
/***************************************************************************\
            CÁC HÀM CHUYỂN ĐỔI CÁC KIỂU DỮ LIỆU
\***************************************************************************/

// Hàm convert binary thành hex
HEX ConvertBinaryToHex(BINARY binary)
{
    // Xây dựng 1 từ điển với key là 4 bit binary, value là giá trị của mã hexa tương ứng
    map <string,string> hex = 
    {
        {"0000","0"}, {"0001","1"}, {"0010","2"}, {"0011","3"},
        {"0100","4"}, {"0101","5"}, {"0110","6"}, {"0111","7"},
        {"1000","8"}, {"1001","9"}, {"1010","A"}, {"1011","B"},
        {"1100","C"}, {"1101","D"}, {"1110","E"}, {"1111","F"}
    };
    // Khai báo output
    string output="";
    // Chèn thêm kí tự '0' vào trước input nếu độ dài input không chia hết cho 4
    while (binary.length()%4!=0)
        binary = "0" + binary;

    for (int i = 0; i < binary.length(); i += 4)
    {
        // Lấy ra 4 bít tính từ trái sang phải và dùng map để convert thành mã hex
        string k = binary.substr(i,4);
        output += hex[k];
    }
    // Return mã hexa tương ứng với input
    return output;
}
// Hàm convert hex thành binary
BINARY ConvertHexToBinary(HEX hex)
{
    // Tạo từ điển với key là kí tự trong hệ số hexa, value tương ứng với biểu điễn ở dạng nhị phân của kí tự đó
    map <char,string> binary = 
    {
        {'0',"0000"},
        {'1',"0001"},
        {'2',"0010"},
        {'3',"0011"},
        {'4',"0100"},
        {'5',"0101"},
        {'6',"0110"},
        {'7',"0111"},
        {'8',"1000"},
        {'9',"1001"},
        {'A',"1010"},
        {'B',"1011"},
        {'C',"1100"},
        {'D',"1101"},
        {'E',"1110"},
        {'F',"1111"}
    };
    // khai báo output
    string output = "";
    // sử dụng dict vừa tạo, duyệt từ trái sang phải để tạo ra mã binary tương ứng
    for (int i = 0; i< hex.length();i++)
        output += binary[hex[i]];
    // return kết quả
    return output;
}
// Hàm chuyển từ int [-128->127] sang kiểu binary vì kiểu char có giá trị [-128->127]
string ConvertIntToBinary(int n)
{
    // gán cờ kiểm tra số âm
    bool flag = false;
    // khi n là số âm
    if (n<0) 
    {   
        flag = true;
        n += 128;
    }
    string s="";
    // chuyển số về binary
    while (n>0)
    {
        if (n%2 == 1) s = "1" + s;
        else s = "0" + s;
        n /= 2;
    }
    // đồng bộ 1 char dùng 8 bit
    while (s.length()<8)
        s = "0" + s;
    // dùng bit dầu để nhận biết số âm
    if (flag) s[0] = '1';
    return s;
}
// Hàm chuyển từ binary int [-128->127] 
int ConvertBinaryToInt(BINARY s)
{
    int n= 0;
    if (s[0]=='1') n-=128;
    if (s[1]=='1') n+=64;
    if (s[2]=='1') n+=32;
    if (s[3]=='1') n+=16;
    if (s[4]=='1') n+=8;
    if (s[5]=='1') n+=4;
    if (s[6]=='1') n+=2;
    if (s[7]=='1') n+=1;
    return n;
}
// Hàm chuyển chuỗi 8 kí tự char sang 64 bit binary
BINARY ConvertStringToBinary(string plain)
{
    // plain có 8 kí tự char, ép sang kiểu int khi đó mỗi kí tự có giá trị từ 0-255, khi đó convert từ char sang BINARY
    BINARY block="";
    for (int i=0; i<plain.length();i++)
    {
        // ép kí tự thành int
        int k = (int) plain[i];
        // chuyển sang dạng binary 8 bit, cộng dồn vào block
        block += ConvertIntToBinary(k);
    }
    // return block 64 bit 
    return block;
}
// Hàm chuyển block 64 bit về chuỗi 8 kí tự char
string ConvertBinaryToString(BINARY block)
{
    BINARY s="";
    for (int i=0; i<block.length();i+=8)
    {
        // lấy khối 8bit gán vào m
        BINARY m = block.substr(i,8);
        // chuyển binary(m) thành int lưu vào k
        int k = ConvertBinaryToInt(m);
        // chuyển kiểu k về char
        char c = (char) k;
        // cộng dồn vào s nếu c ko phải là null
        if (k!=0) s += c;
    }
    return s;
}
// Hàm chuyển từ block(64bit) sang hex
HEX ConvertBlock64bitToHex(BINARY text)
{
    HEX s="";
    for (int i = 0; i<text.length();i+=4)
    {
        // lấy m = 4bit và chuyển sang hex, làm đến khi hết
        BINARY m = text.substr(i,4);
        s += ConvertBinaryToHex(m);
    }
    return s;
}
// Hàm chuyển từ hex sang block(64bit)
BINARY ConvertHexToBlock64bit(HEX text)
{
    BINARY s="";
    // lấy kí tự và chuyển về dạng binary
    for (int i = 0; i<text.length();i++)
    {
        HEX m= "";
        m +=text[i];
        s += ConvertHexToBinary(m);
    }
    return s;
}

/***************************************************************************\
            CÁC HÀM HỖ TRỢ CHUYỂN ĐỔI CHUỖI HỖ TRỢ TIẾNG VIỆT
\***************************************************************************/

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

/***************************************************************************\
            CÁC HÀM HỖ TRỢ MÃ HÓA - GIẢI MÃ DES
\***************************************************************************/

// Hàm hoán vị, dựa trên thông tin mảng arr cho trước, arr có giá trị từ 1->k.length()
BINARY Permute(BINARY k, int* arr, int n)
{
    // Khai báo 1 chuỗi BINARY
    BINARY per = "";
    // Output tạo một chuỗi mới, có giá trị tại chỉ số i là giá trị của k tại chỉ số arr[i] - 1
    // chuỗi mới này sẽ có độ dài bằng với n
    for (int i = 0; i < n; i++) 
        per += k[arr[i] - 1];
    // return kết quả
    return per;
}
// Xây dựng hàm dịch trái với số lần dịch định trước
BINARY ShiftLeft(BINARY k, int shifts)
{
    // Dịch trái shifts lần
    for (int i = 0; i < shifts; i++) 
        // kết quả dịch trái của k[0..n] = k[1..n]+k[0]
        k = k.substr(1,k.length()-1) + k[0];
    // return kết quả
    return k;
}
// Hàm xor 2 số binary
BINARY Xor(BINARY a, BINARY b)
{
    string ans = "";
    // xor 2 bit của a và b từ đầu mảng đến cuối mảng và lưu vào ans
    for (int i = 0; i < a.size(); i++) 
        if (a[i] == b[i]) ans += "0";
        else ans += "1";
    return ans;
}
// Tạo ra 16 subkey, mỗi key có độ dài 48 bit
BINARY* CreateSubkey(const BINARY &key)
{
    // Bảng hoán vị PC1, dùng để lấy 56 bit từ 64 bit
    // Bảng này lấy tài liệu tham khảo [1]
    // Bảng có giá trị từ [1->64] do vậy khi tính toán cần chuyển về [0->63]
    int PC_1[56] = {57, 49, 41, 33, 25, 17, 9,
                    1, 58, 50, 42, 34, 26, 18,
                    10, 2, 59, 51, 43, 35, 27,
                    19, 11, 3, 60, 52, 44, 36,
                    63, 55, 47, 39, 31, 23, 15,
                    7, 62, 54, 46, 38, 30, 22,
                    14, 6, 61, 53, 45, 37, 29,
                    21, 13, 5, 28, 20, 12, 4 };
    // Hoán vị key theo PC_1 và lưu vào key56
    BINARY key56 = Permute(key, PC_1, 56);
    
    // Bảng thể hiện số bit dịch trái của khóa Cn,Dn để được Cn+1 và Dn+1 [1]
    int shift_table[16] = { 1, 1, 2, 2,
                            2, 2, 2, 2,
                            1, 2, 2, 2,
                            2, 2, 2, 1 };
    // Chia khóa ra làm 2 nữa, mỗi nửa sẽ có 28 bit
    string C[16]; //nửa trái
    string D[16]; //nửa phải
    // C, D là 2 nửa của key bầu,
    // Ta có C[i] = ShiftLeft(C[i-1], số lần)... Số lần dịch được quy ước trong bản shift_table. Tương tự đối với D cũng vậy
    C[0] = ShiftLeft(key56.substr(0,28),shift_table[0]); 
    D[0] = ShiftLeft(key56.substr(28,28),shift_table[0]);
    // Thuật toán trình bày ở tài liệu tham khảo 1, 2 bắt đầu từ 1-16... code này trình bày 0-15
    for (int i=1; i<16;i++)
    {
        C[i] = ShiftLeft(C[i-1],shift_table[i]);
        D[i] = ShiftLeft(D[i-1],shift_table[i]);
    }
    // Tạo con trỏ trỏ đến địa chỉ lưu giá trị của 16 subkey phải tìm
    BINARY *subkey = new BINARY[16];
    // Bảng hoán vị PC_2, để hoán vị C[i]D[i] thành subkey[i] với số bit là 48 
    // Giá trị trong bảng được quy ước sẵn, đc trình bày trong tài liệu tham khảo 1
    int PC_2[48] = {14, 17, 11, 24, 1, 5,
                    3, 28, 15, 6, 21, 10,
                    23, 19, 12, 4, 26, 8,
                    16, 7, 27, 20, 13, 2,
                    41, 52, 31, 37, 47, 55,
                    30, 40, 51, 45, 33, 48,
                    44, 49, 39, 56, 34, 53,
                    46, 42, 50, 36, 29, 32 };
    // tính giá trị của subkey[i] = hoán vị theo bảng PC_2
    for (int i=0; i<16;i++)
    {
        subkey[i] = C[i] + D[i];
        subkey[i] = Permute(subkey[i],PC_2,48);
    }
    // return con trỏ, trỏ đến vùng địa chỉ chứa 16 subkey
    return subkey;
}
// Hàm hoán vị ban đầu cho plaintext
BINARY InitialPermutation(const BINARY &plaintext)
{
    // Bảng hoán vị ban đầu cho plaintext
    int IP_table[64] = {58, 50, 42, 34, 26, 18, 10, 2,
                        60, 52, 44, 36, 28, 20, 12, 4,
                        62, 54, 46, 38, 30, 22, 14, 6,
                        64, 56, 48, 40, 32, 24, 16, 8,
                        57, 49, 41, 33, 25, 17, 9, 1,
                        59, 51, 43, 35, 27, 19, 11, 3,
                        61, 53, 45, 37, 29, 21, 13, 5,
                        63, 55, 47, 39, 31, 23, 15, 7 };
    // trả về chuỗi đã được hoán vị dựa vào bảng IP_table
    return Permute(plaintext,IP_table,64);
}
// Hàm chuyển đổi từ (R,L xor key) 48bit về 32bit
BINARY SelectionFunction(BINARY data)
{
    // bảng thể hiện giá trị của hàm chuyển đổi chuyên biệt từ S[0..7]
    int S[8][4][16] = { { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
                          0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
                          4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
                          15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 },
                        { 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
                          3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
                          0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
                          13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 },
                        { 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
                          13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
                          13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
                          1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 },
                        { 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
                          13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
                          10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
                          3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 },
                        { 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
                          14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
                          4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
                          11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 },
                        { 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
                          10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
                          9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
                          4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 },
                        { 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
                          13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
                          1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
                          6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 },
                        { 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
                          1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
                          7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
                          2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 } };
    // convert 2 bit binary sang int
    map <string,int> row = 
    {
        {"00",0},
        {"01",1},
        {"10",2},
        {"11",3},
    };
    // convert int[0..15] thành 4 bit binary
    map <int,string> value = 
    {
        {0,"0000"}, {1,"0001"}, {2,"0010"}, {3,"0011"},
        {4,"0100"}, {5,"0101"}, {6,"0110"}, {7,"0111"},
        {8,"1000"}, {9,"1001"}, {10,"1010"}, {11,"1011"},
        {12,"1100"}, {13,"1101"}, {14,"1110"}, {15,"1111"}
    };
    // convert 4 bit binary thành int
    map <string,int> col = 
    {
        {"0000",0}, {"0001",1}, {"0010",2}, {"0011",3},
        {"0100",4}, {"0101",5}, {"0110",6}, {"0111",7},
        {"1000",8}, {"1001",9}, {"1010",10}, {"1011",11},
        {"1100",12}, {"1101",13}, {"1110",14}, {"1111",15}
    };
    // kết quả về về 32 bit, cần chạy 8 lần tương ứng với sử dụng S[i] để tra
    // dữ liệu vào là 48 bit chia làm 8 block, mỗi block 6bit,.. 2bit đầu và cuối để tra hàng, và 4 bít còn lại để tra cột
    BINARY output ="";
    for (int i=0;i<data.length();i+=6)
    {
        BINARY m = data.substr(i,6);
        BINARY row_ = "";
        // lấy giá trị của bit đầu và bit cuối làm hàng
        row_ = row_ + m[0] + m[5]; 
        // lấy giá trị của bit 1,2,3,4 làm cột
        BINARY col_ = m.substr(1,4);
        // Tra bảng S với i/6 là bảng S[0..7] cần được tra, và chuyển đổi col và row thành int để tra 
        output += value[S[i/6][row[row_]][col[col_]]];
    }
    // return kết quả
    return output;
}
// Hàm Hàm mã hóa f(R,K)
BINARY FunctionF(BINARY R, BINARY key)
{
    //bảng E, để hoán đổi vị trí của R được cắt từ plaintext...
    int E[48] = {32, 1, 2, 3, 4, 5, 4, 5,
                6, 7, 8, 9, 8, 9, 10, 11,
                12, 13, 12, 13, 14, 15, 16, 17,
                16, 17, 18, 19, 20, 21, 20, 21,
                22, 23, 24, 25, 24, 25, 26, 27,
                28, 29, 28, 29, 30, 31, 32, 1 };
    // Hoán vị và chuyển đổi từ R_32bit thành R_48bit dựa vào bảng E
    BINARY R_48bit = Permute(R,E,48);
    // Xor R_48bit với key
    BINARY Xor48bit = Xor(R_48bit,key);
    // Chuyển dổi kết quả xor được từ 48 về 32 bít thông qua hàm chuyển đổi
    BINARY f_32bit = SelectionFunction(Xor48bit);
    // Bảng hoán vị của f sau khi qua hàm chuyển đổi
    int per[32] = { 16, 7, 20, 21,
                    29, 12, 28, 17,
                    1, 15, 23, 26,
                    5, 18, 31, 10,
                    2, 8, 24, 14,
                    32, 27, 3, 9,
                    19, 13, 30, 6,
                    22, 11, 4, 25 };
    // Hoán vị f_32bit dựa vào bảng per
    f_32bit = Permute(f_32bit,per,32);
    // return kết quả
    return f_32bit;
}
// Hàm mã hóa plaintext->ciphertext
string EncryptionDES(const BINARY &plaintext, const BINARY &key)
{
    // Khai báo cipher
    BINARY cipher="";
    // tạo ra 16 subkey
    BINARY *SUBKEY = CreateSubkey(key);
    // Chuyển đổi hoán vị plaintext ban đầu
    BINARY plain_IP = InitialPermutation(plaintext);
    // Khai báo các mảng L,R để lưu giá trị sau mỗi vòng 
    BINARY L[17], R[17];
    // Chia plain_IP thành 2 nữa left, right,... 
    // khởi tạo L, R ban đầu làm cơ sở sử dụng đệ quy tính L,R ở các vòng tiếp theo
    L[0] = plain_IP.substr(0,32);
    R[0] = plain_IP.substr(32,32);
    // Lặp lại 16 lần tính L,R thông qua công thức Ln+1 = Rn; Rn+1 = Ln XOR f(Rn,Kn+1)
    for (int i = 1; i<=16;i++)
    {
        L[i] = R[i-1];
        R[i] = Xor(L[i-1],FunctionF(R[i-1],SUBKEY[i-1]));
        // vì L,R từ 1->16, còn subkey từ 0->15
    }
    // lấy giá trị của ciphertext = R[16] + L[16]
    cipher = R[16] + L[16];
    // bảng hoán vị cuối của R+L ở round 16, thành ciphertext
    int FinalPerm[64] = { 40, 8, 48, 16, 56, 24, 64, 32,
                           39, 7, 47, 15, 55, 23, 63, 31,
                           38, 6, 46, 14, 54, 22, 62, 30,
                           37, 5, 45, 13, 53, 21, 61, 29,
                           36, 4, 44, 12, 52, 20, 60, 28,
                           35, 3, 43, 11, 51, 19, 59, 27,
                           34, 2, 42, 10, 50, 18, 58, 26,
                           33, 1, 41, 9, 49, 17, 57, 25 };
    // Hoán vị RL (round 16) theo FinalPerm để tạo ciphertext
    cipher = Permute(cipher,FinalPerm,64);
    // return ciphertext
    return cipher;
}
// Hàm giải mã ciphertext->plaintext
string DecryptionDES(const BINARY &cipher, const BINARY &key)
{
    // Giống hàm mã hóa, nhưng subkey được dùng ngược lại
    // Khai báo plaintext
    BINARY plaintext="";
    // tạo ra 16 subkey
    BINARY *SUBKEY = CreateSubkey(key);
    // Chuyển đổi hoán vị ciphertext ban đầu
    BINARY cipher_IP = InitialPermutation(cipher);
    // Khai báo các mảng L,R để lưu giá trị sau mỗi vòng 
    BINARY L[17], R[17];
    // Chia cipher_IP thành 2 nữa left, right,... 
    // khởi tạo L, R ban đầu làm cơ sở sử dụng đệ quy tính L,R ở các vòng tiếp theo
    L[0] = cipher_IP.substr(0,32);
    R[0] = cipher_IP.substr(32,32);
    // Lặp lại 16 lần tính L,R thông qua công thức Ln+1 = Rn; Rn+1 = Ln XOR f(Rn,Kn+1)
    for (int i = 1; i<=16;i++)
    {
        L[i] = R[i-1];
        R[i] = Xor(L[i-1],FunctionF(R[i-1],SUBKEY[16-i]));
        // vì L,R từ 1->16, còn subkey từ 0->15
    }
    // lấy giá trị của plaintext = R[16] + L[16]
    plaintext = R[16] + L[16];
    // bảng hoán vị cuối của R+L ở round 16, thành plaintext
    int FinalPerm[64] = { 40, 8, 48, 16, 56, 24, 64, 32,
                           39, 7, 47, 15, 55, 23, 63, 31,
                           38, 6, 46, 14, 54, 22, 62, 30,
                           37, 5, 45, 13, 53, 21, 61, 29,
                           36, 4, 44, 12, 52, 20, 60, 28,
                           35, 3, 43, 11, 51, 19, 59, 27,
                           34, 2, 42, 10, 50, 18, 58, 26,
                           33, 1, 41, 9, 49, 17, 57, 25 };
    // Hoán vị RL (round 16) theo FinalPerm để tạo plaintext
    plaintext = Permute(plaintext,FinalPerm,64);
    // return plaintext
    return plaintext;
}

/***************************************************************************\
            CÁC HÀM MÃ HÓA - GIẢI MÃ DES TRÊN MODE ECB
\***************************************************************************/

// Hàm mã hóa plaintext->ciphertext DES mode ECB
string encrytion_mode_ECB(string plaintext, BINARY key)
{
    /**********************************\
     1 kí tự string -> 8 bit
     1 khối (64 bit) - 8 kí tự
    Nếu block của chia thiếu thì sao?
    -> thêm vào bit 0 vào phía bên phải đến khi đủ 
        -> lúc giải mã, convert về string thì loại bỏ kí tự null
    \**********************************/
    
    string cipher;
    int i = 0;
    // chia plain text thành các block có độ dài 8 kí tự
    // 1. Xử lý trường hợp đủ 8 kí tự tức là 64 bit như đã phân tích ở phía trên
    for (; i<plaintext.length()/8;i++)
    {
        // mỗi vòng lặp lấy ra block 8 kí tự, từ trái sang phải lưu vào plain
        string plain = plaintext.substr(i*8,8);
        // chuyển chuỗi kí tự thành chuỗi 64 bit
        plain = ConvertStringToBinary(plain);
        // mã hóa DES với EncryptionDES(plain,key), kết quả trả về là khối 64 bit
        // cần chuyển khối sang dạng hexa, để trực quan, 
        // kết quả cộng dồn vào cipher sau mỗi lần mã hóa 1 khối dữ liệu
        cipher += ConvertBlock64bitToHex(EncryptionDES(plain,key));
    }
    // 2. Xử lý trường hợp block cuối cùng không đủ 8 kí tự (ko đủ 64 bit)
    if (plaintext.length()-i*8 > 0)
    {
        // Lấy các kí tự còn thừa ở block cuối cùng nếu bị lẻ
        string plain = plaintext.substr(i*8,plaintext.length()-i*8);
        // chuyển chuỗi kí tự thành chuỗi bit...
        plain = ConvertStringToBinary(plain);
        // vì đang xét trường hợp khối cuối cùng không đủ 64 bit do đó cần chèn thêm các 
        // bit 0 vào cuối... số bit '0' chèn vào sẽ là bội của 8... 
        //do vậy kí convert sang char thì nó == null, dễ xóa ra khỏi plaintext
        while (plain.length()<64)
            plain+="0";
        // kết quả mã hóa khối cuối cùng cũng cộng dồn vào cipher
        cipher += ConvertBlock64bitToHex(EncryptionDES(plain,key));
    }
    // kết quả trả về là chuỗi các số hệ hexa
    return cipher;
}
// Hàm giải mã ciphertext->plaintext DES mode ECB
string decrytion_mode_ECB(string ciphertext, BINARY key)
{
    /**********************************\
    Input: 1 ciphertext hệ hexa. luôn chia hết cho 16.
            vì output mã hóa DES ở trên là 64 bit mỗi khối, mà 1 kí tự hex biểu diễn bởi 4 bit 
    Output: plaintext kiểu dữ liệu string
    \**********************************/
    string plain="";
    // Chia mỗi block gồm 16 kí tự hex <-> 64 bit
    for (int i = 0; i<ciphertext.length()/16;i++)
    {
        // lấy dạng hexa của block thứ i
        string cipher = ciphertext.substr(i*16,16);
        // chuyển cipher sang dạng binary
        cipher = ConvertHexToBlock64bit(cipher);
        // giải mã khối dữ liệu thông qua hàm DecryptionDES vừa tạo
        string binary_plain = DecryptionDES(cipher,key);
        // Chuyển sang dạng string khối dữ plaintext vừa tìm, và cộng dồn vào biến plain
        plain += ConvertBinaryToString(binary_plain);
    }
    return plain;
}

// Hàm mã hóa plaintext->ciphertext DES mode ECB
HEX encrytion_mode_CBC(string plaintext, BINARY key, BINARY iv)
{
    /**********************************\
     1 kí tự string -> 8 bit
     1 khối (64 bit) - 8 kí tự
    Nếu block của chia thiếu thì sao?
    -> thêm vào bit 0 vào phía bên phải đến khi đủ 
        -> lúc giải mã, convert về string thì loại bỏ kí tự null
    \**********************************/
    
    HEX cipher;
    int i = 0;
    // chia plain text thành các block có độ dài 8 kí tự
    // 1. Xử lý trường hợp đủ 8 kí tự tức là 64 bit như đã phân tích ở phía trên
    for (; i<plaintext.length()/8;i++)
    {
        // mỗi vòng lặp lấy ra block 8 kí tự, từ trái sang phải lưu vào plain
        string SubPlain = plaintext.substr(i*8,8);
        // chuyển chuỗi kí tự thành chuỗi 64 bit
        BINARY Block = ConvertStringToBinary(SubPlain);
        // mã hóa DES với EncryptionDES(plain,key), kết quả trả về là khối 64 bit
        // cần chuyển khối sang dạng hexa, để trực quan, 
        // kết quả cộng dồn vào cipher sau mỗi lần mã hóa 1 khối dữ liệu
        Block = Xor(Block, iv);
        iv = EncryptionDES(Block,key);
        cipher += ConvertBlock64bitToHex(iv);
    }
    // 2. Xử lý trường hợp block cuối cùng không đủ 8 kí tự (ko đủ 64 bit)
    if (plaintext.length()-i*8 > 0)
    {
        // Lấy các kí tự còn thừa ở block cuối cùng nếu bị lẻ
        string SubPlain = plaintext.substr(i*8,plaintext.length()-i*8);
        // chuyển chuỗi kí tự thành chuỗi bit...
        BINARY Block = ConvertStringToBinary(SubPlain);
        // vì đang xét trường hợp khối cuối cùng không đủ 64 bit do đó cần chèn thêm các 
        // bit 0 vào cuối... số bit '0' chèn vào sẽ là bội của 8... 
        //do vậy kí convert sang char thì nó == null, dễ xóa ra khỏi plaintext
        while (Block.length()<64)
            Block+="0";
        // kết quả mã hóa khối cuối cùng cũng cộng dồn vào cipher
        Block = Xor(Block, iv);
        iv = EncryptionDES(Block,key);
        cipher += ConvertBlock64bitToHex(iv);
    }
    // kết quả trả về là chuỗi các số hệ hexa
    return cipher;
}
// Hàm giải mã ciphertext->plaintext DES mode ECB
string decrytion_mode_CBC(HEX ciphertext, BINARY key, BINARY iv)
{
    /**********************************\
    Input: 1 ciphertext hệ hexa. luôn chia hết cho 16.
            vì output mã hóa DES ở trên là 64 bit mỗi khối, mà 1 kí tự hex biểu diễn bởi 4 bit 
    Output: plaintext kiểu dữ liệu string
    \**********************************/
    string plain="";
    // Chia mỗi block gồm 16 kí tự hex <-> 64 bit
    for (int i = 0; i<ciphertext.length()/16;i++)
    {
        // lấy dạng hexa của block thứ i
        HEX SubCipher = ciphertext.substr(i*16,16);
        // chuyển cipher sang dạng binarye
        BINARY Block = ConvertHexToBlock64bit(SubCipher);
        // giải mã khối dữ liệu thông qua hàm DecryptionDES vừa tạo
        BINARY DecryptionBlock = DecryptionDES(Block,key);
        DecryptionBlock = Xor(DecryptionBlock, iv);
        iv = Block;
        // Chuyển sang dạng string khối dữ plaintext vừa tìm, và cộng dồn vào biến plain
        plain += ConvertBinaryToString(DecryptionBlock);
    }
    
    return plain;
}

// Phương thức nhập plaintext từ màn hình
void InputPlainText(string& plain)
{
    // Nhập vào là 1 wstring
	wstring plaintext;
	fflush(stdin);
	getline(wcin,plaintext);
    // Output là 1 string để làm input cho hàm mã hóa
	plain = WStringToString(plaintext);
}

// Hàm nhập key từ màn hình
void InputKeyFormScreen(BINARY& KEY)
{   
    // Nhập vào wsting -> chuyển sang string -> chuyển sang Binary 
    // Key đúng là 1 binary độ dài 128 bit
    wstring wstrText;
	fflush(stdin);
    // Nhập vào 1 wstring
	getline(wcin,wstrText);
    // Chuyển wsatring sang string
    string strText = WStringToString(wstrText);
    // Convert sang Binary
    KEY = ConvertStringToBinary(strText);
    // Check độ dài của KEY
    if (KEY.length() == 64) return;
    wcout<<L"Nhập lại KEY (8 bytes): ";
    InputKeyFormScreen(KEY);
}

// Hàm nhập key từ màn hình
void InputIVFormScreen(BINARY& IV)
{
    // Giống hàm nhập key từ màn hình
    wstring wstrText;
	fflush(stdin);
	getline(wcin,wstrText);

    string strText = WStringToString(wstrText);
    IV = ConvertStringToBinary(strText);
    if (IV.length() == 64) return;
    wcout<<L"Nhập lại IV (8 bytes): ";
    InputIVFormScreen(IV);
}

int main()
{
	#ifdef __linux__
	setlocale(LC_ALL,"");
	#elif _WIN32
	// Cho phép nhập dữ liệu ở dạng utf16
	_setmode(_fileno(stdin), _O_U16TEXT);
	// Cho phép xuất dữ liệu ở dạng utf16
    _setmode(_fileno(stdout), _O_U16TEXT);
	#else
	#endif
    /************************************\
        Tạo khóa và plaintext
    \************************************/
    wcout << L"*** Chương trình DES mode ECB - không sử dụng thư viện ***" <<endl;

    // Nhập plainText
    string plain;
    wcout<<L">> Nhập plaintext: ";
    InputPlainText(plain);

    // Nhập KEY
    BINARY K,IV;
    wcout<<L">> Nhập KEY (độ dài KEY mặc định 8 bytes) :";
    InputKeyFormScreen(K);
    wcout<<L">> Nhập IV (độ dài IV mặc định 8 bytes) :";
    InputIVFormScreen(IV);
    /************************************\
                     MÃ HÓA
    \************************************/
    string cipher = encrytion_mode_CBC(plain,K,IV);
    wcout << L">> Ciphertext: " << StringToWString(cipher) << endl;
    /************************************\
                GIẢI MÃ
    \************************************/
    string encode = decrytion_mode_CBC(cipher,K,IV);
	// In giá trị của thời gian giải mã ra màn hình
    wcout << L">> Recovered plaintext: " << StringToWString(encode)<<endl;
    // system("pause");
    return 0;
}