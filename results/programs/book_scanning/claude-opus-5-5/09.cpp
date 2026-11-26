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

int B,L;long long D;
vector<int> S;
vector<vector<int>> lib;
vector<long long> T,M;

struct Result{
    long long score=0;
    vector<int> order;
    vector<vector<int>> books;
};

Result runGreedy(double e){
    Result res;
    vector<char> scanned(B,0);
    vector<double> denom(L);
    for(int j=0;j<L;j++) denom[j]=pow((double)T[j],e);
    long long cur=0;
    auto evalLib=[&](int j,long long &sumOut)->double{
        long long days=D-cur-T[j];
        if(days<=0){sumOut=0;return 0;}
        long long cap=days*M[j];
        long long sum=0,cnt=0;
        for(int b:lib[j]){
            if(cnt>=cap)break;
            if(scanned[b])continue;
            sum+=S[b];cnt++;
        }
        sumOut=sum;
        return (double)sum/denom[j];
    };
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        long long s;double v=evalLib(j,s);
        if(s>0) pq.push({v,-j});
    }
    while(!pq.empty()){
        auto tp=pq.top();pq.pop();
        int j=-tp.second;
        long long s;double v=evalLib(j,s);
        if(s<=0) continue;
        if(!pq.empty() && v<pq.top().first){
            pq.push({v,-j});
            continue;
        }
        // accept
        long long days=D-cur-T[j];
        long long cap=days*M[j];
        vector<int> chosen;
        long long cnt=0;
        for(int b:lib[j]){
            if(cnt>=cap)break;
            if(scanned[b])continue;
            chosen.push_back(b);scanned[b]=1;res.score+=S[b];cnt++;
        }
        if(chosen.empty()) continue;
        res.order.push_back(j);
        res.books.push_back(move(chosen));
        cur+=T[j];
        if(cur>=D)break;
    }
    return res;
}

int main(){
    B=readInt();L=readInt();D=readInt();
    S.resize(B);
    for(int i=0;i<B;i++)S[i]=readInt();
    lib.resize(L);T.resize(L);M.resize(L);
    for(int j=0;j<L;j++){
        int n=readInt();T[j]=readInt();M[j]=readInt();
        lib[j].resize(n);
        for(int k=0;k<n;k++)lib[j][k]=readInt();
        sort(lib[j].begin(),lib[j].end(),[&](int a,int b){
            if(S[a]!=S[b])return S[a]>S[b];return a<b;});
        lib[j].erase(unique(lib[j].begin(),lib[j].end()),lib[j].end());
    }
    vector<double> exps={1.0,0.7,1.3,0.85,1.15,0.5,1.6,2.0};
    Result best;bool has=false;
    auto start=chrono::steady_clock::now();
    for(double e:exps){
        Result r=runGreedy(e);
        if(!has||r.score>best.score){best=move(r);has=true;}
        double el=chrono::duration<double>(chrono::steady_clock::now()-start).count();
        (void)el;
    }
    string out;
    out.reserve(1<<24);
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
