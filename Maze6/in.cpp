/*
 * in.cpp
 *
 *  Created on: May 30, 2020
 *      Author: Neha
 */

#include "in.h"

/*void getDim(int *nRows, int *nCols) {
	FILE *fp;
	fp = fopen(FILE_NAME, "r+");
	if (fp == NULL) {
		exit(EXIT_FAILURE);
	}
	fscanf(fp, "%d %d", nRows, nCols);
	fclose(fp);
}

void getMap(int nRows, int nCols) {
	FILE *fp;
	fp = fopen(FILE_NAME, "r+");
	if (fp == NULL) {
		exit(EXIT_FAILURE);
	}

	//nRows and nCols
	int x, y;
	fscanf(fp, "%d %d", &x, &y);
	fgetc(fp);

	//first row
	fgetc(fp);
	for (int i = 0; i < nCols; i++) {
		for (int j = 0; j < 3; j++) {
			if (fgetc(fp) == '_')
				(map[0][i]).N = 1;
		}
		fgetc(fp);
	}

	for (int i = 0; i < nRows; i++) {
		for (int b = 0; b < nCols; b++) {
			//west
			if (fgetc(fp) == '|') {
				(map[i][b]).W = 1;
				if (b != 0)
					(map[i][b-1]).E = 1;
			}
			//north and south
			char bit;
			for (int j = 0; j < 3; j++) {
				if ((bit = fgetc(fp)) == '_') {
					(map[i][b]).S = 1;
					if (i != nRows - 1)
						(map[i+1][b]).N = 1;
				} else if (bit == 'x') {
					mapCur[0] = i;
					mapCur[1] = b;
				}
			}
			//east
			if (b == nCols - 1) {
				if (fgetc(fp) == '|')
					(map[i][b]).E = 1;
				fgetc(fp);
			}
		}
	}
}*/

int readDist(Directions dir) {
	return 0;
}
