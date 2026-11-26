#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,C,F,N; ll B,T;
vector<ll> ra,rb,rx,ry,rs,rf,rlen;

struct Var{int type; double K; double bw; double alpha;};

ll runVar(const Var& v, vector<vector<int>>& plan){
    plan.assign(F,{});
    vector<int> rem(N);
    for(int i=0;i<N;i++) rem[i]=i;
    // car state
    vector<ll> cr(F,0),cc(F,0),ct(F,0);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int i=0;i<F;i++) pq.push({0,i});
    ll score=0;
    while(!pq.empty()&&!rem.empty()){
        auto [t,car]=pq.top();pq.pop();
        ll pr=cr[car],pc=cc[car];
        int best=-1; double bv=-1e300; ll bStart=0;
        for(size_t k=0;k<rem.size();){
            int i=rem[k];
            if(rf[i]-rlen[i]<t){ rem[k]=rem.back(); rem.pop_back(); continue; }
            ll arr=t+llabs(pr-ra[i])+llabs(pc-rb[i]);
            ll st=max(arr,rs[i]);
            ll fin=st+rlen[i];
            if(fin<=rf[i]){
                double bonus=(st==rs[i])?(double)B:0.0;
                double val;
                if(v.type==0){
                    val=(rlen[i]+v.bw*bonus)/((double)(st-t)+rlen[i]+v.K);
                }else{
                    val=rlen[i]+v.bw*bonus-v.alpha*(double)(st-t);
                }
                if(val>bv){bv=val;best=i;bStart=st;}
            }
            k++;
        }
        if(best<0) continue; // retire car
        // remove best
        for(size_t k=0;k<rem.size();k++) if(rem[k]==best){rem[k]=rem.back();rem.pop_back();break;}
        plan[car].push_back(best);
        score+=rlen[best]+(bStart==rs[best]?B:0);
        ll fin=bStart+rlen[best];
        cr[car]=rx[best];cc[car]=ry[best];ct[car]=fin;
        if(fin<T) pq.push({fin,car});
    }
    return score;
}

int main(){
    auto t0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T)!=6) return 0;
    ra.resize(N);rb.resize(N);rx.resize(N);ry.resize(N);rs.resize(N);rf.resize(N);rlen.resize(N);
    for(int i=0;i<N;i++){
        scanf("%lld %lld %lld %lld %lld %lld",&ra[i],&rb[i],&rx[i],&ry[i],&rs[i],&rf[i]);
        rlen[i]=llabs(ra[i]-rx[i])+llabs(rb[i]-ry[i]);
        if(rf[i]>T) rf[i]=T;
    }
    vector<Var> vars={
        {0,1,1,0},{0,5,1,0},{0,20,1,0},{0,1,2,0},{0,5,2,0},{0,0.5,1,0},{0,50,1,0},
        {1,0,1,1},{1,0,1,2},{1,0,1,0.5},{1,0,2,1},{1,0,1,4}
    };
    ll bestScore=-1; vector<vector<int>> bestPlan(F), plan;
    for(auto& v:vars){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(el>8.5) break;
        ll s=runVar(v,plan);
        if(s>bestScore){bestScore=s;bestPlan=plan;}
    }
    string out;
    for(int i=0;i<F;i++){
        out+=to_string(bestPlan[i].size());
        for(int r:bestPlan[i]){out+=' ';out+=to_string(r);}
        out+='\n';
    }
    fputs(out.c_str(),stdout);
    return 0;
}
