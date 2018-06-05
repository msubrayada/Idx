//
//  MetricDB.hpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 14/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#ifndef MetricDB_hpp
#define MetricDB_hpp

#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

namespace MetricDB_App
{

    class MetricDB{
    protected:
        long  numdist;
        //int  Dimension;
        //float P;
        int N;
    public:

        //string Name;
        //double** Vectors;

        //MetricDB();
        virtual double Dist(double* a, double* b)=0;
        virtual double* Parse(string s)=0;
        virtual double* Get(int docid)=0;
        virtual int Count()=0;
        virtual long NumberDistances()=0;
        virtual void Load(ifstream* Input)=0;
        virtual void Save(ofstream* Output)=0;
        virtual void Load(stringstream* ss)=0;
        virtual string ToString()=0;




        //void Build(string file,int dim, int n,float p);
        //virtual void Build(string inputname)=0;
        virtual MetricDB* Subspace(int* subset,int size)=0;
    };

}
#endif /* MetricDB_hpp */
