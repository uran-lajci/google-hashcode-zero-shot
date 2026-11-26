#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,C,F,N; ll B,T;
vector<ll> ra,rb,rx,ry,rs,rf,rl;

struct Res{ ll score; vector<vector<int>> plan; };

Res runGreedy(double bw,double w,double c){
    vector<char> used(N,0);
    vector<vector<int>> plan(F);
    vector<ll> px(F,0),py(F,0);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int i=0;i<F;i++) pq.push({0,i});
    ll score=0;
    while(!pq.empty()){
        auto [t,v]=pq.top();pq.pop();
        ll x=px[v],y=py[v];
        int best=-1;double bv=-1;ll bfin=0,bst=0;
        for(int r=0;r<N;r++){
            if(used[r])continue;
            ll arr=t+llabs(x-ra[r])+llabs(y-rb[r]);
            ll st=max(arr,rs[r]);
            ll fin=st+rl[r];
            if(fin>rf[r])continue;
            ll empty=arr-t, wait=st-arr;
            double bon=(st==rs[r])?(double)B:0.0;
            double val=(rl[r]+bw*bon)/(rl[r]+empty+w*wait+c);
            if(val>bv){bv=val;best=r;bfin=fin;bst=st;}
        }
        if(best<0)continue;
        used[best]=1;
        plan[v].push_back(best);
        score+=rl[best]+(bst==rs[best]?B:0);
        px[v]=rx[best];py[v]=ry[best];
        pq.push({bfin,v});
    }
    return {score,plan};
}

int main(){
    auto t0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T)!=6)return 0;
    ra.resize(N);rb.resize(N);rx.resize(N);ry.resize(N);rs.resize(N);rf.resize(N);rl.resize(N);
    for(int i=0;i<N;i++){
        scanf("%lld %lld %lld %lld %lld %lld",&ra[i],&rb[i],&rx[i],&ry[i],&rs[i],&rf[i]);
        rl[i]=llabs(ra[i]-rx[i])+llabs(rb[i]-ry[i]);
    }
    vector<array<double,3>> params={
        {1,1,1},{1,0.5,1},{1,2,1},{2,1,1},{0.5,1,1},{1,1,10},{3,1,1},{1,0.25,1},
        {1,3,1},{2,0.5,5},{0,1,1},{1,1,50},{5,1,1},{2,2,1},{1,0.5,20},{0.25,1,1}
    };
    Res best{-1,{}};
    double lastDur=0;
    for(auto &p:params){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(best.score>=0 && el+lastDur*1.3>7.5)break;
        auto s=chrono::steady_clock::now();
        Res r=runGreedy(p[0],p[1],p[2]);
        lastDur=chrono::duration<double>(chrono::steady_clock::now()-s).count();
        if(r.score>best.score)best=move(r);
    }
    string out;
    for(int v=0;v<F;v++){
        out+=to_string(best.plan[v].size());
        for(int r:best.plan[v]){out+=' ';out+=to_string(r);}
        out+='\n';
    }
    fputs(out.c_str(),stdout);
}
