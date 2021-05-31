#include "PRB.h"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <set>
#include <stdint.h>
#include <stdio.h>
#include <vector>

using namespace std;

#define inf 254

PRB::PRB(const uint8_t _N, const uint8_t _S, const uint8_t _T)
    : N(_N), S(_S), T(_T), target(_N + 1) {
  vector<uint8_t> temp(T + 1, 0);
  temp[0] = inf;
  priority.resize(S + 1, temp);
  quality.resize(S + 1, temp);
  height.resize(S + 1, 0);
  atStack.resize(N + 1, 0);
  atTier.resize(N + 1, 0);
}

void PRB::initializeBlock(const uint8_t stack, const uint8_t tier,
                          const uint8_t block) {
  priority[stack][tier] = block;
  height[stack] = max(height[stack], tier);
  atStack[block] = stack;
  atTier[block] = tier;
  target = min(target, block);
}

void PRB::generateQuality() {
  for (int s = 1; s <= S; s++) {
    for (int t = 1; t <= height[s]; t++) {
      quality[s][t] = min(quality[s][t - 1], priority[s][t]);
    }
  }
}

int PRB::relocateBlock(const uint8_t src, const uint8_t dst) {
  if (height[dst] == T || height[src] == 0) {
    return -1;
  }

  uint8_t block = priority[src][height[src]];

  priority[src][height[src]] = 0;
  quality[src][height[src]] = 0;
  height[src]--;

  height[dst]++;
  priority[dst][height[dst]] = block;
  quality[dst][height[dst]] = min(quality[dst][height[dst] - 1], block);

  atStack[block] = dst;
  atTier[block] = height[dst];

  return 1;
}

int PRB::retrieveBlock(const uint8_t src) {
  uint8_t block = priority[src][height[src]];

  if (target != block) {
    return -1;
  }

  priority[src][height[src]] = 0;
  quality[src][height[src]] = 0;
  height[src]--;

  atStack[block] = 0;
  atTier[block] = 0;

  target++;
  return 1;
}

int PRB::putbackBlock(const uint8_t block, const uint8_t dst) {
  if (height[dst] == T) {
    return -1;
  }

  height[dst]++;
  priority[dst][height[dst]] = block;
  quality[dst][height[dst]] = min(quality[dst][height[dst] - 1], block);

  atStack[block] = dst;
  atTier[block] = height[dst];

  target = block;

  return 1;
}

vector<pair<uint8_t, uint8_t>> PRB::heuristic() {
  vector<pair<uint8_t, uint8_t>> relocations;
  while (target <= N) {
    multiset<pair<uint8_t, uint8_t>> all;
    for (uint8_t s = 1; s <= S; s++) {
      all.insert(make_pair(quality[s][height[s]], s));
    }
    uint8_t x = atStack[target];
    uint8_t y = atTier[target];

    while (height[x] > y) {
      uint8_t block = priority[x][height[x]];
      uint8_t Qmax = all.rbegin()->first;
      uint8_t relocate = 0;
      if (block < Qmax) {
        for (multiset<pair<uint8_t, uint8_t>>::iterator it = all.begin();
             it != all.end(); it++) {
          if (block < it->first) {
            uint8_t dst = it->second;
            if (relocateBlock(x, dst) != -1) {
              relocate = 1;
              all.erase(it);
              all.insert(make_pair(quality[dst][height[dst]], dst));
              relocations.push_back(make_pair(x, dst));
              break;
            }
          }
        }
      }
      if (relocate == 0) {
        uint8_t dstTemp;
        uint8_t valTemp;
        for (multiset<pair<uint8_t, uint8_t>>::reverse_iterator rit =
                 all.rbegin();
             rit != all.rend(); rit++) {
          uint8_t dst = rit->second;
          if (height[dst] < T && dst != x) {
            if (height[dst] == T - 1 && relocate == 0) {
              relocate = 1;
              dstTemp = dst;
              valTemp = rit->first;
            } else {
              relocateBlock(x, dst);
              all.erase(--(rit.base()));
              all.insert(make_pair(quality[dst][height[dst]], dst));
              relocations.push_back(make_pair(x, dst));
              relocate = 2;
              break;
            }
          }
        }

        if (relocate == 1) {
          relocateBlock(x, dstTemp);
          all.erase(make_pair(valTemp, dstTemp));
          all.insert(make_pair(quality[dstTemp][height[dstTemp]], dstTemp));
          relocations.push_back(make_pair(x, dstTemp));
        } else if (relocate == 0) {
          printf("I do not found some place to relocate this block\n");
          assert(0 == 1);
        }
      }
    }
    if (retrieveBlock(x) == -1) {
      printf("target = %hhu\n", target);
      assert(0 == 1);
    }
    }
  return relocations;
}

/*
New lower bound using LIS
*/
uint8_t PRB::lowerBoundLIS() {
  vector<uint8_t> remain = height;
  vector<uint8_t> q;
  vector<uint8_t> p;

  /* initialization */
  for (uint8_t s = 1; s <= S; s++) {
    if (height[s] < T) {
      q.push_back(quality[s][height[s]]);
    }
  }

  sort(q.rbegin(), q.rend());

  /* iterative process */
  uint8_t lb = 0;
  for (uint8_t n = target; n <= N; n++) {
    uint8_t x = atStack[n];
    uint8_t y = atTier[n];
    uint8_t h = remain[x];

    if (h < y) {
      continue;
    }
    if (h < T) {
      q.pop_back();
    }

    if (h > y) { // subroutine of LB-LIS
      /* step 1 */
      lb += h - y;
      uint8_t largest = q.front();
      /* steps 2 & 3*/
      p.clear();
      for (uint8_t yy = y + 1; yy <= h; yy++) {
        uint8_t block = priority[x][yy]; // B[x][yy]
        if (block > largest) {           // zeros(yy) = 0
          lb++;
        } else {
          /* binary search */
          uint8_t left = 0;
          uint8_t right = p.size();
          while (left < right) {
            uint8_t mid = left + (right - left) / 2;
            if (p[mid] < block) {
              right = mid;
            } else {
              left = mid + 1;
            }
          }
          if (left < p.size()) {
            p[left] = block;
          } else {
            p.push_back(block);
          }
        }
      }

      /* step 4 */
      vector<uint8_t>::iterator it = q.begin();
      for (uint8_t i = 0; i < p.size(); i++) {
        if (it != q.end() && p[i] < *it) {
          it++;
        } else {
          lb++;
        }
      }
    }
    remain[x] = y - 1; // remove blocks

    vector<uint8_t>::iterator it = q.begin();
    while (it != q.end() && quality[x][y - 1] < *it) {
      it++;
    }
    q.insert(it, quality[x][y - 1]); // update qList
  }

  return lb;
}
