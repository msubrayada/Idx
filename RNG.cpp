//
//  RNG.cpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 14/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//
#include <stdlib.h>
#include <random>
#include <unordered_set>
#include "RNG.hpp"

using namespace std;

namespace Tools_App
{
    RNG::RNG()
    {
        this->mt=std::mt19937_64(this->rd());
    }
    int RNG::Next(int min, int max)
    {
        this->dist=std::uniform_int_distribution<int> (min,max);
        return this->dist(this->mt);
    }
    int* RandomSets::GetRandomSubset(int count, int n)
    {
        std::unordered_set<unsigned int> indices;
        RNG r;
        while (indices.size() < count)
            indices.insert(r.Next(0, n-1));
        int *S=new int[count];
        int i=0;
        for (unordered_set<unsigned int>::iterator it=indices.begin();it!=indices.end();++it)
            S[i++]=*it;
        return S;
    }
}
