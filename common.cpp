#include<iostream>
#include<QImage>
#include"common.h"

using std::cout;
using std::endl;

int **getOriginMap(QImage img){
    int width = img.width();
    int height = img.height();
    int **map;
    map = new int*[width];
    for(int i=0;i<width;i++){
        map[i] = new int[height];
    }
    for (int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++){
            QRgb rgb = img.pixel(i, j);
            map[i][j] = qGray(rgb);
        }
    }

    return map;
}

int **getVaildMap(QImage img){
    int width = img.width();
    int height = img.height();
    int **map;
    map = getOriginMap(img);
    Rect rect = getVaildMapRect(map, width, height);
    int left = rect.left;
    int top = rect.top;
    int right = rect.right;
    int bottom = rect.bottom;
    for (int i = left; i <= right; i++)
    {
        for (int j = top; j <= bottom; j++)
        {
            if (map[i][j] == 0)
            {
                for (int x = i - ROBOT_RADIUS >= 0 ? i - ROBOT_RADIUS : 0; x <= (i + ROBOT_RADIUS < width ? i + ROBOT_RADIUS : width - 1); x++)
                {
                    for (int y = j - ROBOT_RADIUS >= 0 ? j - ROBOT_RADIUS : 0; y <= (j + ROBOT_RADIUS < height ? j + ROBOT_RADIUS : height - 1); y++)
                    {
                        if (map[x][y] == 255)
                        {
                            int distance = pow(x - i, 2) + pow(y - j, 2);
                            if (distance <= ROBOT_RADIUS * ROBOT_RADIUS)
                            {
                                map[x][y] = 5;
                            }
                        }
                    }
                }
            }
        }
    }

    return map;
}

Rect getVaildMapRect(int **map, int width, int height){
    int left = 10e9, top = 10e9, right = -1, bottom = -1;
    for (int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++){
            if(map[i][j] == 0){
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
    return Rect(left, top, right, bottom);
}

void addData(int **map, int x, int y, Point end, AStarData *parent, vector<AStarData*> &open, bool **close){
    if(map[x][y] == 255){
        if(!close[x][y]){
            for(int i = x - ROBOT_RADIUS / 2; i <= x + ROBOT_RADIUS / 2; i++){
                for(int j = y - ROBOT_RADIUS / 2; j <= y + ROBOT_RADIUS / 2; j++){
                    if(map[i][j] == 5 && pow(i - x, 2) + pow(j - y, 2) <= ROBOT_RADIUS * ROBOT_RADIUS / 4)
                        return;
                }
            }
            AStarData *data = new AStarData();
            data->g = parent->g + 1;
            data->h = abs(x - end.x) + abs(y - end.y);
            data->f = data->g + data->h;
            data->point.x = x;
            data->point.y = y;
            data->parent = parent;
            open.push_back(data);
            close[x][y] = true;
        }
    }
}

vector<Point> AStar(int **map, int width, int height, Point start, Point end){
    int start_x = start.x;
    int start_y = start.y;
    int end_x = end.x;
    int end_y = end.y;
    vector<Point> pointVec;
    vector<AStarData*> open;
    bool **close;
    close = new bool*[width];
    for(int i = 0; i < width; i++){
        close[i] = new bool[height];
    }
    memset(close, false, sizeof(close));
    AStarData *endData;
    AStarData* data = new AStarData();
    data->point.x = start_x;
    data->point.y = start_y;
    data->g = 0;
    data->h = abs(start_x - end_x) + abs(start_y - end_y);
    data->f = data->g + data->h;
    data->parent = NULL;
    open.push_back(data);
    while(true){
        int min_f = 10e9;
        int nextDataIndex = -1;
        for(int i=0;i<open.size();i++){
            if(open.at(i)->f <= min_f){
                min_f = open.at(i)->f;
                nextDataIndex = i;
            }
        }
        AStarData *data = open.at(nextDataIndex);
        int x = data->point.x;
        int y = data->point.y;
        int g = data->g;
        if(x == end_x && y == end_y){
            endData = data;
            break;
        }
        open.erase(open.begin() + nextDataIndex);
        close[x][y] = true;
        addData(map, x - 1, y, end, data, open, close);
        addData(map, x, y + 1, end, data, open, close);
        addData(map, x + 1, y, end, data, open, close);
        addData(map, x, y - 1, end, data, open, close);
        if(open.size() == 0 || data->f > 100){
            endData = NULL;
            break;
        }
    }

    if(endData){
        while(true){
            Point point = endData->point;
            pointVec.push_back(point);
            if(endData->parent != NULL){
                endData = endData->parent;
            }
            else{
                break;
            }
        }
    }

    std::reverse(pointVec.begin(), pointVec.end());

    return pointVec;
}
