#include <iostream>
using namespace std;
int main()
{
    int s = 0;
    for (int i = 0; i<100; i++)
        if (i%2 == 0)
            s+=i;
    cout<<s<<endl;
    return 0;
}