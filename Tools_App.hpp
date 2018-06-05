//
//  Tools_App.hpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 15/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#ifndef Tools_App_hpp
#define Tools_App_hpp

#include <stdio.h>
#include <string>
#include <set>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>


using namespace std;

namespace Tools_App
{
    struct ItemPair{
    public:
        int ObjID;
        double Dist;
        ItemPair(int objID,double dist):ObjID(objID),Dist(dist){};
        bool operator < (ItemPair other) const
        {
            double cmp=Dist-other.Dist;
            if (cmp==0)
                return ObjID<other.ObjID;
            return Dist<other.Dist;
        }
    };
    struct ItemPairCompare{
        bool operator() (const ItemPair& A, const ItemPair& B) const
        {
            if (A.Dist==B.Dist)
                return A.ObjID<B.ObjID;
            return A.Dist<B.Dist;
        }
    };
    
    class Tools{
    public:
        static void ltrim (string &s);
        static void rtrim (string &s);
        static ostringstream Array2stream(int A[], int size);
        static ostringstream Array2stream(double A[], int size);
        static vector<string> Parse2Strings(string s);
    };
    
    class Result{
    public:
        int _K;
        set<ItemPair,ItemPairCompare> pairset;
        Result(int k);
        double CoveringRadius();
        int Count();
        bool Push(int docid, double d);
        int K();
        
    };
}
#endif /* Tools_App_hpp */
