#include "PRB.h"

#include <assert.h>
#include <iostream>
#include <set>
#include <stdint.h>
#include <stdio.h>
#include <vector>

using namespace std;

#define inf 254

PRB::PRB(const uint8_t _N, const uint8_t _S, const uint8_t _T)
    : N(_N), S(_S), T(_T), targetBlock(_N + 1) {
  vector<uint8_t> temp(S + 1, 0);
  stackedArea.resize(T + 1, temp);
  heightS.resize(S + 1, 1);
  Q.resize(S + 1, inf);
  indS.resize(N + 1, 0);
  indT.resize(N + 1, 0);
}

int PRB::putBlock(const uint8_t block, const uint8_t stack) {
  if (heightS[stack] == T + 1) {
    return -1;
  }

  stackedArea[heightS[stack]][stack] = block;
  indS[block] = stack;
  indT[block] = heightS[stack];
  heightS[stack]++;
  targetBlock = block;
  Q[stack] = min(Q[stack], block);

  return 1;
}

int PRB::retrievalBlock(const uint8_t stack) {
  uint8_t block = stackedArea[heightS[stack] - 1][stack];

  if (targetBlock != block) {
    return -1;
  }
  stackedArea[heightS[stack] - 1][stack] = 0;
  indS[block] = 0;
  indT[block] = 0;
  heightS[stack]--;
  Q[stack] = inf;
  for (uint8_t t = 1; t < heightS[stack]; t++) {
    Q[stack] = min(Q[stack], stackedArea[t][stack]);
  }
  targetBlock++;
  return 1;
}

// RELOCATION from the stackSrc to stackDest
int PRB::relocationBlock(const uint8_t stackSource, const uint8_t stackDesti) {

  if (heightS[stackDesti] == T + 1 || heightS[stackSource] == 1) {
    return -1;
  }
  uint8_t block = stackedArea[heightS[stackSource] - 1][stackSource];
  stackedArea[heightS[stackDesti]][stackDesti] = block;
  indS[block] = stackDesti;
  indT[block] = heightS[stackDesti];
  heightS[stackDesti]++;
  Q[stackDesti] = min(Q[stackDesti], block);

  stackedArea[heightS[stackSource] - 1][stackSource] = 0;
  heightS[stackSource]--;
  Q[stackSource] = inf;
  for (int t = 1; t < heightS[stackSource]; t++) {
    Q[stackSource] = min(Q[stackSource], stackedArea[t][stackSource]);
  }
  return 1;
}

bool PRB::operator==(const PRB &prb1) {
  bool bDimentions =
      ((this->S == prb1.S) && (this->T == prb1.T) && (this->N == prb1.N));
  bool bTargetBlock = (this->targetBlock == prb1.targetBlock);
  bool bVectors = ((this->stackedArea == prb1.stackedArea) &&
                   (this->indS == prb1.indS) && (this->indT == prb1.indT) &&
                   (this->heightS == prb1.heightS) && (this->Q == prb1.Q));

  return (bDimentions && bTargetBlock && bVectors);
}

void PRB::insertBlock(const uint8_t stack, const uint8_t height,
                      const uint8_t block) {
  stackedArea[height][stack] = block;
  Q[stack] = min(Q[stack], block);
  heightS[stack] = max(heightS[stack], (uint8_t)(height + 1));
  indS[block] = stack;
  indT[block] = height;
  targetBlock = min(targetBlock, block);
}

uint8_t PRB::lowerBound1() {
  uint8_t relocations = 0;
  for (uint8_t stack = 1; stack <= S; stack++) {
    uint8_t target = inf;
    for (uint8_t height = 1; height < heightS[stack]; height++) {
      if (target < stackedArea[height][stack]) {
        relocations++;
      }
      target = min(target, stackedArea[height][stack]);
    }
  }
  return relocations;
}

// IMPORTANT: You need increase lowerBound1 when you use only LowerBound2

uint8_t PRB::lowerBound2() {
  uint8_t relocations = 0;
  uint8_t stackTarget = indS[targetBlock];
  uint8_t heightTarget = indT[targetBlock];
  uint8_t Qmax = 1;
  for (uint8_t stack = 1; stack <= S; stack++)
    if (stack != stackTarget && heightS[stack] <= T) {
      Qmax = max(Q[stack], Qmax);
    }

  for (uint8_t height = heightTarget + 1; height < heightS[stackTarget];
       height++) {
    if (stackedArea[height][stackTarget] > Qmax) {
      relocations++;
    }
    if (stackedArea[height][stackTarget] > Q[stackTarget]) {
      relocations++;
    }
  }
  return relocations;
}

uint8_t PRB::lowerBound3() {
  PRB prb1 = *this;
  uint8_t relocations = 0;
  for (; prb1.targetBlock <= prb1.N; prb1.targetBlock++) {
    uint8_t stackTarget = prb1.indS[prb1.targetBlock];
    uint8_t heightTarget = prb1.indT[prb1.targetBlock];
    // printf("%hhu :: %hhu\n", stackTarget, heightTarget);
    if (!(stackTarget == 0 && heightTarget == 0)) {
      relocations += prb1.lowerBound2();
      for (uint8_t height = heightTarget; height < prb1.heightS[stackTarget];
           height++) {
        uint8_t &tempBlock = prb1.stackedArea[height][stackTarget];
        prb1.indS[tempBlock] = 0;
        prb1.indT[tempBlock] = 0;
        tempBlock = 0;
      }
      prb1.Q[stackTarget] = inf;
      for (uint8_t height = 1; height < heightTarget; height++) {
        prb1.Q[stackTarget] =
            min(prb1.Q[stackTarget], prb1.stackedArea[height][stackTarget]);
      }
      prb1.heightS[stackTarget] = heightTarget;
    }
  }
  return relocations;
}
void PRB::lowerBound2PDB(vector<vector<int>> &extraRelocations) {
  uint8_t stackTarget = indS[targetBlock];
  uint8_t heightTarget = indT[targetBlock];
  uint8_t Qmax = 1;
  for (uint8_t stack = 1; stack <= S; stack++)
    if (stack != stackTarget && heightS[stack] <= T) {
      Qmax = max(Q[stack], Qmax);
    }

  for (uint8_t height = heightTarget + 1; height < heightS[stackTarget];
       height++) {
    if (stackedArea[height][stackTarget] > Qmax) {
      extraRelocations[height][stackTarget]++;
    }
    if (stackedArea[height][stackTarget] > Q[stackTarget]) {
      extraRelocations[height][stackTarget]++;
    }
  }
}

vector<vector<int>> PRB::lowerBound3PDB() {
  PRB prb1 = *this;
  vector<vector<int>> extraRelocations(prb1.T + 1, vector<int>(prb1.S + 1, 0));
  for (; prb1.targetBlock <= prb1.N; prb1.targetBlock++) {
    uint8_t stackTarget = prb1.indS[prb1.targetBlock];
    uint8_t heightTarget = prb1.indT[prb1.targetBlock];
    // printf("%hhu :: %hhu\n", stackTarget, heightTarget);
    if (!(stackTarget == 0 && heightTarget == 0)) {
      prb1.lowerBound2PDB(extraRelocations);
      for (uint8_t height = heightTarget; height < prb1.heightS[stackTarget];
           height++) {
        uint8_t &tempBlock = prb1.stackedArea[height][stackTarget];
        prb1.indS[tempBlock] = 0;
        prb1.indT[tempBlock] = 0;
        tempBlock = 0;
      }
      prb1.Q[stackTarget] = inf;
      for (uint8_t height = 1; height < heightTarget; height++) {
        prb1.Q[stackTarget] =
            min(prb1.Q[stackTarget], prb1.stackedArea[height][stackTarget]);
      }
      prb1.heightS[stackTarget] = heightTarget;
    }
  }
  return extraRelocations;
}

vector<pair<uint8_t, uint8_t>> PRB::upperBound() {
  vector<pair<uint8_t, uint8_t>> relocations;
  PRB prb1 = *this;
  for (; prb1.targetBlock <= prb1.N;) {
    multiset<pair<uint8_t, uint8_t>> stTargets;
    multiset<pair<uint8_t, uint8_t>>::iterator it;
    multiset<pair<uint8_t, uint8_t>>::reverse_iterator rit;
    for (uint8_t stack = 1; stack <= prb1.S; stack++) {
      stTargets.insert(make_pair(prb1.Q[stack], stack));
    }
    uint8_t stackTarget = prb1.indS[prb1.targetBlock];
    uint8_t heightTarget = prb1.indT[prb1.targetBlock];
    for (uint8_t height = prb1.heightS[stackTarget] - 1; height > heightTarget;
         height--) {
      uint8_t block = prb1.stackedArea[height][stackTarget];
      it = stTargets.end();
      it--;
      uint8_t Qmax = (*it).first;
      uint8_t relocate = 0;
      if (block < Qmax) {
        // the block (height, stackTarget) is relocatable without making it a
        // blocking block.
        for (it = stTargets.begin(); it != stTargets.end(); it++) {
          if (block < ((*it).first)) {
            uint8_t stackDesti = (*it).second;
            if (prb1.relocationBlock(stackTarget, stackDesti) != -1) {
              relocate = 1;
              stTargets.erase(it);
              stTargets.insert(make_pair(prb1.Q[stackDesti], stackDesti));
              relocations.push_back(make_pair(stackTarget, stackDesti));
              break;
            }
          }
        }
      }
      if (relocate == 0) {
        // the block (height, stackTarget) always become a blocking block after
        // relocation
        uint8_t stackTemp;
        uint8_t valTemp;
        for (rit = stTargets.rbegin(); rit != stTargets.rend(); rit++) {
          uint8_t stackDesti = (*rit).second;

          if (prb1.heightS[stackDesti] <= prb1.T && stackDesti != stackTarget) {
            if (prb1.heightS[stackDesti] == T && relocate == 0) {
              relocate = 1;
              stackTemp = stackDesti;
              valTemp = (*rit).first;
            } else {
              prb1.relocationBlock(stackTarget, stackDesti);
              stTargets.erase(--(rit.base()));
              stTargets.insert(make_pair(prb1.Q[stackDesti], stackDesti));
              relocations.push_back(make_pair(stackTarget, stackDesti));
              relocate = 2;
              break;
            }
          }
        }

        if (relocate == 1) {
          prb1.relocationBlock(stackTarget, stackTemp);
          stTargets.erase(make_pair(valTemp, stackTemp));
          stTargets.insert(make_pair(prb1.Q[stackTemp], stackTemp));
          relocations.push_back(make_pair(stackTarget, stackTemp));
        } else if (relocate == 0) {
          printf("I do not found some place to relocate this block\n");
          assert(0 == 1);
        }
      }
    }
    if (prb1.retrievalBlock(stackTarget) == -1) {
      printf("targetBlock = %hhu", prb1.targetBlock);
      prb1.print();
      assert(0 == 1);
    }
  }

  return relocations;
}

uint8_t getQmax(multiset<uint8_t> s1) {
  multiset<uint8_t>::iterator it;
  it = s1.end();
  it--;
  return *it;
}

void relocateinQ(multiset<uint8_t> &s1, uint8_t block) {
  for (multiset<uint8_t>::iterator it = s1.begin(); it != s1.end(); it++) {
    if (block < *it) {
      s1.erase(it);
      s1.insert(block);
      return;
    }
  }
}

uint8_t PRB::lowerBoundPrev4() {
  uint8_t relocations = 200;

  uint8_t stackTarget = indS[targetBlock];
  uint8_t heightTarget = indT[targetBlock];
  multiset<uint8_t> oriQi;
  for (uint8_t stack = 1; stack <= S; stack++)
    if (heightS[stack] <= T && stack != stackTarget) {
      oriQi.insert(Q[stack]);
      // printf("%hhu ", Q[stack]);
    }
  // printf("\n");
  int dif = heightS[stackTarget] - heightTarget - 1;
  // printf("diff = %d\n", dif);
  for (int mask = 0; mask < (1 << dif); mask++) {
    multiset<uint8_t> Qi = oriQi;
    int tmp = 1;
    uint8_t tmpRelocations = 0;
    for (int tmpHeight = 0; tmpHeight < dif; tmpHeight++) {
      uint8_t heightC = heightS[stackTarget] - 1 - tmpHeight;
      if ((1 << tmpHeight) & mask) {
        // I have to relocate it, (it is not a blocking block)
        uint8_t Qmax = getQmax(Qi);
        if (Qmax < stackedArea[heightC][stackTarget]) {
          tmp = 0;
          break;
        } else {
          relocateinQ(Qi, stackedArea[heightC][stackTarget]);
        }
      } else {
        // become a blocking Block
        uint8_t Qmax = getQmax(Qi);
        if (stackedArea[heightC][stackTarget] < Qmax) {
          tmp = 0;
          break;
        } else {
          tmpRelocations++;
        }
      }
    }
    if (tmp) {
      // printf("mask = %d\n", mask);
      // printf("val = %hhu\n", tmpRelocations);
      relocations = min(tmpRelocations, relocations);
    }
  }
  // printf("relocatios for targetBlock = %hhu, is %hhu\n", targetBlock,
  // relocations);
  return relocations;
}

uint8_t PRB::lowerBound4() {
  PRB prb1 = *this;
  uint8_t relocations = prb1.lowerBound1();
  for (; prb1.targetBlock <= prb1.N; prb1.targetBlock++) {
    uint8_t stackTarget = prb1.indS[prb1.targetBlock];
    uint8_t heightTarget = prb1.indT[prb1.targetBlock];
    // printf("%hhu :: %hhu\n", stackTarget, heightTarget);
    if (!(stackTarget == 0 && heightTarget == 0)) {
      relocations += prb1.lowerBoundPrev4();
      for (uint8_t height = heightTarget; height < prb1.heightS[stackTarget];
           height++) {
        uint8_t &tempBlock = prb1.stackedArea[height][stackTarget];
        prb1.indS[tempBlock] = 0;
        prb1.indT[tempBlock] = 0;
        tempBlock = 0;
      }
      prb1.Q[stackTarget] = inf;
      for (uint8_t height = 1; height < heightTarget; height++) {
        prb1.Q[stackTarget] =
            min(prb1.Q[stackTarget], prb1.stackedArea[height][stackTarget]);
      }
      prb1.heightS[stackTarget] = heightTarget;
    }
  }
  return relocations;
}

void PRB::print() {
  printf("::::::::::::::::::STACKED AREA:::::::::::::::::::::::\n");
  for (uint8_t height = 1; height <= T; height++) {
    for (uint8_t stack = 1; stack <= S; stack++) {
      printf("%hhu ", stackedArea[T - height + 1][stack]);
    }
    printf("\n");
  }

  printf("heightS: ");
  for (int stack = 1; stack <= S; stack++) {
    printf("%hhu ", heightS[stack]);
  }
  printf("\n");

  printf("indS: ");
  for (int block = 1; block <= N; block++) {
    printf("%hhu ", indS[block]);
  }
  printf("\n");

  printf("indT: ");
  for (int block = 1; block <= N; block++) {
    printf("%hhu ", indT[block]);
  }
  printf("\n");

  printf("targetBlock = %hhu\n", targetBlock);
}

/*
New lower bound using LIS
*/

void PRB::getLIS(vector<uint8_t> &blocks, vector<uint8_t> &LIS) {
  int n = LIS.size();
  for (int i = 0; i < n; i++) {
    LIS[i] = 0;
    for (int j = 0; j < i; j++) {
      if (blocks[i] < blocks[j]) {
        LIS[i] = max(LIS[i], LIS[j]);
      }
    }
    LIS[i] += 1;
  }
}

uint8_t PRB::lowerBoundLISPrev() {
  uint8_t relocations = 0;
  uint8_t stackTarget = indS[targetBlock];
  uint8_t heightTarget = indT[targetBlock];
  uint8_t Qmax = 0;

  vector<uint8_t> temp;

  for (uint8_t stack = 1; stack <= S; stack++)
    if (stack != stackTarget && heightS[stack] <= T) {
      Qmax = max(Q[stack], Qmax);
    }

  for (uint8_t height = heightTarget + 1; height < heightS[stackTarget];
       height++) {
    if (stackedArea[height][stackTarget] > Qmax) {
      relocations++;
    } else {
      temp.push_back(stackedArea[height][stackTarget]);
    }
    if (stackedArea[height][stackTarget] > Q[stackTarget]) {
      relocations++;
    }
  }

  int n = temp.size();
  vector<uint8_t> LIS(n, 0);
  getLIS(temp, LIS);

  uint8_t maxi = 0;
  for (uint8_t index = 0; index < n; index++) {
    uint8_t freeRelocation = 0;
    for (uint8_t stack = 1; stack <= S; stack++)
      if (stack != stackTarget && heightS[stack] <= T) {
        if (temp[index] < Q[stack]) {
          freeRelocation++;
        }
      }

    if (LIS[index] > freeRelocation) {
      uint8_t rest = LIS[index] - freeRelocation;
      maxi = max(rest, maxi);
    }
  }

  return relocations + maxi;
}

uint8_t PRB::lowerBoundLIS() {
  PRB prb1 = *this;
  uint8_t relocations = 0;
  for (; prb1.targetBlock <= prb1.N; prb1.targetBlock++) {
    uint8_t stackTarget = prb1.indS[prb1.targetBlock];
    uint8_t heightTarget = prb1.indT[prb1.targetBlock];
    // printf("%hhu :: %hhu\n", stackTarget, heightTarget);
    if (!(stackTarget == 0 && heightTarget == 0)) {
      relocations += prb1.lowerBoundLISPrev();
      for (uint8_t height = heightTarget; height < prb1.heightS[stackTarget];
           height++) {
        uint8_t &tempBlock = prb1.stackedArea[height][stackTarget];
        prb1.indS[tempBlock] = 0;
        prb1.indT[tempBlock] = 0;
        tempBlock = 0;
      }
      prb1.Q[stackTarget] = inf;
      for (uint8_t height = 1; height < heightTarget; height++) {
        prb1.Q[stackTarget] =
            min(prb1.Q[stackTarget], prb1.stackedArea[height][stackTarget]);
      }
      prb1.heightS[stackTarget] = heightTarget;
    }
  }
  return relocations;
}
