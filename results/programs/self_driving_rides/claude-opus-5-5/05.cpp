#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,C,F,N; ll B,T;
vector<ll> ra,rb,rx,ry,rs,rf,rl;

struct Res{ ll score; vector<vector<int>> asg; };

Res runGreedy(double wWait,double wTravel,double bonusF){
    Res res; res.score=0; res.asg.assign(F,{});
    vector<int> alive(N); iota(alive.begin(),alive.end(),0);
    typedef tuple<ll,int> Q; // time, vehicle
    priority_queue<Q,vector<Q>,greater<Q>> pq;
    vector<ll> px(F,0),py(F,0);
    for(int v=0;v<F;v++) pq.push({0,v});
    while(!pq.empty() && !alive.empty()){
        auto [t,v]=pq.top(); pq.pop();
        double best=-1e300; int bi=-1; ll bestEnd=0;bool bestBonus=false;
        ll x=px[v],y=py[v];
        for(int k=0;k<(int)alive.size();k++){
            int r=alive[k];
            ll travel=llabs(x-ra[r])+llabs(y-rb[r]);
            ll arr=t+travel;
            ll st=max(arr,rs[r]);
            ll en=st+rl[r];
            if(en>rf[r]||en>T) continue;
            bool bon=(st==rs[r]);
            double sc=(double)rl[r]+(bon?bonusF*B:0.0)-wTravel*travel-wWait*(double)(st-arr);
            if(sc>best){best=sc;bi=k;bestEnd=en;bestBonus=bon;}
        }
        if(bi<0) continue; // retire vehicle
        int r=alive[bi];
        alive[bi]=alive.back(); alive.pop_back();
        res.asg[v].push_back(r);
        res.score+=rl[r]+(bestBonus?B:0);
        px[v]=rx[r];py[v]=ry[r];
        pq.push({bestEnd,v});
    }
    return res;
}

int main(){
    auto t0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T)!=6) return 0;
    ra.resize(N);rb.resize(N);rx.resize(N);ry.resize(N);rs.resize(N);rf.resize(N);rl.resize(N);
    for(int i=0;i<N;i++){
        scanf("%lld %lld %lld %lld %lld %lld",&ra[i],&rb[i],&rx[i],&ry[i],&rs[i],&rf[i]);
        rl[i]=llabs(ra[i]-rx[i])+llabs(rb[i]-ry[i]);
    }
    vector<array<double,3>> params={
        {1,1,1},{0.5,1,1},{1,0.5,1},{2,1,1},{1,2,1},{0.2,1,1},{0.5,0.5,1},
        {0,1,1},{1,0,1},{3,1.5,1},{1,1,2},{1,1,0.5},{0.3,0.6,1},{0.1,0.3,1},
        {2,2,1},{1.5,1,1},{0.7,1.3,1},{1,1,3},{0.5,1,2},{0.05,0.1,1}
    };
    Res best; best.score=-1; best.asg.assign(F,{});
    for(auto &p:params){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(best.score>=0 && el>8.3) break;
        auto t1=chrono::steady_clock::now();
        Res r=runGreedy(p[0],p[1],p[2]);
        if(r.score>best.score) best=std::move(r);
        double one=chrono::duration<double>(chrono::steady_clock::now()-t1).count();
        el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(el+one*1.2>8.5) break;
    }
    string out;
    for(int v=0;v<F;v++){
        out+=to_string(best.asg[v].size());
        for(int r:best.asg[v]){out+=' ';out+=to_string(r);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
