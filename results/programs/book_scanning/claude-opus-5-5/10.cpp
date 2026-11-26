#include <bits/stdc++.h>
using namespace std;

int B,L,D;
vector<int> S;
vector<vector<int>> lb;
vector<int> T,M;
chrono::steady_clock::time_point st;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-st).count();}

struct Result{
    long long score=-1;
    vector<int> order;
    vector<vector<int>> books;
};

vector<char> scanned;

double evalLib(int j,long long t,double alpha,double*sumOut=nullptr){
    long long cap=(long long)(D-t-T[j])*M[j];
    if(cap<=0) return -1;
    long long sum=0; long long c=0;
    for(int b:lb[j]){
        if(c>=cap) break;
        if(scanned[b]) continue;
        sum+=S[b]; c++;
    }
    if(sumOut)*sumOut=(double)sum;
    if(sum==0) return 0;
    return (double)sum/pow((double)T[j],alpha);
}

bool runGreedy(double alpha,bool first,Result&res){
    scanned.assign(B,0);
    long long t=0;
    priority_queue<pair<double,int>> pq;
    for(int j=0;j<L;j++){
        double v=evalLib(j,0,alpha);
        if(v>0) pq.push({v,-j});
    }
    res=Result(); res.score=0;
    long long cnt=0;
    while(!pq.empty()){
        if(!first && ((++cnt)&255)==0 && elapsed()>9.0) return false;
        auto [ov,nj]=pq.top(); pq.pop();
        int j=-nj;
        if(t+T[j]>=D) continue;
        double v=evalLib(j,t,alpha);
        if(v<=0) continue;
        if(pq.empty() || v>=pq.top().first-1e-12){
            long long cap=(long long)(D-t-T[j])*M[j];
            vector<int> chosen;
            for(int b:lb[j]){
                if((long long)chosen.size()>=cap) break;
                if(scanned[b]) continue;
                scanned[b]=1; chosen.push_back(b); res.score+=S[b];
            }
            if(chosen.empty()) continue;
            res.order.push_back(j);
            res.books.push_back(move(chosen));
            t+=T[j];
            if(t>=D) break;
        } else {
            pq.push({v,-j});
        }
    }
    return true;
}

int main(){
    st=chrono::steady_clock::now();
    if(scanf("%d %d %d",&B,&L,&D)!=3) return 0;
    S.resize(B);
    for(int i=0;i<B;i++) scanf("%d",&S[i]);
    lb.resize(L);T.resize(L);M.resize(L);
    for(int j=0;j<L;j++){
        int n; scanf("%d %d %d",&n,&T[j],&M[j]);
        lb[j].resize(n);
        for(int k=0;k<n;k++) scanf("%d",&lb[j][k]);
        sort(lb[j].begin(),lb[j].end(),[&](int a,int b){
            if(S[a]!=S[b]) return S[a]>S[b];
            return a<b;
        });
    }
    vector<double> alphas={1.0,0.9,1.1,0.8,1.2,0.7,1.3,0.6,1.5,0.5,2.0};
    Result best;
    bool first=true;
    for(double a:alphas){
        if(!first && elapsed()>8.0) break;
        Result r;
        bool ok=runGreedy(a,first,r);
        first=false;
        if(ok && r.score>best.score) best=move(r);
    }
    string out;
    out.reserve(1<<20);
    out+=to_string(best.order.size())+"\n";
    for(size_t i=0;i<best.order.size();i++){
        out+=to_string(best.order[i])+" "+to_string(best.books[i].size())+"\n";
        for(size_t k=0;k<best.books[i].size();k++){
            if(k) out+=' ';
            out+=to_string(best.books[i][k]);
        }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
