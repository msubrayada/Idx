//
//  MinHash_App.hpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 11/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#ifndef MinHash_App_hpp
#define MinHash_App_hpp

#include <stdio.h>
#include <fstream>
#include "Tools_App.hpp"

namespace MinHash_App{
    class MinHash
    {
    public:
        int n;
        int k;
        int** Params;

        MinHash(){}
        MinHash(int max, int hashfuncs);
        int Bits(int max);
        int* GetMinHash(int A[],int size);
        int Hash(int x,int HashFuncIndex);
        double Compare(int A[], int size, int B[]);
        void Save(std::ofstream* Output);
        void Load(std::ifstream* Input);
        void Load(std::stringstream* ss);
        std::string ToString();
    };
}
#endif /* MinHash_App_hpp */
