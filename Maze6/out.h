/*
 * out.h
 *
 *  Created on: May 30, 2020
 *      Author: Neha
 */

#ifndef OUT_H_
#define OUT_H_

#include "common.h"

#ifdef _WIN32
#define clear() system("CLS");
#else
#define clear() system("clear");
#endif

/*void printMap(int nRows, int nCols);

void printVisited2();

void printVisited(int nRows, int nCols);*/

#endif /* OUT_H_ */
