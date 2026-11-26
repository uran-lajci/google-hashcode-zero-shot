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
vector<vector<int>> lb;
vector<long long> T,M;

struct Result{
    long long score=-1;
    vector<int> order;
    vector<vector<int>> books;
};

vector<char> taken;

double computeVal(int j,long long cur,double alpha,vector<double>&powT){
    long long days=(long long)D-cur-T[j];
    if(days<=0) return -1;
    long long cap=min<long long>((long long)lb[j].size(),days*M[j]);
    long long sum=0,cnt=0;
    for(int b:lb[j]){
        if(cnt>=cap)break;
        if(!taken[b]){sum+=S[b];cnt++;}
    }
    if(cnt==0) return -1;
    if(sum==0) return 0;
    return (double)sum/powT[j];
}

Result runGreedy(double alpha){
    Result r; r.score=0;
    taken.assign(B,0);
    vector<double> powT(L);
    for(int j=0;j<L;j++) powT[j]=pow((double)T[j],alpha);
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        double v=computeVal(j,0,alpha,powT);
        if(v>0) pq.push({v,-j});
    }
    long long cur=0;
    while(!pq.empty()){
        auto tp=pq.top();pq.pop();
        int j=-tp.second;
        double v=computeVal(j,cur,alpha,powT);
        if(v<=0) continue;
        if(pq.empty() || v>=pq.top().first-1e-12){
            long long days=(long long)D-cur-T[j];
            long long cap=min<long long>((long long)lb[j].size(),days*M[j]);
            vector<int> chosen;
            for(int b:lb[j]){
                if((long long)chosen.size()>=cap)break;
                if(!taken[b]){taken[b]=1;chosen.push_back(b);r.score+=S[b];}
            }
            if(chosen.empty()) continue;
            r.order.push_back(j);
            r.books.push_back(move(chosen));
            cur+=T[j];
            if(cur>=D) break;
        } else {
            pq.push({v,-j});
        }
    }
    return r;
}

int main(){
    auto start=chrono::steady_clock::now();
    B=readInt();L=readInt();D=readInt();
    S.resize(B);
    for(int i=0;i<B;i++)S[i]=readInt();
    lb.resize(L);T.resize(L);M.resize(L);
    for(int j=0;j<L;j++){
        int n=readInt();T[j]=readInt();M[j]=readInt();
        lb[j].resize(n);
        for(int k=0;k<n;k++)lb[j][k]=readInt();
        sort(lb[j].begin(),lb[j].end(),[&](int a,int b){
            if(S[a]!=S[b])return S[a]>S[b];
            return a<b;
        });
    }
    vector<double> alphas={1.0,0.9,1.1,0.8,1.2,1.4,0.7,1.6,0.5,2.0,0.0};
    Result best;
    double lastDur=0;
    for(double a:alphas){
        double el=chrono::duration<double>(chrono::steady_clock::now()-start).count();
        if(best.score>=0 && el+lastDur*1.5>8.0) break;
        auto t0=chrono::steady_clock::now();
        Result r=runGreedy(a);
        lastDur=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(r.score>best.score) best=move(r);
    }
    string out;
    out.reserve(1<<20);
    out+=to_string(best.order.size());out+='\n';
    for(size_t i=0;i<best.order.size();i++){
        out+=to_string(best.order[i]);out+=' ';
        out+=to_string(best.books[i].size());out+='\n';
        for(size_t k=0;k<best.books[i].size();k++){
            if(k)out+=' ';
            out+=to_string(best.books[i][k]);
        }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
