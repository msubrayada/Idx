//
//  VectorDB.hpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 17/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#ifndef VectorDB_hpp
#define VectorDB_hpp

#include <stdio.h>
#include <string>
#include "MetricDB.hpp"
#include <fstream>

using namespace std;

namespace MetricDB_App
{
    class VectorDB : public MetricDB{
        int  Dimension;
        float P;
        int Len_Vector;

    public:
        double** Vectors;
        VectorDB();

        //vector<string> Parse2Strings(string s);
        //void Build(string inputname);
        double* Get(int docid);
        int Count();
        void Build(string file,int dim, int n,float p,int len_vector);
        virtual double Dist(double* a, double* b);
        long NumberDistances();
        double* Parse(string s);
        virtual void Load(ifstream* Input);
        virtual void Save(ofstream* Output);
        MetricDB* Subspace(int* subset,int size);
        virtual void Load(stringstream* ss);
        virtual string ToString();


    };
}
#endif /* VectorDB_hpp */
