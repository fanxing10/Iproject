//
// Created by WQSDP2019 on 2019/12/9.
//

#include "Utility.h"
#include "Graph.h"
//#include "MemoryUsage.h"

#include<math.h>
#include<stdlib.h>
#include<string.h>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<queue>
#include<functional>

#define S_STATE 0  //未激活节点
#define I_STATE 1  //当前轮激活节点
#define SI_STATE 2 //下一轮激活的节点
#define R_STATE 3  //之前激活的节点
#define REDUND 10

class Pair {
public:
    int key;
    float value;
    Pair(int key, float value) :key(key), value(value) {};
};
using namespace std;

void parseArg(int argn, char ** argv);
void run(Graph *g, string data, int k);

float mc_influence(Graph *g, int *seed_arr, int k);

int main(int argn, char ** argv)
{
    cout << "Program Start at: " << currentTimestampStr() << endl;
    cout << "Arguments: ";
    for(int i = 0; i < argn; i++){
        cout << argv[i]<<" ";
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
    //string data="G:\\data\\football.txt";  // the path of the dataset
    string data="C:\\Users\\WQSDP2019\\CLionProjects\\Iuntitled\\untitled\\tntp2.txt";
    int k=50;  //the # of seeds to be found

    for(int i=0; i<argn; i++)
    {
        if(argv[i] == string("-data"))
            data=string(argv[i+1]);
        if(argv[i] == string("-k"))
            k=atoi(argv[i+1]);
    }
    if (data=="")
        ExitMessage("argument data missing");
    if(k == 0)
        ExitMessage("argument k missing");
    Graph *g = new Graph(data);
    cout << "graph " << data << " was built successfully!" << endl;
    run(g, data, k);
}

// The Greedy algorithm
void run(Graph *g, string data, int k){
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << "Start greedy algorithm" << endl;
    cout << "data:" << data << " k:" << k << endl;
    time_t time_start = time(NULL);
    cout << "Finding top " << k << " nodes with greedy algorithm" << endl;
    cout << "No.\tnode_id\tinfluence\ttime(s)" << endl;

    // find the top k nodes
    int *seed_arr = new int[k];
    queue<Pair> pqueue;    //应该是普通队列
    for(int i = 0; i < g->num_nodes; i++){    //给每个节点压一个初始值为0的影响力
        seed_arr[0] = i;    //？？？
        float inf = 0;
        pqueue.push(Pair(i, inf));
    }
    int *updated = new int[g->num_nodes]; //表明节点是否选为种子节点,1表示没选中
    for (int i = 0; i < g->num_nodes; i++)
        updated[i] = 1;
    float total_inf = 0;
    ofstream seedfile("Greedy+network.txt");
    for (int i = 0; i < k; i++)
    {
        Pair best_pair = pqueue.front();
        Pair state_pair = pqueue.front();
        pqueue.pop();
        for(int j = 0; j < g->num_nodes;j++)  //在所有种子中选一个种子节点
        {
            if(updated[state_pair.key]==1)     //状态1表示没选中
            {
                seed_arr[i] = state_pair.key;
                float m_gain = mc_influence(g, seed_arr, i+1) - total_inf;
                if(m_gain>best_pair.value)
                {      //如果此节点收益更高，记录优秀值，以便下一步比较
                    best_pair.value = m_gain;      //则下一步被比较的影响力大小换为此节点边际收益
                    best_pair.key=state_pair.key;    //候选节点换位此节点
                    pqueue.push(state_pair);     //压入队列
                }
                else
                {
                    pqueue.push(state_pair);        //如果影响力较小，不记录优秀值
                }
            }
            else
            {
                pqueue.push(state_pair);
            }
            // cout<<j<<endl;
            state_pair = pqueue.front();
            pqueue.pop();
        }
        seed_arr[i] = best_pair.key;  //选出一个种子
        total_inf += best_pair.value;  //影响力加上此种子的边际收益
        updated[best_pair.key] = 0;
        if(seedfile.is_open()){
            seedfile<<best_pair.key<<"\t"<<total_inf<<endl;
        }
        cout << i + 1 << "\t" << best_pair.key << "\t" << total_inf << "\t\t" << time(NULL) - time_start << endl;
    }
    if(seedfile.is_open()){
        seedfile<<time(NULL) - time_start;
    }
    seedfile.close();
    //disp_mem_usage("");
    cout << "Time used: " << time(NULL) - time_start << " s" << endl;
}

//Compute the influence spread using Mento-Carlo simulation
float mc_influence(Graph *g, int *seed_arr, int k){
    srand((unsigned)time(NULL));
    double inf = 0;
    int *i_arr = new int[g->num_nodes]; //the array of current active nodes
    int i_size = 0; // the # of newly active nodes
    int *r_arr = new int[g->num_nodes]; // the array of previous active nodes
    int r_size = 0; // the # of previously active nodes
    int *si_arr = new int[g->num_nodes];  // the array of nodes to be active in t+1
    int si_size = 0; // the # of nodes to be active in t+1
    int *state_arr = new int[g->num_nodes]; // the state of nodes
    memset(state_arr, S_STATE, g->num_nodes * sizeof(int)); // initialize the state array
    int *rand_arr = new int[g->num_nodes]; //the 0 ~ n-1 numbers sorted by random order
    for(int r = 0; r < NUM_SIMUS; r++){
        double active_size = 0;
        //reset the state of all nodes
        for(int i = 0; i < r_size; i++){
            state_arr[r_arr[i]] = S_STATE;     //标记为未活动节点
        }
        r_size = 0;
        // initialize the seed nodes
        for(int i = 0; i < k; i++){
            i_arr[i_size++] = seed_arr[i];
            state_arr[i_arr[i]] = I_STATE;       //把k个种子节点标记为活动节点
        }
        while(i_size > 0){   //权重级联模型    当总有节点处于活动状态
            active_size += i_size;    //先把种子加入到影响力大小中
            si_size = 0;
            randomOrder(rand_arr, i_size);
            for(int i = 0; i < i_size; i++){     //对k个种子循环
                int i_node = i_arr[i];
                int k_out = g->node_array[i_node].k_out;
                for(int j = 0; j < k_out; j++){          //对种子的邻居遍历
                    int neigh = g->node_array[i_node].id_array[j];
                    if (state_arr[neigh] == S_STATE) {     //如果邻居是未激活状态，则进行激活操作
                        int k_in = g->node_array[neigh].k_in;
                        double pp = 1.0 / k_in;  //激活概率为入度的倒数
                        //double pp = 0.01;      //独立级联模型
                        double rand_float = ((double)rand()) / RAND_MAX;//生成0到1之间的随机数
                        if(rand_float < pp) {
                            state_arr[neigh] = SI_STATE;   //改变活动节点的邻居neigh的状态为下一步激活
                            si_arr[si_size++] = neigh;
                        }
                    }
                }
            }
            for(int i = 0; i < i_size; i++){    //i即当前轮节点激活过程结束，变成过去激活节点
                state_arr[i_arr[i]] = R_STATE;
                r_arr[r_size++] = i_arr[i];
            }
            i_size = 0;
            for(int i = 0; i < si_size; i++){  //i+1即刚激活的节点准备去激活其他节点
                state_arr[si_arr[i]] = I_STATE;
                i_arr[i_size++] = si_arr[i];
            }
        }
        inf += active_size;
    }
    delete[] i_arr;
    delete[] r_arr;
    delete[] si_arr;
    delete[] state_arr;
    delete[] rand_arr;
    return inf / NUM_SIMUS;
}
