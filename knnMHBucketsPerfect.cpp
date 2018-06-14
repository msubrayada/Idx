//
//  knnMHBucketsPerfect.cpp
//  KnnMHIndex
//
//  Created by Luis Guillermo on 08/06/18.
//  Copyright © 2018 Luis Guillermo. All rights reserved.
//

#include "knnMHBucketsPerfect.hpp"
using namespace std;
using namespace MetricDB_App;
using namespace Tools_App;


namespace MinHash_App
{
    knnMHBucketsPerfect::knnMHBucketsPerfect()
    {}


    const wchar_t* knnMHBucketsPerfect::Build(wchar_t* file,wchar_t* savefile,int dim,int len_vec,int size, int R, int k, int errors,int mh, int minibuckets)
    {

        //this->Name="knnMHBuckets";
        wstring ws(file);
        string strfile(ws.begin(),ws.end());
        wstring ws2(savefile);
        string strsavefile(ws2.begin(),ws2.end());

        VectorDB *db=new VectorDB;
        db->Build(strfile,dim,size,2,len_vec);
        int* refs=RandomSets::GetRandomSubset(R, size);
        this->Build(db,refs,R,k,errors ,mh,minibuckets);
//        ofstream *output;
//        output=new ofstream();
//        output->open(strsavefile,ios::out|ios::binary);
//        this->Save(output);
//        output->close();
        cout << "Preparing output ..."<<endl;

        std::ofstream out(strsavefile);
        out << this->K << "\n";
        out << this->Errors <<"\n";
        out << this->NumMH <<'\n';
        out << this->MiniBuckets <<'\n';
        out << this->REFS->ToString();
        out << this->MH->ToString();
        out.close();


        stringstream ss;
        ss << "[";
        for(int i=0;i<this->Buckets*this->MiniBuckets;++i)
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
            if(i<this->Buckets-1)
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
    void knnMHBucketsPerfect::Build(MetricDB* db,int refs[],int r,int k, int errors, int mh, int minibuckets)

    {
        //this->DB=db;
        this->REFS=db->Subspace(refs, r);
        this->K=k;
        this->R=r;
        this->Errors=errors;
        this->NumMH=mh;
        this->MiniBuckets=minibuckets;
        this->Buckets=(this->Errors+1)/2;
        //this->DB->Count();
        int ** KNN=new int*[db->Count()];
        cout<< "Building knn's ..."<<endl;
        // Find the knn

        auto begin = chrono::high_resolution_clock::now();
#pragma omp parallel for schedule(dynamic)
        for (int i=0;i<db->Count();++i)
        {
            if (i%100000==0)
            {
                cout << "*\t\t* knn "<<i+1 <<" of "<< db->Count()  << "\n";
            }
            KNN[i] = GetKnn (db->Get(i));
        }
        auto end = chrono::high_resolution_clock::now();
        auto dur = end - begin;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        cout << "Knn Time: "<<ms << endl;

        cout<< "Building MinHashes ..."<< endl;

        // Get minhashes
        int *** HASHES=new int**[db->Count()];
        int ** MinHASHES=new int*[db->Count()];
        this->MH=new MinHash(this->REFS->Count(),this->NumMH);

#pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < db->Count(); ++i) {
            MinHASHES[i] = MH->GetMinHash(KNN[i],k);
            HASHES[i]=new int*[Buckets];
            for(int j=0;j<this->Buckets;++j)
                HASHES[i][j]=GetHashes(MinHASHES[i], j);
        }
        cout << "Storing into dictionary ..."<<endl;
        begin = chrono::high_resolution_clock::now();

        this->Tables=new unordered_map<int, vector<int>> [this->Buckets*this->MiniBuckets];
        for(int i = 0; i <this->Buckets*this->MiniBuckets; ++i)
            this->Tables[i]=*new unordered_map<int, vector<int>> ();

#pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < db->Count(); ++i)
        {
            for (int j = 0; j < Buckets; ++j) {
                for (int s=0;s<this->MiniBuckets; ++s)
                {
                    int hash = HASHES [i][j][s];
                    int dict=j*this->MiniBuckets+s;
                    unordered_map<int, vector<int>>::iterator got=this->Tables[dict].find(hash);
                    if (got!=this->Tables[dict].end()) // ya hay
                    {
                        got->second.push_back(i);
                    }
                    else // nuevo valor
                    {
                        vector<int> L;
                        L.push_back(i);
                        this->Tables[dict].insert({hash,L});
                    }
                }
            }
        }
        end = chrono::high_resolution_clock::now();
        dur = end - begin;
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        cout << "Storing into dictionary Time: "<<ms << endl;
        delete[] HASHES;
    }

    int* knnMHBucketsPerfect::GetHashes(int *MH,int Bucket)
    {
//        cout<<"MH:"<< Tools::Array2stream(MH, this->NumMH).str()<< endl;
        int* H=new int[this->MiniBuckets];
        int BucketLen=this->NumMH/this->Buckets;
//        cout<< this->Buckets << " "<< this->NumMH<< " "<< BucketLen<< endl;
        int s=(BucketLen)*Bucket;
        int skip=s-1;
        for(int i=0;i<this->MiniBuckets;++i)
        {
            skip++;

            int signature [BucketLen-BucketLen/this->MiniBuckets];
            int k=0;
            for (int j=s;j<(Bucket+1)*BucketLen;++j)
            {
                if (skip==(j))
                    continue;
                signature[k++]=MH[j];
            }

//            cout << Tools::Array2stream(signature,BucketLen-BucketLen/this->MiniBuckets).str()<< endl;
            H[i]=this->MiniBuckets;
            for (int j=0;j<BucketLen-BucketLen/this->MiniBuckets ;++j)
            {
                H[i]=(H[i]*2011+signature[j]);
            }
//            cout << H[i]<< endl;

        }
        return H;
    }
    int* knnMHBucketsPerfect::GetKnn(double *Ob)
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
//    Result knnMHBucketsPerfect::SearchKNN(double *q, int k, Tools_App::Result res)
//    {
//        int* knnq=GetKnn(q);
//        int* mhq=MH->GetMinHash(knnq, this->K);
//        int** hq=new int*[this->Buckets];
//        for(int i=0;i<this->Buckets;++i)
//        {
//            hq[i]=GetHashes(mhq,i);
//        }
//        for (int i=0;i<this->Buckets;++i){
//            for (int j=0; j<this->MiniBuckets;++j)
//            {
//                int hash =hq[i][j];
//                unordered_map<int, vector<int>>::iterator got=this->Tables[i].find(hash);
//                if (got!=this->Tables[i].end())
//                {
//                    for(vector<int>::const_iterator it=got->second.begin() ;it!=got->second.end();++it)
//                    {
//                        res.Push(*it,this->REFS->Dist(q, this->DB->Get(*it)));
//                        //cout<<*it<<", ";
//                    }
//                    //cout<<endl;
//                }
//
//            }
//        }
//        return res;
//    }
//    int* knnMHBucketsPerfect::SearchKNN(double *q,wchar_t* loadfile )
//    {
//        //        auto begin = chrono::high_resolution_clock::now();
//        wstring ws(loadfile);
//        string strloadfile(ws.begin(),ws.end());
//        ifstream* input;
//        input=new ifstream();
//        input->open(strloadfile,ios::in|ios::binary);
//        this->Load(input);
//        input->close();
//        //        auto end = chrono::high_resolution_clock::now();
//        //        auto dur = end - begin;
//        //        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
//        //        cout << "Loading Time: "<<ms << endl;
//
//        int* knnq=GetKnn(q);
//        int* mhq=MH->GetMinHash(knnq, this->K);
//        int* hq=GetHashes(mhq);
//        return hq;
//    }
    int** knnMHBucketsPerfect::SearchKNN_Params(double *q,  wchar_t *params)
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
        this->Errors=stoi(line);
        getline(*ss, line);
        this->NumMH=stoi(line);
        getline(*ss, line);
        this->MiniBuckets=stoi(line);




        this->REFS=new VectorDB();

        this->REFS->Load(ss);


        this->MH=new MinHash();
        this->MH->Load(ss);

        this->R=this->REFS->Count();
        this->Buckets=(this->Errors+1)/2;


        //        auto end = chrono::high_resolution_clock::now();
        //        auto dur = end - begin;
        //        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        //        cout << "Loading Time: "<<ms << endl;



        int* knnq=GetKnn(q);
        int* mhq=MH->GetMinHash(knnq, this->K);
        int** hq=new int*[this->Buckets];
        for(int i=0;i<this->Buckets;++i)
        {
            hq[i]=GetHashes(mhq,i);
        }
        return hq;
    }
    extern "C"{
        knnMHBucketsPerfect* HashesExtractorP_new(){return new knnMHBucketsPerfect();}
        const wchar_t * HashesExtractorP_Bulkbuild(knnMHBucketsPerfect* foo,wchar_t * mfilename,wchar_t* savefile,int arg1,int arg2,int arg3,int arg4,int arg5,int arg6,int arg7, int arg8){return foo->Build(mfilename,savefile,arg1,arg2,arg3,arg4, arg5,arg6,arg7,arg8);}
        int**  HashesExtractorP_Build_query(knnMHBucketsPerfect* foo,double* query,wchar_t* loadfile){return foo->SearchKNN_Params(query,loadfile);}
    }
}
