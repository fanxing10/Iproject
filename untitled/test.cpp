//
// Created by WQSDP2019 on 2019/12/5.
//
#include <iostream>
int main()
{
    using namespace std;
    cout << "HelloWorld\n";
    int * a = new int [100];
    cout<< sizeof(a)<<endl;
    for (int i = 0; i < 50; ++i) {
        a[i]=i;cout<<"a["<<i<<"]:"<<a[i]<<"";
    }
    cout<<endl<< sizeof(a);
    //cout << endl;
    cout << "2333";
    return 0;
}

