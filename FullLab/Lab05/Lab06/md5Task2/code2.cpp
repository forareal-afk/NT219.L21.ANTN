#include <iostream>
using namespace std;
int main()
{
    int s = 0;
    while (true)
    {
        if(s>1000) break;
        s = s*2 - 3;
    }
    cout<<s<<endl;
    return 0;
}