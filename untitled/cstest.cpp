//
// Created by 16848 on 2019/12/10.
//
/*
随机选择一个节点，基于传播概率反向随机游走，记录游走过节点的次数。
选出次数最高的k个节点作为种子节点。
与RIS反向抽样类似的游走，但是不记录抽样样本，只记录游走经过的节点次数。
游走次数以指数来增加，基于两次选择的节点交集是否重合，重合说明稳定，结束游走。
排序用堆选出最高的k个节点，同时考虑相似性，排除相似性太高的种子节点
2019.1.20
yuanjingliang
被B遍历，visit=1，被A遍历,visit=2，否则visit=0
*/
//成功概率p表示系统感染概率，节点被感染阈值则随机选择，阈值越大越不易激活。若p大于阈值，激活
//概率影响过程  prioximity算法得重新写
#include "Utility.h"
#include "Graph.h"
#include "MemoryUsage.h"

#include<stdlib.h>
#include<string.h>
#include<string>
#include<stdio.h>
#include<vector>

#include<sstream>
#include<queue>
#include <windows.h>
#include <psapi.h>

#pragma comment(lib,"psapi.lib")


#define S_STATE 0
#define I_STATE 1
#define SI_STATE 2
#define R_STATE 3

#define p 0.5
#define CLOCKS_PER_SEC ((clock_t)1000)

using namespace std;



void showMemory(void){
    HANDLE handle=GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(handle,&pmc,sizeof(pmc));
    cout<<"use Memory:"<<pmc.WorkingSetSize/1000 <<"K/"<<pmc.PeakWorkingSetSize/1000<<"K + "<<pmc.PagefileUsage/1000 <<"K/"<<pmc.PeakPagefileUsage/1000 <<"K"<<endl;
}


float SIMILARITY = 0;        //相似性
struct Pair
{
    int key;
    int value;
    Pair(int key, int value) :key(key), value(value) {};
};
typedef struct Pair Pair;
//运算符重载，根据value的值来排序
bool operator > (Pair a, Pair b)
{
    return a.value < b.value;
}



string data="C:\\Users\\16848\\CLionProjects\\lProject\\untitled\\tntp2.txt";
Graph *g = new Graph(data);
//int *aai=new int[g->num_nodes];     //存储根据B的游走选出的
void parseArg(int argn, char ** argv);
void run(int c,int s,int model,int anode);
//float mc_influence(Graph *g, int *seed_arr, int k);
//bool coincidence(int *seed_tmp, int *seed_arr, int s, int k);
void diffusion(int ka,int *aai);
void select(int cc,int t_pair);
int distributor1(int ss,int ii,int ccon,int nodenum);
int distributor2(int ss,int ii,int ccon,int nodenum);
int distributor3(int ss,int ii,int ccon,int nodenum);
int distributor4(int ss,int ii,int ccon,int nodenum);
int distributor5(int ss,int ii,int ccon,int nodenum);
priority_queue<Pair, vector<Pair>, greater<Pair> > tmp_pqueue;//节点在游走中经过的次数，升序队列
//priority_queue<Pair, vector<Pair>, greater<Pair> > tmp_pqueue1;
queue<int> bseed;//节点的度;  //把B种子压栈
queue<int> ql;   //游走过程中广度优先遍历存储    定义queue对象
queue<int>degree;
//string data="C:\\Users\\liqian\\Desktop\\CV IBM\\netscience";
//Graph *g = new Graph(data);

int *walk_arr = new int[g->num_nodes]; //存储游走经过节点的次数        *
int *aseed_arr = new int[20];  //存储本次迭代选出的种子节点     *
int *aseed_num = new int[20];  //存储种子节点游走中经过的次数    *
int *ai_arr = new int[g->num_nodes]; //the array of current active nodes   *
int ai_size = 0; // the # of newly active nodes
int *ar_arr = new int[g->num_nodes]; // the array of previous active nodes  *
int ar_size = 0; // the # of previously active nodes
int *at_arr = new int[g->num_nodes];  // the array of nodes to be active in t+1*
int at_size = 0; // the # of nodes to be active in t+1
int *state_arr = new int[g->num_nodes]; // the state of nodes  *
int *arand_arr = new int[g->num_nodes]; //the 0 ~ n-1 numbers sorted by random order  *
int *bi_arr = new int[g->num_nodes]; //the array of current active nodes          *
int bi_size = 0; // the # of newly active nodes         *
int *br_arr = new int[g->num_nodes]; // the array of previous active nodes  *
int br_size = 0; // the # of previously active nodes
int *bt_arr = new int[g->num_nodes];  // the array of nodes to be active in t+1 *
int bt_size = 0; // the # of nodes to be active in t+1
time_t time_start;
//int *bstate_arr = new int[g->num_nodes]; // the state of nodes *
int *visit = new int[g->num_nodes];  //一次游走中记录节点是否被访问过
double aactive_size = 0;     //第二次循环时不再执行此语句
double bactive_size = 0;

//int *ai=new int[g->num_nodes];     //存储根据B的游走选出的


/*bool coincidence(int *seed_tmp, int *seed_arr, int s, int k)
{
    int *tmp_s = new int[s]; //比较两个数组相似性，空间换时间
	int cover = 0; //记录重合的节点个数
	bool flag1 = true;
	for(int i=0; i<s; i++)
    {
		tmp_s[i] = 0;
	}
	for(int i=0; i<k; i++)
	{
		tmp_s[seed_tmp[i]] = 1;   //当前被选为种子的节点标记为1
	}
	for(int i=0; i<k; i++)
	{
		if(tmp_s[seed_arr[i]]==1)
        {    //对上一轮种子节点进行循环，如果其节点为1，证明两轮其都被选为种子
			cover++;
	    }
	}
	if(cover/k >=0.98)
	{   //重合度大于98%停止迭代
		flag1 = false;
	}

	return flag1;
}
*/

int main(int argn, char ** argv)
{
    cout << "Program Start at: " << currentTimestampStr() << endl;
    cout << "Arguments: ";
    memset(state_arr, S_STATE, g->num_nodes * sizeof(int)); // initialize the state array 未激活
    //  memset(bstate_arr, S_STATE, g->num_nodes * sizeof(int)); // initialize the state array
    for(int i = 0; i < argn; i++)
    {
        cout << argv[i]<<" ";
    }
    cout << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    parseArg(argn,argv);
    cout << "--------------------------------------------------------------------------------" << endl;
    cout<<"Program Ends Successfully at: " << currentTimestampStr() << endl;
    delete[] walk_arr;
    delete[] bi_arr;
    delete[] visit;
    delete[] ai_arr;
    delete[] ar_arr;
    delete[] at_arr;
    delete[] state_arr;
    delete[] arand_arr;
    delete[] br_arr;
    delete[] bt_arr;
    //delete[] bstate_arr;
    delete[] aseed_arr;
    delete[] aseed_num;
   // disp_mem_usage();

    showMemory();
    cout << "Time used: " << time(NULL) - time_start << " s" << endl;
    cout << "Time used: " << clock()/CLOCKS_PER_SEC << " ms" << endl;
    return 0;
}

void parseArg(int argn, char ** argv)
{
    // the parameters
    //   string data="E:\\data\\NetHEPT.txt";  // the path of the dataset
    int c=0,s=0,anode=0;  //the # of seeds to be found ,a种子数量
    int model=0;

    for(int i=0; i<argn; i++)
    {
        if(argv[i] == string("-c"))   //几步邻居之中选
            c=atoi(argv[i+1]);
        if(argv[i] == string("-s"))    //走几步，也即是时间限制
            s=atoi(argv[i+1]);        //atoi:字符串转换成整型数
        if(argv[i] == string("-model"))
            model=atoi(argv[i+1]);  //atof：字符串转换成浮点数，可能是相似性衡量标准
        if(argv[i] == string("-anode"))   //
            anode=atoi(argv[i+1]);       //可能是游走次数
    }
    if (c==0)
        ExitMessage("c argument  missing");
    if(s == 0)
        ExitMessage("s argument  missing");
    if(model== 0)
        ExitMessage("model argument  missing");
    if(anode == 0)
        ExitMessage("argument anode missing");
    cout << "graph " << data << "was built successfully!" << endl;
    run(c,s,model,anode);    //k表示A种子数量
}

void run(int c,int s,int model,int anode)    //随机游走函数
{
    cout << "--------------------------------------------------------------------------------" << endl;
//	time_t time_start = time(NULL);
    cout << "No.\tpositive_influence\tnegative_influence\ttime(s)" << endl;
    time_start = time(NULL);
    srand((unsigned)time(NULL));
    int node,ka,num,con=0;             //存储选出的种子个数
    int *ai=new int[g->num_nodes];  //存储根据B的游走选出的
    bi_size = 20; // the # of newly active nodes
    int m=g->num_nodes;
    srand((unsigned)time(NULL));//srand函数是随机数发生器的初始化函数
    for(int i=0; i<g->num_nodes; i++)
    {   //对每个节点初始化
        walk_arr[i] = 0;
        visit[i] = 0;   //标记
    }
    for(int i=0;i<bi_size;i++)
    {
        node=rand()%m;  //给B种子赋初值
        visit[node] = 1;    //标记
        //bseed.push(node);
        bi_arr[i]=node;
        bactive_size++;
    }
    for(int k=0;k<s;k++)  //对步数的循环
    {
        switch(model)
        {
            case 1: num=distributor1(s,k,con,anode);con=con+num;break;
            case 2: num=distributor2(s,k,con,anode);con=con+num;break;
            case 3: num=distributor3(s,k,con,anode);con=con+num;break;
            case 4: num=distributor4(s,k,con,anode);con=con+num;break;
            case 5: num=distributor5(s,k,con,anode);con=con+num;break;
        }
        //       ai=select(c,num,bi_arr,bi_size);
        //select(c,num,bi_arr,bi_size);
        for(int i=0; i<bi_size; i++)
        {
            bseed.push(bi_arr[i]);
            state_arr[bi_arr[i]] = I_STATE;
        }
        cout<<endl;
        int t_pair;
        for(int i=0; i<bi_size; i++)
        {
            t_pair=bseed.front();
            bseed.pop();
            select(c,t_pair);
        }
        for(int i = 0; i < g->num_nodes; i++)
        {
            Pair m_pair(i,walk_arr[i]);
            tmp_pqueue.push(m_pair);  //用来存储此节点被经过了几次
            // tmp_pqueue1.push(m_pair);
        }
/*
        cout<<"****";
        for(int i=0;i<30;i++)
        {
            Pair s_pair1 = tmp_pqueue1.top();
            tmp_pqueue1.pop();
            cout<<s_pair1.key<<" ";
        }
        cout<<"****"<<endl;*/
        for(int i=0; i<num; i++)//选k个种子节点
        {

            Pair s_pair = tmp_pqueue.top();
            tmp_pqueue.pop();
            while(state_arr[s_pair.key]!=S_STATE)
            {
                s_pair = tmp_pqueue.top();
                tmp_pqueue.pop();
            }
            aseed_arr[i] = s_pair.key;  //把此节点加入种子数组
            aseed_num[i] = s_pair.value;   //存储此种子被经过了几次
            cout<<aseed_arr[i]<<" ";
        }
        cout<<endl;
        ka=num;   //需要选的种子节点数目
        cout <<k+1<< "\t\t";
        ai=aseed_arr;
        diffusion(ka,ai);
    }
    delete[] ai;
}


void select(int cc,int t_pair)   //选出at_arr，即下一步可以选为种子节点的，然后把选出的加入正在活动节点
//结束条件多样化，可以考虑基于网络规模直接给出,或者指数增加游走次数
{
    int k_out,*point,k_out1,*point1;
    if(cc==1)
    {
        k_out1=g->node_array[t_pair].k_out;
        point1= g->node_array[t_pair].id_array;
        for(int i=0;i<k_out1;i++)
        {
            walk_arr[point1[i]]++;
        }

    }
    else if(cc>1)
    {
        k_out=g->node_array[t_pair].k_out;
        point= g->node_array[t_pair].id_array;
        for(int j=0;j<k_out;j++)
        {
            t_pair=point[j];
            select(cc-1,t_pair);
        }
        //cc--;
    }
}

void diffusion(int ka,int *aai)
{
    srand((unsigned)time(NULL));
//	memset(astate_arr, S_STATE, g->num_nodes * sizeof(int)); // initialize the state array 未激活
//	memset(bstate_arr, S_STATE, g->num_nodes * sizeof(int)); // initialize the state array
    // initialize the seed nodes
    for(int i = 0; i < ka; i++)              //初始化种子节点
    {
        ai_arr[ai_size++] = aai[i];       //当前活动的种子节点被赋值作为种子节点,加上上一轮循环选出的激活节点，成为新的激活种子
        state_arr[ai_arr[i]] = I_STATE;        // 改变种子节点的状态 ，已激活
        aactive_size++;
    }
    //权重级联模型
    at_size = 0;                      //t+1活动节点为0
    //randomOrder(arand_arr, ai_size);       //rand_arr为k个种子数组 ，i_size是当前活动节点数量
    for(int i = 0; i < ai_size; i++)         //对当前活动节点的循环， 激活活动节点的邻居，计算t+1时刻激活的节点。
        //即完成当前活动节点在下一步对邻居的激活
    {
        int ai_node = ai_arr[i];                    //把第i个活动节点赋值给本循环中要用到的变量i-node
        int ak_out = g->node_array[ai_node].k_out;     //第i个活动节点的出度赋值给k_out
        for(int j = 0; j < ak_out; j++)              //对第i个活动节点的邻居进行循环
        {
            int neigh = g->node_array[ai_node].id_array[j];    // neigh表示为第i个活动节点的第j个邻居
            if (state_arr[neigh] == S_STATE)                    //如果此邻居是未激活的
            {
                int k_in = g->node_array[neigh].k_in;           //则把此邻居的入度赋值给k_in
                // double pp = 1.0 / k_in;  //激活概率为入度的倒数  重力级联模型
                double pp = p;      //独立级联模型   pp是成功概率
                double rand_float = ((double)rand()) / RAND_MAX;//生成0到1之间的随机数 表示激活概率
                if(rand_float < pp)                           //激活概率小于成功概率，则表示激活
                {
                    state_arr[neigh] = SI_STATE;               //改变活动节点的邻居neigh的状态为下一步激活，实质的扩散
                    at_arr[at_size++] = neigh;                   //加入到下一步激活的数组里，si_size只记录每一次扩散t+1时刻激活的num
                }
            }
        }
    }
    for(int i = 0; i < ai_size; i++)                 //加入到过去激活的数组里
    {    //i即当前轮节点激活过程结束，变成过去激活节点
        state_arr[ai_arr[i]] = R_STATE;          //设置节点状态为激活过
        ar_arr[ar_size++] = ai_arr[i];               // 加入到先前激活的数组中
    }
    ai_size = 0;                    //t时刻活动数量为0
    for(int i = 0; i < at_size; i++)     //对t+1时刻活动节点的遍历
    {  //i+1即刚激活的节点准备去激活其他节点
        state_arr[at_arr[i]] = I_STATE;   //设置状态为激活
        ai_arr[ai_size++] = at_arr[i];     //加入到现在激活的数组中
        aactive_size++;
    }
    //      ainf += aactive_size;
    /////////////////////////////////////////////////////b扩散
    // initialize the seed nodes
    bt_size = 0;                      //t+1活动节点为0
//    randomOrder(brand_arr, bi_size);       //rand_arr为k个种子数组 ，i_size是当前活动节点数量
    for(int i = 0; i < bi_size; i++)         //对当前活动节点的循环， 激活活动节点的邻居，计算t+1时刻激活的节点。
        //即完成当前活动节点在下一步对邻居的激活
    {
        int bi_node = bi_arr[i];                    //把第i个活动节点赋值给本循环中要用到的变量i-node
        int bk_out = g->node_array[bi_node].k_out;    //第i个活动节点的出度赋值给k_out
        for(int j = 0; j < bk_out; j++)              //对第i个活动节点的邻居进行循环
        {
            int neigh = g->node_array[bi_node].id_array[j];    // neigh表示为第i个活动节点的第j个邻居
            if (state_arr[neigh] == S_STATE)                    //如果此邻居是未激活的
            {
                int k_in = g->node_array[neigh].k_in;           //则把此邻居的入度赋值给k_in
                //double pp = 1.0 / k_in;  //激活概率为入度的倒数  重力级联模型
                double pp = p;      //独立级联模型   pp是成功概率
                double rand_float = ((double)rand()) / RAND_MAX;//生成0到1之间的随机数 表示激活概率
                if(rand_float <pp)                           //激活概率小于成功概率，则表示激活
                {
                    state_arr[neigh] = SI_STATE;               //改变活动节点的邻居neigh的状态为下一步激活，实质的扩散
                    bt_arr[bt_size++] = neigh;                   //加入到下一步激活的数组里，si_size只记录每一次扩散t+1时刻激活的num
                }
            }
        }
    }
    for(int i = 0; i < bi_size; i++)                 //加入到过去激活的数组里
    {    //i即当前轮节点激活过程结束，变成过去激活节点
        state_arr[bi_arr[i]] = R_STATE;          //设置节点状态为激活过
        br_arr[br_size++] = bi_arr[i];               // 加入到先前激活的数组中
    }
    bi_size = 0;                    //t时刻活动数量为0
    for(int i = 0; i < bt_size; i++)     //对t+1时刻活动节点的遍历
    {  //i+1即刚激活的节点准备去激活其他节点
        state_arr[bt_arr[i]] = I_STATE;   //设置状态为激活
        bi_arr[bi_size++] = bt_arr[i];     //加入到现在激活的数组中
        bactive_size++;
    }
    //   bactive_size+=b_size;
    cout << aactive_size << "\t\t\t" <<bactive_size << "\t\t\t" << currentTimestampStr() <<endl;
    cout<<"*******************************************************************************************"<<endl;
}

int distributor1(int ss,int ii,int ccon,int nodenum)
{
    if (ii==0) return nodenum;
    else return 0;
}
int distributor2(int ss,int ii,int ccon,int nodenum)
{
    int dis;
    dis=nodenum/ss;
    return dis;
}
int distributor3(int ss,int ii,int ccon,int nodenum)    //按照等差数列递增
{
    int dis,d;
    d=(2*(nodenum-ss))/(ss*(ss-1));
    if (ii==0)  return 1;
    else
    {
        dis=1+(ii*d);
        return dis;
    }
}
int distributor4(int ss,int ii,int ccon,int nodenum)
{
    int dis;
    dis=(2*(nodenum-ccon))/(ss-ii);
    return dis;
}
int distributor5(int ss,int ii,int ccon,int nodenum)
{
    int dis;
    dis=nodenum/3;
    if (ii==0) return dis;
    else if((ii+1)==(ss/2)) return dis;
    else if(ii==(ss-1))   return dis;
    else return 0;
}



//Compute the influence spread using Mento-Carlo simulation
/*float mc_influence(Graph *g, int *seed_arr, int k)
{
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
	for(int r = 0; r < NUM_SIMUS; r++)
    {
		double active_size = 0;
		//reset the state of all nodes
		for(int i = 0; i < r_size; i++)
		{
			state_arr[r_arr[i]] = S_STATE;
		}
		r_size = 0;
		// initialize the seed nodes
		for(int i = 0; i < k; i++)
		{
			i_arr[i_size++] = seed_arr[i];
			state_arr[i_arr[i]] = I_STATE;
		}
		while(i_size > 0)
		{   //权重级联模型
			active_size += i_size;
			si_size = 0;
			randomOrder(rand_arr, i_size);
			for(int i = 0; i < i_size; i++)
			{
				int i_node = i_arr[i];
				int k_out = g->node_array[i_node].k_out;
				for(int j = 0; j < k_out; j++)
				{
					int neigh = g->node_array[i_node].id_array[j];
					if (state_arr[neigh] == S_STATE)
					{
						int k_in = g->node_array[neigh].k_in;
					//	double pp = 1.0 / k_in;  //激活概率为入度的倒数
						double pp = P;      //独立级联模型
						double rand_float = ((double)rand()) / RAND_MAX;//生成0到1之间的随机数
						if(rand_float < pp)
						{
							state_arr[neigh] = SI_STATE;
							si_arr[si_size++] = neigh;
						}
					}
				}
			}
			for(int i = 0; i < i_size; i++)
			{    //i即当前轮节点激活过程结束，变成过去激活节点
				state_arr[i_arr[i]] = R_STATE;
				r_arr[r_size++] = i_arr[i];
			}
			i_size = 0;
			for(int i = 0; i < si_size; i++)
			{  //i+1即刚激活的节点准备去激活其他节点
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
*/

