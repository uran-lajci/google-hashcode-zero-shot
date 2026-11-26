#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,C,F,N; ll B,T;
vector<ll> ra,rb,rx,ry,rs,rf,rl;

struct Var{int type; double wa,wd,k,bscale,w;};

ll runGreedy(const Var&v, vector<vector<int>>&out){
    out.assign(F,{});
    vector<int> alive(N); iota(alive.begin(),alive.end(),0);
    // vehicle state
    vector<ll> vt(F,0),vr(F,0),vc(F,0);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int i=0;i<F;i++) pq.push({0,i});
    ll score=0;
    while(!pq.empty()&&!alive.empty()){
        auto [t,id]=pq.top(); pq.pop();
        ll pr=vr[id],pc=vc[id];
        // prune globally infeasible rides
        int best=-1; double bv=-1e300; ll bst=0;
        for(size_t j=0;j<alive.size();){
            int r=alive[j];
            if(rf[r]-rl[r]<t){alive[j]=alive.back();alive.pop_back();continue;}
            ll d=llabs(pr-ra[r])+llabs(pc-rb[r]);
            ll st=max(t+d,rs[r]);
            if(st+rl[r]<=rf[r]&&st+rl[r]<=T){
                bool bon=(st==rs[r]);
                double val;
                if(v.type==0){
                    double g=rl[r]+(bon?B*v.bscale:0);
                    double c=v.wa*(st-t-d)+v.wd*d+rl[r]+v.k;
                    val=g/c;
                }else{
                    val=rl[r]+(bon?B:0)-v.w*(double)(st-t);
                }
                if(val>bv){bv=val;best=r;bst=st;}
            }
            j++;
        }
        if(best<0) continue;
        score+=rl[best]+(bst==rs[best]?B:0);
        out[id].push_back(best);
        vt[id]=bst+rl[best]; vr[id]=rx[best]; vc[id]=ry[best];
        for(size_t j=0;j<alive.size();j++) if(alive[j]==best){alive[j]=alive.back();alive.pop_back();break;}
        if(vt[id]<T) pq.push({vt[id],id});
    }
    return score;
}

int main(){
    auto t0=chrono::steady_clock::now();
    scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T);
    ra.resize(N);rb.resize(N);rx.resize(N);ry.resize(N);rs.resize(N);rf.resize(N);rl.resize(N);
    for(int i=0;i<N;i++){
        scanf("%lld %lld %lld %lld %lld %lld",&ra[i],&rb[i],&rx[i],&ry[i],&rs[i],&rf[i]);
        rl[i]=llabs(ra[i]-rx[i])+llabs(rb[i]-ry[i]);
    }
    vector<Var> vars;
    vars.push_back({0,1,1,0,1,0});
    vars.push_back({1,0,0,0,0,1});
    vars.push_back({0,0.5,1,0,1,0});
    vars.push_back({0,2,1,0,1,0});
    vars.push_back({1,0,0,0,0,0.5});
    vars.push_back({1,0,0,0,0,2});
    vars.push_back({0,1,1,10,1,0});
    vars.push_back({0,1,2,0,1,0});
    vars.push_back({0,1,1,0,2,0});
    vars.push_back({0,1,1,0,0.5,0});
    vars.push_back({1,0,0,0,0,4});
    vars.push_back({1,0,0,0,0,0.25});
    vars.push_back({0,0.25,1,0,1,0});
    vars.push_back({0,4,1,0,1,0});
    vars.push_back({0,1,1,50,1,0});
    vars.push_back({0,1,0.5,0,1,0});
    vector<vector<int>> best,cur; ll bs=-1;
    double lastDur=0;
    for(auto&v:vars){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(el+lastDur*1.3>8.5) break;
        auto a=chrono::steady_clock::now();
        ll s=runGreedy(v,cur);
        lastDur=chrono::duration<double>(chrono::steady_clock::now()-a).count();
        if(s>bs){bs=s;best=cur;}
    }
    if(best.empty()) best.assign(F,{});
    string o;
    for(int i=0;i<F;i++){
        o+=to_string(best[i].size());
        for(int r:best[i]){o+=' ';o+=to_string(r);}
        o+='\n';
    }
    fputs(o.c_str(),stdout);
}
