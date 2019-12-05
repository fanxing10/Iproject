//
// Created by WQSDP2019 on 2019/12/5.
//
#include <iostream>
using namespace std;
int main()
{

    cout << "HelloWorld\n";
    int b = 30;
    int *a = &b;
    cout<<"a:"<<a<<"*a:"<<*a<<endl;
    int ** p =&a;
    cout<<"p:"<<p<<"*p:"<<*p<<"**p"<<**p;

    char* ss[]= {"2","2","3"};
    cout<<endl<< "ss_length:"<<sizeof(ss)<< "\t*ss_length:"<<sizeof(*ss);

    for (int i = 0; i < sizeof(ss)/ sizeof(*ss); ++i) {
        cout<<endl<<"ss["<<i<<"]:"<<ss[i]<<endl;
    }

    char * asc[] = {"1","2","3"};
    char ** ppp=    asc;
    for (int j = 0; j < sizeof(asc)/ sizeof(*asc); ++j) {
        cout<<ppp[j]<<" ";
    }
    char *str = "china";
    while(*str)
    {
        std::cout << *str++ << std::endl;
    }




}

