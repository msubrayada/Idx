//
//  Tools_App.cpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 15/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//
#include <iostream>
#include "Tools_App.hpp"
#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <limits>
#include <iterator>


using namespace std;
namespace Tools_App
{
    void Tools::ltrim(string &s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),std::not1(std::ptr_fun<int, int>(std::isspace))));
    }
    void Tools::rtrim(string &s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(),std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    }
    ostringstream Tools::Array2stream(int A[], int size)
    {
        ostringstream os;
        for (int i=0;i<size;++i )
            os<<A[i]<<" ";
        return os;
    }
    ostringstream Tools::Array2stream(double A[], int size)
    {
        ostringstream os;
        for (int i=0;i<size;++i )
        {
            os<<A[i];
            if (i<size-1)
                os<<",";
            
        }
        return os;
    }
    vector<string> Tools::Parse2Strings(string s)
    {
        vector<string> output;
        Tools::ltrim(s);
        Tools::rtrim(s);
        if (s.length()==0)
            return output;

        istringstream buf(s);
        istream_iterator<string> beg(buf), end;
        vector<string> tokens (beg, end);

        return tokens;
    }
    Result::Result(int k)
    {
        this->_K=k;
        //this->pairset= *new priority_queue<ItemPair>();
    }
    double Result::CoveringRadius(){
        if (this->Count()<this->K())
            return __DBL_MAX__;
        //cout<<(*(this->pairset.rbegin())).ObjID<<endl;
        return (*(this->pairset.rbegin())).Dist;
    }
    int Result::Count()
    {
        return this->pairset.size();
    }
    bool Result::Push(int docid, double d)
    {
        double covering=this->CoveringRadius();
        if (d>=covering)
            return false;
        if (this->Count()==this->K())
            this->pairset.erase(*(this->pairset.rbegin()));
        this->pairset.insert(ItemPair(docid,d));
        return true;
    }
    int Result::K()
    {
        return this->_K;
    }

}
