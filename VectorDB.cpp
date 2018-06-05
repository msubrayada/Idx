//
//  VectorDB.cpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 17/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#include "VectorDB.hpp"
#include "math.h"
#include "Tools_App.hpp"
#include <sstream>
#include <vector>
#include <iostream>
#include <iterator>

using namespace std;
using namespace  Tools_App;
namespace MetricDB_App {
    VectorDB::VectorDB()
    {
        this->P=2;
        this->Dimension=2;
        this->numdist=0;
        this->Len_Vector=2;
    }
    void VectorDB::Save(std::ofstream *Output)
    {
        Output->write(reinterpret_cast<char*>(&this->N),sizeof(this->N));
        Output->write(reinterpret_cast<char*>(&this->Dimension),sizeof(this->Dimension));
        Output->write(reinterpret_cast<char*>(&this->P),sizeof(this->P));
        Output->write(reinterpret_cast<char*>(&this->Len_Vector),sizeof(this->Len_Vector));


        for(int i=0;i<this->N;++i)
        {
            for (int j=0;j<this->Len_Vector;++j)
                Output->write(reinterpret_cast<char*>(&this->Vectors[i][j]),sizeof(this->Vectors[i][j]));
        }

//        unsigned long l=this->Name.size();
//        cout << "l "<< l<<endl;
//        Output->write(reinterpret_cast<char*>(&l),sizeof(unsigned long));
//        Output->write(reinterpret_cast<char*>(&this->Name),sizeof(this->Name));
    }
    void VectorDB::Load(std::ifstream * Input)
    {
        this->numdist=0;
        Input->read((char*)&this->N, sizeof(this->N));
        Input->read((char*)&this->Dimension, sizeof(this->Dimension));
        Input->read((char*)&this->P, sizeof(this->P));
        Input->read((char*)&this->Len_Vector, sizeof(this->Len_Vector));

        this->Vectors=new double*[this->N];
        for(int i=0;i<this->N;++i)
        {
            this->Vectors[i]=new double[this->Len_Vector];
            for (int j=0;j<this->Len_Vector;++j)
                Input->read((char*)&this->Vectors[i][j], sizeof(this->Vectors[i][j]));
        }
//        unsigned long l=0;
//        Input->read((char*)&l, sizeof(unsigned long));
//
//        Input->read((char*)&this->Name, l);
    }
    string VectorDB::ToString()
    {
        stringstream ss;
        ss<<this->Dimension<< "\n" << this->N << "\n" <<this->P<<'\n'<<this->Len_Vector<<'\n';
        for(int i=0;i<this->N;i++)
        {
            for (int j=0;j<this->Len_Vector;++j)
                ss<<this->Vectors[i][j]<< " ";
            ss<<'\n';
        }
        return ss.str();
    }
    void VectorDB::Load(std::stringstream* ss)
    {
        string line;

        *ss>>this->Dimension;
        getline(*ss, line);
        *ss>>this->N;
        getline(*ss, line);
        *ss>>this->P;
        getline(*ss, line);
        *ss>>this->Len_Vector;
        getline(*ss, line);

        this->Vectors=new double*[this->N];
        for(int i=0;i<this->N;++i)
        {
            this->Vectors[i]=new double[this->Dimension];
            for (int j=0;j<this->Dimension;++j)
            {
//                getline(*ss,line,' ');
//                this->Vectors[i][j]=stof(line);
                *ss>>this->Vectors[i][j];
            }
            getline(*ss,line);
        }
    }
//    void VectorDB::Build(string headersname)
//    {
////        this->Name=headersname;
//        ifstream infile(headersname);
//        if (!infile)
//            cout << "Problema al leer archivo."<<endl;
//        string line="";
//        getline(infile,line);
//        vector<string> field=Tools::Parse2Strings(line);
//        this->Dimension=stoi(field[0]);
//        this->N=stoi(field[1]);
//        this->P=stof(field[2]);
//        this->Vectors=new double*[this->N];
//        cout << "dim:"<<this->Dimension<<" size:"<< this->N<< " P:"<<this->P<< endl;
//        cout<<"** Reading vectors from file " << headersname << ".vecs";
//        Build(headersname+".vec", this->Dimension, this->N, this->P);
//        infile.close();
//    }
    double* VectorDB::Get(int docid)
    {
        return this->Vectors[docid];
    }
    int VectorDB::Count()
    {
        return this->N;
    }
    void VectorDB::Build(std::string file, int dim, int n, float p,int len_vector)
    {
        this->Dimension=dim;
        this->N=n;
        this->P=p;
        this->Len_Vector=len_vector;

        this->Vectors=new double*[n];
        ifstream infile (file,std::ifstream::in);
        string line;
        int i=0;
        if (!infile)
            cout << "Problema al leer archivo .vec"<<endl;
        while (getline(infile,line))
        {
            if (i%100000==0)
            {
                cout << "*\t\t* reading "<<i+1 <<" of "<< n  << "\n";
            }
            this->Vectors[i]=Parse(line);
            i++;
        }
        cout << "Finished building DB.\n";
        infile.close();
    }
    MetricDB* VectorDB::Subspace(int *subset,int size)
    {
        VectorDB* S;
        S=new VectorDB();
        S->Dimension=this->Dimension;
        S->N=size;
        S->P=this->P;
        S->Len_Vector=this->Len_Vector;
        S->Vectors=new double*[size];
        for (int i=0;i<size;++i)
        {
            S->Vectors[i]=this->Vectors[subset[i]];
        }
        return S;
    }
    double VectorDB::Dist(double *a, double *b)
    {
        double d=0;
        this->numdist++;
        if (this->P==2){
            for(int i=0;i<this->Dimension;++i)
                d+=pow( a[i]-b[i],2);
            d=sqrt(d);
        }
        return d;
    }
    long VectorDB::NumberDistances(){
        return this->numdist;
    }
    double* VectorDB::Parse(string s)
    {
        Tools::ltrim(s);
        Tools::rtrim(s);
        if (s.length()==0)
            return new double[0];
        double* output=new double[this->Len_Vector];
        stringstream buf(s);
        istream_iterator<string> beg(buf), end;
        vector<string> tokens (beg, end);

        for (int i=0;i<tokens.size();++i){
            double d=stod(tokens[i]);
            output[i]=d;
            //cout<< '"' << tokens[i] << '"' << '\n';
        }
        return output;
    }

}
