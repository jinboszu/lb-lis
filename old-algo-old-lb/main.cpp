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

int cont = 0;
void Astar(PRB &prb1, int &UB, const int target, int depth,
           clock_t &timeClockIni, bool &opt, int &newPossibleTarget) {
  if (clock() - timeClockIni > 300000000LL) {
    opt = 0;
    return;
  }

  cont++;
  if (prb1.targetBlock == prb1.N + 1) {
    if (UB > depth) {
      UB = depth;
    }
    return;
  }

  uint8_t stackTarget = prb1.indS[prb1.targetBlock],
          heightTarget = prb1.indT[prb1.targetBlock];
  if (heightTarget + 1 == prb1.heightS[stackTarget]) {
    int block = prb1.targetBlock;

    prb1.retrievalBlock(stackTarget);

    Astar(prb1, UB, target, depth, timeClockIni, opt, newPossibleTarget);

    prb1.putBlock(block, stackTarget);
  } else {
    uint8_t LB = prb1.lowerBoundLIS();
    if (depth + LB > target) {
      newPossibleTarget = min(newPossibleTarget, depth + LB);
      return;
    }

    int blockToBeRelocate =
        prb1.stackedArea[prb1.heightS[stackTarget] - 1][stackTarget];
    for (uint8_t stack = 1; stack <= prb1.S; stack++)
      if (stack != stackTarget && prb1.heightS[stack] <= prb1.T) {
        if (blockToBeRelocate < prb1.Q[stack]) {
          prb1.relocationBlock(stackTarget, stack);
          Astar(prb1, UB, target, depth + 1, timeClockIni, opt,
                newPossibleTarget);
          prb1.relocationBlock(stack, stackTarget);
        }
      }

    for (uint8_t stack = 1; stack <= prb1.S; stack++)
      if (stack != stackTarget && prb1.heightS[stack] <= prb1.T) {
        if (blockToBeRelocate > prb1.Q[stack]) {
          prb1.relocationBlock(stackTarget, stack);
          Astar(prb1, UB, target, depth + 1, timeClockIni, opt,
                newPossibleTarget);
          prb1.relocationBlock(stack, stackTarget);
        }
      }
  }
}

int IterativeDeepingAStar(PRB prb1, bool &optGlobal, long long &timeTotal) {
  cont = 0;
  int UB = (prb1.upperBound()).size();

  int target = prb1.lowerBoundLIS();

  printf("%d\t%d\t", target, UB);
  clock_t timeClock = clock();
  for (; target < UB; target++) {
    bool opt = 1;
    int newPossibleTarget = 10000;
    Astar(prb1, UB, target, 0, timeClock, opt, newPossibleTarget);

    if (newPossibleTarget > target + 1) {
      target = newPossibleTarget - 1;
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
    PRB prb1(N, S, T);
    for (uint8_t block = 1; block <= N; block++) {
      scanf("%hhu %hhu", &Si, &Ti);
      prb1.insertBlock(Si, Ti, block);
    }
    bool optGlobal = true;
    long long timeTotal = 0;
    int relocations = IterativeDeepingAStar(prb1, optGlobal, timeTotal);
    printf("%d\t%d\t%llu\t%d\n", relocations, optGlobal, timeTotal, cont);
    fflush(stdout);
  }
  return 0;
}
