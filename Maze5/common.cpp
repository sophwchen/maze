#include <Arduino.h>
#include "common.h"
#include "in.h"
#include "out.h"
#include "robotIn.h"
#include "robotOut.h"
#include "main.h"

//tile map[MSide][MSide] = { 0 };
int MSide = 20;
tile field[20][20] = { 0 };
int curR = 9, curC = 9;
//int mapCur[2];
tile *start;
Directions curDir = N;
int distTotal = 0;
int rotsTotal = 0;

void simpleTraverse() {
  Directions dir = N;
  bool success;
  for (int fails = 0; fails < 4;) {
    switch (dir) {
      case N: {
          if (field[curR][curC].N == 0) {
            if (curR == 0) {
              printf("out of bounds");
              while (true)
                ;
            }
            if ((field[curR - 1][curC]).visited == 0) {
              //mapCur[0]--;
              curR--;
              if (curDir != N) {
                turn(N);
                success = goForward(750);
              }
              else {
                success = goForward(850);
              }
              //printMap(nRows, nCols);
              readTile();
              if (!success) {
                field[curR][curC].N = 1;
                field[curR][curC].E = 1;
                field[curR][curC].S = 1;
                field[curR][curC].W = 1;
                if (curR) field[curR - 1][curC].S = 1;
                if (curR != MSide) field[curR + 1][curC].N = 1;
                if (curC) field[curR][curC - 1].E = 1;
                if (curC != MSide) field[curR][curC + 1].W = 1;
                curR++;
              }
              else {
                fails = 0;
                break;
              }
            }
          }
          fails++;
        }
      case E: {
          dir = E;
          if ((field[curR][curC]).E == 0) {
            if (curC == MSide - 1) {
              printf("out of bounds");
              while (true)
                ;
            }
            if ((field[curR][curC + 1]).visited == 0) {
              //mapCur[1]++;
              curC++;
              if (curDir != E) {
                turn(E);
                success = goForward(750);
              }
              else {
                success = goForward(850);
              }
              //printMap(nRows, nCols);
              readTile();
              if (!success) {
                field[curR][curC].N = 1;
                field[curR][curC].E = 1;
                field[curR][curC].S = 1;
                field[curR][curC].W = 1;
                if (curR) field[curR - 1][curC].S = 1;
                if (curR != MSide) field[curR + 1][curC].N = 1;
                if (curC) field[curR][curC - 1].E = 1;
                if (curC != MSide) field[curR][curC + 1].W = 1;
                curC--;
              }
              else {
                fails = 0;
                break;
              }
            }
          }
          fails++;
        }
      case S: {
          dir = S;
          if (field[curR][curC].S == 0) {
            if (curR == MSide - 1) {
              printf("out of bounds");
              while (true)
                ;
            }
            if ((field[curR + 1][curC]).visited == 0) {
              //mapCur[0]++;
              curR++;
              if (curDir != S) {
                turn(S);
                success = goForward(750);
              }
              else {
                success = goForward(850);
              }
              //printMap(nRows, nCols);
              readTile();
              if (!success) {
                field[curR][curC].N = 1;
                field[curR][curC].E = 1;
                field[curR][curC].S = 1;
                field[curR][curC].W = 1;
                if (curR) field[curR - 1][curC].S = 1;
                if (curR != MSide) field[curR + 1][curC].N = 1;
                if (curC) field[curR][curC - 1].E = 1;
                if (curC != MSide) field[curR][curC + 1].W = 1;
                curR--;
              }
              else {
                fails = 0;
                break;
              }
            }
          }
          fails++;
        }
      case W: {
          dir = W;
          if (field[curR][curC].W == 0) {
            if (curC == 0) {
              printf("out of bounds");
            }
            if (field[curR][curC - 1].visited == 0) {
              //mapCur[1]--;
              curC--;
              if (curDir != W) {
                turn(W);
                success = goForward(750);
              }
              else {
                success = goForward(850);
              }
              //printMap(nRows, nCols);
              readTile();
              if (!success) {
                field[curR][curC].N = 1;
                field[curR][curC].E = 1;
                field[curR][curC].S = 1;
                field[curR][curC].W = 1;
                if (curR) field[curR - 1][curC].S = 1;
                if (curR != MSide) field[curR + 1][curC].N = 1;
                if (curC) field[curR][curC - 1].E = 1;
                if (curC != MSide) field[curR][curC + 1].W = 1;
                curC++;
              }
              else {
                fails = 0;
                break;
              }
            }
          }
          fails++;
          dir = N;
        }
    }
  }
}

node* enq(node *wq, node* tailNode, int curR, int curC) {
  node *newNode;
  newNode = (node*) malloc(sizeof(node));
  if (newNode == 0) {
    printf("\nerror\n");
    return 0;
  }

  memset(newNode, 0, sizeof(node));
  newNode->value[0] = curR;
  newNode->value[1] = curC;

  if (wq != 0) { //not empty list
    tailNode->next = newNode;
    newNode->prev = tailNode;
  }

  return newNode;
}

node* deq(node *wq) {
  if (wq->next == 0)
    return 0;
  node *headNode = wq->next;
  free(wq);
  headNode->prev = 0;
  return headNode;
}

bool bfs() {
  node *wq = 0;
  node *tailNode = 0;

  int pq[400];
  memset(&pq, -1, sizeof(int) * 400);

  int search[2] = { curR, curC };
  wq = enq(wq, tailNode, curR, curC);
  tailNode = wq;

  pq[search[0] * MSide + search[1]] = search[0] * MSide + search[1];

  while (true) {
    if ((field[search[0]][search[1]]).N == 0) {
      if (search[0] == 0) {
        printf("out of bounds");
        while (true)
          ;
      } else if ((field[search[0] - 1][search[1]]).visited == 0) {
        search[0]--;
        break;
      } else if (pq[(search[0] - 1) * MSide + search[1]] == -1) {
        tailNode = enq(wq, tailNode, search[0] - 1, search[1]);
        pq[(search[0] - 1) * MSide + search[1]] = (wq->value[0]) * MSide
            + wq->value[1];
      }
    }

    if ((field[search[0]][search[1]]).E == 0) {
      if (search[1] == MSide - 1) {
        printf("out of bounds");
        while (true)
          ;
      } else if ((field[search[0]][search[1] + 1]).visited == 0) {
        search[1]++;
        break;
      } else if (pq[search[0] * MSide + search[1] + 1] == -1) {
        tailNode = enq(wq, tailNode, search[0], search[1] + 1);
        pq[search[0] * MSide + search[1] + 1] = (wq->value[0]) * MSide
                                                + wq->value[1];
      }
    }

    if ((field[search[0]][search[1]]).S == 0) {
      if (search[0] == MSide - 1) {
        printf("out of bounds");
        while (true)
          ;
      } else if ((field[search[0] + 1][search[1]]).visited == 0) {
        search[0]++;
        break;
      } else if (pq[(search[0] + 1) * MSide + search[1]] == -1) {
        tailNode = enq(wq, tailNode, search[0] + 1, search[1]);
        pq[(search[0] + 1) * MSide + search[1]] = (wq->value[0]) * MSide
            + wq->value[1];
      }
    }

    if ((field[search[0]][search[1]]).W == 0) {
      if (search[1] == 0) {
        printf("out of bounds");
        while (true)
          ;
      } else if ((field[search[0]][search[1] - 1]).visited == 0) {
        search[1]--;
        break;
      } else if (pq[search[0] * MSide + search[1] - 1] == -1) {
        tailNode = enq(wq, tailNode, search[0], search[1] - 1);
        pq[search[0] * MSide + search[1] - 1] = (wq->value[0]) * MSide
                                                + wq->value[1];
      }
    }

    wq = deq(wq);
    if (wq == 0)
      return 0;

    search[0] = wq->value[0];
    search[1] = wq->value[1];
  }
  pq[search[0] * MSide + search[1]] = (wq->value[0]) * MSide + wq->value[1];

  //create path
  int path[400];
  int ipath = 0;
  memset(&path, -1, 400);
  path[ipath] = search[0] * MSide + search[1];
  while (path[ipath] != (curR * MSide + curC)) {
    ipath++;
    path[ipath] = pq[path[ipath - 1]];
  }
  ipath--;

  bool success;

  //follow path
  while (path[ipath] != search[0] * MSide + search[1]) {
    if ((curR * MSide + curC) - path[ipath] == MSide) {
      //mapCur[0]--;
      if (curDir != N) {
        turn(N);
        success = goForward(750);
      }
      else {
        success = goForward(850);
      }
    } else if ((curR * MSide + curC) - path[ipath] == (-1 * MSide)) {
      //mapCur[0]++;
      if (curDir != S) {
        turn(S);
        success = goForward(750);
      }
      else {
        success = goForward(850);
      }
    } else if ((curR * MSide + curC) - path[ipath] == 1) {
      //mapCur[1]--;
      if (curDir != W) {
        turn(W);
        success = goForward(750);
      }
      else {
        success = goForward(850);
      }
    } else if ((curR * MSide + curC) - path[ipath] == -1) {
      //mapCur[1]++;
      if (curDir != E) {
        turn(E);
        success = goForward(750);
      }
      else {
        success = goForward(850);
      }
    }
    curR = path[ipath] / MSide;
    curC = path[ipath] % MSide;
    //printMap(nRows, nCols);
    ipath--;
  }
  if (curR - search[0] == 1) {
    //mapCur[0]--;
    if (curDir != N) {
      turn(N);
      success = goForward(750);
    }
    else {
      success = goForward(850);
    }
  } else if (curR - search[0] == -1) {
    //mapCur[0]++;
    if (curDir != S) {
      turn(S);
      success = goForward(750);
    }
    else {
      success = goForward(850);
    }
  } else if (curC - search[1] == 1) {
    //mapCur[1]--;
    if (curDir != W) {
      turn(W);
      success = goForward(750);
    }
    else {
      success = goForward(850);
    }
  } else if (curC - search[1] == -1) {
    //mapCur[1]++;
    if (curDir != E) {
      turn(E);
      success = goForward(750);
    }
    else {
      success = goForward(850);
    }
  }
  curR = search[0];
  curC = search[1];
  readTile();

  if (!success) {
    field[curR][curC].N = 1;
    field[curR][curC].E = 1;
    field[curR][curC].S = 1;
    field[curR][curC].W = 1;
    if (curR) field[curR - 1][curC].S = 1;
    if (curR != MSide) field[curR + 1][curC].N = 1;
    if (curC) field[curR][curC - 1].E = 1;
    if (curC != MSide) field[curR][curC + 1].W = 1;
    if (curDir == N) curR--;
    if (curDir == E) curC--;
    if (curDir == S) curR++;
    if (curDir == W) curC++;
  }

  return 1;
}

void toStart() {
  node *wq = 0;
  node *tailNode = 0;

  int pq[400];
  memset(&pq, -1, sizeof(int) * 400);

  int search[2] = { curR, curC };
  wq = enq(wq, tailNode, curR, curC);
  tailNode = wq;

  pq[search[0] * MSide + search[1]] = search[0] * MSide + search[1];

  while (true) {
    if (field[search[0]][search[1]].N == 0) {
      if ((search[0] - 1) == 9) {
        search[0]--;
        break;
      } else if (pq[(search[0] - 1) * MSide + search[1]] == -1) {
        tailNode = enq(wq, tailNode, search[0] - 1, search[1]);
        pq[(search[0] - 1) * MSide + search[1]] = (wq->value[0]) * 20
            + wq->value[1];
      }
    }

    if (field[search[0]][search[1]].E == 0) {
      if ((search[1] + 1) == 9) {
        search[1]++;
        break;
      } else if (pq[search[0] * MSide + search[1] + 1] == -1) {
        tailNode = enq(wq, tailNode, search[0], search[1] + 1);
        pq[search[0] * MSide + search[1] + 1] = (wq->value[0]) * 10
            + wq->value[1];
      }
    }

    if (field[search[0]][search[1]].S == 0) {
      if ((search[0] + 1) == 9) {
        search[0]++;
        break;
      } else if (pq[(search[0] + 1) * 10 + search[1]] == -1) {
        tailNode = enq(wq, tailNode, search[0] + 1, search[1]);
        pq[(search[0] + 1) * 10 + search[1]] = (wq->value[0]) * 10
            + wq->value[1];
      }
    }

    if (field[search[0]][search[1]].W == 0) {
      if ((field[search[0]][search[1] - 1]).visited == 0) {
        search[1]--;
        break;
      } else if (pq[search[0] * 10 + search[1] - 1] == -1) {
        tailNode = enq(wq, tailNode, search[0], search[1] - 1);
        pq[search[0] * 10 + search[1] - 1] = (wq->value[0]) * 10
            + wq->value[1];
      }
    }

    wq = deq(wq);

    search[0] = wq->value[0];
    search[1] = wq->value[1];
  }

  pq[search[0] * 10 + search[1]] = (wq->value[0]) * 10 + wq->value[1];

  //create path
  int path[200];
  int ipath = 0;
  memset(&path, -1, 200);
  path[ipath] = search[0] * 10 + search[1];
  while (path[ipath] != (curR * 10 + curC)) {
    ipath++;
    path[ipath] = pq[path[ipath - 1]];
  }
  ipath--;

  //follow path
  while (path[ipath] != search[0] * 10 + search[1]) {
    if ((curR * 10 + curC) - path[ipath] == 10) {
      if (curDir != N) {
        turn(N);
        goForward(750);
      } else
        goForward(850);
    } else if ((curR * 10 + curC) - path[ipath] == (-1 * 10)) {
      if (curDir != S) {
        turn(S);
        goForward(750);
      } else
        goForward(850);
    } else if ((curR * 10 + curC) - path[ipath] == 1) {
      if (curDir != W) {
        turn(W);
        goForward(750);
      } else
        goForward(850);
    } else if ((curR * 10 + curC) - path[ipath] == -1) {
      if (curDir != E) {
        turn(E);
        goForward(750);
      } else
        goForward(850);
    }
    curR = path[ipath] / 10;
    curC = path[ipath] % 10;
    ipath--;
  }
  if (curR - search[0] == 1) {
    if (curDir != N) {
      turn(N);
      goForward(750);
    }
    goForward(850);
  } else if (curR - search[0] == -1) {
    if (curDir != S) {
      turn(S);
      goForward(750);
    }
    goForward(850);
  } else if (curC - search[1] == 1) {
    if (curDir != W) {
      turn(W);
      goForward(750);
    }
    goForward(850);
  } else if (curC - search[1] == -1) {
    if (curDir != E) {
      turn(E);
      goForward(750);
    }
    goForward(850);
  }
  curR = search[0];
  curC = search[1];
}
