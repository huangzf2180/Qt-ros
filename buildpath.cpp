#include <iostream>
#include <vector>
#include<QImage>
#include "common.h"


using std::cout;
using std::endl;
using std::vector;

vector<int> print_path(double **, int);


// Global Varibals
vector<Vertex> vertexVec;
int map_width;
int map_height;

//获取独立障碍物Vector
vector<ObstacleScanned> getObstacleVec(QImage img)
{
    cout << "getObstacleVec" << endl;
    int width = img.width();
    int height = img.height();
    int map_array[width][height];
    int left = INT_MAX, top = INT_MAX, right = -1, bottom = -1;
    for (int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++){
            QRgb rgb = img.pixel(i, j);
            map_array[i][j] = qGray(rgb);
            if (map_array[i][j] == 0)
            {
                if (left > i)
                    left = i;
                if (top > j)
                    top = j;
                if (right < i)
                    right = i;
                if (bottom < j)
                    bottom = j;
            }
        }
    }

    cout << "rect = " << left << " " << top << " " << right << " " << bottom << endl;

    for (int i = left; i <= right; i++)
    {
        for (int j = top; j <= bottom; j++)
        {
            if (map_array[i][j] == 0)
            {
                for (int x = i - ROBOT_RADIUS >= 0 ? i - ROBOT_RADIUS : 0; x <= (i + ROBOT_RADIUS < width ? i + ROBOT_RADIUS : width - 1); x++)
                {
                    for (int y = j - ROBOT_RADIUS >= 0 ? j - ROBOT_RADIUS : 0; y <= (j + ROBOT_RADIUS < height ? j + ROBOT_RADIUS : height - 1); y++)
                    {
                        if (map_array[x][y] == 255)
                        {
                            int distance = pow(x - i, 2) + pow(y - j, 2);
                            if (distance <= ROBOT_RADIUS * ROBOT_RADIUS)
                            {
                                map_array[x][y] = 75;
                            }
                        }
                    }
                }
            }
        }
    }

    vector<ObstacleScanned> obstacleVec;

    //独立物体编号
    int id = 1000;

    for (int i = left; i <= right; i++)
    {
        for (int j = top; j <= bottom; j++)
        {
            if (map_array[i][j] == 0)
            {
                ObstacleScanned obstacle;
                obstacle.id = id;
                vector<Point> pointVec;
                Point point;
                point.x = i;
                point.y = j;
                pointVec.push_back(point);
                map_array[i][j] += 1;
                for (int k = 0; k < pointVec.size(); k++)
                {
                    int x = pointVec.at(k).x;
                    int y = pointVec.at(k).y;
                    for (int start_i = x - 1 > 0 ? x - 1 : 0; start_i <= (x + 1 < width ? x + 1 : width - 1); start_i++)
                    {
                        for (int start_j = y - 1 > 0 ? y - 1 : 0; start_j <= (y + 1 < height ? y + 1 : height - 1); start_j++)
                        {
                            if (map_array[start_i][start_j] == 0 /*|| map_array[start_i][start_j] == 75*/)
                            {
                                Point point;
                                point.x = start_i;
                                point.y = start_j;
                                pointVec.push_back(point);
                                map_array[start_i][start_j] += 1;
                            }
                        }
                    }
                }

                obstacle.pointVec.assign(pointVec.begin(), pointVec.end());
                obstacleVec.push_back(obstacle);
                id++;
            }
        }
    }

    for (int i = 0; i < obstacleVec.size(); i++)
    {
        ObstacleScanned &obstacle = obstacleVec.at(i);
        for (vector<Point>::iterator it = obstacle.pointVec.begin(); it != obstacle.pointVec.end();)
        {
            cout << obstacle.pointVec.size() << endl;
            if (
                    map_array[(*it).x][(*it).y] == 76 ||
                    map_array[(*it).x][(*it).y] == 1 &&
                    map_array[(*it).x + 1][(*it).y] == 1 &&
                    map_array[(*it).x][(*it).y + 1] == 1 &&
                    map_array[(*it).x - 1][(*it).y] == 1 &&
                    map_array[(*it).x][(*it).y - 1] == 1
                    )
            {
                it = obstacle.pointVec.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

    cout << "obstacleVec size = " << obstacleVec.size() << endl;

    return obstacleVec;
}

//获取独立障碍物的扫描路径
vector<Point> getObstacleScanPath(QImage img)
{
    cout << "getObstacleScanPath" << endl;
    vector<Point> pixelPointVec;

    int width = img.width();
    int height = img.height();
    int **map_array;
    map_array = new int*[width];
    for(int i = 0; i < width; i++){
        map_array[i] = new int[height];
    }
    int left = INT_MAX, top = INT_MAX, right = -1, bottom = -1;
    for (int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++){
            QRgb rgb = img.pixel(i, j);
            map_array[i][j] = qGray(rgb);
            if (map_array[i][j] == 0)
            {
                if (left > i)
                    left = i;
                if (top > j)
                    top = j;
                if (right < i)
                    right = i;
                if (bottom < j)
                    bottom = j;
            }
        }
    }

    vector<ObstacleScanned> obstacleVec = getObstacleVec(img);

    for (int i = left; i <= right; i++)
    {
        for (int j = top; j <= bottom; j++)
        {
            if (map_array[i][j] == 0)
            {
                for (int x = i - ROBOT_RADIUS >= 0 ? i - ROBOT_RADIUS : 0; x <= (i + ROBOT_RADIUS < width ? i + ROBOT_RADIUS : width - 1); x++)
                {
                    for (int y = j - ROBOT_RADIUS >= 0 ? j - ROBOT_RADIUS : 0; y <= (j + ROBOT_RADIUS < height ? j + ROBOT_RADIUS : height - 1); y++)
                    {
                        if (map_array[x][y] == 255)
                        {
                            int distance = pow(x - i, 2) + pow(y - j, 2);
                            if (distance <= ROBOT_RADIUS * ROBOT_RADIUS)
                            {
                                map_array[x][y] = 75;
                            }
                        }
                    }
                }
            }
        }
    }

    vector<Point> pathVec;

    //获取扫描路径
    for (int scan_radius = ROBOT_RADIUS - 1; scan_radius <= SCAN_PIXELS_RADIUS; scan_radius++)
    {
        for (vector<ObstacleScanned>::iterator it = obstacleVec.begin(); it != obstacleVec.end(); it++)
        {
            int id = (*it).id;
            vector<Point> pointVec = (*it).pointVec;
            for (vector<Point>::iterator it_point = pointVec.begin(); it_point != pointVec.end(); it_point++)
            {
                int obstaclePosX = (*it_point).x;
                int obstaclePosY = (*it_point).y;
                for (int i = obstaclePosX - scan_radius > 0 ? obstaclePosX - scan_radius : 0; i <= (obstaclePosX + scan_radius < width ? obstaclePosX + scan_radius : width - 1); i++)
                {
                    for (int j = obstaclePosY - scan_radius > 0 ? obstaclePosY - scan_radius : 0; j <= (obstaclePosY + scan_radius < height ? obstaclePosY + scan_radius : height - 1); j++)
                    {
                        if (map_array[i][j] == 255 /*&& pow(i - obstaclePosX, 2) + pow(j - obstaclePosY, 2) <= scan_radius * scan_radius*/)
                        {
                            map_array[i][j] = id;
                            int x, y;
                            for (x = i - 1 > 0 ? i - 1 : 0; x <= (i + 1 < width ? i + 1 : width - 1); x++)
                            {
                                for (y = j - 1 > 0 ? j - 1 : 0; y <= (j + 1 < height ? j + 1 : height - 1); y++)
                                {
                                    if (map_array[x][y] >= 1000 && map_array[x][y] != id || scan_radius == SCAN_PIXELS_RADIUS && id >= 1000)
                                    {
                                        Point point;
                                        point.x = i;
                                        point.y = j;
                                        pathVec.push_back(point);
                                        break;
                                    }
                                }
                                if (y <= j + 1)
                                    break;
                            }
                        }
                        if(i > (obstaclePosX - scan_radius > 0 ? obstaclePosX - scan_radius : 0) && i < (obstaclePosX + scan_radius < width ? obstaclePosX + scan_radius : width - 1) && j == (obstaclePosY - scan_radius > 0 ? obstaclePosY - scan_radius : 0))
                            j = obstaclePosY + scan_radius;
                    }
                }
            }
        }
    }

    return pathVec;
}

//void getEdgeVec(nav_msgs::OccupancyGrid map, int **map_array, Rect rect)
//{
//    int width = map.info.width;
//    int height = map.info.height;
//    int left = rect.left;
//    int top = rect.top;
//    int right = rect.right;
//    int bottom = rect.bottom;

//    for (int i = top; i <= bottom; i++)
//    {
//        for (int j = left; j <= right; j++)
//        {
//            if (map_array[i][j] == 50)
//            {
//                vector<Point> vec;

//            }
//        }
//    }
//}

void getCurPathVertex(int start, int end, int **middle_vertex, vector<Vertex> &pathVec, vector<Vertex> &vertexVec){
    int k = middle_vertex[start][end];
    //    cout << start << " " << end << " " << k << endl;
    if(start != k){
        getCurPathVertex(start, k, middle_vertex, pathVec, vertexVec);
        getCurPathVertex(k, end, middle_vertex, pathVec, vertexVec);
    }
    else{
        pathVec.push_back(vertexVec.at(end));
    }
}

vector<Vertex> rebuildPath(vector<Vertex> unreachedVertexVec, vector<Vertex> visitedVertexVec, Vertex cur_vertex){
    cout << "rebuildPath" << endl;

    int vertex_count = vertexVec.size();

    double **graph;
    graph = new double *[vertex_count];
    for (int i = 0; i < vertex_count; i++)
    {
        graph[i] = new double[vertex_count];
    }

    int **middle_vertex;
    middle_vertex = new int *[vertex_count];
    for (int i = 0; i < vertex_count; i++)
    {
        middle_vertex[i] = new int[vertex_count];
    }

    for (int i = 0; i < vertex_count; i++)
    {
        graph[i][i] = 0;
        Vertex vertex_1 = vertexVec.at(i);
        int id_1 = vertex_1.id;
        Point point_1 = vertex_1.point;
        for (int j = i + 1; j < vertex_count; j++)
        {
            Vertex vertex_2 = vertexVec.at(j);
            int id_2 = vertex_2.id;
            Point point_2 = vertex_2.point;
            int distance = sqrt(pow(point_1.x - point_2.x, 2) + pow(point_1.y - point_2.y, 2));
            if (distance > ROBOT_RADIUS * 3)
                distance = 10e9;
            graph[id_1][id_2] = distance;
            graph[id_2][id_1] = distance;
            middle_vertex[id_1][id_2] = id_1;
            middle_vertex[id_2][id_1] = id_2;
        }
    }

    for(vector<Vertex>::iterator it = unreachedVertexVec.begin(); it != unreachedVertexVec.end(); it++){
        int vertexId = (*it).id;
        for(int i = 0; i < vertex_count; i++){
            graph[vertexId][i] = 10e9;
            graph[i][vertexId] = 10e9;
        }
    }

    for (int k = 0; k < vertex_count; k++)
    {
        for (int i = 0; i < vertex_count; i++)
        {
            for (int j = 0; j < vertex_count; j++)
            {
                if (graph[i][j] > graph[i][k] + graph[k][j])
                {
                    graph[i][j] = graph[i][k] + graph[k][j];
                    middle_vertex[i][j] = k;
                }
            }
        }
    }

    cout << "start rebuild path" << endl;
    bool vis[vertex_count];
    vector<Vertex> pathVec;
    int count = visitedVertexVec.size();
    int curVertexId = cur_vertex.id;
    //init vis
    memset(vis, false, sizeof(vis));
    for(vector<Vertex>::iterator it = unreachedVertexVec.begin(); it != unreachedVertexVec.end(); it++){
        int vertexId = (*it).id;
        vis[vertexId] = true;
    }
    for(vector<Vertex>::iterator it = visitedVertexVec.begin(); it != visitedVertexVec.end(); it++){
        int vertexId = (*it).id;
        vis[vertexId] = true;
    }
    for(int i = 0; i < vertex_count; i++){
        if(graph[curVertexId][i] >= 10e8){
            vis[i] = true;
        }
    }

    int vis_count = 0;
    for(int i = 0; i < vertex_count; i++){
        if(vis[i]) vis_count++;
    }
    if(vis_count == vertex_count)
        return pathVec;

    pathVec.push_back(vertexVec.at(curVertexId));

    //make path by greedy
    //    while(count < vertex_count){
    //        double min_dis = 10e9;
    //        int index = -1;
    //        for (int i = 0; i < vertex_count; i++)
    //        {
    //            if(vis[i]) continue;
    //            if(graph[curVertexId][i] < min_dis){
    //                min_dis = graph[curVertexId][i];
    //                index = i;
    //            }
    //        }

    //        if(index == -1){
    //            cout << "rebuild pathVec size = " << pathVec.size() << endl;
    //            return pathVec;
    //        }

    //        getCurPathVertex(curVertexId, index, middle_vertex, pathVec, vertexVec);
    //        curVertexId = index;
    //        vis[curVertexId] = true;
    //        count++;
    //    }

    //make path by ant
    vector<int> vec = makePathByAnt(graph, vis, curVertexId, vertex_count);

    int current_index = vec.at(0);
    int next_index;
    for(int i = 1; i < vec.size(); i++){
        next_index = vec.at(i);
        getCurPathVertex(current_index, next_index, middle_vertex, pathVec, vertexVec);
        current_index = next_index;
    }

    delete graph;

    return pathVec;
}

vector<Vertex> get_path(QImage img)
{
    cout << "get path" << endl;

    vertexVec.clear();

    int width = img.width();
    int height = img.height();

    map_width = width;
    map_height = height;

    cout << width << " " << height << endl;

    vector<Point> obstacleScanPath = getObstacleScanPath(img);

    int map_array[width][height];
    int left = INT_MAX, top = INT_MAX, right = -1, bottom = -1;
    for (int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++){
            QRgb rgb = img.pixel(i, j);
            map_array[i][j] = qGray(rgb);
            if (map_array[i][j] < 5)
            {
                map_array[i][j] = 0;
                if (left > i)
                    left = i;
                if (top > j)
                    top = j;
                if (right < i)
                    right = i;
                if (bottom < j)
                    bottom = j;
            }
        }
    }

    for (vector<Point>::iterator it = obstacleScanPath.begin(); it != obstacleScanPath.end(); it++)
    {
        map_array[(*it).x][(*it).y] = 4;
    }

    int vertex_id = 0;

    for (int i = left; i <= right; i++)
    {
        for (int j = top; j <= bottom; j++){
            if (map_array[i][j] == 4)
            {
                for (int x = i - ROBOT_RADIUS * 1 > 0 ? i - ROBOT_RADIUS * 1 : 0; x <= (i + ROBOT_RADIUS * 1 < width ? i + ROBOT_RADIUS * 1 : width - 1); x++)
                {
                    for (int y = j - ROBOT_RADIUS * 1 > 0 ? j - ROBOT_RADIUS * 1 : 0; y <= (j + ROBOT_RADIUS * 1 < height ? j + ROBOT_RADIUS * 1 : height - 1); y++)
                    {
                        if (map_array[x][y] == 4)
                            map_array[x][y] = 0;
                    }
                }
                Vertex vertex;
                Point point;
                point.x = i;
                point.y = j;
                vertex.id = vertex_id++;
                vertex.point = point;
                vertexVec.push_back(vertex);
            }
        }
    }

    int vertex_count = vertexVec.size();
    cout << "vertex count: " << vertexVec.size() << endl;
    double **graph;
    graph = new double *[vertex_count];
    for (int i = 0; i < vertex_count; i++)
    {
        graph[i] = new double[vertex_count];
    }

    int **middle_vertex;
    middle_vertex = new int *[vertex_count];
    for (int i = 0; i < vertex_count; i++)
    {
        middle_vertex[i] = new int[vertex_count];
    }

    cout << "make graph" << endl;

    for (int i = 0; i < vertex_count; i++)
    {
        graph[i][i] = 0;
        Vertex vertex_1 = vertexVec.at(i);
        int id_1 = vertex_1.id;
        Point point_1 = vertex_1.point;
        for (int j = i + 1; j < vertex_count; j++)
        {
            Vertex vertex_2 = vertexVec.at(j);
            int id_2 = vertex_2.id;
            Point point_2 = vertex_2.point;
            int distance = sqrt(pow(point_1.x - point_2.x, 2) + pow(point_1.y - point_2.y, 2));
            if (distance > ROBOT_RADIUS * 2.5)
                distance = 10e9;
            graph[id_1][id_2] = distance;
            graph[id_2][id_1] = distance;
            middle_vertex[id_1][id_2] = id_1;
            middle_vertex[id_2][id_1] = id_2;
        }
    }

    for (int k = 0; k < vertex_count; k++)
    {
        for (int i = 0; i < vertex_count; i++)
        {
            for (int j = 0; j < vertex_count; j++)
            {
                if (graph[i][j] > graph[i][k] + graph[k][j])
                {
                    graph[i][j] = graph[i][k] + graph[k][j];
                    middle_vertex[i][j] = k;
                }
            }
        }
    }

    cout << "start get path" << endl;

    vector<Vertex> pathVec;
    bool vis[vertex_count];
    memset(vis, false, sizeof(vis));
    int count = 1;
    int current_index;
    int next_index;

    //makePathByAnt
    vector<int> indexVec = makePathByAnt(graph, vis, 0, vertex_count);
    current_index = indexVec.at(0);
    next_index = -1;
    for(int i = 1; i < indexVec.size(); i++){
        next_index = indexVec.at(i);
        getCurPathVertex(current_index, next_index, middle_vertex, pathVec, vertexVec);
        current_index = next_index;
    }

    //makePathByGreedy
    //    vis[current_index] = true;
    //    pathVec.push_back(vertexVec.at(current_index));

    //    while(count < vertex_count){
    //        double min_dis = 10e9;
    //        int index = -1;
    //        for (int i = 0; i < vertex_count; i++)
    //        {
    //            if(vis[i]) continue;
    //            if(graph[current_index][i] < min_dis){
    //                min_dis = graph[current_index][i];
    //                index = i;
    //            }
    //        }

    //        getCurPathVertex(current_index, index, middle_vertex, pathVec, vertexVec);
    //        current_index = index;
    //        vis[current_index] = true;
    //        count++;
    //    }

    delete graph;

    return pathVec;
}
