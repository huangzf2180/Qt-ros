#include "map.h"


Map* Map::map = NULL;

Map::Map()
{

}

Map* Map::getInstance(){
    if(map == NULL){
        map = new Map();
    }
    return map;
}

void Map::setImg(QImage img){
    this->img = img;
}

QImage Map::getImg(){
    return pixmap->toImage();
}

void Map::setPixmap(QPixmap *pixmap){
    this->pixmap = pixmap;
}

QPixmap* Map::getPixmap(){
    return pixmap;
}
