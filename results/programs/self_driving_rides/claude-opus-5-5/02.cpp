#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,C,F,N; ll B,T;
vector<ll> ra,rb,rx,ry,rs,rf,rl;

struct Result{ ll score; vector<vector<int>> plan; };

Result run(double bonusMult,double waitW,double cst){
    Result res; res.score=0; res.plan.assign(F,{});
    vector<int> avail(N); iota(avail.begin(),avail.end(),0);
    // vehicle state
    vector<ll> px(F,0),py(F,0);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int v=0;v<F;v++) pq.push({0,v});
    while(!pq.empty() && !avail.empty()){
        auto [t,v]=pq.top(); pq.pop();
        int best=-1; double bv=-1; ll bstart=0;
        for(size_t i=0;i<avail.size();){
            int r=avail[i];
            if(t+rl[r]>rf[r]){ // infeasible forever
                avail[i]=avail.back(); avail.pop_back(); continue;
            }
            ll travel=llabs(px[v]-ra[r])+llabs(py[v]-rb[r]);
            ll arr=t+travel;
            ll st=max(arr,rs[r]);
            if(st+rl[r]>rf[r]){ i++; continue; }
            ll wait=st-arr;
            double gain=rl[r]+(st==rs[r]?bonusMult*B:0.0);
            double cost=travel+waitW*wait+rl[r]+cst;
            double val=gain/cost;
            if(val>bv){bv=val;best=(int)i;bstart=st;}
            i++;
        }
        if(best<0) continue;
        int r=avail[best];
        avail[best]=avail.back(); avail.pop_back();
        res.score+=rl[r]+(bstart==rs[r]?B:0);
        res.plan[v].push_back(r);
        px[v]=rx[r]; py[v]=ry[r];
        pq.push({bstart+rl[r],v});
    }
    return res;
}

int main(){
    scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T);
    ra.resize(N);rb.resize(N);rx.resize(N);ry.resize(N);rs.resize(N);rf.resize(N);rl.resize(N);
    for(int i=0;i<N;i++){
        scanf("%lld %lld %lld %lld %lld %lld",&ra[i],&rb[i],&rx[i],&ry[i],&rs[i],&rf[i]);
        rl[i]=llabs(ra[i]-rx[i])+llabs(rb[i]-ry[i]);
        if(rf[i]>T) rf[i]=T;
    }
    vector<array<double,3>> params={
        {1,1,1},{1,0.5,1},{1,2,1},{2,1,1},{1,0.2,1},{0.5,1,1},
        {1,0.05,1},{3,1,1},{1,1,20},{1,0.5,50},{0,1,1}
    };
    Result best; best.score=-1;
    for(auto&p:params){
        Result r=run(p[0],p[1],p[2]);
        if(r.score>best.score) best=move(r);
    }
    string out;
    for(int v=0;v<F;v++){
        out+=to_string(best.plan[v].size());
        for(int r:best.plan[v]){out+=' ';out+=to_string(r);}
        out+='\n';
    }
    fputs(out.c_str(),stdout);
}
