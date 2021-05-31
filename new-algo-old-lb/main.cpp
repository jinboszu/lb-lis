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

int cont = 0;
int Astar(PRB &prb1, const int target, int depth, clock_t &timeClockIni) {
  if (clock() - timeClockIni > 300000000LL) {
    return TIMEOUT;
  }

  cont++;
  if (prb1.targetBlock == prb1.N + 1) {
    return OPTIMAL;
  }

  uint8_t stackTarget = prb1.indS[prb1.targetBlock],
          heightTarget = prb1.indT[prb1.targetBlock];
  if (heightTarget + 1 == prb1.heightS[stackTarget]) {
    int block = prb1.targetBlock;

    prb1.retrievalBlock(stackTarget);

    int result = Astar(prb1, target, depth, timeClockIni);

    prb1.putBlock(block, stackTarget);

    return result;
  } else {
    uint8_t LB = prb1.lowerBoundLIS();
    if (depth + LB > target) {
      return depth + LB;
    }

    int newPossibleTarget = 10000;
    int blockToBeRelocate =
        prb1.stackedArea[prb1.heightS[stackTarget] - 1][stackTarget];
    for (uint8_t stack = 1; stack <= prb1.S; stack++)
      if (stack != stackTarget && prb1.heightS[stack] <= prb1.T) {
        if (blockToBeRelocate < prb1.Q[stack]) {
          prb1.relocationBlock(stackTarget, stack);
          int result = Astar(prb1, target, depth + 1, timeClockIni);
          prb1.relocationBlock(stack, stackTarget);
          if (result < 0) {
            return result;
          }
          newPossibleTarget = min(newPossibleTarget, result);
        }
      }

    for (uint8_t stack = 1; stack <= prb1.S; stack++)
      if (stack != stackTarget && prb1.heightS[stack] <= prb1.T) {
        if (blockToBeRelocate > prb1.Q[stack]) {
          prb1.relocationBlock(stackTarget, stack);
          int result = Astar(prb1, target, depth + 1, timeClockIni);
          prb1.relocationBlock(stack, stackTarget);
          if (result < 0) {
            return result;
          }
          newPossibleTarget = min(newPossibleTarget, result);
        }
      }
    return newPossibleTarget;
  }
}

int IterativeDeepingAStar(PRB prb1, bool &optGlobal, long long &timeTotal) {
  cont = 0;
  int UB = (prb1.upperBound()).size();

  int target = prb1.lowerBoundLIS();

  printf("%d\t%d\t", target, UB);
  clock_t timeClock = clock();
  while (true) {
    if (target == UB) {
      optGlobal = true;
      break;
    }

    int result = Astar(prb1, target, 0, timeClock);

    if (result == TIMEOUT) {
      optGlobal = false;
      break;
    }

    if (result == OPTIMAL) {
      UB = target;
      optGlobal = true;
      break;
    }

    target = result;
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
    PRB prb1(N, S, T);
    for (uint8_t block = 1; block <= N; block++) {
      scanf("%hhu %hhu", &Si, &Ti);
      prb1.insertBlock(Si, Ti, block);
    }
    bool optGlobal;
    long long timeTotal;
    int relocations = IterativeDeepingAStar(prb1, optGlobal, timeTotal);
    printf("%d\t%d\t%llu\t%d\n", relocations, optGlobal, timeTotal, cont);
    fflush(stdout);
  }
  return 0;
}
