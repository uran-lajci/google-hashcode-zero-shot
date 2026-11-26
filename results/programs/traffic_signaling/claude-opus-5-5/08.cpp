#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){
    if(bufPos==bufLen){bufLen=fread(buf,1,sizeof(buf),stdin);bufPos=0;if(bufLen<=0)return -1;}
    return buf[bufPos++];
}
inline bool readTok(string&s){
    s.clear();int c=gc();
    while(c!=-1&&isspace(c))c=gc();
    if(c==-1)return false;
    while(c!=-1&&!isspace(c)){s.push_back((char)c);c=gc();}
    return true;
}
inline int readInt(){string s;readTok(s);return atoi(s.c_str());}

int D,I,S,V,F;
vector<int> SB,SE,SL;
vector<string> SN;
vector<vector<int>> paths;
vector<char> valid;
vector<int> cyc,off,dur; // per street (cyc via intersection)
vector<int> icyc;
vector<vector<int>> order; // per intersection
vector<int> durS;
vector<long long> waitI, waitBest;
long long work=0;

void recompute(int i){
    int o=0;
    for(int s:order[i]){off[s]=o;o+=durS[s];}
    icyc[i]=o;
}

vector<int> lastP,posC;
vector<vector<int>> bucket;

long long simulate(){
    fill(lastP.begin(),lastP.end(),-1);
    fill(waitI.begin(),waitI.end(),0);
    for(auto&b:bucket)b.clear();
    work+=S+I+D+V;
    for(int c=0;c<V;c++) if(valid[c]){posC[c]=0;bucket[0].push_back(c);}
    long long score=0;
    for(int t=0;t<=D;t++){
        auto&b=bucket[t];
        work+=b.size();
        for(size_t k=0;k<b.size();k++){
            int c=b[k];
            int s=paths[c][posC[c]];
            int d=durS[s];
            if(d==0)continue;
            int x=max(t,lastP[s]+1);
            int C=icyc[SE[s]];
            int r=x%C;int o=off[s];int g;
            if(r>=o&&r<o+d)g=x;
            else if(r<o)g=x-r+o;
            else g=x-r+C+o;
            if(g>D)continue;
            lastP[s]=g;
            waitI[SE[s]]+=g-t;
            int p=++posC[c];
            int ns=paths[c][p];
            int arr=g+SL[ns];
            if(arr>D)continue;
            if(p==(int)paths[c].size()-1) score+=F+(D-arr);
            else bucket[arr].push_back(c);
        }
    }
    return score;
}

int main(){
    D=readInt();I=readInt();S=readInt();V=readInt();F=readInt();
    SB.resize(S);SE.resize(S);SL.resize(S);SN.resize(S);
    unordered_map<string,int> mp;mp.reserve(S*2);
    for(int i=0;i<S;i++){
        SB[i]=readInt();SE[i]=readInt();readTok(SN[i]);SL[i]=readInt();
        mp[SN[i]]=i;
    }
    paths.resize(V);valid.assign(V,0);
    string tok;
    for(int c=0;c<V;c++){
        int P=readInt();paths[c].resize(P);
        for(int k=0;k<P;k++){readTok(tok);paths[c][k]=mp[tok];}
        long long len=0;for(int k=1;k<P;k++)len+=SL[paths[c][k]];
        valid[c]=(len<=D);
    }
    vector<long long> cnt(S,0);
    vector<int> earliest(S,INT_MAX);
    for(int c=0;c<V;c++){
        if(!valid[c])continue;
        int t=0;
        for(size_t k=0;k+1<paths[c].size();k++){
            int s=paths[c][k];
            if(k>0)t+=SL[s];
            cnt[s]++;
            earliest[s]=min(earliest[s],t);
        }
    }
    order.assign(I,{});
    durS.assign(S,0);off.assign(S,0);icyc.assign(I,1);
    for(int s=0;s<S;s++) if(cnt[s]>0){order[SE[s]].push_back(s);durS[s]=1;}
    vector<int> cand;
    for(int i=0;i<I;i++){
        sort(order[i].begin(),order[i].end(),[&](int a,int b){
            if(earliest[a]!=earliest[b])return earliest[a]<earliest[b];
            if(cnt[a]!=cnt[b])return cnt[a]>cnt[b];
            return a<b;});
        if(!order[i].empty())recompute(i);
        if(order[i].size()>=2)cand.push_back(i);
    }
    lastP.assign(S,-1);posC.assign(V,0);bucket.assign(D+1,{});
    waitI.assign(I,0);
    long long best=simulate();
    waitBest=waitI;
    vector<long long> pref(cand.size()+1,0);
    auto buildPref=[&](){
        for(size_t k=0;k<cand.size();k++)pref[k+1]=pref[k]+waitBest[cand[k]];
    };
    buildPref();
    mt19937_64 rng(1234567);
    const long long BUDGET=250000000LL;
    if(!cand.empty()){
        while(work<BUDGET){
            int i;
            if((rng()&1)&&pref.back()>0){
                long long r=rng()%pref.back();
                size_t k=upper_bound(pref.begin(),pref.end(),r)-pref.begin()-1;
                if(k>=cand.size())k=cand.size()-1;
                i=cand[k];
            } else i=cand[rng()%cand.size()];
            auto&ord=order[i];
            int n=ord.size();
            int mv=rng()%3;
            int a=-1,b=-1,s=-1,old=0;
            if(mv==0){
                a=rng()%n;b=rng()%n;if(a==b)b=(a+1)%n;
                swap(ord[a],ord[b]);
            } else if(mv==1){
                s=ord[rng()%n];old=durS[s];
                if(old>=D)continue;
                durS[s]=old+1;
            } else {
                s=ord[rng()%n];old=durS[s];
                if(old<=1)continue;
                durS[s]=old-1;
            }
            recompute(i);
            long long sc=simulate();
            if(sc>=best){
                best=sc;waitBest=waitI;buildPref();
            } else {
                if(mv==0)swap(ord[a],ord[b]);
                else durS[s]=old;
                recompute(i);
            }
        }
    }
    // output
    string out;
    int A=0;
    for(int i=0;i<I;i++)if(!order[i].empty())A++;
    out+=to_string(A)+"\n";
    for(int i=0;i<I;i++){
        if(order[i].empty())continue;
        out+=to_string(i)+"\n"+to_string(order[i].size())+"\n";
        for(int s:order[i]){
            int d=min(max(durS[s],1),D);
            out+=SN[s]+" "+to_string(d)+"\n";
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
