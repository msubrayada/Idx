//
//  knnMHBucketsIndex.hpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 14/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#ifndef knnMHBucketsIndex_hpp
#define knnMHBucketsIndex_hpp

#include <stdio.h>
#include "MinHash_App.hpp"
#include <string>
#include <sstream>
#include <istream>
#include "MetricDB.hpp"
#include "VectorDB.hpp"
#include "Tools_App.hpp"
#include "RNG.hpp"
#include <unordered_map>
#include <vector>
#include <chrono>


//using namespace MinHash_App;
using namespace std;
using namespace MetricDB_App;
using namespace Tools_App;

namespace MinHash_App
{
    class knnMHBuckets
    {
    public:
        MetricDB *REFS;
        //int **HASHES;
        MinHash *MH;
        unordered_map<int, vector<int>>* Tables;
        int R;
        int K;
        int BucketsLength;
        int Buckets;
        //string Name;
        //MetricDB *DB;

        knnMHBuckets();
        void Build(MetricDB* db,int refs[],int R,int k, int bucketlen, int buckets);
        const wchar_t* Build(wchar_t* file,wchar_t* savefile, int dim,int len_vec,int size,int R,int k, int bucketlen, int buckets);
        int* GetHashes(int MH[]);
        int* GetKnn(double Ob[]);
//        Result SearchKNN(double* q, int k,Result res);
        int* SearchKNN(double* q,wchar_t* loadfile);
        int* SearchKNN_Params(double* q, wchar_t* params);
        void Load(ifstream* Input);
        void Save(ofstream* Output);

    };
}
#endif /* knnMHBucketsIndex_hpp */


