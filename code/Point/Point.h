/* 
 * File:   Point.h
 * Author: Toshiba
 *
 * Created on December 9, 2012, 10:28 AM
 */

#ifndef POINT_H
#define	POINT_H

class Point {
public:
    Point();
    Point(int y, int x);
    virtual ~Point();
    int similar(Point p2);
    int y;
    int x;

};

#endif	/* POINT_H */

