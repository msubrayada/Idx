//
//  knnMHBucketsPerfect.hpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 08/06/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#ifndef knnMHBucketsPerfect_hpp
#define knnMHBucketsPerfect_hpp

#include <stdio.h>
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
#include "MinHash_App.hpp"


using namespace std;
using namespace MetricDB_App;
using namespace Tools_App;


namespace MinHash_App
{
    class knnMHBucketsPerfect
    {
    public:
        MetricDB *REFS;
        MinHash *MH;
        unordered_map<int, vector<int>>* Tables;
        int R;
        int K;
        int Errors;
        int NumMH;
        int MiniBuckets;
        int Buckets;
        //MetricDB *DB;

        knnMHBucketsPerfect();
        void Build(MetricDB* db,int refs[],int R,int k, int errors, int mh, int minibuckets);
        const wchar_t* Build(wchar_t* file,wchar_t* savefile, int dim,int len_vec,int size,int R,int k, int errors, int mh, int minibuckets);
        int* GetHashes(int MH[],int Bucket);
        int* GetKnn(double Ob[]);
        Result SearchKNN(double* q, int k,Result res);
//        int* SearchKNN(double* q,wchar_t* loadfile);
        int** SearchKNN_Params(double* q, wchar_t* params);
        //void Load(ifstream* Input);
        //void Save(ofstream* Output);

    };
}
#endif /* knnMHBucketsPerfect_hpp */
