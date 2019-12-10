//
// Created by WQSDP2019 on 2019/12/9.
//
#include "Utility.h"
#include "Graph.h"
#include "MemoryUsage.h"

#include<stdlib.h>
#include<string.h>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<queue>
#include<functional>

#define S_STATE 0
#define I_STATE 1
#define SI_STATE 2
#define R_STATE 3
#define REDUND 10
#define CLOCKS_PER_SEC ((clock_t)1000)

using namespace std;

struct Pair {
    int key;
    float value;
    Pair(int key, float value) :key(key), value(value) {};
};
typedef struct Pair Pair;
//运算符重载，根据value的值来排序
bool operator > (Pair a, Pair b) {
    return a.value < b.value;
}

struct MinPair {
    int key;
    float value;
    MinPair(int key, float value) :key(key), value(value) {};
};
typedef struct MinPair MinPair;

bool operator > (MinPair a, MinPair b) {
    return a.value > b.value;
}


void parseArg(int argn, char ** argv);
void run(Graph *g, int k);
float mc_influence(Graph *g, int *seed_arr, int k);
//argn 是 argument number的缩写，表示传入main函数的参数个数；
//argv 是 argument vector的缩写，表示传入main函数的参数序列或指针，
//并且第一个参数argv[0]一定是程序的名称，并且包含了程序所在的完整路径，
//所以确切的说需要我们输入的main函数的参数个数应该是argc-1个
//argv是指向指针的指针，main函数的第二个参数“char *argv[]“也可以替换为 “char **argv“，
//两者是等价的
int main(int argn, char ** argv)
{
    cout << "Program Start at: " << currentTimestampStr() << endl;
    cout << "Arguments: ";
    for(int i = 0; i < argn; i++)
    {
        cout << argv[i]<<" ";    //输出：     Arguments: ./Degree -data NetHEPT.txt -k 50
    }
    cout << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    parseArg( argn, argv );
    cout << "--------------------------------------------------------------------------------" << endl;
    cout<<"Program Ends Successfully at: " << currentTimestampStr() << endl;
    return 0;
}

void parseArg(int argn, char ** argv)
{
    // the parameters
    //string data="";  // the path of the dataset
    string data="C:\\Users\\WQSDP2019\\CLionProjects\\Iuntitled\\untitled\\tntp2.txt";
    int k=50;  //the # of seeds to be found

    for(int i=0; i<argn; i++)
    {
        if(argv[i] == string("-data"))
            data=string(argv[i+1]);
        if(argv[i] == string("-k"))
            k=atoi(argv[i+1]);//atoi：把字符串换成整数
    }
    if (data=="")
        ExitMessage("argument data missing");
    if(k == 0)
        ExitMessage("argument k missing");
    Graph *g = new Graph(data);   //建了一个图，把图按照数组存储起来
    cout << "graph " << data << " was built successfully!" << endl;
    run(g, k);
}


void run(Graph *g, int k)
{
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << "Start Degree Heursitic algorithm" << endl;
    time_t time_start = time(NULL);
    cout << "No.\tnode_id\tDegree\ttime(s)" << endl;   //No1节点       度       时间
    //priority_queue 对于基本类型的使用方法相对简单。
    //他的模板声明带有三个参数，priority_queue<Type, Container, Functional>
    //Type 为数据类型， Container 为保存数据的容器，Functional 为元素比较方式。
    //Container 必须是用数组实现的容器，比如 vector, deque 但不能用 list.
    //STL里面容器默认用的是 vector. 比较方式默认用 operator< ,
    //所以如果你把后面俩个参数 缺省的话，优先队列就是大顶堆，队头元素最大。
    //STL里面定义了一个仿函数 greater<>，对于基本类型可以用这个仿函数声明小顶堆，重载运算符大小反过来
    priority_queue<MinPair, vector<MinPair>, greater<MinPair> > tmp_pqueue;    //priority_queue本质是一个堆 ，定义了tmp_pqueue
    float total_inf = 0;
    int *seed_arr = new int[k];
    for (int i = 0; i < g->num_nodes; i++)
    {
        int k_out = g->node_array[i].k_out;
        MinPair m_pair(i, k_out);
        tmp_pqueue.push(m_pair);
        if ((int)tmp_pqueue.size() > k)
        {  // Kepp the top 10*k nodes with the maximum score
            tmp_pqueue.pop();           // tmp_pqueue 里面存放了k个度最大的节点
        }
    }

    priority_queue<Pair, vector<Pair>, greater<Pair> > pqueue;     //定义了pqueue
    while (!tmp_pqueue.empty())   //当栈非空
    {
        MinPair min_pair = tmp_pqueue.top();
        tmp_pqueue.pop();
        Pair pair(min_pair.key, min_pair.value);//两个小堆转换，变成大堆    也是存放种子节点的地方
        pqueue.push(pair);    //把节点压栈，所以pqueue是栈
    }


    for(int i = 0; i < k; i++)
    {
        Pair pair = pqueue.top();    //种子在pqueue中
        pqueue.pop();
        seed_arr[i] = pair.key;    //即种子节点存在pair里
        //	total_inf = mc_influence(g, seed_arr, i+1);

        cout << i+1 << "\t" << pair.key <<"\t" <<pair.value<<"\t" <<time(NULL) - time_start << endl;    //key表示第几个节点，   value表示节点的度
    }
    disp_mem_usage();
    cout << "Time used: " << time(NULL) - time_start << " s" << endl;
    cout << "Time used: " << clock()/CLOCKS_PER_SEC << " ms" << endl;
}



