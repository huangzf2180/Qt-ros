#ifndef MAP_H
#define MAP_H

#include<QImage>
#include<QPixmap>

class Map
{
public:
    ~Map();
    void setImg(QImage);
    QImage getImg();
    void setPixmap(QPixmap *);
    QPixmap* getPixmap();




    static Map* getInstance();

private:
    Map();

    static Map* map;
    QImage img;
    QPixmap* pixmap;
    int** map__array;
};

#endif // MAP_H
