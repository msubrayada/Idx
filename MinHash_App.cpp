//
//  MinHash_App.cpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 11/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <iostream>
#include <limits.h>
#include "RNG.hpp"
#include "MinHash_App.hpp"
#include <climits>
#include <sstream>

using namespace std;
using namespace Tools_App;
namespace MinHash_App
{
    void MinHash::Save(std::ofstream* Output)
    {
        Output->write(reinterpret_cast<char*>(&this->n),sizeof(this->n));
        Output->write(reinterpret_cast<char*>(&this->k),sizeof(this->k));
//        cout<<this-> n << " : "<< this->k<< endl;
        for (int i=0;i<this->k;++i)
        {
            Output->write(reinterpret_cast<char*>(&this->Params[i][0]),sizeof(this->Params[i][0]));
            Output->write(reinterpret_cast<char*>(&this->Params[i][1]),sizeof(this->Params[i][1]));
//            cout<<this->Params[i][0]<<"," <<this->Params[i][1]<<endl;
        }
    }
    void MinHash::Load(std::ifstream* Input)
    {
        if (!Input)
            cout<< "Fail";
        Input->read((char*)&this->n, sizeof(this->n));
        Input->read((char*)&this->k, sizeof(this->k));
        this->Params=new int*[this->k];
//        cout<<this-> n << " : "<< this->k<< endl;
        for (int i=0;i<this->k;++i)
        {
            this->Params[i]=new int[2];
            Input->read((char*)&this->Params[i][0],sizeof(this->Params[i][0]));
            Input->read((char*)&this->Params[i][1],sizeof(this->Params[i][1]));

//            cout<<this->Params[i][0]<<"," <<this->Params[i][1]<<endl;
        }
    }
    string MinHash::ToString()
    {
        stringstream ss;
        ss<<this->n<<"\n"<<this->k<<'\n';
        for (int i=0;i<this->k;++i)
        {
            ss<<this->Params[i][0]<< " "<<this->Params[i][1]<<'\n';
        }
        return ss.str();
    }
    void MinHash::Load(std::stringstream* ss)
    {
        string line;
        getline(*ss, line);
        this->n=stoi(line);
        getline(*ss, line);
        this->k=stoi(line);
        this->Params=new int*[this->k];
        for(int i=0;i<this->k;++i)
        {
            this->Params[i]=new int[2];
            *ss>>this->Params[i][0];
            *ss>>this->Params[i][1];
            getline(*ss,line);
        }

    }
    MinHash::MinHash(int max,int hashfuncs)
    {
        this->n=Bits(max);
        //cout<< "Bits: "<< n<<"\n";
        this->k=hashfuncs;
        this->Params=new int*[k];
        RNG rnd;
        int maxb=1<<(32-this->n);
        for (int i=0;i<k;++i)
        {
            this->Params[i]=new int[2];
            int a=2;
            while(a%2==0)
                a=rnd.Next(1,1<<31);
            this->Params[i][0]=a;
            int b=rnd.Next(1,maxb);
            this->Params[i][1]=b;
            //cout<< "(" << Params[i][0] << ","<< Params[i][1]<< ")\n";
        }

    }
    int* MinHash::GetMinHash(int A[], int size)
    {
        int* minhash=new int[this->k];
        for (int i=0;i<this->k;++i){
            int min=INT_MAX;
            int minelem=-1;

            for (int j =0;j<size;++j)
            {
                int val=Hash(A[j],i);
                //cout<< val<< " ";
                if (val<min){
                    min=val;
                    minelem=A[j];
                }
            }
            minhash[i]=minelem;
            //cout<< minelem<<" ";
        }
        return minhash;
    }
    int MinHash::Hash(int x, int HashFuncIndex){
        int v=(this->Params[HashFuncIndex][0]*x+this->Params[HashFuncIndex][1])>>(32-this->n);
        return v;
    }

    int MinHash::Bits(int max)
    {
        return (int)trunc(log2(max))+1;
    }
    double MinHash::Compare(int *A, int size, int *B)
    {
        int c=0;
        for (int i=0;i<size;++i){
            if (A[i]==B[i])
                c++;
        }
        //cout<< "Coinciden: "<< c << "\n";
        return (1.0*c)/this->k;

    }

}

