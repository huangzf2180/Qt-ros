#include"common.h"

vector<int> makePathByGreedy(double** graph, bool vis[], int cur_index, int vertex_count){
    cout << "make path by greedy" << endl;

    int next_index;
    vis[cur_index] = true;
    vector<int> indexVec;
    while(true){
        double min_dis = 10e9;
        for (int i = 0; i < vertex_count; i++)
        {
            next_index = -1;
            if(vis[i]) continue;
            if(graph[cur_index][i] < min_dis){
                min_dis = graph[cur_index][i];
                next_index = i;
            }
        }
        if(next_index == -1)
            return indexVec;
        indexVec.push_back(next_index);
        vis[next_index] = true;
        cur_index = next_index;
    }
    return indexVec;
}
