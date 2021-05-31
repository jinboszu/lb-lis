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
  uint8_t targetBlock;
  vector<vector<uint8_t>> stackedArea;
  vector<uint8_t> indS;
  vector<uint8_t> indT;
  vector<uint8_t> heightS;
  vector<uint8_t> Q;

  PRB();
  PRB(const uint8_t _N, const uint8_t _S, const uint8_t _T);
  int putBlock(const uint8_t block, const uint8_t stack);
  int retrievalBlock(const uint8_t stack);
  int relocationBlock(const uint8_t stackSouce, const uint8_t stackDesti);
  void insertBlock(const uint8_t stack, const uint8_t height,
                   const uint8_t block);

  void print();

  uint8_t lowerBound1();
  uint8_t lowerBound2();
  uint8_t lowerBound3();
  uint8_t lowerBoundPrev4();
  uint8_t lowerBound4();
  void lowerBound2PDB(vector<vector<int>> &extraRelocations);
  vector<vector<int>> lowerBound3PDB();

  void getLIS(vector<uint8_t> &blocks, vector<uint8_t> &LIS);
  uint8_t lowerBoundLISPrev();
  uint8_t lowerBoundLIS();

  vector<pair<uint8_t, uint8_t>> upperBound();

  bool operator==(const PRB &prb1);
};

#endif
