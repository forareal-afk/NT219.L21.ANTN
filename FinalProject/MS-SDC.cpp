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
#include <stdlib.h>     /* srand, rand */
#include <time.h>  
 
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
#include "include/cryptopp/sha3.h"
using CryptoPP::SHA3_256;

using namespace std;
using namespace CryptoPP;
// Số lượng server là 5
#define MAXSERVER 5
// Max size cua block la 4KB
#define MAXSIZEBLOCK 4096

int randomNumber(int n)
{
    int s;
    /* generate secret number between 0 and n: */
    s = rand() % n;
    return s;
}
bool checkFile(string path)
{
  ifstream isf(path);
  return isf.good();
}
string ConvertHexToString(string);
string HMACSHA3_256(CryptoPP::byte key[32], string plain);
string Decryption_CTR_Mode(string cipher, CryptoPP::byte *key, int KEYLENGTH, CryptoPP::byte *iv, int IVLENGTH);
string Encryption_CTR_Mode(string plain, CryptoPP::byte key[32], CryptoPP::byte iv[16]);
////////////////////////////////////////////
//  SETUP FAKE DATABASE
////////////////////////////////////////////

//Server
class FakeServer
{
public:
    string Name;
    string Key;
    string IV;
    string Path;
    void Getkey()
    {
        AutoSeededRandomPool prng;
		CryptoPP::byte KeyP[32];
		prng.GenerateBlock(KeyP,sizeof(KeyP));
        Key.clear();
        StringSource(KeyP, sizeof(KeyP), true, new HexEncoder(new StringSink(Key))); 
    }
    void GetIV()
    {
        AutoSeededRandomPool prng;
		CryptoPP::byte KeyP[16];
		prng.GenerateBlock(KeyP,sizeof(KeyP));
        IV.clear();
        StringSource(KeyP, sizeof(KeyP), true, new HexEncoder(new StringSink(IV))); 
    }
};
class ListServer
{
public:
    vector<FakeServer> List;
    ListServer()
    {
        if (checkFile("DATABASE/ListServer.csv"))
        {
            ifstream MyReadFile("DATABASE/ListServer.csv");
            while (!MyReadFile.eof())
            {
                FakeServer item;
                MyReadFile >> item.Name;
                MyReadFile >> item.Key;
                MyReadFile >> item.IV;
                MyReadFile >> item.Path;
                List.push_back(item);
            }
            List.pop_back();
            MyReadFile.close();
        }
        else
        {
            for(int i = 0; i<MAXSERVER; i++)
            {
                FakeServer item;
                string s = "Server"+ std::to_string(i);
                item.Name = s;
                item.Getkey();
                item.GetIV();
                item.Path ="CLOUD/"+s+"/";
                List.push_back(item);
            }
        }
    }
    string GetPath(string name)
    {
        for (int i = 0; i<List.size(); i++)
        {
            if (List[i].Name == name)
                return List[i].Path;
        }
        return "";
    }
    int GetIndex(string name)
    {
        for (int i = 0; i<List.size(); i++)
        {
            if (List[i].Name == name)
                return i;
        }
        return 0;
    }
    void Print()
    {
        cout<<"\t\t\t___SERVER LIST TABLE___"<<endl;
        for (int i = 0; i<List.size(); i++)
        {
            cout<<"\t>Line "<<i+1<<":"<<endl;
            cout<<"\t\t*Name: "<<List[i].Name<<" "<<endl;
            cout<<"\t\t*Key : "<<List[i].Key<<endl;
            cout<<"\t\t*IV  : "<<List[i].IV<<endl;
            cout<<"\t\t*Path: "<<List[i].Path<<endl;
        }
    }
    ~ListServer()
    {
        if (!checkFile("DATABASE/ListServer.csv"))
        {
            ofstream WriteFile("DATABASE/ListServer.csv");
            for (int i = 0; i<List.size(); i++)
            {
                WriteFile<<List[i].Name <<" "<<List[i].Key<<" "<<List[i].IV<<" "<<List[i].Path<<endl;
            }
            WriteFile.close();
        }
    }
};

// IndexEntryTable
class IndexEntry
{
public:
    string Filename;
    string Token;
    string BlockID;
    int BlockIndex;
    string ServerName;
    string ServerKey;
};
class IndexEntryTable
{
public:
    vector<IndexEntry> at;
    IndexEntryTable()
    {
        if (checkFile("DATABASE/IndexEntryTable.csv"))
        {
            ifstream MyReadFile("DATABASE/IndexEntryTable.csv");
            while (!MyReadFile.eof())
            {
                IndexEntry item;
                MyReadFile >> item.Filename;
                MyReadFile >> item.Token;
                MyReadFile >> item.BlockID;
                MyReadFile >> item.BlockIndex;
                MyReadFile >> item.ServerName;
                MyReadFile >> item.ServerKey;
                at.push_back(item);
            }
            at.pop_back();
            MyReadFile.close();
        }
    }
    ~IndexEntryTable()
    {
        Upload();
    }
    void Upload()
    {
        ofstream WriteFile("DATABASE/IndexEntryTable.csv");
        for (int i = 0; i<at.size(); i++)
        {
            WriteFile<<at[i].Filename <<" "<<at[i].Token<<" "<<at[i].BlockID<<" ";
            WriteFile<<at[i].BlockIndex<<" "<<at[i].ServerName<<" "<<at[i].ServerKey<<endl;
        }
        WriteFile.close();
    }
    void append(IndexEntry IE)
    {
        at.push_back(IE);
    }
    void Print()
    {
        cout<<"\t\t\t___INDEX ENTRY TABLE___"<<endl;
        for (int i = 0; i< at.size();i++)
        {
            cout<<"\t>Line "<<i+1<<":" <<endl;
            cout<<"\t\t*Filename  : "<<at[i].Filename<<endl;
            cout<<"\t\t*Token     : "<<at[i].Token<<endl;
            cout<<"\t\t*BlockID   : "<<at[i].BlockID<<endl;
            cout<<"\t\t*BlockIndex: "<<at[i].BlockIndex<<endl;
            cout<<"\t\t*ServerName: "<<at[i].ServerName<<endl;
            cout<<"\t\t*ServerKey : "<<at[i].ServerKey<<endl;
        }
    }
};

// QueryKeywordTable
class QueryKeyword
{
public:
    string Token;
    string Keyword;
};
class QueryKeywordTable
{
public:
    vector<QueryKeyword> at;
    QueryKeywordTable()
    {
        if (checkFile("DATABASE/QueryKeywordTable.csv"))
        {
            ifstream MyReadFile("DATABASE/QueryKeywordTable.csv");
            while (!MyReadFile.eof())
            {
                QueryKeyword item;
                MyReadFile >> item.Token;
                MyReadFile >> item.Keyword;
                at.push_back(item);
            }
            at.pop_back();
            MyReadFile.close();
        }
    }
    void append(QueryKeyword a)
    {
        at.push_back(a);
    }
    vector<string> GetToken(string Keyword)
    {
        vector<string> Token;
        for (int i = 0; i< at.size();i++)
            if (Keyword == at[i].Keyword)
                Token.push_back(at[i].Token);
        return Token;
    }
    void Print()
    {
        cout<<"\t\t\t___QUERY KEYWORD TABLE___"<<endl;
        for (int i = 0; i< at.size();i++)
        {
            cout<<"\t>Line "<<i<<":\n";
            cout<<"\t\t*ToKen  : "<<at[i].Token<<endl;
            cout<<"\t\t*Keyword: "<<at[i].Keyword<<endl;
        }
        cout<<endl;
    }
    ~QueryKeywordTable()
    {
        ofstream WriteFile("DATABASE/QueryKeywordTable.csv");
        for (int i = 0; i<at.size(); i++)
        {
            WriteFile<<at[i].Token <<" "<<at[i].Keyword<<endl;
        }
        WriteFile.close();
    }
};

ListServer SERVER_LIST;
IndexEntryTable INDEX_ENTRY_TABLE;
QueryKeywordTable QUERY_KEYWORD_TABLE;

////////////////////////////////////////////
//  DONE SETUP FAKE DATABASE
////////////////////////////////////////////



////////////////////////////////////////////
//  SETUP OBJECT
////////////////////////////////////////////

class KeywordExtraction
{
    string token;
    vector<string> fileKeyword;
    CryptoPP::byte wordKey[32];
    vector<string> keyWordTrapdoor;
public:
    KeywordExtraction()
    {
        if (checkFile("WordKey.key"))
        {
            /* Reading key from file*/
            FileSource fs("WordKey.key", false);
            /*Create space  for key*/
            CryptoPP::ArraySink copykey(wordKey, sizeof(wordKey));
            /*Copy data from FileKey.key  to  key */
            fs.Detach(new Redirector(copykey));
            fs.Pump(sizeof(wordKey)); // Pump first 16 bytes
        }
        else
        {
            AutoSeededRandomPool prng;
            prng.GenerateBlock(wordKey, sizeof(wordKey));
            StringSource ss(wordKey, sizeof(wordKey), true, new FileSink("WordKey.key"));
        }
    }
    void GetFileKeyWord(string filename, string ToKen, string Pathfile)
    {
        token = ToKen;
        // Kiểm tra file keyword 
        // File name cũng là một từ khóa
        fileKeyword.push_back(filename);
        if (checkFile("Upload/"+Pathfile))
        {
            ifstream MyReadFile("Upload/"+Pathfile);
            while (!MyReadFile.eof())
            {
                string s;
                getline(MyReadFile, s);
                fileKeyword.push_back(s);
            }
            MyReadFile.close();
        }
        else
        {
            cout<<">> Error : \""<<Pathfile<<"\" file does not exist"<<endl;
        }
        // PrintProsess
        for (int i=0;i<fileKeyword.size();i++)
            cout<<"\t* Keyword: ("<<fileKeyword[i]<<")\n";

        genTrapdoor();
        addTrapdoor();

        // Xóa
        fileKeyword.clear();
        keyWordTrapdoor.clear();
    }
    // Hàm tạo trapdoor dựa trên fileKeyword và wordKey
    void genTrapdoor()
    {
        for (int i = 0; i < fileKeyword.size(); i++)
            keyWordTrapdoor.push_back(HMACSHA3_256(wordKey, fileKeyword[i]));
    }
    // Hàm thêm trapdoor (vào cột keyword tương ứng với token) vô trong bảng QUERY_KEYWORD_TABLE
    void addTrapdoor()
    {
        for (int i = 0; i < keyWordTrapdoor.size(); i++)
        {
            QueryKeyword item;
            item.Token = token;
            item.Keyword = keyWordTrapdoor[i];
            QUERY_KEYWORD_TABLE.append(item);
        }
    }
};
class FileUpload
{
    // Key của thằng master
    CryptoPP::byte MasterKey[32];
    // Cái để dùng mã hóa từng block
    vector <string> SecretsKeys;
    string filename;
    // xử lý qua hàm hmacSHA3_256 của thằng filename
    string KeywordToken;
    // 1 bảng tạm. sau khi hoàn tất quá trình tải lên 1 file thì sẽ push lên INDEX_ENTRY_TABLE
    IndexEntryTable Table;
    // Biến block -> sau khi mà chia xong thì nó sẽ sử dụng con trỏ, tạo tối tượng -> lấy từng block gán vào
    vector <string> Block; 
    // thằng server nào lưu nó
    vector <int> ServerSaveBlock;
    // BlockID của từng khối
    vector <string> BlockID;
    KeywordExtraction KE;
public:
    FileUpload()
    {
        DerivesSecretsKeys();
    }
    bool UploadFile(string filename, string fileKeyword)
    {
        if (checkFile("Upload/"+filename))
        {
            // Lấy mốc thời gian 
	        auto start = std::chrono::high_resolution_clock::now();

            this->filename = filename;

            // PrintProsess
            cout<<"\t* FileName: ("<<filename<<")\n";

            GennerateToken();
            KE.GetFileKeyWord(filename,KeywordToken, fileKeyword);
            SplitBlocks();

            // PrintProsess
            cout<<"\t* Block count: "<<Block.size()<<"\n";

            ServerSelection();
            AESCTR256();
            GenerateBlocksIDs();

            UpdateDatabase();
            Upload();
            Clear();

            auto stop = std::chrono::high_resolution_clock::now();
            // Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
	        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
            cout<<"\t* Timer Counter = "<<duration.count()<<" milliseconds"<<endl;
            return 1;
        }
        cout<<"Error \""<<filename<<"file does not exist!"<<endl;
        return 0;
    }
    // Step 2
    void DerivesSecretsKeys()
    {
        if (checkFile("MasterKey.key"))
        {
            /* Reading key from file*/
            FileSource fs("MasterKey.key", false);
            /*Create space  for key*/ 
            CryptoPP::ArraySink copykey(MasterKey,sizeof(MasterKey));
            /*Copy data from FileKey.key  to  key */ 
            fs.Detach(new Redirector(copykey));
            fs.Pump(sizeof(MasterKey));          // Pump first 16 bytes
        }
        else
        {
            AutoSeededRandomPool prng;
            prng.GenerateBlock(MasterKey,sizeof(MasterKey));
            StringSource ss(MasterKey,sizeof(MasterKey), true , new FileSink( "MasterKey.key"));
        }

        string encoded;
        StringSource ss3(MasterKey,sizeof(MasterKey), true,
            new HexEncoder(
                new StringSink(encoded)
            ) // HexEncoder
        ); // StringSource
        cout<<"Master Key : "<<encoded<<endl;

        for(int i = 0;i<SERVER_LIST.List.size();i++)
        {
            SecretsKeys.push_back(HMACSHA3_256(MasterKey,SERVER_LIST.List[i].Key));
        }
    }//Done
    // Step 1 
    void GennerateToken()
    {
        KeywordToken = HMACSHA3_256(MasterKey, filename);
    }// Done
    // Step 3
    void SplitBlocks()
    {
        string myText;
        string fulltext = "";
        string pathFileUpload = "Upload/"+filename;
        const char *path = pathFileUpload.c_str();
        /*
        ifstream MyReadFile(pathFileUpload);
        while (!MyReadFile.eof())
        {
            getline(MyReadFile, myText);
            fulltext += myText + '\n';
        }
        */
        // Readfile 
        FileSource readfile(path, true, new StringSink(fulltext));
        for (unsigned int i = 0; i < fulltext.length(); i += MAXSIZEBLOCK)
        {
            string s = fulltext.substr(i, MAXSIZEBLOCK);
            Block.push_back(s);
        }
        // MyReadFile.close();
	}//Done
    // Step 4
    void ServerSelection()
    {
        for (int i = 0; i<Block.size();i++)
        {
            ServerSaveBlock.push_back(randomNumber(SERVER_LIST.List.size()));
        }
    }//Done
    // Step 5 Encryption
    void AESCTR256()
    {
        for (int i = 0;i<Block.size();i++)
        {
            string Key = SecretsKeys[ServerSaveBlock[i]];
            Key = ConvertHexToString(Key);
            string IV = ConvertHexToString(SERVER_LIST.List[ServerSaveBlock[i]].IV);
            Block[i] = Encryption_CTR_Mode(Block[i], (CryptoPP::byte*)Key.c_str(), (CryptoPP::byte*)IV.c_str());
        }
    }// Done
    // Step 6 Generate Blocks IDs
    void GenerateBlocksIDs()
    {
        // PrintProsess
        cout<<"\t* Blocks Names on Servers 'BlockIds' :\n";
        for (int i = 0;i<Block.size();i++)
        {
            string key = ConvertHexToString(KeywordToken);
            BlockID.push_back(HMACSHA3_256((CryptoPP::byte*)key.c_str(), Block[i]));
            // PrintProsess
            cout<<"\t* "<<SERVER_LIST.List[ServerSaveBlock[i]].Name<<","<<BlockID[i]<<endl;
        }
    } // Done
    // Step 8 connect database, update database
    void UpdateDatabase()
    {
        for (int i = 0;i<Block.size();i++)
        {
            IndexEntry item;
            item.Filename = filename;
            item.Token = KeywordToken;
            item.BlockID = BlockID[i];
            item.BlockIndex = i;
            item.ServerName = SERVER_LIST.List[ServerSaveBlock[i]].Name;
            item.ServerKey = SERVER_LIST.List[ServerSaveBlock[i]].Key;
            INDEX_ENTRY_TABLE.append(item);
        }
    }
    // Step 7 Save block lên Server
    void Upload()
    {
        for (int i = 0;i<Block.size();i++)
        {
            string Path = SERVER_LIST.List[ServerSaveBlock[i]].Path;
            Path += BlockID[i];
            ofstream MyWriteFile(Path);
            MyWriteFile << Block[i];
            MyWriteFile.close();
        }
    }
    //Clear
    void Clear()
    {
        filename.clear();
        KeywordToken.clear();
        Block.clear();
        ServerSaveBlock.clear();
        BlockID.clear();
    }
};

class FileDownload
{
    // Key của thằng master
    CryptoPP::byte MasterKey[32];
    // Cái để dùng mã hóa từng block
    vector <string> SecretsKeys;
    string filename;
    // xử lý qua hàm hmacSHA3_256 của thằng filename
    string KeywordToken;
    // 1 bảng tạm. sau khi hoàn tất quá trình tải lên 1 file thì sẽ push lên INDEX_ENTRY_TABLE
    IndexEntryTable Table;
    string Doccument;
    vector <string> Block;
public:
    void GetKey()
    {
        /* Reading key from file*/
        FileSource fs("MasterKey.key", false);
        /*Create space  for key*/ 
        CryptoPP::ArraySink copykey(MasterKey,sizeof(MasterKey));
        /*Copy data from FileKey.key  to  key */ 
        fs.Detach(new Redirector(copykey));
        fs.Pump(sizeof(MasterKey));          // Pump first 16 bytes

        string encoded;
        StringSource ss3(MasterKey,sizeof(MasterKey), true,
            new HexEncoder(
                new StringSink(encoded)
            ) // HexEncoder
        ); // StringSource

        for(int i = 0;i<SERVER_LIST.List.size();i++)
        {
            SecretsKeys.push_back(HMACSHA3_256(MasterKey,SERVER_LIST.List[i].Key));
        }
    }
    // Step 0
    void DownloadFile(string token)
    {
        // Lấy mốc thời gian 
        auto start = std::chrono::high_resolution_clock::now();
        GetKey();
        GetTable(token);
        CollectBlocks();
        Decryption();
        ReconstructFile();
        SaveFile();
        Clear();
        auto stop = std::chrono::high_resolution_clock::now();
        // Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
        cout<<"\t* Timer Counter = "<<duration.count()<<" milliseconds"<<endl;
    }
    // Step 1
    void GetTable(string Token)
    {
        KeywordToken = Token;// Không cần thiết
        for (int i=0; i< INDEX_ENTRY_TABLE.at.size(); i++)
        {
            if (INDEX_ENTRY_TABLE.at[i].Token == Token)
                Table.append(INDEX_ENTRY_TABLE.at[i]);
        }        
    }// Done
    // Step 2
    void CollectBlocks()
    {
        //PrintProsess
        cout<<"\t* FileName: ("<<Table.at[0].Filename<<")\n";
        cout<<"\t* Block count: "<<Table.at.size()<<"\n";
        cout<<"\t* Blocks Names on Servers 'BlockIds' :\n";
        for (int i= 0; i<Table.at.size(); i++)
        {
            //PrintProsess
            cout<<"\t* "<<Table.at[i].ServerName<<","<<Table.at[i].BlockID<<endl;

            string path = SERVER_LIST.GetPath(Table.at[i].ServerName);
            path += Table.at[i].BlockID;
            
            ifstream block(path);
            string fullBlock = "";
            string recovered;
            while (!block.eof())
            {
                getline(block, recovered);
                fullBlock += recovered;
            }
            block.close();

            Block.push_back(fullBlock);
        }
    }
    // Step 3 
    void Decryption()
    {  
        for (int i = 0; i< Table.at.size();i++)
        {
            int indexServer = SERVER_LIST.GetIndex(Table.at[i].ServerName);
            string Key = ConvertHexToString(SecretsKeys[indexServer]);
            string IV = ConvertHexToString(SERVER_LIST.List[indexServer].IV);
            // vì encryption đang ở dạng hex nên phải decode lại
            string cipher;
            StringSource(Block[i], true, new HexDecoder(new StringSink(cipher)));
            Block[i] = Decryption_CTR_Mode(cipher, (CryptoPP::byte*)Key.c_str(), 32, (CryptoPP::byte*)IV.c_str(), 16);
        }
    }
    // Step 4
    void ReconstructFile()
    {
        Doccument = "";
        for (int i = 0; i< Table.at.size();i++)
        {
            Doccument+=Block[i];
        }
    }
    // Step 5 
    void SaveFile()
    {
        string pathFile = "Download/"+Table.at[0].Filename;
        const char *path = pathFile.c_str();
        StringSource writefile(Doccument, true, new FileSink(path));
        // ofstream WriteFile("Download/"+Table.at[0].Filename);
        // WriteFile << Doccument;
        // WriteFile.close();
    }
    // Step 6
    void Clear()
    {
        Block.clear();
        Doccument.clear();
        Table.at.clear();
        filename.clear();
    }
};

class Search
{
    // word key
    CryptoPP::byte wordKey[32];
    string trapdoorSearch;
    bool fKey;

public:
    Search()
    {
        if (checkFile("WordKey.key"))
        {
            /* Reading key from file*/
            FileSource fs("WordKey.key", false);
            /*Create space  for key*/
            CryptoPP::ArraySink copykey(wordKey, sizeof(wordKey));
            /*Copy data from FileKey.key  to  key */
            fs.Detach(new Redirector(copykey));
            fs.Pump(sizeof(wordKey)); // Pump first 16 bytes

            fKey = true;
        }
        else
        {
            fKey = false;
            cout << ">> Error : \""
                 << "WordKey.key"
                 << "\" file does not exist" << endl;
        }
    }
    // hàm tìm kiếm xem có tồn tại token nào bằng với token của keyword không
    // trả về danh sách token
    vector<string> Searching(string query)
    {
        // Danh sách trả về
        vector<string> token;
        if (!fKey)
            return token;
        // Query do người dùng nhập vào hàm mình tạo sẽ có nó
        // hàm lấy trapdoor key word dựa vào hàm hash
        trapdoorSearch=HMACSHA3_256(wordKey, query);
        token = QUERY_KEYWORD_TABLE.GetToken(trapdoorSearch);

        return token;
    }

    // Hàm lấy thông tin các file sau khi đã có token
    vector<string> getFileName(vector<string> token)
    {
        vector<string> FileName;
        for (int i = 0; i < token.size(); i++)
        {
            for (int j = 0; j < INDEX_ENTRY_TABLE.at.size(); j++)
                if (token[i] == INDEX_ENTRY_TABLE.at[j].Token)
                {
                    FileName.push_back(INDEX_ENTRY_TABLE.at[j].Filename);
                    break;
                }
        }
        return FileName;
    }
};

FileUpload UPLOAD_FILE;
FileDownload DOWNLOAD_FILE;
Search SEARCH_KEYWORD;

////////////////////////////////////////////
//  DONE SETUP OBJECT
////////////////////////////////////////////
void HelpCommandLine();
int main()
{
    srand (time(NULL));
    string command;
    vector <string> token;
    vector <string> filenameoftoken;
    cout<<"\n";
    cout<<"\t----------DEMO SIMPLE MS-SDC----------\n";
    token.clear();
    filenameoftoken.clear();

    while (true)
    {
        bool flagSearching = false;
        cout<<">>> ";
        cin>> command;
        if (command =="uploadfile")
        {
            // Xóa đi những kết quả tìm của lần trước đó
            token.clear();
            filenameoftoken.clear();

            string filename;
            cin>>filename;
            string filekeyword;
            cin>>filekeyword;
            cout<<"* Uploading files:"<<filename<<","<<filekeyword<<"..."<<endl;
            if (UPLOAD_FILE.UploadFile(filename,filekeyword))
                cout<<"* Uploaded..."<<endl;
        }
        else if ( command =="search")
        {
            // Xóa đi những kết quả tìm của lần trước đó
            token.clear();
            filenameoftoken.clear();

            flagSearching = true;
            string keyword;
            cin>>keyword;
            cout<<"* Searching:" <<keyword<<"..."<<endl;
            
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i<100;i++)
            {
                token.clear();
                token = SEARCH_KEYWORD.Searching(keyword);
            }
            auto stop = std::chrono::high_resolution_clock::now();
            // Xác định sai lệch giữa 2 mốc thời gian chuyển về đơn vị microseconds
            auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
            cout<<"\t* Search Time = "<<duration.count()/100<<" microseconds"<<endl;
            
            filenameoftoken = SEARCH_KEYWORD.getFileName(token);
        }
        else if ( command =="downloadfile")
        {
            int index;
            cin>>index;
            if (index>= filenameoftoken.size())
            {
                cout<<"Choose the wrong index"<<endl;
            }
            else
            {
                cout<<"* Downloading file:" <<filenameoftoken[index]<<endl;
                DOWNLOAD_FILE.DownloadFile(token[index]);
                cout<<"* Downloaded ..."<<endl;
            }
        }
        // Check database
        else if ( command =="CheckIndexEntryTable")
        {
            INDEX_ENTRY_TABLE.Print();
        }
        else if ( command =="CheckServerList")
        {
            SERVER_LIST.Print();
        }
        else if ( command =="CheckQueryKeywordTable")
        {
            QUERY_KEYWORD_TABLE.Print();
        }
        else if (command == "--help")
        {   
            HelpCommandLine();
        }
        else if (command == "exit()")
        {   
            break;
        }
        else
        {
            cout<<"---bash: "<< command <<": command not found"<<endl;
            cout<<"You can type \"--help\" for instructions"<<endl;
        }
        cin.clear();
        fflush(stdin);
        if (flagSearching)
        {
            cout<<"* Search results: "<<filenameoftoken.size()<<" file \n";
            for (int i=0;i<filenameoftoken.size();i++)
                cout<<"\t["<<i<<"] "<<filenameoftoken[i]<<endl;
            flagSearching = false;
        }
    }
    return 0;
}
void HelpCommandLine()
{
    cout<<endl;
    cout<<"\t-usage: uploadfile [filename] [fileKeyWord]"<<endl;
    cout<<"\t\tto upload filename to cloud with keyword lists stored in fileKeyWord.\n";
    cout<<"\t-usage: seach [keyword]"<<endl;
    cout<<"\t\tto search for files with the keyword \"keyword\""<<endl;
    cout<<"\t-usage: downloadfile [number]"<<endl;
    cout<<"\t\tto download files whose index in the searched files is \"number\""<<endl;
    cout<<"\t-Check database using: "<<endl;
    cout<<"\t\t*CheckIndexEntryTable"<<endl;
    cout<<"\t\t*CheckServerList"<<endl;
    cout<<"\t\t*CheckQueryKeywordTable"<<endl;
}
string ConvertHexToString(string encoded)
{
    string decoded;
    StringSource ss(encoded, true,
		new HexDecoder(
			new StringSink(decoded)
		) // HexDecoder
	);
    return decoded;
}
string HMACSHA3_256(CryptoPP::byte key[32], string plain)
{
        string mac, encoded;
        try
        {
                HMAC< SHA3_256 > hmac(key, 16);

                StringSource(plain, true,
                        new HashFilter(hmac,
                                new StringSink(mac)
                        ) // HashFilter      
                ); // StringSource
        }
        catch(const CryptoPP::Exception& e)
        {
                cerr << e.what() << endl;
        }

        encoded.clear();
        StringSource ss3(mac, true,
            new HexEncoder(
                new StringSink(encoded)
            ) // HexEncoder
        ); // StringSource
        return encoded;
}

/*********************************\
[5]CTR_Mode
\*********************************/
// Xây dựng hàm mã hóa để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Encryption_CTR_Mode(string plain, CryptoPP::byte key[32], CryptoPP::byte iv[16])
{
	// Khai báo biến e thuộc CTR_Mode< AES >::Encryption
	CTR_Mode<AES>::Encryption e;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho e
	e.SetKeyWithIV(key, 32, iv);

	string cipher;
	try
	{
		// tạo ra StreamTransformationFilter() cho CBC< AES >::Encryption [e], chuyển plaintext (ASCII) thành cipher (bit)
		StringSource s(plain, true, new StreamTransformationFilter(e, new StringSink(cipher)));
	}
	// Bắt lỗi và in ra màn hình nếu có lỗi xảy ra
	catch (const CryptoPP::Exception &e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	string encoded;
	encoded.clear();
	StringSource(cipher, true, new HexEncoder(new StringSink(encoded)));
	// Trả về cipher dạng byte string
	return encoded;
}
// Xây dựng hàm giải mã để tách ra làm nhiều vòng, hỗ trợ việc đo thời gian chạy
string Decryption_CTR_Mode(string cipher, CryptoPP::byte *key, int KEYLENGTH, CryptoPP::byte *iv, int IVLENGTH)
{
	// Khai báo biến d thuộc CTR_Mode< AES >::Decryption
	CTR_Mode<AES>::Decryption d;
	// Đặt khóa [key] với vector khởi tạo ban đầu [iv] cho d
	d.SetKeyWithIV(key, KEYLENGTH, iv, IVLENGTH);

	string recovered;
	try
	{
		// Giải mã cipher (bit) lưu vào encovered (ASCII)
		StringSource s(cipher, true, new StreamTransformationFilter(d, new StringSink(recovered)));
	}
	// bắt lỗi và in ra màn hình nếu trong quá trình giải mã xuất hiện lỗi
	catch (const CryptoPP::Exception &e)
	{
		cerr << e.what() << endl;
		exit(1);
	}
	// Trả về kết quả giải mã _ ngôn ngữ tự nhiên
	return recovered;
}