#include "path_planning.h"
#include "../Motion/motion.h"
#include "../Robot/robot.h"
#include "../Memory/memory_saved.h"
#include "../EEPROM/EEPROM.h"

StackArray<Point> stack;
QueueArray<Point> queue;

bool visited[5][4]; // visited[y][x] == true if visited. 
Point order[5][4]; // order[y][x] -- > parent point

bool hasCovered = 0;

bool covered[5][4]; // covered[y][x]

unsigned short robotOrientationMatrix[5][4]; // orientations when robot visit each nodes..

short int dxOrientations[] =  {0,1,0,-1};
short int dyOrientations[] = {1,0,-1,0};

/*bool edgeData[] = {1,1,1,1,
				   1,1,1,1,
				   1,1,1,1,
				   1,1,1,1,
				   1,1,1,1};*/

//start of seraching and related functions
short int stack_size = 0;

short int dirX[] = {1,-1,0,0};
short int dirY[] = {0,0,1,-1};

#define GRID_Y 4
#define GRID_X 3

bool found = 0;

bool ringNode = 0;

Point defaultPoint(-1,-1);

void loadEEPROM(){
	loadNodeData(edgeData);
	ringNode = EEPROM.read(20);
}

void initSearch(){

  while(!queue.isEmpty()){
      queue.pop();
  }
  found = 0;

  while(!stack.isEmpty()){
      stack.pop();
  }

  stack_size = 0;

  Point p(-1,-1);

  for(int i = 0; i < 6; i++){
    for(int j = 0; j < 7; j++){
      visited[i][j] = 0;
      order[i][j] = p ;
    }
  }

  found = 0;
 // Serial.println(" search initeddd..");
  
  

} //end of inilizing steps

short int i = 0;

//0,1
bool aValidPoint(Point check){
	if(check.x < 0 || check.x > GRID_X){
		return 0;
	}
	if(check.y < 0 || check.y > GRID_Y){
		return 0;
	}
	return edgeData[check.y * GRID_SIZE_X + check.x] == 1;
	
}
/////////
Point current, before;
short currentDx, currentDy;
void doBFS(Point from, Point to){
	
	visited[from.y][from.x] = 1; // mark as visited.
	order[from.y][from.x] = defaultPoint;
	queue.push(from); // enqueue to queue.
	Serial.println("BFS started....");
	
	found = 0;
	while(!queue.isEmpty() && !found){
		Serial.println("===============");
		Serial.println("In While, POPED...");
		current = queue.pop();
		before = order[current.y][current.x];

		
		//new
		if(before.y != -1){
			currentDx = current.x - before.x;
			currentDy = current.y - before.y;
			
			dirY[0] = currentDy;
			dirX[0] = currentDx;
			
			dirY[1] = currentDy == 0 ? 1 : 0;
			dirX[1] = currentDx == 0 ? 1 : 0;
			
			dirY[2] = currentDy == 0 ? -1 : 0;
			dirX[2] = currentDx == 0 ? -1 : 0;
			
			dirY[3] = -currentDy;
			dirX[3] = -currentDx;
			
		} // end of new
		for(i = 0; i < 4; i++){
			Point localPoint(current.y + dirY[i], current.x + dirX[i]);
			Serial.println("Searching for points...");
			if(aValidPoint(localPoint) && !visited[localPoint.y][localPoint.x]){
				Serial.println("Push....");
				visited[localPoint.y][localPoint.x] = 1;
				queue.push(localPoint);
				order[localPoint.y][localPoint.x] = current;
				
				if(localPoint.similar(to)){
					Serial.println("Search finished...");
					found = 1;
					
					
				}
				
			}
		}
		
	}
	
	// initializg backtracking stack
	
	if(found){
		//Serial.println("No error...");
		Point back = to;;
		while(back.similar(from) == 0 && back.similar(defaultPoint) == 0 ){
			stack.push(back);
			back = order[back.y][back.x];
			
		}
		
		
	}
	
	
}
///////

void changeOrientation(short int new_orientation){
	
	short int new_orientation1 =  new_orientation - robotOrientation;
	
	switch(new_orientation1){
		 case 1:
		 case -3: turnRight(); break;
		 case 3:
		 case -1:turnLeft1(); break;
		 case 2:
		 case -2:
		 		turnBack(); break;
		 default: ;
	}
	robotOrientation = new_orientation;
	
}
///////////
short int dxLocal, dyLocal;
void goToNeighbourCoordinate(Point go, unsigned short mode){
	dxLocal = go.x - robotX;
	dyLocal = go.y - robotY;
	//changing orientation
	if(dxLocal == 1){
		changeOrientation(2);
		correctSensorRotation();
	}else if(dxLocal == -1){
		changeOrientation(4);
		correctSensorRotation();
	}else if(dyLocal == 1){
		changeOrientation(1);
		correctSensorRotation();
	}else if(dyLocal == -1){
		changeOrientation(3);
		correctSensorRotation();
	}
	
	if(mode == GRAB_MODE){
		
		return;
	}
	
	robotX += dxLocal ;
	robotY += dyLocal;
	if(mode == NAVIGATION_MODE){	
		moveForwardOneJunction();
	}else if(mode == DRY_RUN_MODE){
		moveForwardOneJunctionForDryRun();
	}else if(mode == BOX_SEARCH_MODE){
		moveForwardOneJunctionForBoxSearch();
	}
	
	
	
	//Serial.print("==== Moving: ");Serial.print(robotY); Serial.print("--");Serial.println(robotX);
	
	
}

short localOrientation;
void doSpiralDryRun(){
	
	Point next;
	visited[0][0] = 1;
	Serial.println("Spiral run initiated....");
	
	for(i = 0; i < GRID_SIZE_X * GRID_SIZE_Y-1; i++){
		next.x  = robotX + dxOrientations[robotOrientation - 1];
		next.y  = robotY + dyOrientations[robotOrientation - 1];
		
		//Serial.print("=== Before changed.. : ");Serial.print(next.y);Serial.print("--");Serial.println(next.x);
				
		if(visited[next.y][next.x] || next.x > GRID_X || next.y > GRID_Y || next.x < 0 || next.y < 0){
			localOrientation = robotOrientation + 1; 
			if(localOrientation > 4){
				localOrientation = 1;
			}
			//Serial.println("=== Orientation changed.. "+ localOrientation);
			next.x = robotX + dxOrientations[localOrientation - 1];
			next.y = robotY + dyOrientations[localOrientation - 1];
		}
		visited[next.y][next.x] = 1;
		
		//Serial.print("Coordinate : ");Serial.print(next.y);Serial.print("--");Serial.println(next.x);
		goToNeighbourCoordinate(next,DRY_RUN_MODE);
		
	}
	
}


void checkCovered(){
	
	hasCovered = 1;
	
	for(short m = 0; m < GRID_SIZE_Y; m++){
		for(short n = 0; n < GRID_SIZE_X; n++){
			if(covered[m][n] == 0){
				hasCovered = 0;
				return ;
			}
		}
	}
	
	
//	Serial.print("Covering .. ");Serial.println(covered[4][0]);
	
}


short bestCoverage = 0;
Point finallyBestCovered ;

void calculateCovered(Point lastC) {

	//total_coverage = 0;
	short total_coverage = 0;

	for (short m = 0; m < GRID_SIZE_Y; m++) {
		for (short n = 0; n < GRID_SIZE_X; n++) {
			if (covered[m][n] == 1 ) {
				total_coverage++;
				//return;
			}
		}
	}
	
	if(bestCoverage <= total_coverage){
		bestCoverage = total_coverage;
		finallyBestCovered.y = lastC.y;
		finallyBestCovered.x = lastC.x;
	}

}





short yCoverage[] = {0,0,1,-1};
short xCoverage[] = {1,-1,0,0};

//Ex: direction = 1, x = 0 --> sensor is 1
uint8_t directionCoverageMap[4][4] = {{1,2,0,3}, 
		                      {0,3,2,1},
		                      {2,1,3,0},
		                      {3,0,1,2}};


void clearPreviousCovered(Point on){
	Point covering;
		short d = 0;
		
		Point chk(4,0);
		
		covered[on.y][on.x] = 1;
		for(d = 0; d < 4; d++){
			if(directionCoverageMap[ robotOrientationMatrix[on.y][on.x] -1][d] != 3){
				covering.y =on.y + yCoverage[d];
				covering.x = on.x + xCoverage[d];
				if(aValidPoint(covering)){
					
					
					
					covered[covering.y][covering.x] = 1;
				}
				// second
				covering.y =on.y + 2*yCoverage[d];
				covering.x = on.x + 2*xCoverage[d];
						if(aValidPoint(covering)){
							covered[covering.y][covering.x] = 1;
						}
			}
		}
		

}

void coverNode(Point on){
	Point covering;
	short d = 0;
	
	Point chk(4,0);
	
	covered[on.y][on.x] = 1;
	for(d = 0; d < 4; d++){
		if(directionCoverageMap[ robotOrientationMatrix[on.y][on.x] -1][d] != 3){
			covering.y =on.y + yCoverage[d];
			covering.x = on.x + xCoverage[d];
			if(aValidPoint(covering)){
				
				
				
				covered[covering.y][covering.x] = 1;
			}
			// second
			covering.y =on.y + 2 * yCoverage[d];
			covering.x = on.x + 2 * xCoverage[d];
					if(aValidPoint(covering)){
							
						covered[covering.y][covering.x] = 1;
					}
		}
	}
	
	
}


void planSearchPath(Point from){
		
		// 
		short r,c;
		for(r = 0; r < GRID_SIZE_Y; r++){
			for(c = 0; c < GRID_SIZE_X; c++){
				if(edgeData[r * GRID_SIZE_Y + c] == 0){
					covered[r][c] = 1;
				}
			}
		}
		//end of initing 
		
	
	
	
		visited[from.y][from.x] = 1; // mark as visited.
		covered[from.y][from.x] = 1;
		coverNode(from);
		order[from.y][from.x] = defaultPoint;
		queue.push(from); // enqueue to queue.
		//
		
		
		
		
		
		//Serial.println("BFS started....");
		hasCovered = 0;
		while(! queue.isEmpty() && ! hasCovered ){
			//Serial.println("In While, POPED...");
			current = queue.pop();
			
			coverNode(current);
			
			before = order[current.y][current.x];

			
			//new
			if(before.y != -1){
				currentDx = current.x - before.x;
				currentDy = current.y - before.y;
				
				dirY[0] = currentDy;
				dirX[0] = currentDx;
				
				dirY[1] = currentDy == 0 ? 1 : 0;
				dirX[1] = currentDx == 0 ? 1 : 0;
				
				dirY[2] = currentDy == 0 ? -1 : 0;
				dirX[2] = currentDx == 0 ? -1 : 0;
				
				dirY[3] = -currentDy;
				dirX[3] = -currentDx;
				
			} // end of new
			for(i = 0; i < 4; i++){
				Point localPoint(current.y + dirY[i], current.x + dirX[i]);
				//Serial.println("Searching for points...");
				if(aValidPoint(localPoint) && !visited[localPoint.y][localPoint.x]){
					//Serial.println("Push....");
					visited[localPoint.y][localPoint.x] = 1;
					covered[localPoint.y][localPoint.x] = 1;
										
					
					queue.push(localPoint);
					order[localPoint.y][localPoint.x] = current;
					
					
					
				}
			}
			
		}
	
	
	
	
	
	
}


void planBoxSearchPath(){
	Point from(0,0), to;
	for(int r = GRID_SIZE_Y - 1; r >= 0; r--){
		for(int c = GRID_SIZE_X - 1; c >=  0; c--){
			
			if(edgeData[r * GRID_SIZE_X + c] == 1){
				to.y = r; to.x = c;
				initSearch();
				doBFS(from, to);
				return;
			}
			
		}
	}
	
	
}

void printPlannedPath(){
	
	
	Point a;
	while(!stack.isEmpty() && ! boxFound){
		a = stack.pop();	
		Serial.print(a.y);Serial.print(", ");Serial.println(a.x);
	}
	
	
}

void goToPlannedBoxSearchPath(){
	Point a;
	while(!stack.isEmpty() && ! boxFound  ){
		//a = stack.pop();	
		//Serial.print(a.y);Serial.print(", ");Serial.println(a.x);
		//robotY = a.y;
		//robotX = a.x;
		goToNeighbourCoordinate(stack.pop(), BOX_SEARCH_MODE);
	
	}
	
	if(boxFound){
		Serial3.println("Box found..");
	}
}



void goToPlannedPath(){
	Point a;
	while(!stack.isEmpty()){
		//a = stack.pop();
		//Serial.print(a.y); Serial.print(" = ");Serial.println(a.x);
		goToNeighbourCoordinate(stack.pop(), NAVIGATION_MODE);
	}
	
}

void goToNeighbourOfLast(){
	
	Point a;
	while(!stack.isEmpty()){
		a = stack.pop();
		if(!stack.isEmpty()){
			goToNeighbourCoordinate(a, NAVIGATION_MODE); 
		}
	}
	
	
	goToNeighbourCoordinate(a, GRAB_MODE);
	
	correctSensorRotation();
}


/////////////////////////////////////////////////////////////////////

void clearCoverage(){
	
	for(int r = 0; r < GRID_SIZE_Y ; r++){
		for(int c = 0; c < GRID_SIZE_X; c++){
			
			covered[r][c] = 0;
			
			if(edgeData[r * GRID_SIZE_X + c] == 0){
				covered[r][c] = 1;
			}
			
		}
		covered[0][0] = 1;
	}
	
}

//start, last
void coverWholePath(Point s, Point l){
	clearCoverage();
	Point b = l;
	
	coverNode(l);
	visited[l.y][l.x] = 1;
	
	while(b.similar(s) == 0 && b.similar(defaultPoint) == 0 ){
				
		coverNode(b);		
		visited[b.y][b.x] = 1;
		b = order[b.y][b.x];
				
				
	}
	
	coverNode(s);
	visited[s.y][s.x]= 1;
	
	
	
	
}


void clearVisited(){
	
	for(int r  = 0; r < GRID_SIZE_Y; r++){
		for(int c = 0; c < GRID_SIZE_X; c++){
			visited[r][c] = 0;
		}
	}
	
	
}


void doNodeCovering(Point from){
	
	visited[from.y][from.x] = 1; // mark as visited.
	order[from.y][from.x] = defaultPoint;
	queue.push(from); // enqueue to queue.
	
	robotOrientationMatrix[from.y][from.x] = robotOrientation; // begin.
	
	hasCovered = 0;
	
	Serial.println("Node Covering started.");
	
	short visitingOrientation;
	
	Point last;
	
	
	
	found = 0;
	while(!queue.isEmpty() && !hasCovered){
	//	Serial3.println("===============");
		//Serial.println("In While, POPED...");
		current = queue.pop();
		
		//Serial.print("covering to : "); Serial.print(current.y);
		//Serial.print(", ");Serial.println(current.x);
		
		clearVisited();
		coverWholePath(from, current);
		
		before = order[current.y][current.x];

		
		//new
	/*if(before.y != -1){
			currentDx = current.x - before.x;
			currentDy = current.y - before.y;
			
			dirY[0] = currentDy;
			dirX[0] = currentDx;
			
			dirY[1] = currentDy == 0 ? 1 : 0;
			dirX[1] = currentDx == 0 ? 1 : 0;
			
			dirY[2] = currentDy == 0 ? -1 : 0;
			dirX[2] = currentDx == 0 ? -1 : 0;
			
			dirY[3] = -currentDy;
			dirX[3] = -currentDx;
			
			/*
			if(currentDy == 1){
				dirY[0] = 1; dirY[1] = 0; dirY[2]  = 0; dirY[3] = -1;
				dirX[0] = 0; dirX[1] = 1; dirX[2]  = -1; dirX[3] = 0;
			}else if(currentDy == -1){
				dirY[0] = -1; dirY[1] = 0; dirY[2]  = 0; dirY[3] = 1;
				dirX[0] = 0; dirX[1] = 1; dirX[2]  = -1; dirX[3] = 0;
			}else if(currentDx == 1){
				dirY[0] = 0; dirY[1] = 1; dirY[2]  = -1; dirY[3] = 0;
				dirX[0] = 1; dirX[1] = 0; dirX[2]  = 0; dirX[3] = -1;
			
			}else if(currentDx == -1){
				dirY[0] = 0; dirY[1] = -1; dirY[2]  = 1; dirY[3] = 0;
				dirX[0] = -1; dirX[1] = 0; dirX[2]  = 0; dirX[3] = 1;
			}
			
			*/
	
	//} */ // end of new*/
		for(i = 0; i < 4; i++){
			Point localPoint(current.y + dirY[i], current.x + dirX[i]);
			//Serial.print("Searching for points @ ");
			//Serial.print(current.y);Serial.print(", "); Serial.println(current.x);
			
			//Serial.print(localPoint.y);Serial.print(", ");Serial.print(localPoint.x);
			//Serial.print(" == > "); Serial.println(aValidPoint(localPoint));
			
			
			if(aValidPoint(localPoint) && !visited[localPoint.y][localPoint.x]){
			//	Serial.println("Push....");
				visited[localPoint.y][localPoint.x] = 1;
				queue.push(localPoint);
				order[localPoint.y][localPoint.x] = current;
				
				//update orientations...
								
				visitingOrientation = 1;
				switch(localPoint.y - current.y){
					
				case 1: visitingOrientation = 1; break;
				case -1: visitingOrientation = 3; break;
				default :  break;
					
				}
				
				switch(localPoint.x - current.x){
									
								case 1: visitingOrientation = 2; break;
								case -1: visitingOrientation = 4; break;
								default : break;
									
				}
				robotOrientationMatrix[localPoint.y][localPoint.x] = visitingOrientation;
				
				
				
		//		coverNode(localPoint);
				
				clearVisited();
				coverWholePath(from, localPoint);
							
				
				
				checkCovered();
				
				calculateCovered(localPoint);
				
				if(hasCovered){
				//Serial.println("Covered=======================");
					last = localPoint;
					break;
					
				}
				
				
				
			}
		}
		
	}
	
	// initializg backtracking stack
	
	
	if(hasCovered){
		//	Serial.println("Backtracking....");
			Point back = last;
			while(back.similar(from) == 0 && back.similar(defaultPoint) == 0 ){
				stack.push(back);
				back = order[back.y][back.x];
				
			}
	}
	else{
		Point back = finallyBestCovered;
					while(back.similar(from) == 0 && back.similar(defaultPoint) == 0 ){
						stack.push(back);
						back = order[back.y][back.x];
						
	}
	}
}
