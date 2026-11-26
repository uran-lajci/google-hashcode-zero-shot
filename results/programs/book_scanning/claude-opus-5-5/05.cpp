#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){
    if(bufPos==bufLen){bufLen=fread(buf,1,sizeof(buf),stdin);bufPos=0;if(bufLen<=0)return -1;}
    return buf[bufPos++];
}
inline long long readInt(){
    int c=gc();while(c!='-'&&(c<'0'||c>'9')){if(c==-1)return 0;c=gc();}
    bool neg=false;if(c=='-'){neg=true;c=gc();}
    long long x=0;while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();}
    return neg?-x:x;
}

int B,L,D;
vector<int> S;
vector<vector<int>> books;
vector<int> T,M;
vector<char> scanned;
chrono::steady_clock::time_point st;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-st).count();}

long long evalLib(int j,long long t){
    long long cap=(long long)(D-t-T[j])*M[j];
    if(cap<=0)return -1;
    long long s=0,c=0;
    for(int b:books[j]){
        if(c>=cap)break;
        if(!scanned[b]){s+=S[b];c++;}
    }
    return s;
}

struct Result{long long score;vector<pair<int,vector<int>>> plan;};

bool run(double alpha,Result&res,bool limited,double tl){
    scanned.assign(B,0);
    res.score=0;res.plan.clear();
    vector<double> denom(L);
    for(int j=0;j<L;j++)denom[j]=pow((double)T[j],alpha);
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        long long v=evalLib(j,0);
        if(v>0)pq.push({v/denom[j],-j});
    }
    long long t=0;int iter=0;
    while(!pq.empty()){
        if(limited&&((++iter)&255)==0&&elapsed()>tl)return false;
        auto [k,nj]=pq.top();pq.pop();
        int j=-nj;
        if(t+T[j]>=D)continue;
        long long v=evalLib(j,t);
        if(v<=0)continue;
        double nk=v/denom[j];
        if(!pq.empty()&&nk<pq.top().first-1e-12){pq.push({nk,-j});continue;}
        long long cap=(long long)(D-t-T[j])*M[j];
        vector<int> chosen;long long c=0;
        for(int b:books[j]){
            if(c>=cap)break;
            if(!scanned[b]){scanned[b]=1;chosen.push_back(b);res.score+=S[b];c++;}
        }
        if(chosen.empty())continue;
        res.plan.push_back({j,move(chosen)});
        t+=T[j];
    }
    return true;
}

int main(){
    st=chrono::steady_clock::now();
    B=readInt();L=readInt();D=readInt();
    S.resize(B);for(int i=0;i<B;i++)S[i]=readInt();
    books.resize(L);T.resize(L);M.resize(L);
    for(int j=0;j<L;j++){
        int n=readInt();T[j]=readInt();M[j]=readInt();
        books[j].resize(n);
        for(int i=0;i<n;i++)books[j][i]=readInt();
        sort(books[j].begin(),books[j].end(),[&](int a,int b){if(S[a]!=S[b])return S[a]>S[b];return a<b;});
    }
    vector<double> alphas={1.0,0.8,1.2,0.9,1.1,0.6,1.5,0.7,1.3,0.5,2.0};
    Result best;best.score=-1;
    bool first=true;
    for(double a:alphas){
        if(!first&&elapsed()>8.0)break;
        Result r;
        bool ok=run(a,r,!first,8.5);
        first=false;
        if(ok&&r.score>best.score)best=move(r);
    }
    string out;
    out.reserve(1<<20);
    out+=to_string(best.plan.size());out+='\n';
    for(auto&p:best.plan){
        out+=to_string(p.first);out+=' ';out+=to_string(p.second.size());out+='\n';
        for(size_t i=0;i<p.second.size();i++){
            if(i)out+=' ';
            out+=to_string(p.second[i]);
        }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
