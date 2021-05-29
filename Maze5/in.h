/*
 * in.h
 *
 *  Created on: May 30, 2020
 *      Author: Neha
 */

//#pragma warning(disable:4996)
#ifndef IN_H_
#define IN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _WIN32
#define FILE_NAME "C:\\Users\\Neha\\ws\\mazeWArray\\src\\field.txt"
#else
#define FILE_NAME "/mnt/c/users/Neha/ws/mazeWArray/src/field.txt"
#endif

//void getDim(int *nRows, int *nCols);
//void getMap(int nRows, int nCols);
int readDist(Directions dir);

#endif /* IN_H_ */
