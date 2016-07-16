/* 
 * File:   Point.cpp
 * Author: Toshiba
 * 
 * Created on December 9, 2012, 10:28 AM
 */

#include "Point.h"
#include<stdlib.h>


Point::Point() {
    
    this->y = -1;
    this->x = -1;

}

Point::Point(int y, int x) {
    this->y = y ;
    this->x = x;


}

int Point::similar(Point p2){

    return (p2.y == this->y) && (p2.x == this->x) ? 1 : 0;

    

}

Point::~Point() {

  
    
}

