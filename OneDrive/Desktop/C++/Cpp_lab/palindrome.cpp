#include<iostream>
#include<string>
using namespace std;
int main()
{
    string S;
    cout<<"Enter a string: ";
    cin>>S;
    cout<<"the given is"<<S<<endl;
    for(char C:S)
    cout<<(char)toupper(C);

    bool pal;
    size_t i=0, j=S.size()-1;
    for(i=0; i<j;++i,--j)
    {
        if(S[i]==S[j])
        {
            pal=true;
            cout<<"Is a palindrome"<<endl;
            break;
        }
        else
        {
            cout<<"not a palindrome"<<endl;
        }
    }
    return 0;
}