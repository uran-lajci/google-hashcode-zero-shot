#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,C,F,N; ll B,T;
vector<ll> ra,rb,rx,ry,rs,rf,rl;

struct Cfg{int type; double a,b;};

ll runGreedy(const Cfg& cfg, vector<vector<int>>& out){
    out.assign(F,{});
    vector<int> pool(N); iota(pool.begin(),pool.end(),0);
    vector<ll> vr(F,0),vc(F,0);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int i=0;i<F;i++) pq.push({0,i});
    ll total=0;
    while(!pq.empty() && !pool.empty()){
        auto [t,v]=pq.top(); pq.pop();
        int bestIdx=-1; double bestSc=-1e300; ll bestStart=0;
        for(int k=0;k<(int)pool.size();k++){
            int r=pool[k];
            ll d=llabs(vr[v]-ra[r])+llabs(vc[v]-rb[r]);
            ll st=t+d; if(st<rs[r]) st=rs[r];
            if(st+rl[r]>rf[r]) continue;
            ll idle=st-t;
            bool bon=(st==rs[r]);
            double sc;
            if(cfg.type==0){
                sc=(rl[r]+(bon?cfg.a*B:0.0))/(double)(idle+rl[r]+cfg.b);
            }else{
                sc=(double)rl[r]+(bon?B:0)-cfg.a*idle;
            }
            if(sc>bestSc){bestSc=sc;bestIdx=k;bestStart=st;}
        }
        if(bestIdx<0) continue;
        int r=pool[bestIdx];
        pool[bestIdx]=pool.back(); pool.pop_back();
        out[v].push_back(r);
        total+=rl[r]+(bestStart==rs[r]?B:0);
        vr[v]=rx[r]; vc[v]=ry[r];
        ll nt=bestStart+rl[r];
        if(nt<T) pq.push({nt,v});
    }
    return total;
}

int main(){
    scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T);
    ra.resize(N);rb.resize(N);rx.resize(N);ry.resize(N);rs.resize(N);rf.resize(N);rl.resize(N);
    for(int i=0;i<N;i++){
        scanf("%lld %lld %lld %lld %lld %lld",&ra[i],&rb[i],&rx[i],&ry[i],&rs[i],&rf[i]);
        rl[i]=llabs(ra[i]-rx[i])+llabs(rb[i]-ry[i]);
    }
    vector<Cfg> cfgs={
        {0,1,0},{0,1,10},{0,1,100},{0,0.5,0},{0,2,0},{0,0,0},{0,1,1000},{0,0.25,10},
        {1,0.5,0},{1,1,0},{1,2,0},{1,4,0},{1,8,0},{1,0.25,0},{1,16,0},{1,1.5,0}
    };
    ll best=-1; vector<vector<int>> bestOut,cur;
    for(auto&c:cfgs){
        ll s=runGreedy(c,cur);
        if(s>best){best=s;bestOut=cur;}
    }
    string o;
    for(int v=0;v<F;v++){
        o+=to_string(bestOut[v].size());
        for(int r:bestOut[v]){o+=' ';o+=to_string(r);}
        o+='\n';
    }
    fputs(o.c_str(),stdout);
    fprintf(stderr,"score %lld\n",best);
}
