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

int cont;
void Astar(PRB &prb, int &UB, const int threshold, int depth,
           clock_t &timeClockInit, bool &opt, int &newPossibleThreshold) {
  if (clock() - timeClockInit > 300000000LL) {
    opt = 0;
    return;
  }

  cont++;
  if (prb.target == prb.N + 1) {
    if (UB > depth) {
      UB = depth;
    }
    return;
  }

  uint8_t x = prb.atStack[prb.target];
  uint8_t y = prb.atTier[prb.target];
  if (y == prb.height[x]) {
    int block = prb.target;

    prb.retrieveBlock(x);

    Astar(prb, UB, threshold, depth, timeClockInit, opt, newPossibleThreshold);

    prb.putbackBlock(block, x);
  } else {
    uint8_t LB = prb.lowerBoundLIS();
    if (depth + LB > threshold) {
      newPossibleThreshold = min(newPossibleThreshold, depth + LB);
      return;
    }

    int block = prb.priority[x][prb.height[x]];
    for (uint8_t s = 1; s <= prb.S; s++) {
      if (s != x && prb.height[s] < prb.T &&
          block < prb.quality[s][prb.height[s]]) {
        prb.relocateBlock(x, s);
        Astar(prb, UB, threshold, depth + 1, timeClockInit, opt,
              newPossibleThreshold);
        prb.relocateBlock(s, x);
      }
    }

    for (uint8_t s = 1; s <= prb.S; s++) {
      if (s != x && prb.height[s] < prb.T &&
          block > prb.quality[s][prb.height[s]]) {
        prb.relocateBlock(x, s);
        Astar(prb, UB, threshold, depth + 1, timeClockInit, opt,
              newPossibleThreshold);
        prb.relocateBlock(s, x);
      }
    }
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
  for (; threshold < UB; threshold++) {
    bool opt = 1;
    int newPossibleThreshold = 10000;
    Astar(prb, UB, threshold, 0, timeClock, opt, newPossibleThreshold);

    if (newPossibleThreshold > threshold + 1) {
      threshold = newPossibleThreshold - 1;
    }

    if (opt == 0) {
      optGlobal = false;
      break;
    }
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
    bool optGlobal = true;
    long long timeTotal = 0;
    int relocations = IterativeDeepingAStar(prb, optGlobal, timeTotal);
    printf("%d\t%d\t%llu\t%d\n", relocations, optGlobal, timeTotal, cont);
    fflush(stdout);
  }
  return 0;
}
