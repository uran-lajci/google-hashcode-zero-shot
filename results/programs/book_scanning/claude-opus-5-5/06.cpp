#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int readChar(){
    if(bufPos==bufLen){bufLen=fread(buf,1,sizeof(buf),stdin);bufPos=0;if(bufLen<=0)return -1;}
    return buf[bufPos++];
}
inline long long readInt(){
    int c=readChar();while(c!='-'&&(c<'0'||c>'9')){if(c==-1)return 0;c=readChar();}
    bool neg=false;if(c=='-'){neg=true;c=readChar();}
    long long x=0;while(c>='0'&&c<='9'){x=x*10+(c-'0');c=readChar();}
    return neg?-x:x;
}

int B,L,D;
vector<int> S;
vector<int> N,T,M;
vector<vector<int>> books;
chrono::steady_clock::time_point startT;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-startT).count();}

struct Result{
    long long score=-1;
    vector<int> order;
    vector<vector<int>> lists;
};

vector<char> taken;

// compute gain with start day cur
long long gainOf(int j,long long cur){
    long long slots=(long long)(D-cur-T[j]);
    if(slots<=0) return 0;
    long long cap=slots*(long long)M[j];
    long long s=0,cnt=0;
    for(int b:books[j]){
        if(cnt>=cap)break;
        if(!taken[b]){s+=S[b];cnt++;}
    }
    return s;
}

bool runGreedy(double p,bool mustFinish,Result&res){
    taken.assign(B,0);
    res.score=0;res.order.clear();res.lists.clear();
    vector<double> denom(L);
    for(int j=0;j<L;j++)denom[j]=pow((double)T[j],p);
    priority_queue<pair<double,int>> pq; // value, -id for determinism
    for(int j=0;j<L;j++){
        if(T[j]>=D)continue;
        long long g=gainOf(j,0);
        if(g<=0)continue;
        pq.push({g/denom[j],-j});
    }
    long long cur=0;
    long long iter=0;
    while(!pq.empty()){
        if(!mustFinish && ((++iter)&255)==0 && elapsed()>9.0) return false;
        auto tp=pq.top();pq.pop();
        int j=-tp.second;
        if(cur+T[j]>=D)continue;
        long long g=gainOf(j,cur);
        if(g<=0)continue;
        double v=g/denom[j];
        if(!pq.empty() && v<pq.top().first){pq.push({v,-j});continue;}
        // select
        long long slots=(long long)(D-cur-T[j]);
        long long cap=slots*(long long)M[j];
        vector<int> lst;
        for(int b:books[j]){
            if((long long)lst.size()>=cap)break;
            if(!taken[b]){taken[b]=1;lst.push_back(b);res.score+=S[b];}
        }
        if(lst.empty())continue;
        cur+=T[j];
        res.order.push_back(j);
        res.lists.push_back(move(lst));
        if(cur>=D)break;
    }
    return true;
}

int main(){
    startT=chrono::steady_clock::now();
    B=readInt();L=readInt();D=readInt();
    S.resize(B);
    for(int i=0;i<B;i++)S[i]=readInt();
    N.resize(L);T.resize(L);M.resize(L);books.resize(L);
    for(int j=0;j<L;j++){
        N[j]=readInt();T[j]=readInt();M[j]=readInt();
        books[j].resize(N[j]);
        for(int k=0;k<N[j];k++)books[j][k]=readInt();
        sort(books[j].begin(),books[j].end(),[&](int a,int b){
            if(S[a]!=S[b])return S[a]>S[b];return a<b;});
        books[j].erase(unique(books[j].begin(),books[j].end()),books[j].end());
    }
    vector<double> exps={1.0,0.9,1.1,0.8,1.2,0.7,1.3,0.6,1.5,0.5,2.0,0.3};
    Result best;bool first=true;
    double lastDur=0;
    for(double p:exps){
        double st=elapsed();
        if(!first && st+lastDur*1.5>9.0)break;
        Result r;
        bool ok=runGreedy(p,first,r);
        lastDur=elapsed()-st;
        if(ok && r.score>best.score)best=move(r);
        first=false;
    }
    // output
    string out;
    out.reserve(1<<22);
    out+=to_string(best.order.size());out+='\n';
    for(size_t i=0;i<best.order.size();i++){
        out+=to_string(best.order[i]);out+=' ';
        out+=to_string(best.lists[i].size());out+='\n';
        for(size_t k=0;k<best.lists[i].size();k++){
            if(k)out+=' ';
            out+=to_string(best.lists[i][k]);
        }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
