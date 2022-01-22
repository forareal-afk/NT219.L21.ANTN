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
// thêm thư tính random
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
// thêm thư viện hỗ trợ đo thời gian
#include <chrono>
// Định nghĩa kiểu dữ liệu binary
using namespace std;
using namespace std::chrono;
typedef string BINARY;//chuỗi binary 
typedef string BYTE; //chuỗi binary 8bit
typedef string HEX; //Chuỗi binary 4bit
/***************************************************************************\
            CÁC HÀM CHUYỂN ĐỔI CÁC KIỂU DỮ LIỆU
\***************************************************************************/

// Hàm convert binary thành hex
HEX Convert4bitToHex(BINARY input)
{
    // Xây dựng 1 từ điển với key là 4 bit binary, value là giá trị của mã hexa tương ứng
    map <BINARY,HEX> hex = 
    {
        {"0000","0"}, {"0001","1"}, {"0010","2"}, {"0011","3"},
        {"0100","4"}, {"0101","5"}, {"0110","6"}, {"0111","7"},
        {"1000","8"}, {"1001","9"}, {"1010","A"}, {"1011","B"},
        {"1100","C"}, {"1101","D"}, {"1110","E"}, {"1111","F"}
    };
    // Khai báo output
    HEX output="";
    // Chèn thêm kí tự '0' vào trước input nếu độ dài input không chia hết cho 4
    while (input.length()%4!=0)
        input = "0" + input;

    for (int i = 0; i < input.length(); i += 4)
    {
        // Lấy ra 4 bít tính từ trái sang phải và dùng map để convert thành mã hex
        HEX k = input.substr(i,4);
        output += hex[k];
    }
    // Return mã hexa tương ứng với input
    return output;
}
// Done

// Hàm convert hex thành binary
BINARY ConvertHexTo4Bit(HEX input)
{
    // Tạo từ điển với key là kí tự trong hệ số hexa, value tương ứng với biểu điễn ở dạng nhị phân của kí tự đó
    map <char,BINARY> binary = 
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
    BINARY output = "";
    // sử dụng dict vừa tạo, duyệt từ trái sang phải để tạo ra mã binary tương ứng
    for (int i = 0; i< input.length();i++)
        output += binary[input[i]];
    // return kết quả
    return output;
}
// Done

// Hàm chuyển từ int [-128->127] sang kiểu binary vì kiểu char có giá trị [-128->127]
BYTE ConvertIntToByte(int input)
{
    // gán cờ kiểm tra số âm
    bool flag = false;
    // khi n là số âm
    if (input<0) 
    {   
        flag = true;
        input += 128;
    }
    BYTE output="";
    // chuyển số về binary
    while (input>0)
    {
        if (input%2 == 1) output = "1" + output;
        else output = "0" + output;
        input /= 2;
    }
    // đồng bộ 1 char dùng 8 bit
    while (output.length()<8)
        output = "0" + output;
    // dùng bit dầu để nhận biết số âm
    if (flag) output[0] = '1';
    return output;
}
// Done

// Hàm chuyển từ byte int [-128->127] 
int ConvertByteToInt(BYTE input)
{
    int output= 0;
    if (input[0]=='1') output-=128;
    if (input[1]=='1') output+=64;
    if (input[2]=='1') output+=32;
    if (input[3]=='1') output+=16;
    if (input[4]=='1') output+=8;
    if (input[5]=='1') output+=4;
    if (input[6]=='1') output+=2;
    if (input[7]=='1') output+=1;
    return output;
}
// Done

// Hàm chuyển từ 4bit binary thành int [0-15] 
int Convert4bitToInt(BINARY input)
{
    int output= 0;
    if (input[0]=='1') output+=8;
    if (input[1]=='1') output+=4;
    if (input[2]=='1') output+=2;
    if (input[3]=='1') output+=1;
    return output;
}
// Done

// Hàm chuyển chuỗi string sang dạng binary
BINARY ConvertStringToBinary(string input)
{
    // input có 8 kí tự char, ép sang kiểu int khi đó mỗi kí tự có giá trị từ 0-255, khi đó convert từ char sang BINARY
    BINARY output="";
    for (int i=0; i<input.length();i++)
    {
        // ép kí tự thành int
        int tmp = (int) input[i];
        // chuyển sang dạng 1byte 8 bit, cộng dồn vào block
        output += ConvertIntToByte(tmp);
    }
    // return block 
    return output;
}
// Done

// Hàm chuyển input độ dài chia hết cho 8, về dạng string
string ConvertBinaryToString(BINARY input)
{
    string output="";
    for (int i=0; i<input.length();i+=8)
    {
        // lấy khối 8bit gán vào m
        BYTE m = input.substr(i,8);
        // chuyển binary(m) thành int lưu vào k
        int k = ConvertByteToInt(m);
        // chuyển kiểu k về char
        char c = (char) k;
        // cộng dồn vào output nếu c ko phải là null
        if (k!=0)
            output += c;
    }
    return output;
}
// Done

// Hàm chuyển từ chuỗi binary thành chuỗi hex
HEX ConvertBinaryToHex(BINARY input)
{
    HEX output="";
    for (int i = 0; i<input.length();i+=4)
    {
        // lấy m = 4bit và chuyển sang hex, làm đến khi hết
        BINARY m = input.substr(i,4);
        output += Convert4bitToHex(m);
    }
    return output;
}
// Done

// Hàm chuyển từ hex thành hệ cơ số 2
BINARY ConvertHexToBinary(string input)
{
    BINARY output="";
    // lấy kí tự và chuyển về dạng binary
    for (int i = 0; i<input.length();i++)
    {
        string m= "";
        m +=input[i];
        // chuyển kí tự hiện tại thành dạng binary và cộng dồn vào output
        output += ConvertHexTo4Bit(m);
    }
    return output;
}
// Done

/***************************************************************************\
            CÁC HÀM HỖ TRỢ CHUYỂN ĐỔI CHUỖI HỖ TRỢ TIẾNG VIỆT
\***************************************************************************/

// Xây dựng hàm chuyển đổi từ kiểu string sang wstring để hỗ trợ tiếng Việt
wstring ConvertStringToWString(const string& str)
{
    // Khai báo đối tượng converterX thuộc lớp <wstring_convert> convert từ kiểu utf8
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;
    // Trả về kết quả chuyển đổi được từ một byte string sang wide string
    return converterX.from_bytes(str);
}
// Done

// Xây dựng hàm chuyển wstring sang string
string ConvertWStringToString(const wstring& wstr)
{
    // Khai báo đối tượng converterX thuộc lớp <wstring_convert>
    using convert_typeX = codecvt_utf8<wchar_t>;
    wstring_convert<convert_typeX, wchar_t> converterX;
    // Trả về kết quả chuyển đổi được từ một wide string sang byte string
    return converterX.to_bytes(wstr);
}
// Done

/***************************************************************************\
            CÁC HÀM HỖ TRỢ MÃ HÓA MÃ AES
Tài liệu tham khảo:
Bảng S_BOX: https://en.wikipedia.org/wiki/Rijndael_S-box
Giải thuật mã hóa AES: http://nguyenquanicd.blogspot.com/2019/09/aes-bai-1-ly-thuyet-ve-ma-hoa-aes-128.html
\***************************************************************************/

// Hàm AddRoundKey:  XOR khóa mã với ma trận dữ liệu
BINARY AddRoundKey(BINARY block, BINARY key)
{
    string output = "";
    // xor 2 bit của a và b từ đầu mảng đến cuối mảng và lưu vào ans
    for (int i = 0; i < key.size(); i++)
        // kiểm tra từng bit, nếu giống nhau thì trả về 0, ngược lại trả về 1
        if (key[i] == block[i]) output += "0";
        else output += "1";
    return output;
}
// Done

// Hàm SubByte: Thay thế byte input bỡi 1 giá trị được tra trong bảng
BYTE SubByte (BYTE input)
{
    // Bảng S_Box được quy định trong chuẩn AES
    // Bảng thể hiện ở dạng HEX
    string S_Box[16][16] = {
        {"63", "7C", "77", "7B", "F2", "6B", "6F", "C5", "30", "01", "67", "2B", "FE", "D7", "AB", "76"},
        {"CA", "82", "C9", "7D", "FA", "59", "47", "F0", "AD", "D4", "A2", "AF", "9C", "A4", "72", "C0"},
        {"B7", "FD", "93", "26", "36", "3F", "F7", "CC", "34", "A5", "E5", "F1", "71", "D8", "31", "15"},
        {"04", "C7", "23", "C3", "18", "96", "05", "9A", "07", "12", "80", "E2", "EB", "27", "B2", "75"},
        {"09", "83", "2C", "1A", "1B", "6E", "5A", "A0", "52", "3B", "D6", "B3", "29", "E3", "2F", "84"},
        {"53", "D1", "00", "ED", "20", "FC", "B1", "5B", "6A", "CB", "BE", "39", "4A", "4C", "58", "CF"},
        {"D0", "EF", "AA", "FB", "43", "4D", "33", "85", "45", "F9", "02", "7F", "50", "3C", "9F", "A8"},
        {"51", "A3", "40", "8F", "92", "9D", "38", "F5", "BC", "B6", "DA", "21", "10", "FF", "F3", "D2"},
        {"CD", "0C", "13", "EC", "5F", "97", "44", "17", "C4", "A7", "7E", "3D", "64", "5D", "19", "73"},
        {"60", "81", "4F", "DC", "22", "2A", "90", "88", "46", "EE", "B8", "14", "DE", "5E", "0B", "DB"},
        {"E0", "32", "3A", "0A", "49", "06", "24", "5C", "C2", "D3", "AC", "62", "91", "95", "E4", "79"},
        {"E7", "C8", "37", "6D", "8D", "D5", "4E", "A9", "6C", "56", "F4", "EA", "65", "7A", "AE", "08"},
        {"BA", "78", "25", "2E", "1C", "A6", "B4", "C6", "E8", "DD", "74", "1F", "4B", "BD", "8B", "8A"},
        {"70", "3E", "B5", "66", "48", "03", "F6", "0E", "61", "35", "57", "B9", "86", "C1", "1D", "9E"},
        {"E1", "F8", "98", "11", "69", "D9", "8E", "94", "9B", "1E", "87", "E9", "CE", "55", "28", "DF"},
        {"8C", "A1", "89", "0D", "BF", "E6", "42", "68", "41", "99", "2D", "0F", "B0", "54", "BB", "16"}
    };
    
    // lấy ra vị trí của hàng cần tra cứu trong bảng S-box
    int row = Convert4bitToInt(input.substr(0,4));
    // lấy ra vị trí của cột cần tra cứu trong bảng S-box
    int col = Convert4bitToInt(input.substr(4,4));
    // tra bảng và lấy giá trị ở dạng hex, chuyển về dạng binary[khối 8bit] và cộng dồn vào output
    BYTE output = ConvertHexToBinary(S_Box[row][col]);
    return output;
}
// Done

// * Hàm SubBytes thực hiện thay thế từng byte của ma trận trạng thái bằng một giá trị đã quy định trong chuẩn AES
BINARY SubBytes(BINARY input)
{
    BINARY output = "";
    // Vì block vào là dạng binary có độ dài 8x16 bit, tách ra 16 cụm, mỗi cụm 8 bit và thay thế từng cụm
    for (int i= 0; i< input.length(); i+=8)
    {
        // Lấy cụm 8 bit đang xét
        BYTE tmp = input.substr(i,8);
        // tra bảng và lấy giá trị, chuyển về dạng binary[khối 8bit] và cộng dồn vào output
        output += SubByte(tmp);
    }
    return output;
}
// Done

// Xây dựng hàm dịch trái với số lần dịch định trước <=> Xoay vong
BINARY CircularLeft(BINARY k, int shifts)
{
    // Dịch trái shifts lần
    for (int i = 0; i < shifts; i++) 
        // kết quả dịch trái của k[0..n] = k[1..n]+k[0]
        k = k.substr(1,k.length()-1) + k[0];
    // return kết quả
    return k;
}
// Done

// Xây dựng hàm dịch hàng của block kích thước 4x4 mỗi ô chứa 2 kí tự hex, tức chứa 8 bit
BINARY ShiftRows(BINARY block)
{
    BYTE state[16];
    for (int i = 0; i<16;i++)
        state[i] = block.substr(i*8,8);

    // Xếp các byte trên ma trận 4x4, từ trên xuống dưới, từ trái qua phải
    //      0 | 4 | 8 |12
    //      1 | 5 | 9 |13
    //      2 | 6 |10 |14
    //      3 | 7 |11 |15
    // theo thuật mô tả của thuật toán ShiftRows ta thu được mận
    //      0 | 4 | 8 |12
    //      5 | 9 |13 | 1
    //     10 |14 | 2 | 6
    //     15 | 3 | 7 |11
    BINARY s = "";
    // cột 1
    s += state[0] + state[5] + state[10] + state[15];
    // cột 2
    s += state[4] + state[9] + state[14] + state[3];
    // cột 3
    s += state[8] + state[13] + state[2] + state[7];
    // cột 4
    s += state[12] + state[1] + state[6] + state[11];
    return s;
}
// Done

// Trả về kết quả phép xor 2 chuỗi nhị phân
BINARY _xor(BINARY a, BINARY b)
{
    BINARY ans = "";
    // xor 2 bit của a và b từ đầu mảng đến cuối mảng và lưu vào ans
    for (int i = 0; i < a.size(); i++) 
        // bit giống nhau thì bằng 0, khác nhau thì bằng 1
        if (a[i] == b[i]) ans += "0";
        else ans += "1";
    return ans;
}
// Done

// Trả về 1 byte là kết quả của phép nhân theo quy tắc quy định trong chuẩn AES
BYTE Multiply( int b,BYTE a)
{
    // giải thuật tham khảo từ trang http://nguyenquanicd.blogspot.com/2019/09/aes-bai-1-ly-thuyet-ve-ma-hoa-aes-128.html
    // kiểm thử với test ở https://en.wikipedia.org/wiki/Rijndael_MixColumns thì cho kết quả đúng

    BYTE output = a;
    // Nếu b = 1, thì kết quả phép nhân bằng input a
    if (b==1) return output;
    // Nếu b = 2,3 thì dịch trái
    output = a.substr(1,7)+"0";
    // Nếu bit có trọng số lớn nhấn của a là bằng 1 thì xor với 1b
    if(a[0] == '1') output = _xor(output, "00011011");
    // Nếu b = 3 thì lấy kết quả vừa tìm xor với a
    if (b == 3) output = _xor(output, a);
    return output;
}
// Done

// Hàm nhân 2 ma truần 4x4 hỗ trợ cho giải thuật MixClumn
BYTE** MultiplyMatrix(int A[4][4], BYTE** B)
{
    // Khai báo biến output là 1 con trỏ cấp 2 kiểu BYTE, cấp phát động 4 con trỏ cấp 1
    BYTE** output = new BYTE*[4];
    // chạy lần lược các hàng
    for (int i = 0; i<4; i++)
    {
        // Mỗi con trỏ cấp 1, đc cấp phát động là 4 phần tử byte
        output[i] = new BYTE[4];
        // chạy lần lược ở các cột 
        for(int j=0; j<4;j++)
        {
            // mặt định cho mỗi phần tử là byte 0
            output[i][j] = "00000000";
            // Tính giá trị tại ô [i,j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j] + A[i][3]*B[3][j]
            for (int k = 0; k<4;k++)
            {   
                // tính giá trị A[i][k]*B[k][j] cộng dồn vào output[i][j]
                BYTE tmp = Multiply(A[i][k],B[k][j]);
                output[i][j] = _xor(tmp, output[i][j]);
            }
        }
    }
    return output;
}
// Done

// Hàm Mixcolumns 
BINARY MixColumns(BINARY str)
{
    int  MDSmatrix[4][4] = 
    {   
        {2,3,1,1},
        {1,2,3,1},
        {1,1,2,3},
        {3,1,1,2}
    };
    /*************************\
    1. Convert chuỗi BINARY thành ma trận 4x4 mỗi ô chứa 1 byte
    \*************************/
    // Khai báo con trỏ cấp2
    BYTE** before_block;
    // Khởi tạo ma trận 4 nhân 4
    before_block = new BYTE*[4];
    for (int i=0; i<4; i++)
        before_block[i] = new BYTE[4];
    // Duyệt từng cột
    for (int j=0; j<4; j++)
        // Duyệt từng cột
        for (int i = 0; i<4;i++)
            // Gán giá trị thích hợp cho từng ô 
            before_block[i][j] = str.substr(j*32 + i*8, 8);

    /*************************\
    2. Thực hiện phép biến đổi
    \*************************/
    // Thực hiện thực toán nhân ma trận với quy tắc được quy định trong AES, ma trận kết quả vào after_blocc
    BYTE** after_block = MultiplyMatrix(MDSmatrix,before_block);

    /*************************\
    3. Convert từ ma trận 4x4 mỗi ô 1 byte về chuỗi BINARY và return kết quả
    \*************************/
    BINARY output="";
    // Duyệt từng hàng, từng cột và cộng vào output theo thứ tự
    for (int j=0;j<4;j++)
        for (int i=0;i<4;i++)
            output += after_block[i][j];

    return output;
}
// Done

// Hàm RotWord: thực hiện quay trái từ w[j] một byte
BINARY RotWord(BINARY word)
{
    return CircularLeft(word,8);
}
// Done

// SubWord thực hiện thay thế các phi tuyến từng byte của kết quả RotWord theo bảng S-box
BINARY SubWord(BINARY word)
{
    BINARY output="";
    // tách input thành 4 byte
    for (int i=0;i<word.length();i+=8)
    {
        // thay thế từng bye cộng dồn vào output
        BYTE tmp=word.substr(i,8);
        output += SubByte(tmp);
    }
    return output;
}
// Done

// AddRcon thực hiện XOR kết quả SubWord và giá trị Rcon[j/4] với j là bội số của 4
BINARY AndRcon(BINARY word, int i)
{
    // Giá trị rcon đc quy định trước
    HEX Rcon[10] = { "01000000", "02000000","04000000","08000000","10000000",
                    "20000000", "40000000","80000000","1B000000","36000000"};
    // Lấy giá trị của rcon cần
    BINARY rconValue = ConvertHexToBinary(Rcon[i/4 - 1]);
    // Kết quả ra xor của rcon và word
    return _xor(rconValue, word);
}
//Done

// Hàm trans dùng khi tính các từ ở vị trí j là bội số của 4
BINARY Trans(BINARY word, int i)
{
    // Quay trái một byte
    word = RotWord(word);
    // Thay thế từng byte bỡi 1 byte khác trong bảng S
    word = SubWord(word);
    // Xor subword với Rcon
    return AndRcon(word,i);
}
// Done

// Hàm KeyExpansion
BINARY* KeyExpansion(BINARY key)
{
    BINARY word[44];
    // chia key word thành 4 word 1 word độ dài 32 bit
    for (int i=0; i<key.length();i+=32)
        word[i/32] = key.substr(i,32);
    // tính word thứ i theo công thức dệ quy
    for (int i=4; i<44; i++)
        if (i%4 ==0)
            word[i] = _xor(Trans(word[i-1],i),word[i-4]);
        else word[i] = _xor(word[i-1],word[i-4]);
    // Convert 40 word từ 5->44, thành 10 key
    BINARY* output = new BINARY[11];
    for (int i=0;i<44;i++)
        output[i/4] += word[i];

    return output;
}
// Done

// Hàm mã hóa
BINARY Encryption(BINARY plain, BINARY key)
{
    BINARY* KeyEx = KeyExpansion(key);
    BINARY round = AddRoundKey(plain,KeyEx[0]);
    // Chạy 9 vòng lặp theo thuật toán 
    for (int i = 0;i<9;i++)
    {
        round = SubBytes(round);
        round = ShiftRows(round);
        round = MixColumns(round);
        round = AddRoundKey(round, KeyEx[i+1]);
    }
    // Bước tọa ngõ ra
    round = SubBytes(round);
    round = ShiftRows(round);
    round = AddRoundKey(round,KeyEx[10]);
    // Kết thúc
    return round;
}

/***************************************************************************\
            CÁC HÀM HỖ TRỢ GIẢI MÃ MÃ AES
Tài liệu tham khảo:
Bảng S_BOX: https://en.wikipedia.org/wiki/Rijndael_S-box
Giải thuật giải mã AES: http://nguyenquanicd.blogspot.com/2019/10/aes-bai-2-ly-thuyet-ve-giai-ma-aes-128.html
\***************************************************************************/

// Hàm AddRoundKey:  Giống phần mã hóa

// Hàm InvSubByte: Thay thế byte input bỡi 1 giá trị được tra trong bảng
BYTE InvSubByte (BYTE input)
{
    // Bảng S_Box được quy định trong chuẩn AES
    // Bảng thể hiện ở dạng HEX
    string S_Box[16][16] = {
        {"52", "09", "6A", "D5", "30", "36", "A5", "38", "BF", "40", "A3", "9E", "81", "F3", "D7", " FB "},  
        {"7C", "E3", "39", "82", "9B", "2F", "FF", "87", "34", "8E", "43", "44", "C4", "DE", "E9", " CB "},
        {"54", "7B", "94", "32", "A6", "C2", "23", "3D", "EE", "4C", "95", "0B", "42", "FA", "C3", " 4E "},
        {"08", "2E", "A1", "66", "28", "D9", "24", "B2", "76", "5B", "A2", "49", "6D", "8B", "D1", " 25 "},
        {"72", "F8", "F6", "64", "86", "68", "98", "16", "D4", "A4", "5C", "CC", "5D", "65", "B6", " 92 "},
        {"6C", "70", "48", "50", "FD", "ED", "B9", "DA", "5E", "15", "46", "57", "A7", "8D", "9D", " 84 "},
        {"90", "D8", "AB", "00", "8C", "BC", "D3", "0A", "F7", "E4", "58", "05", "B8", "B3", "45", " 06 "},
        {"D0", "2C", "1E", "8F", "CA", "3F", "0F", "02", "C1", "AF", "BD", "03", "01", "13", "8A", " 6B "},
        {"3A", "91", "11", "41", "4F", "67", "DC", "EA", "97", "F2", "CF", "CE", "F0", "B4", "E6", " 73 "},
        {"96", "AC", "74", "22", "E7", "AD", "35", "85", "E2", "F9", "37", "E8", "1C", "75", "DF", " 6E "},
        {"47", "F1", "1A", "71", "1D", "29", "C5", "89", "6F", "B7", "62", "0E", "AA", "18", "BE", " 1B "},
        {"FC", "56", "3E", "4B", "C6", "D2", "79", "20", "9A", "DB", "C0", "FE", "78", "CD", "5A", " F4 "},
        {"1F", "DD", "A8", "33", "88", "07", "C7", "31", "B1", "12", "10", "59", "27", "80", "EC", " 5F "},
        {"60", "51", "7F", "A9", "19", "B5", "4A", "0D", "2D", "E5", "7A", "9F", "93", "C9", "9C", " EF "},
        {"A0", "E0", "3B", "4D", "AE", "2A", "F5", "B0", "C8", "EB", "BB", "3C", "83", "53", "99", " 61 "},
        {"17", "2B", "04", "7E", "BA", "77", "D6", "26", "E1", "69", "14", "63", "55", "21", "0C", " 7D "} 
    };
    
    // lấy ra vị trí của hàng cần tra cứu trong bảng S-box
    int row = Convert4bitToInt(input.substr(0,4));
    // lấy ra vị trí của cột cần tra cứu trong bảng S-box
    int col = Convert4bitToInt(input.substr(4,4));
    // tra bảng và lấy giá trị ở dạng hex, chuyển về dạng binary[khối 8bit] và cộng dồn vào output
    BYTE output = ConvertHexToBinary(S_Box[row][col]);
    return output;
}
// Done

// * Hàm SubBytes thực hiện thay thế từng byte của ma trận trạng thái bằng một giá trị đã quy định trong chuẩn AES
BINARY InvSubBytes(BINARY input)
{
    BINARY output = "";
    // Vì block vào là dạng binary có độ dài 8x16 bit, tách ra 16 cụm, mỗi cụm 8 bit và thay thế từng cụm
    for (int i= 0; i< input.length(); i+=8)
    {
        // Lấy cụm 8 bit đang xét
        BYTE tmp = input.substr(i,8);
        // tra bảng và lấy giá trị, chuyển về dạng binary[khối 8bit] và cộng dồn vào output
        output += InvSubByte(tmp);
    }
    return output;
}
// Done

// Xây dựng hàm dịch phải với số lần dịch định trước <=> Xoay vong
BINARY CircularRight(BINARY k, int shifts)
{
    // Dịch trái shifts lần
    for (int i = 0; i < shifts; i++) 
        // kết quả dịch trái của k[0..n] = k[1..n]+k[0]
        k = k[k.length()-1] + k.substr(0,k.length()-1);
    // return kết quả
    return k;
}
// Done

// Xây dựng hàm dịch hàng của block kích thước 4x4 mỗi ô chứa 2 kí tự hex, tức chứa 8 bit
BINARY InvShiftRows(BINARY block)
{
    BYTE state[16];
    for (int i = 0; i<16;i++)
        state[i] = block.substr(i*8,8);

    // Xếp các byte trên ma trận 4x4, từ trên xuống dưới, từ trái qua phải
    //      0 | 4 | 8 |12
    //      1 | 5 | 9 |13
    //      2 | 6 |10 |14
    //      3 | 7 |11 |15
    // theo thuật mô tả của thuật toán ShiftRows ta thu được mận
    //      0 | 4 | 8 |12
    //     13 | 1 | 5 | 9
    //     10 |14 | 2 | 6
    //      7 |11 |15 | 3
    BINARY s = "";
    // cột 1
    s += state[0] + state[13] + state[10] + state[7];
    // cột 2
    s += state[4] + state[1] + state[14] + state[11];
    // cột 3
    s += state[8] + state[5] + state[2] + state[15];
    // cột 4
    s += state[12] + state[9] + state[6] + state[3];
    return s;



    // BINARY s = "";
    // // chuyển block 128 bit thành 4 block 32 bit
    // for (int i = 0; i<block.length();i+=4*8)
    // {
    //     // lấy dữ liệu của một hàng
    //     BINARY m = block.substr(i,4*8);
    //     // theo thuật toán mỗi lần dịch là một ô chứa, do vậy số lần dịch bit cơ số 2 là bộ số của 8
    //     m = CircularRight(m,i/32*8);
    //     // Cộng 4 khối đã chia
    //     s += m;
    // }
    // return s;
}
// Done

// Trả về 1 byte là kết quả của phép nhân theo quy tắc quy định trong chuẩn AES
BYTE Multiply( HEX b,BYTE a)
{
    // http://nguyenquanicd.blogspot.com/2019/10/aes-bai-2-ly-thuyet-ve-giai-ma-aes-128.html

    BYTE output="00000000";
    // Tách 4 bit của hex ra, 15 = 8 + 4 + 2 + 1, ta nhân từng số với a và xor với nhau ta được kết quả của phép nhân
    // Tính + 8a
    BYTE tmp = a;
    if (b[0]=='1')
    {
        for (int i= 0;i<3;i++)
            tmp = Multiply(2,tmp);
        output = _xor(tmp,output);
    }
    // Tính + 4a
    tmp = a;
    if (b[1]=='1')
    {
        for (int i= 0;i<2;i++)
            tmp = Multiply(2,tmp);
        output = _xor(tmp,output);
    }
    // Tính + 2a
    if (b[2]=='1')
    {
        tmp = Multiply(2,a);
        output = _xor(tmp,output);
    }
    // Tính + a
    if (b[3]=='1') 
        output = _xor(a,output);

    return output;
}
// Done

// Hàm nhân 2 ma truần 4x4 hỗ trợ cho giải thuật InvMixColumns
BYTE** InvMultiplyMatrix(HEX A[4][4], BYTE** B)
{
    // Khai báo biến output là 1 con trỏ cấp 2 kiểu BYTE, cấp phát động 4 con trỏ cấp 1
    BYTE** output = new BYTE*[4];
    // chạy lần lược các hàng
    for (int i = 0; i<4; i++)
    {
        // Mỗi con trỏ cấp 1, đc cấp phát động là 4 phần tử byte
        output[i] = new BYTE[4];
        // chạy lần lược ở các cột 
        for(int j=0; j<4;j++)
        {
            // mặt định cho mỗi phần tử là byte 0
            output[i][j] = "00000000";
            // Tính giá trị tại ô [i,j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j] + A[i][3]*B[3][j]
            for (int k = 0; k<4;k++)
            {   
                // tính giá trị A[i][k]*B[k][j] cộng dồn vào output[i][j]
                BYTE tmp = Multiply(A[i][k],B[k][j]);
                output[i][j] = _xor(tmp, output[i][j]);
            }
        }
    }
    return output;
}
// Done

// Hàm InvMixColumns 
BINARY InvMixColumns(BINARY str)
{
    HEX MDSmatrix[4][4] = 
    {   
        {"1110","1011","1101","1001"},
        {"1001","1110","1011","1101"},
        {"1101","1001","1110","1011"},
        {"1011","1101","1001","1110"}
    };
    
    /*************************\
    1. Convert chuỗi BINARY thành ma trận 4x4 mỗi ô chứa 1 byte
    \*************************/
        // Khai báo con trỏ cấp2
    BYTE** before_block;
    // Khởi tạo ma trận 4 nhân 4
    before_block = new BYTE*[4];
    for (int i=0; i<4; i++)
        before_block[i] = new BYTE[4];
    // Duyệt từng cột
    for (int j=0; j<4; j++)
        // Duyệt từng cột
        for (int i = 0; i<4;i++)
            // Gán giá trị thích hợp cho từng ô 
            before_block[i][j] = str.substr(j*32 + i*8, 8);

    /*************************\
    2. Thực hiện phép biến đổi
    \*************************/
    // Thực hiện thực toán nhân ma trận với quy tắc được quy định trong AES, ma trận kết quả vào after_blocc
    BYTE** after_block = InvMultiplyMatrix(MDSmatrix,before_block);

    /*************************\
    3. Convert từ ma trận 4x4 mỗi ô 1 byte về chuỗi BINARY và return kết quả
    \*************************/
    BINARY output="";
    // Duyệt từng hàng, từng cột và cộng vào output theo thứ tự
    for (int j=0;j<4;j++)
        for (int i=0;i<4;i++)
            output += after_block[i][j];
    return output;
}
// Done

// Hàm KeyExpansion
BINARY* InvKeyExpansion(BINARY key)
{
    BINARY* output = new BINARY[11];
    BINARY* KeyEx = KeyExpansion(key);
    for (int i=0;i<11;i++)
        output[i] = KeyEx[10-i];
    return output;
}
// Done

// Hàm giải mã
BINARY Decryption(BINARY cipher, BINARY key)
{
    BINARY* InvKeyEx = InvKeyExpansion(key);
    BINARY round = AddRoundKey(cipher,InvKeyEx[0]);
    // Chạy 9 vòng lặp theo thuật toán 
    for (int i = 0;i<9;i++)
    {
        round = InvShiftRows(round);
        round = InvSubBytes(round);
        round = AddRoundKey(round, InvKeyEx[i+1]);
        round = InvMixColumns(round);
    }
    // Bước tọa ngõ ra
    round = InvShiftRows(round);
    round = InvSubBytes(round);
    round = AddRoundKey(round,InvKeyEx[10]);
    // Kết thúc
    return round;
}
// Done

/***************************************************************************\
            CÁC HÀM MÃ HÓA - GIẢI MÃ DES TRÊN MODE ECB
\***************************************************************************/
// Hàm mã hóa plaintext->ciphertext DES mode ECB
string EncrytionModeECB(string plaintext, BINARY key)
{
    /**********************************\
     1 kí tự string -> 8 bit
     1 khối (128 bit) - 16 kí tự
    Nếu block của chia thiếu thì sao?
    -> thêm vào bit 0 vào phía bên phải đến khi đủ 
        -> lúc giải mã, convert về string thì loại bỏ kí tự null
    \**********************************/
    string cipher;
    int i = 0;
    // chia plain text thành các block có độ dài 16 kí tự
    // 1. Xử lý trường hợp đủ 16 kí tự tức là 128 bit như đã phân tích ở phía trên
    for (; i<plaintext.length()/16;i++)
    {
        // mỗi vòng lặp lấy ra block 8 kí tự, từ trái sang phải lưu vào plain
        string plain = plaintext.substr(i*16,16);
        // chuyển chuỗi kí tự thành chuỗi 128 bit
        plain = ConvertStringToBinary(plain);
        // mã hóa DES với encryption(plain,key), kết quả trả về là khối 128 bit
        // cần chuyển khối sang dạng hexa, để trực quan, 
        // kết quả cộng dồn vào cipher sau mỗi lần mã hóa 1 khối dữ liệu
        cipher += ConvertBinaryToHex(Encryption(plain,key));
    }
    // 2. Xử lý trường hợp block cuối cùng không đủ 16 char (ko đủ 128 bit)
    if (plaintext.length()-i*16 > 0)
    {
        // Lấy các kí tự còn thừa ở block cuối cùng nếu bị lẻ
        string plain = plaintext.substr(i*16,plaintext.length()-i*16);
        // chuyển chuỗi kí tự thành chuỗi bit...
        plain = ConvertStringToBinary(plain);
        // vì đang xét trường hợp khối cuối cùng không đủ 128 bit do đó cần chèn thêm các 
        // bit 0 vào cuối... số bit '0' chèn vào sẽ là bội của 8... 
        //do vậy kí convert sang char thì nó == null, dễ xóa ra khỏi plaintext
        while (plain.length()<128)
            plain+="0";
        // kết quả mã hóa khối cuối cùng cũng cộng dồn vào cipher
        cipher += ConvertBinaryToHex(Encryption(plain,key));
    }
    // kết quả trả về là chuỗi các số hệ hexa
    return cipher;
}
// Hàm giải mã ciphertext->plaintext AES mode ECB
string DecrytionModeECB(string ciphertext, BINARY key)
{
    /**********************************\
    Input: 1 ciphertext hệ hexa. luôn chia hết cho 32.
            vì output mã hóa AES ở trên là 128 bit mỗi khối, mà 1 kí tự hex biểu diễn bởi 4 bit 
    Output: plaintext kiểu dữ liệu string
    \**********************************/
    string plain="";
    // Chia mỗi block gồm 32 kí tự hex <-> 128 bit
    for (int i = 0; i<ciphertext.length()/32;i++)
    {
        // lấy dạng hexa của block thứ i
        string cipher = ciphertext.substr(i*32,32);
        // chuyển cipher sang dạng binary
        cipher = ConvertHexToBinary(cipher);
        // giải mã khối dữ liệu thông qua hàm decryption vừa tạo
        string binary_plain = Decryption(cipher,key);
        // Chuyển sang dạng string khối dữ plaintext vừa tìm, và cộng dồn vào biến plain
        plain += ConvertBinaryToString(binary_plain);
    }
    return plain;
}

// Hàm mã hóa plaintext->ciphertext DES mode ECB
HEX EncrytionModeCBC(string plaintext, BINARY key, BINARY iv)
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
    for (; i<plaintext.length()/16;i++)
    {
        // mỗi vòng lặp lấy ra block 8 kí tự, từ trái sang phải lưu vào plain
        string SubPlain = plaintext.substr(i*16,16);
        // chuyển chuỗi kí tự thành chuỗi 64 bit
        BINARY Block = ConvertStringToBinary(SubPlain);
        // mã hóa DES với EncryptionDES(plain,key), kết quả trả về là khối 64 bit
        // cần chuyển khối sang dạng hexa, để trực quan, 
        // kết quả cộng dồn vào cipher sau mỗi lần mã hóa 1 khối dữ liệu
        Block = _xor(Block, iv);
        iv = Encryption(Block,key);
        cipher += ConvertBinaryToHex(iv);
    }
    // 2. Xử lý trường hợp block cuối cùng không đủ 8 kí tự (ko đủ 64 bit)
    if (plaintext.length()-i*16 > 0)
    {
        // Lấy các kí tự còn thừa ở block cuối cùng nếu bị lẻ
        string SubPlain = plaintext.substr(i*16,plaintext.length()-i*16);
        // chuyển chuỗi kí tự thành chuỗi bit...
        BINARY Block = ConvertStringToBinary(SubPlain);
        // vì đang xét trường hợp khối cuối cùng không đủ 64 bit do đó cần chèn thêm các 
        // bit 0 vào cuối... số bit '0' chèn vào sẽ là bội của 8... 
        //do vậy kí convert sang char thì nó == null, dễ xóa ra khỏi plaintext
        while (Block.length()<128)
            Block+="0";
        // kết quả mã hóa khối cuối cùng cũng cộng dồn vào cipher
        Block = _xor(Block, iv);
        iv = Encryption(Block,key);
        cipher += ConvertBinaryToHex(iv);
    }
    // kết quả trả về là chuỗi các số hệ hexa
    return cipher;
}
// Hàm giải mã ciphertext->plaintext DES mode ECB
string DecrytionModeCBC(HEX ciphertext, BINARY key, BINARY iv)
{
    /**********************************\
    Input: 1 ciphertext hệ hexa. luôn chia hết cho 32.
            vì output mã hóa AES ở trên là 128 bit mỗi khối, mà 1 kí tự hex biểu diễn bởi 4 bit 
    Output: plaintext kiểu dữ liệu string
    \**********************************/
    string plain="";
    // Chia mỗi block gồm 32 kí tự hex <-> 128 bit
    for (int i = 0; i<ciphertext.length()/32;i++)
    {
        // lấy dạng hexa của block thứ i
        HEX SubCipher = ciphertext.substr(i*32,32);
        // chuyển cipher sang dạng binary
        BINARY Block = ConvertHexToBinary(SubCipher);
        // giải mã khối dữ liệu thông qua hàm DecryptionDES vừa tạo
        BINARY DecryptionBlock = Decryption(Block,key);
        // Thực hiện phép xor với iv
        DecryptionBlock = _xor(DecryptionBlock, iv);
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
	plain = ConvertWStringToString(plaintext);
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
    string strText = ConvertWStringToString(wstrText);
    // Convert sang Binary
    KEY = ConvertStringToBinary(strText);
    // Check độ dài của KEY
    if (KEY.length() == 128) return;
    wcout<<L"Nhập lại KEY (16 bytes): ";
    InputKeyFormScreen(KEY);
}

// Hàm nhập key từ màn hình
void InputIVFormScreen(BINARY& IV)
{
    // Giống hàm nhập key từ màn hình
    wstring wstrText;
	fflush(stdin);
	getline(wcin,wstrText);

    string strText = ConvertWStringToString(wstrText);
    IV = ConvertStringToBinary(strText);
    if (IV.length() == 128) return;
    wcout<<L"Nhập lại IV (16 bytes): ";
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
    wcout << L"*** Chương trình AES mode CBC - không sử dụng thư viện ***" <<endl;

    // Nhập plainText
    string plain;
    wcout<<L">> Nhập plaintext: ";
    InputPlainText(plain);

    // Nhập KEY
    BINARY K,IV;
    wcout<<L">> Nhập KEY (độ dài KEY mặc định 16 bytes) :";
    InputKeyFormScreen(K);
    wcout<<L">> Nhập IV (độ dài IV mặc định 16 bytes) :";
    InputIVFormScreen(IV);
    /************************************\
                     MÃ HÓA
    \************************************/
    string cipher = EncrytionModeCBC(plain,K,IV);
    wcout << L">> Ciphertext: " << ConvertStringToWString(cipher) << endl;
    /************************************\
                GIẢI MÃ
    \************************************/
    string encode = DecrytionModeCBC(cipher,K,IV);
	// In giá trị của thời gian giải mã ra màn hình
    wcout << L">> Recovered plaintext: " << ConvertStringToWString(encode)<<endl;
    // system("pause");
    return 0;
}