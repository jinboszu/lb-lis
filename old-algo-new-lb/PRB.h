#ifndef PRB_H
#define PRB_H

#include <stdint.h>
#include <vector>

using namespace std;

class PRB {
public:
  uint8_t N;
  uint8_t S;
  uint8_t T;
  uint8_t target;
  vector<vector<uint8_t>> priority;
  vector<vector<uint8_t>> quality;
  vector<uint8_t> height;
  vector<uint8_t> atStack;
  vector<uint8_t> atTier;

  PRB(const uint8_t _N, const uint8_t _S, const uint8_t _T);
  void initializeBlock(const uint8_t stack, const uint8_t tier,
                       const uint8_t block);
  void generateQuality();

  int relocateBlock(const uint8_t src, const uint8_t dst);
  int retrieveBlock(const uint8_t src);
  int putbackBlock(const uint8_t block, const uint8_t dst);

  vector<pair<uint8_t, uint8_t>> heuristic();

  uint8_t lowerBoundLIS();
};

#endif
