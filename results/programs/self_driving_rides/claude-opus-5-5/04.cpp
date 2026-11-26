#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,C,F,N; ll B,T;
vector<ll> ra,rb,rx,ry,rs,rf,rlen;

struct Params{double ww,aw,c,bm;};

ll runGreedy(const Params&p, vector<vector<int>>&out){
    out.assign(F,{});
    vector<int> alive(N); iota(alive.begin(),alive.end(),0);
    // sort alive by latest start for nicer pruning (not required)
    priority_queue<tuple<ll,int>,vector<tuple<ll,int>>,greater<tuple<ll,int>>> pq;
    vector<ll> px(F,0),py(F,0);
    for(int i=0;i<F;i++) pq.push({0,i});
    ll total=0;
    while(!pq.empty()&&!alive.empty()){
        auto [t,v]=pq.top();pq.pop();
        // prune dead rides
        double best=-1; int bi=-1; ll bstart=0;
        for(size_t k=0;k<alive.size();){
            int r=alive[k];
            if(rf[r]-rlen[r]<t){ alive[k]=alive.back(); alive.pop_back(); continue; }
            ll ap=llabs(px[v]-ra[r])+llabs(py[v]-rb[r]);
            ll arr=t+ap;
            ll st=max(arr,rs[r]);
            if(st+rlen[r]<=rf[r]){
                double val=rlen[r]+(st==rs[r]?p.bm*B:0);
                double cost=p.aw*ap+p.ww*(st-arr)+rlen[r]+p.c;
                double sc=val/cost;
                if(sc>best){best=sc;bi=r;bstart=st;}
            }
            k++;
        }
        if(bi<0) continue;
        out[v].push_back(bi);
        total+=rlen[bi]+(bstart==rs[bi]?B:0);
        px[v]=rx[bi];py[v]=ry[bi];
        ll nt=bstart+rlen[bi];
        for(size_t k=0;k<alive.size();k++) if(alive[k]==bi){alive[k]=alive.back();alive.pop_back();break;}
        pq.push({nt,v});
    }
    return total;
}

int main(){
    auto t0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T)!=6) return 0;
    ra.resize(N);rb.resize(N);rx.resize(N);ry.resize(N);rs.resize(N);rf.resize(N);rlen.resize(N);
    for(int i=0;i<N;i++){
        scanf("%lld %lld %lld %lld %lld %lld",&ra[i],&rb[i],&rx[i],&ry[i],&rs[i],&rf[i]);
        rlen[i]=llabs(ra[i]-rx[i])+llabs(rb[i]-ry[i]);
    }
    vector<Params> ps={{1,1,0,1},{0.5,1,0,1},{2,1,0,1},{1,1,10,1},{1,2,0,1},{0.3,1,0,1.5},
                       {1,1,50,1},{0.7,1.5,5,1},{1.5,1,20,0.7},{0.2,1,0,1},{1,1,0,2},{3,1,0,1}};
    ll bestScore=-1; vector<vector<int>> bestOut,out;
    double maxRun=0;
    for(auto&p:ps){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(bestScore>=0 && el+maxRun*1.3>8.5) break;
        auto s=chrono::steady_clock::now();
        ll sc=runGreedy(p,out);
        double d=chrono::duration<double>(chrono::steady_clock::now()-s).count();
        maxRun=max(maxRun,d);
        if(sc>bestScore){bestScore=sc;bestOut=out;}
    }
    string o;
    for(int i=0;i<F;i++){
        o+=to_string(bestOut[i].size());
        for(int r:bestOut[i]){o+=' ';o+=to_string(r);}
        o+='\n';
    }
    fputs(o.c_str(),stdout);
}
