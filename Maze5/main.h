/*
 * main.h
 *
 *  Created on: May 30, 2020
 *      Author: Neha
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "in.h"
#include "out.h"

void turnRight90();
void turnLeft90();
void turn180();
void turn(Directions target);
bool goForward(int ntiles);
void readTile();
void victim();


#endif /* MAIN_H_ */
