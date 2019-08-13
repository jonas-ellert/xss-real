/*******************************************************************************
 *
 * This version of Nicola Prezzas LCE data structure was implemented by
 * Alexander Herlez, see https://github.com/herlez/lce-test for more info
 *
 ******************************************************************************/
#pragma once

#include <cmath>
#include <algorithm>
#include "lcePrezzaUtil.hpp"

#define lce_unlikely(x)    __builtin_expect(!!(x), 0)

#ifndef always_inline
#define lce_always_inline __attribute__((always_inline))
#else
#define lce_always_inline always_inline
#endif


/* This class builds Prezza's in-place LCE data structure and
 * answers LCE-queries in O(log(n)). */

class LcePrezza {

  __extension__ typedef unsigned __int128 uint128_t;

public:
  LcePrezza() = delete;

  template <typename byte_type>
  LcePrezza(byte_type * text, const uint64_t n) :
                              textLengthInBytes(n),
                              textLengthInBlocks(textLengthInBytes/8 + (textLengthInBytes % 8 == 0 ? 0 : 1)),
                              prime(util::getLow64BitPrime()),
                              fingerprints(reinterpret_cast<uint64_t *>(text)),
                              powerTable(new uint64_t[((int) std::log2(textLengthInBlocks)) + 6]) {
    calculateFingerprints();
    calculatePowers();
    static_assert(sizeof(byte_type) == 1);
  }

  /* Fast LCE-query in O(log(n)) time */
  uint64_t lce(const uint64_t i, const uint64_t j) {
    if (lce_unlikely(i == j)) {
      return textLengthInBytes - i;
    }

    const uint64_t maxLce = textLengthInBytes - ((i < j) ? j : i);

    /* naive part of lce query */
    const int offsetLce1 = (i % 8) * 8;
    const int offsetLce2 = (j % 8) * 8;
    uint64_t blockI = getBlock(i/8);
    uint64_t blockI2 = getBlock(i/8 + 1);
    uint64_t blockJ = getBlock(j/8);
    uint64_t blockJ2 = getBlock(j/8 + 1);
    uint64_t compBlockI = (blockI << offsetLce1) + ((blockI2 >> 1) >> (63-offsetLce1));
    uint64_t compBlockJ = (blockJ << offsetLce2) + ((blockJ2 >> 1) >> (63-offsetLce2));

    /* compare blockwise */
    const uint64_t maxBlocksNaive = maxLce < 1024 ? maxLce/8 : 1024/8;

    uint64_t lce = 0;

    while(lce < maxBlocksNaive) {
      if(compBlockI != compBlockJ) {
        break;
      }
      ++lce;
      blockI = blockI2;
      blockI2 = getBlock((i/8)+lce+1);
      blockJ = blockJ2;
      blockJ2 = getBlock((j/8)+lce+1);
      compBlockI = (blockI << offsetLce1) + ((blockI2 >> 1) >> (63-offsetLce1));
      compBlockJ = (blockJ << offsetLce2) + ((blockJ2 >> 1) >> (63-offsetLce2));
    }



    lce *= 8;
    /* If everything except the stub matches, we compare the stub character-wise and return the result */
    if(lce != 1024) {

      unsigned char * compBlockIP = (unsigned char *) &compBlockI;
      unsigned char * compBlockJP = (unsigned char *) &compBlockJ;
      unsigned int maxStub = (maxLce - lce) < 8 ? (maxLce - lce) : 8;
      for(unsigned int k = 0; k < maxStub; k++) {
        if(compBlockIP[7-k] != compBlockJP[7-k]) {
          return lce;
        }
        ++lce;
      }
      return lce;
    }

    /* exponential search */
    int exp = 11;
    uint64_t dist = 2048;

    while( dist <= maxLce ) {
      if (fingerprintExp(i, exp) != fingerprintExp(j, exp)) {
        break;
      }
      ++exp;
      dist *= 2;
    }

    /* binary search , we start it at i2 and j2, because we know that
     * up until i2 and j2 everything matched */
    --exp;
    dist /= 2;
    uint64_t i2 = i + dist;
    uint64_t j2 = j + dist;


    const uint64_t maxRest = textLengthInBytes - ((i2 < j2) ? j2 : i2);
    while(exp != 0) {
      --exp;
      dist /= 2;
      if (lce_unlikely(dist > maxRest)) {
        continue;
      }
      if(fingerprintExp(i2, exp) == fingerprintExp(j2, exp)) {
        i2 += dist;
        j2 += dist;
      }
    }
    return i2-i;
  }

  /* Returns the prime*/
  uint128_t getPrime() const {
    return prime;
  }

  /* Returns the character at index i */
  char operator[] (const uint64_t i) {
    uint64_t blockNumber = i / 8;
    uint64_t offset = 7 - (i % 8);
    return (getBlock(blockNumber)) >> (8*offset) & 0xff;
  }

  int isSmallerSuffix(const uint64_t i, const uint64_t j) {
    uint64_t lceS = lce(i, j);
    if(lce_unlikely((i + lceS + 1 == textLengthInBytes) || (j + lceS + 1 == textLengthInBytes))) {
      return true;
    }
    return (operator[](i + lceS) < operator[](j + lceS));
  }

private:
  const uint64_t textLengthInBytes;
  const uint64_t textLengthInBlocks;
  const uint128_t prime;
  uint64_t * const fingerprints;
  uint64_t * const powerTable;

  /* Returns the i'th block. A block contains 8 character. */
  uint64_t getBlock(const uint64_t i) {
    if (lce_unlikely(i > textLengthInBlocks)) {
      return 0;
    }
    if (lce_unlikely(i == 0)) {
      if(fingerprints[0] >= 0x8000000000000000ULL) {
        return fingerprints[0] - 0x8000000000000000ULL + prime;
      } else {
        return fingerprints[0];
      }
    } else {
      uint128_t X = fingerprints[i-1] & 0x7FFFFFFFFFFFFFFFULL;
      X <<= 64;
      X %= prime;

      uint64_t currentFingerprint = fingerprints[i];
      uint64_t sBit = currentFingerprint >> 63;
      currentFingerprint &= 0x7FFFFFFFFFFFFFFFULL;

      uint64_t Y = (uint64_t) X;

      Y = Y <= currentFingerprint ? currentFingerprint - Y : prime - (Y - currentFingerprint);

      return Y + sBit*(uint64_t)prime;
    }
  }

  /* Calculates the fingerprint of T[from, from + 2^exp) */
  uint64_t fingerprintExp(const uint64_t from, const int exp) {
    if (lce_unlikely(from == 0)) {
      return fingerprintTo((1 << exp)-1); // ie if exponent = 3, we want P[0..7];
    } else {
      uint128_t fingerprintToI = fingerprintTo(from - 1);
      uint128_t fingerprintToJ = fingerprintTo(from + (1 << exp) - 1);
      fingerprintToI *= powerTable[exp];
      fingerprintToI %= prime;

      return fingerprintToJ >= fingerprintToI ? (uint64_t) (fingerprintToJ - fingerprintToI) : (uint64_t)  (prime - (fingerprintToI - fingerprintToJ));
    }
  }

  /* Calculates the fingerprint of T[0..i] */
  uint64_t fingerprintTo(const uint64_t i) {
    uint128_t fingerprint = 0;
    int pad = ((i+1) & 7) * 8; // &7 is equal to % 8
    if(pad == 0) {
      // This fingerprints is already saved. We only have to remove the helping bit.
      return fingerprints[i/8] & 0x7FFFFFFFFFFFFFFFULL;
    }
    /* Add fingerprint from previous block */
    if (i > 7) {
      fingerprint = fingerprints[((i/8) - 1)] & 0x7FFFFFFFFFFFFFFFULL;
      fingerprint <<= pad;
    }
    /* Add relevant part of block */
    fingerprint += (getBlock(i/8) >> (64-pad));
    fingerprint %= prime;
    return (uint64_t) fingerprint;
  }

  /* Overwrites the n'th block with the fingerprint of the first n blocks. Because the Rabin-Karp fingerprint uses a rolling hash function, this is done in O(n) time. */
  void calculateFingerprints() {
    /* We run into problems with small endian systems, if we do not reverse the order of the characters.
     * I could not find a way to calculate fingerprints  without this "endian reversal".
     * Luckily this step is not that slow. */
    char * input = (char*) fingerprints;
    for(uint64_t i = 0; i < textLengthInBlocks; i++) {
      uint64_t paquet = *(uint64_t*)"\x1\x0\x0\x0\x0\x0\x0\x0";
      if(paquet == 1){
        //reverse
        char *f=&input[0], *b=&input[7];
        while(f<b){
          char tmp = *f;
          *f++ = *b;
          *b-- = tmp;
        }
      }
      input += 8;
    }
    uint64_t previousFingerprint = 0;
    uint64_t currentBlock = fingerprints[0];

    for (uint64_t i = 0; i < textLengthInBlocks; i++) {
      currentBlock = fingerprints[i];
      uint128_t X = previousFingerprint;
      X = X << 64;
      X = X + currentBlock;
      X = X % prime;
      previousFingerprint = (uint64_t) X;

      /* Additionally store if block > prime */
      if(currentBlock > prime) {
        X = X + 0x8000000000000000ULL;
      }
      fingerprints[i] = (uint64_t) X;
    }
  }

  /* Calculates the powers of 2. This supports LCE queries and reduces the time from polylogarithmic to logarithmic. */
  void calculatePowers() {
    unsigned int numberOfLevels = ((int) std::log2(textLengthInBlocks)) + 6; // +1 to round up and +4 because we need bit shifts by 1byte, 2byte, 4byte and 8byte
    //powerTable = new uint64_t[numberOfLevels];
    uint128_t X = 256;
    powerTable[0] = (uint64_t) X;
    for (unsigned int i = 1; i < numberOfLevels; i++) {
      X = (X*X) % prime;
      powerTable[i] = (uint64_t) X;
    }
  }

};