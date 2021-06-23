/*
 * out.cpp
 *
 *  Created on: May 30, 2020
 *      Author: Neha
 */
#include "common.h"
#include "out.h"

/*void printMap(int nRows, int nCols) {
	getchar();
	clear()
	;

	for (int i = 0; i < nCols; i++) {
		if ((map[0][i]).N == 1)
			printf(" ___");
	}
	printf("\n");
	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nCols; j++) {

			//west
			if ((map[i][j]).W == 1)
				printf("|");
			else
				printf(",");
			//north and south
			if ((map[i][j]).S == 1) {
				if (i == mapCur[0] && j == mapCur[1])
					printf("_x_");
				else
					printf("___");
			} else {
				if (i == mapCur[0] && j == mapCur[1])
					printf(".x.");
				else
					printf("...");
			}
			//east
			if (j == nCols - 1) {
				if ((map[i][j]).E == 1) {
					printf("|");
				} else
					printf(",");
				printf("\n");
			}
		}
	}
}*/

/*void printVisited2() {
	for (int i = 0; i < curRows * curCols; i++) {
		if (i % curCols == 0)
			printf("\n");
		if ((field + i)->visited == 1)
			printf("%-4d ", i);
		else
			printf("     ");
	}
}*/

/*void printVisited(int nRows, int nCols) {
	for (int i = 0; i < nCols; i++) {
		if ((map[0][i]).N == 1)
			printf(" ___");
	}
	printf("\n");
	for (int i = 0; i < nRows; i++) {
		for (int j = 0; j < nCols; j++) {
			//west
			if ((map[i][j]).W == 1)
				printf("|");
			else
				printf(",");
			//north and south
			if ((map[i][j]).S == 1) {
				if ((map[i][j]).visited == 1)
					printf("_x_");
				else
					printf("___");
			} else {
				if ((map[i][j]).visited == 1)
					printf(".x.");
				else
					printf("...");
			}
			//east
			if (j == nCols - 1) {
				if ((map[i][j]).E == 1) {
					printf("|");
				} else
					printf(",");
				printf("\n");
			}
		}
	}
}
*/
