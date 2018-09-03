#include <iostream>
#include <algorithm>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include<vector>

using namespace std;

unsigned seed=(unsigned)time(0);//原型：void srand(unsigned seed);

#define ANT_NUM 50   //蚁群数量
#define TMAC 30   //迭代最大次数
#define ROU 0.5     //误差大小
#define ALPHA 1     // 信息素重要程度的参数
#define BETA 50     // 启发式因子重要程度的参数
#define Q 10000 //信息素残留参数
const int maxn = 1000;
int vertex_count;   //节点数量
double dis[maxn][maxn];        //距离
double info[maxn][maxn];  //信息素矩阵
double Bestlength;
const double mmax = 10e9;
bool visited[maxn];
int curVertexIndex;
int visitedVertexCount;

//返回指定范围内的随机整数
int rnd(int start,int end)
{
    return start + rand() % (end - start);
}

//返回指定范围内的随机浮点数
double rnd(double start,double end)
{
    double dbTemp=rand()/((double)RAND_MAX+1.0);
    return start+dbTemp*(end-start);
}
//返回浮点数四舍五入取整后的浮点数
double ROUND(double dbA)
{
    return (double)((int)(dbA+0.5));
}

struct Ant
{

    int Path[maxn];  //蚂蚁走的路径
    double length;  //路径总长度
    int vis[maxn]; //走过城市标记
    int cur_cityno;   //当前城市
    int moved_cnt;    //已走的数量
    //初始化
    void Init()
    {
        //        cout << "ant init" << endl;
        memset(vis, 0, sizeof(vis));
        for(int i = 0; i < vertex_count; i++){
            vis[i] = (visited[i] == true ? 1 : 0);
        }
        length = 0;
        cur_cityno = curVertexIndex;
        moved_cnt = 0;
        Path[moved_cnt] = cur_cityno;
        vis[cur_cityno] = 1;
        moved_cnt = 1;
    }
    //选择下一个城市
    //返回值 为城市编号
    int chooseNextCity()
    {
        int nSelectedCity=-1; //返回结果，先暂时把其设置为-1
        //计算当前城市和没去过的城市之间的信息素总和
        double dbTotal=0.0;
        double prob[vertex_count]; //保存各个城市被选中的概率
        for(int i = 0; i < vertex_count; i++)
        {
            if (!vis[i])
            {
                prob[i]=pow(info[cur_cityno][i],ALPHA)
                        *pow(1.0/dis[cur_cityno][i], BETA);
                dbTotal += prob[i];
            }
            else
            {
                prob[i] = 0;
            }
        }
        //进行轮盘选择
        double dbTemp=0.0;
        if (dbTotal > 0.0) //总的信息素值大于0
        {
            dbTemp = rnd(0.0, dbTotal);
            for (int i = 0; i < vertex_count; i++)
            {
                if (!vis[i])
                {
                    dbTemp -= prob[i];
                    if (dbTemp < 0.0)
                    {
                        nSelectedCity = i;
                        break;
                    }
                }
            }
        }
        //如果城市间的信息素非常小 ( 小到比double能够表示的最小的数字还要小 )
        //出现这种情况，就把第一个没去过的城市作为返回结果

        if (nSelectedCity == -1)
        {
            for (int i=0; i<vertex_count; i++)
            {
                if (!vis[i]) //城市没去过
                {
                    nSelectedCity=i;
                    break;
                }
            }
        }
        return nSelectedCity;
    }

    //蚂蚁在城市间移动
    void Move()
    {
        int nCityno = chooseNextCity();//选择下一个城市
        Path[moved_cnt] = nCityno;//保存蚂蚁走的路径
        vis[nCityno] = 1;//把这个城市设置成已经去过
        cur_cityno = nCityno;
        //更新已走路径长度
        length += dis[Path[moved_cnt-1]][Path[moved_cnt]];
        moved_cnt++;

    }
    //蚂蚁进行搜索一次
    void Search()
    {
        cout << "ant search" << endl;
        Init();
        //如果蚂蚁去过的城市数量小于城市数量，就继续移动
        while(moved_cnt < vertex_count - visitedVertexCount)
        {
            Move();
        }
        cout << "move end" << endl;
        //蚂蚁到达终点后返回起始点的距离
        //        length += dis[Path[vertex_count-1]][Path[0]];
    }

};


struct TSP
{
    Ant ants[ANT_NUM];  //定义一群蚂蚁
    Ant ant_best;       //保存最好结果的蚂蚁

    void Init()
    {
        //初始化为最大值
        ant_best.length = mmax;

        //初始化环境信息素
        puts("init info");
        for (int i=0; i<vertex_count; i++)
        {
            for (int j=0; j<vertex_count; j++)
            {
                info[i][j]=1.0;
            }
        }
    }
    //更新信息素,当前每条路上的信息素等于过去保留的信息素
    //加上每个蚂蚁这次走过去剩下的信息素
    void Updateinfo()
    {
        cout << "updateinfo" << endl;
        cout << vertex_count << endl;
        double **tmpinfo;
        tmpinfo = new double*[vertex_count];
        for(int i = 0;i<vertex_count;i++){
            tmpinfo[i] = new double[vertex_count];
        }
        for(int i = 0;i<vertex_count;i++){
            for(int j = 0;j<vertex_count;j++){
                tmpinfo[i][j] = 0;
            }
        }
        cout << "init tmpinfo" << endl;
        int m = 0;
        int n = 0;
        //遍历每只蚂蚁
        for (int i = 0; i < ANT_NUM; i++) {
            for (int j = 1; j < vertex_count - visitedVertexCount; j++)
            {
                m = ants[i].Path[j];
                n = ants[i].Path[j-1];
                tmpinfo[n][m] = tmpinfo[n][m]+Q/ants[i].length;
                tmpinfo[m][n] = tmpinfo[n][m];
                // cout << tmp
            }
            //最后城市和开始城市之间的信息素
            // n = ants[i].Path[0];
            // tmpinfo[n][m] = tmpinfo[n][m]+Q/ants[i].length;
            // tmpinfo[m][n] = tmpinfo[n][m];
        }

        //更新环境信息素
        for (int i = 0; i < vertex_count; i++)
        {
            for (int j = 0; j < vertex_count; j++) {
                //最新的环境信息素 = 留存的信息素 + 新留下的信息素
                info[i][j] = info[i][j]*ROU + tmpinfo[i][j];

            }
        }
    }
    //寻找路径，迭代TMAC次
    void Search()
    {
        cout << "search" << endl;
        for (int i = 0; i < TMAC; i++) {
            cout << "current iteration times: " << i << endl;
            for (int j = 0; j < ANT_NUM; j++) {
                ants[j].Search();
            }

            //保存最佳结果
            for (int j = 0; j < ANT_NUM; j++) {
                if (ant_best.length > ants[j].length) {
                    ant_best = ants[j];
                }
            }

            //更新环境信息素
            Updateinfo();

            cout << "current minimum length" << endl;;
        }
    }

};



vector<int> makePathByAnt(double** graph, bool vis[], int curIndex, int count)
{
    visitedVertexCount = 0;
    for(int i = 0; i < count; i++){
        visited[i] = vis[i];
        if(visited[i]) visitedVertexCount++;
        for(int j = 0; j < count; j++){
            dis[i][j] = graph[i][j];
        }
    }

    cout << "print path" << endl;

    vertex_count = count;
    curVertexIndex = curIndex;

    vector<int> vec;
    srand(seed);
    TSP tsp;
    //初始化蚁群
    tsp.Init();
    //开始查找
    tsp.Search();
    puts("The Minimum length route is :\n");
    for (int i = 0; i < vertex_count - visitedVertexCount; i++) {
        printf("%d ", tsp.ant_best.Path[i]);
        vec.push_back(tsp.ant_best.Path[i]);
    }

    return vec;
}
