#include "Arduino.h"
#include "NetworkSearch.h"

#include <QueueList.h>

NetworkSearch::NetworkSearch(){

}

void NetworkSearch::BFS(Segment **coreSegments, uint16_t coreSegAm){
	QueueList <Segment *> queue;
	
	for(int i=0; i<coreSegAm; i++){
		coreSegments[i]->visited = true;
		coreSegments[i]->parent = NULL;
		queue.push(coreSegments[i]);
	}
	
	
	while(!queue.isEmpty ()){
		Segment *currentSegment = queue.pop();
		
		for(int i=0; i<currentSegment->connAm; i++){
			Segment *nextSegment = currentSegment->connSeg[i];
			if(!nextSegment->visited){
				nextSegment->visited = true;
				nextSegment->parent = currentSegment;
				queue.push(nextSegment);
			}
		}
		Serial.println(queue.count());
	}
}

void NetworkSearch::animPath(Segment *startSeg){
	Segment *currentSegment = startSeg;
	Color effectColor(0,0,100,HSB_MODE);
	Color outColor(0,0,0,RGB_MODE);
	
	while(currentSegment != NULL){
		currentSegment->setFadeInOut(effectColor,outColor,4,1);
		currentSegment = currentSegment->parent;
	}
	
}