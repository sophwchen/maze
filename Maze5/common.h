/*
 * bfs.h
 *
 *  Created on: May 30, 2020
 *      Author: Neha
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char ubyte;
struct tile {
	ubyte N :1;
	ubyte E :1;
	ubyte S :1;
	ubyte W :1;
	ubyte visited :1;
};

enum Directions {
	N, E, S, W
};

struct node {
	struct node* prev;
	int value[2];
	struct node* next;
};

//extern tile map[10][10];
extern tile field[20][20];
extern int curR, curC;
//extern int mapCur[2];
extern tile *start;
extern Directions curDir;
extern int distTotal;
extern int rotsTotal;

void simpleTraverse();
void toStart();
node* enq(node *wq, node* tailNode, int curR, int curC);
node* deq(node *wq);
bool bfs();

#endif /* COMMON_H_ */
