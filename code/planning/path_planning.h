#ifndef PATH_PLANNING_H_
#define PATH_PLANNING_H_

#include "../QueueArray/QueueArray.h"
#include "../StackArray/StackArray.h"
#include "../Point/Point.h"

#define DRY_RUN_MODE 0
#define NAVIGATION_MODE 1
#define BOX_SEARCH_MODE 2
#define GRAB_MODE 3


extern StackArray<Point> stack;
void initSearch();
void doBFS(Point from, Point to);
void goToPlannedPath();
void goToNeighbourCoordinate(Point go,unsigned short mode);
void changeOrientation(short int new_orientation);


void planBoxSearchPath();

void findBestSearchPath(Point current);

void doNodeCovering(Point from);

void printPlannedPath();

void goToPlannedBoxSearchPath();

void goToNeighbourOfLast();

void doSpiralDryRun();

void loadEEPROM();

extern bool ringNode;

#endif /*PATH_PLANNING_H_*/
