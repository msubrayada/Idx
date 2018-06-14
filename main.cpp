//
//  main.cpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 11/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#include <iostream>
//#include "MinHash_App.hpp"
#include "RNG.hpp"
#include "VectorDB.hpp"
//#include "Tools_App.hpp"
#include "knnMHBucketsIndex.hpp"
#include "knnMHBucketsPerfect.hpp"

#include <fstream>
#include <chrono>

using namespace std;
using namespace MetricDB_App;
using namespace Tools_App;
using namespace MinHash_App;

int main(int argc, const char * argv[]) {


    VectorDB DB;
    DB.Build("colors2.ascii.vecs.1000", 10, 1000, 2,112);
//    DB.Load(DBT);
//    cout <<DB.ToString();
//    DB.Load(DB.ToString());
//    cout<<DB.ToString();

//    MinHash MH(1000,8);
//    cout<< MH.ToString();
//    MH.Load(sMHT );
//    cout<< MH.ToString();



    knnMHBucketsPerfect idx;

    idx.Build(L"colors2.ascii.vecs.1000", L"savefile.idx",112,112,1000, 200, 150, 30, 100, 6);
    
    std::ifstream file("savefile.idx");
    std::stringstream buffer;
    
    buffer << file.rdbuf();
    std::string str = buffer.str();
    std::wstring widestr = std::wstring(str.begin(), str.end());
    wchar_t* T = (wchar_t*)widestr.c_str();
    
    int** HQ=idx.SearchKNN_Params(DB.Get(100),T);
    for(int i=0;i<idx.Buckets;++i)
        cout<<  Tools:: Array2stream(HQ[i]  ,idx.MiniBuckets).str()<<endl;
    

//    knnMHBuckets idx2;



    //auto begin = chrono::high_resolution_clock::now();
//    cout<<Tools:: Array2stream( idx2.SearchKNN_Params(DB.Get(100),T) ,16).str()<<endl;
    //  cout<<  Tools:: Array2stream( idx2.SearchKNN_Params(DB.Get(100),T) ,16).str()<<endl;
    //  cout<<  Tools:: Array2stream( idx2.SearchKNN_Params(DB.Get(100),T) ,16).str()<<endl;


//    cout<<  Tools:: Array2stream( idx2.SearchKNN(DB.Get(100),DBT,MHT,4,2,4) ,2).str()<<endl;

//    cout<<  Tools:: Array2stream( idx2.SearchKNN(DB.Get(100),DBT,MHT,4,2,4) ,2).str()<<endl;
//    auto end = chrono::high_resolution_clock::now();
//    auto dur = end - begin;
//    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
//    cout << "Search Time: "<<ms << endl;


    return 0;
}
