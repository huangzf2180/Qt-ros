#include "pathplanning.h"

PathPlanning::PathPlanning()
{
    map = Map::getInstance();
}

vector<Vertex> PathPlanning::getOriPath(){
    cout << "get origin path" << endl;

    QImage img = map->getImg();

    int width = img.width();
    int height = img.height();

    cout << "map_width = " << width << " map_height = " << height << endl;

    vector<Point> obstacleScanPath = getObstacleScanPath(img);

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

    for (vector<Point>::iterator it = obstacleScanPath.begin(); it != obstacleScanPath.end(); it++)
    {
        map_array[(*it).x][(*it).y] = 50;
    }

    vertexVec.clear();
    int vertex_id = 0;

    for (int i = left; i <= right; i++)
    {
        for (int j = top; j <= bottom; j++){
            if (map_array[i][j] == 50)
            {
                for (int x = i - ROBOT_RADIUS * 1 > 0 ? i - ROBOT_RADIUS * 1 : 0; x <= (i + ROBOT_RADIUS * 1 < width ? i + ROBOT_RADIUS * 1 : width - 1); x++)
                {
                    for (int y = j - ROBOT_RADIUS * 1 > 0 ? j - ROBOT_RADIUS * 1 : 0; y <= (j + ROBOT_RADIUS * 1 < height ? j + ROBOT_RADIUS * 1 : height - 1); y++)
                    {
                        if (map_array[x][y] == 50)
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


    delete graph;

    return pathVec;
}
