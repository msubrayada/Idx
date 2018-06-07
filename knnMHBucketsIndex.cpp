//
//  knnMHBucketsIndex.cpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 14/05/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#include "knnMHBucketsIndex.hpp"

using namespace std;
using namespace MetricDB_App;

namespace MinHash_App
{
    knnMHBuckets::knnMHBuckets()
    {}
    void knnMHBuckets::Save(std::ofstream *Output)
    {
        Output->write(reinterpret_cast<char*>(&this->R),sizeof(this->R));
        Output->write(reinterpret_cast<char*>(&this->K),sizeof(this->K));
        Output->write(reinterpret_cast<char*>(&this->BucketsLength),sizeof(this->BucketsLength));
        Output->write(reinterpret_cast<char*>(&this->Buckets),sizeof(this->Buckets));

        this->REFS->Save(Output);
//        for (int i=0;i<this->R;++i)
//        {
//            Output->write(reinterpret_cast<char*>(&this->REFS[i]),sizeof(this->REFS[i]));
//        }


        this->MH->Save(Output);
        //this->DB->Save(Output);
       // unsigned long l=this->Name.size();

        //Output->write(reinterpret_cast<char*>(&l),sizeof(unsigned long));
        //Output->write(reinterpret_cast<char*>(&this->Name),sizeof(this->Name));

    }
    void knnMHBuckets::Load(std::ifstream *Input)
    {
        Input->read((char*)&this->R, sizeof(this->R));
        Input->read((char*)&this->K, sizeof(this->K));
        Input->read((char*)&this->BucketsLength, sizeof(this->BucketsLength));
        Input->read((char*)&this->Buckets, sizeof(this->Buckets));

        this->REFS=new VectorDB();
        this->REFS->Load(Input);

//        this->REFS=new int[R];
//        for (int i=0;i<this->R;++i)
//        {
//            Input->read((char*)&this->REFS[i], sizeof(this->REFS[i]));
//        }
        this->MH=new MinHash();
        this->MH->Load(Input);
        //this->DB=new VectorDB();
        //this->DB->Load(Input);
        //unsigned long l=0;

        //Input->read((char*)&l, sizeof(unsigned long));
        //Input->read((char*)&this->Name, l);
        //cout << this->Name<<endl;

    }

    const wchar_t* knnMHBuckets::Build(wchar_t* file,wchar_t* savefile,int dim,int len_vec,int size, int R, int k, int bucketlen, int buckets)
    {

        //this->Name="knnMHBuckets";
        wstring ws(file);
        string strfile(ws.begin(),ws.end());
        wstring ws2(savefile);
        string strsavefile(ws2.begin(),ws2.end());

        VectorDB *db=new VectorDB;
        db->Build(strfile,dim,size,2,len_vec);
        int* refs=RandomSets::GetRandomSubset(R, size);
        this->Build(db,refs,R,k,bucketlen,buckets);
        //ofstream *output;
        //output=new ofstream();
        //output->open(strsavefile,ios::out|ios::binary);
        //this->Save(output);
        //output->close();
        cout << "Preparing output ..."<<endl;

        std::ofstream out(strsavefile);
        out << this->K << "\n";
        out << this->BucketsLength <<"\n";
        out << this->Buckets <<'\n';
        out << this->REFS->ToString();
        out << this->MH->ToString();
        out.close();


        stringstream ss;
        ss << "[";
        for(int i=0;i<buckets;++i)
        {
            ss << "{";
            int conax=0;
            for(auto it =this->Tables[i].begin();it!=this->Tables[i].end();++it)
            {
                conax=conax+1;
                ss<<"\""<< it->first<< "\":[";
                for (auto iter=it->second.begin();iter!=it->second.end();++iter)
                {
                    ss << *iter<< ",[" << Tools::Array2stream(db->Get(*iter), len_vec).str()<<"]"  ;
                    if(iter<it->second.end()-1)
                        ss<<",";
                }
                ss << "]" << endl;
                if(conax<=this->Tables[i].size()-1)
                    ss << ",";
            }
            ss << "}";
            if(i<buckets-1)
                ss << ",";
        }
        ss << "]";

        this->Tables=nullptr;
        string st=ss.str();
        wstring *wss;
        wss=new wstring(st.begin(),st.end());
        //        wstring str =wstring(ss.);
//        str<<ss;
        cout << "Done building!"<<endl;
        return wss->c_str() ;
    }
    void knnMHBuckets::Build(MetricDB* db, int *refs,int r, int k, int bucketlen, int buckets)
    {
        //this->DB=db;
        this->REFS=db->Subspace(refs, r);
        this->K=k;
        this->R=r;
        this->BucketsLength=bucketlen;
        this->Buckets=buckets;
        //this->DB->Count();
        int ** KNN=new int*[db->Count()];
        cout<< "Building knn's ..."<<endl;
        // Find the knn

        auto begin = chrono::high_resolution_clock::now();
#pragma omp parallel for schedule(dynamic)
        for (int i=0;i<db->Count();++i)
        {
            KNN[i] = GetKnn (db->Get(i));
        }
        auto end = chrono::high_resolution_clock::now();
        auto dur = end - begin;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        cout << "Knn Time: "<<ms << endl;
        
        cout<< "Building MinHashes ..."<< endl;

        // Get minhashes
        int ** HASHES=new int*[db->Count()];
        this->MH=new MinHash(this->REFS->Count(),bucketlen*buckets);

#pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < db->Count(); ++i) {
            HASHES[i]=GetHashes( MH->GetMinHash(KNN[i],k));
//            if (i%1000==0 )
//                cout<< "ID:"<<i<< " -> "<<Tools::Array2stream(HASHES[i], buckets).str()<<endl;
        }

        cout << "Storing into dictionary ..."<<endl;
        begin = chrono::high_resolution_clock::now();
        this->Tables=new unordered_map<int, vector<int>> [buckets];
        for(int i = 0; i < buckets; ++i)
           this->Tables[i]=*new unordered_map<int, vector<int>> ();

#pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < db->Count(); ++i) {
            for (int j = 0; j < buckets; ++j) {
                int hash = HASHES [i][j];
                unordered_map<int, vector<int>>::iterator got=this->Tables[j].find(hash);
                if (got!=this->Tables[j].end()) // ya hay
                {
                    got->second.push_back(i);
                }
                else // nuevo valor
                {
                    vector<int> L;
                    L.push_back(i);
                    this->Tables[j].insert({hash,L});
                }
            }
        }
        end = chrono::high_resolution_clock::now();
        dur = end - begin;
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        cout << "Storing into dictionary Time: "<<ms << endl;
        delete[] HASHES;
    }

    int* knnMHBuckets::GetHashes(int *MH)
    {
        //cout<<"MH:"<< Tools::Array2stream(MH, this->K).str()<< endl;
        int* H=new int[this->Buckets];
        for(int i=0;i<this->Buckets;++i)
        {
            int start=i*this->BucketsLength;
            H[i]=this->BucketsLength;
            for (int j=0;j<this->BucketsLength;++j)
            {
                H[i]=(H[i]*2011+MH[start+j]);
            }
        }
        return H;
    }
    int* knnMHBuckets::GetKnn(double *Ob)
    {
        int* knn=new int[this->K];
        Result res(this->K);
        for(int i=0;i<this->R;++i)
        {
            res.Push(i, this->REFS->Dist(Ob, this->REFS->Get(i)));
        }
        int j=0;
        for(ItemPair IP:res.pairset)
            knn[j++]=IP.ObjID;
        return knn;
    }
//    Result knnMHBuckets::SearchKNN(double *q, int k, Tools_App::Result res)
//    {
//        int* knnq=GetKnn(q);
//        int* mhq=MH->GetMinHash(knnq, this->K);
//        int* hq=GetHashes(mhq);
//        for (int i=0;i<this->Buckets;++i){
//            int hash =hq[i];
//            unordered_map<int, vector<int>>::iterator got=this->Tables[i].find(hash);
//            if (got!=this->Tables[i].end())
//            {
//                for(vector<int>::const_iterator it=got->second.begin() ;it!=got->second.end();++it)
//                {
//                    res.Push(*it,this->REFS->Dist(q, this->DB->Get(*it)));
//                    //cout<<*it<<", ";
//                }
//                //cout<<endl;
//            }
//        }
//        return res;
//    }
    int* knnMHBuckets::SearchKNN(double *q,wchar_t* loadfile )
    {
//        auto begin = chrono::high_resolution_clock::now();
        wstring ws(loadfile);
        string strloadfile(ws.begin(),ws.end());
        ifstream* input;
        input=new ifstream();
        input->open(strloadfile,ios::in|ios::binary);
        this->Load(input);
        input->close();
//        auto end = chrono::high_resolution_clock::now();
//        auto dur = end - begin;
//        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
//        cout << "Loading Time: "<<ms << endl;

        int* knnq=GetKnn(q);
        int* mhq=MH->GetMinHash(knnq, this->K);
        int* hq=GetHashes(mhq);
        return hq;
    }
    int* knnMHBuckets::SearchKNN_Params(double *q,  wchar_t *params)
    {
//        auto begin = chrono::high_resolution_clock::now();

        wstring ws1(params);
        string strparams(ws1.begin(),ws1.end());

        stringstream* ss;
        ss=new stringstream(strparams);
        string line;

        getline(*ss, line);
        this->K=stoi(line);
        getline(*ss, line);
        this->BucketsLength=stoi(line);
        getline(*ss, line);
        this->Buckets=stoi(line);




        this->REFS=new VectorDB();

        this->REFS->Load(ss);


        this->MH=new MinHash();
        this->MH->Load(ss);
        this->R=this->REFS->Count();

//        auto end = chrono::high_resolution_clock::now();
//        auto dur = end - begin;
//        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
//        cout << "Loading Time: "<<ms << endl;



        int* knnq=GetKnn(q);
        int* mhq=MH->GetMinHash(knnq, this->K);
        int* hq=GetHashes(mhq);
        return hq;
    }
extern "C"{
    knnMHBuckets* HashesExtractor_new(){return new knnMHBuckets();}
    const wchar_t * HashesExtractor_Bulkbuild(knnMHBuckets* foo,wchar_t * mfilename,wchar_t* savefile,int arg1,int arg2,int arg3,int arg4,int arg5,int arg6,int arg7){return foo->Build(mfilename,savefile,arg1,arg2,arg3,arg4, arg5,arg6,arg7);}
    int*  HashesExtractor_Build_query(knnMHBuckets* foo,double* query,wchar_t* loadfile){return foo->SearchKNN_Params(query,loadfile);}
    }
}
