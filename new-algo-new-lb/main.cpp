#include "PRB.h"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <stack>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>

using namespace std;

#define TIMEOUT -1
#define OPTIMAL -2

int cont;
int Astar(PRB &prb, const int threshold, int depth, clock_t &timeClockInit) {
  if (clock() - timeClockInit > 300000000LL) {
    return TIMEOUT;
  }

  cont++;
  if (prb.target == prb.N + 1) {
    return OPTIMAL;
  }

  uint8_t x = prb.atStack[prb.target];
  uint8_t y = prb.atTier[prb.target];
  if (y == prb.height[x]) {
    int block = prb.target;

    prb.retrieveBlock(x);

    int result = Astar(prb, threshold, depth, timeClockInit);

    prb.putbackBlock(block, x);

    return result;
  } else {
    uint8_t LB = prb.lowerBoundLIS();
    if (depth + LB > threshold) {
      return depth + LB;
    }

    int newPossibleTarget = 10000;
    int block = prb.priority[x][prb.height[x]];
    for (uint8_t s = 1; s <= prb.S; s++) {
      if (s != x && prb.height[s] < prb.T &&
          block < prb.quality[s][prb.height[s]]) {
        prb.relocateBlock(x, s);
        int result = Astar(prb, threshold, depth + 1, timeClockInit);
        prb.relocateBlock(s, x);
        if (result < 0) {
          return result;
        }
        newPossibleTarget = min(newPossibleTarget, result);
      }
    }

    for (uint8_t s = 1; s <= prb.S; s++) {
      if (s != x && prb.height[s] < prb.T &&
          block > prb.quality[s][prb.height[s]]) {
        prb.relocateBlock(x, s);
        int result = Astar(prb, threshold, depth + 1, timeClockInit);
        prb.relocateBlock(s, x);
        if (result < 0) {
          return result;
        }
        newPossibleTarget = min(newPossibleTarget, result);
      }
    }

    return newPossibleTarget;
  }
}

int IterativeDeepingAStar(PRB prb, bool &optGlobal, long long &timeTotal) {
  cont = 0;
  PRB temp = prb;
  int UB = temp.heuristic().size();

  int threshold = prb.lowerBoundLIS();

  printf("%d\t%d\t", threshold, UB);
  fflush(stdout);

  clock_t timeClock = clock();
  while (true) {
    if (threshold == UB) {
      optGlobal = true;
      break;
    }

    int result = Astar(prb, threshold, 0, timeClock);

    if (result == TIMEOUT) {
      optGlobal = false;
      break;
    }

    if (result == OPTIMAL) {
      UB = threshold;
      optGlobal = true;
      break;
    }

    threshold = result;
  }

  timeTotal = clock() - timeClock;

  return UB;
}

int main() {
  int TC;
  uint8_t S, T, N, Si, Ti;
  scanf("%d", &TC);
  while (TC--) {
    scanf("%hhu %hhu %hhu", &S, &T, &N);
    PRB prb(N, S, T);
    for (uint8_t block = 1; block <= N; block++) {
      scanf("%hhu %hhu", &Si, &Ti);
      prb.initializeBlock(Si, Ti, block);
    }
    prb.generateQuality();
    bool optGlobal;
    long long timeTotal;
    int relocations = IterativeDeepingAStar(prb, optGlobal, timeTotal);
    printf("%d\t%d\t%llu\t%d\n", relocations, optGlobal, timeTotal, cont);
    fflush(stdout);
  }
  return 0;
}
