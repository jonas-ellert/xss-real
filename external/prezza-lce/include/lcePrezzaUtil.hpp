/*******************************************************************************
 *
 * This version of Nicola Prezzas LCE data structure was implemented by
 * Alexander Herlez, see https://github.com/herlez/lce-test for more info
 *
 ******************************************************************************/

#pragma once
#include <iostream> 
#include <inttypes.h>
#include <cstdio>
#include <fstream>

/* This header contains useful methods, which are used for my implementation of Prezza's lce data structure */



class util{

__extension__ typedef unsigned __int128 uint128_t;

public:

    /* Prints the 64-bit long integer in HEX*/
    static void printInt64(uint64_t n) {
            printf("%016lX\n", n);
    }
    
    /* Prints the 128-bit long integer in HEX*/
    static void printInt128(uint128_t n) {
        uint64_t * numberHalf = new uint64_t[2];
        numberHalf = (uint64_t*) &n;
        printf("%016lX%016lX\n", numberHalf[1], numberHalf[0]);
    }
    

    /* Returns the length of the file in bytes */
    static uint64_t calculateSizeOfInputFile(std::fstream* stream) {
        uint64_t fileStart, fileEnd;
        stream->seekg(0, std::ios::beg);
        fileStart = stream->tellg();
        stream->seekg(0, std::ios::end);
        fileEnd = stream->tellg();
        if ((fileEnd - fileStart) == 0) {
			return 0;
		} else {
		return (fileEnd - fileStart - 1); //There is a special symbol at the end of a file. We don't want to consider this.
		}
    }
    
    /* Returns the length of the file in bytes */
    static uint64_t calculateSizeOfInputFile(std::string path) {
        std::ifstream input(path, std::ios::in|std::ios::binary);
		util::inputErrorHandling(&input);
		return calculateSizeOfInputFile(&input);
    }
    
    /* Returns a prime number from a precalculated set */
    static uint128_t getLow64BitPrime() {
		
		srand(time(0));
		const uint128_t primes[] = { 0x800000000000001dULL, 0x8000000000000063ULL, 0x800000000000007bULL, 0x8000000000000083ULL, 0x800000000000009bULL };
		unsigned const int numberOfPrimes = 5;
		
		
		uint128_t prime = primes[(rand() % numberOfPrimes)];
        return primes[1];
		return prime;
	}
	
	static void loadFile(std::ifstream* text, char data[], uint64_t fileSizeInByte) {
		text->seekg(0);
		text->read(data, fileSizeInByte);
	}
	
    /* Returns the length of the file in bytes */
    static uint64_t calculateSizeOfInputFile(std::ifstream* stream) {
        uint64_t fileStart, fileEnd;
        stream->seekg(0, std::ios::beg);
        fileStart = stream->tellg();
        stream->seekg(0, std::ios::end);
        fileEnd = stream->tellg();
        if ((fileEnd - fileStart) == 0) {
			return 0;
		} else {
		return (fileEnd - fileStart);
		}
    }
    
    /* Generates a random Index between 0 and max */
    static uint64_t randomIndex(uint64_t max) {
		uint64_t randIndex = rand();
		randIndex <<= 32;
		randIndex += rand();
	
		randIndex = randIndex % max;
		if(max == randIndex) { std::cout << "?????????????????????????????"; }
		return randIndex;
	}
    
    
    /* Checks the state flags of fstream  */
    static void inputErrorHandling(std::fstream* stream) {
        if(stream->good()) {
            return;
        } else {
            std::cerr << "Error in fstream\n";
        }
        
        if(!stream->is_open()) {
            std::cerr << "File not found\n";
            exit(-1);
        }
        
        if(stream->eof()) {
            std::cout << "End of file reached\n";
            exit(0);
        }
        
        if(stream->bad()) {
            std::cerr << "Bad fstream\n";
            exit(-1);
        }
        
        if(stream->fail()) {
            std::cerr << "fsteam failed\n";
            exit(-1);
        } 
    }
	
	/* Checks the state flags of fstream  */
	static void inputErrorHandling(std::ifstream* stream) {
        if(stream->good()) {
            return;
        } else {
            std::cerr << "Error in fstream\n";
        }
        
        if(!stream->is_open()) {
            std::cerr << "File not found\n";
            exit(-1);
        }
        
        if(stream->eof()) {
            std::cout << "End of file reached\n";
            exit(0);
        }
        
        if(stream->bad()) {
            std::cerr << "Bad fstream\n";
            exit(-1);
        }
        
        if(stream->fail()) {
            std::cerr << "fsteam failed\n";
            exit(-1);
        } 
    }
    
};

#undef test
