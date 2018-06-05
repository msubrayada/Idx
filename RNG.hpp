//
//  RNG.hpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 14/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#ifndef RNG_hpp
#define RNG_hpp

#include <random>

#include <stdio.h>


namespace Tools_App
{
    
    class RNG{
    public:
        std::random_device rd;
        std::mt19937_64 mt;
        std::uniform_int_distribution<int> dist;
        RNG();
        int Next(int min, int max);
    };
    class RandomSets{
    public:
        static int* GetRandomSubset(int count, int n);
    };
    
}
#endif /* RNG_hpp */
