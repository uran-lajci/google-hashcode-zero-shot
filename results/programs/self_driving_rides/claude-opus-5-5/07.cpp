#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,C,F,N; ll B,T;
vector<ll> ra,rb,rx,ry,rs,rf,rl;

struct Res{ ll score; vector<vector<int>> asg; };

Res run(double bf, double cc){
    Res res; res.score=0; res.asg.assign(F,{});
    vector<int> rem(N); iota(rem.begin(),rem.end(),0);
    vector<ll> px(F,0),py(F,0);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int v=0;v<F;v++) pq.push({0,v});
    while(!pq.empty() && !rem.empty()){
        auto [t,v]=pq.top(); pq.pop();
        if(t>=T) continue;
        double best=-1; int bi=-1; ll bst=0;
        for(size_t k=0;k<rem.size();){
            int r=rem[k];
            ll lb=max(t,rs[r])+rl[r];
            if(lb>rf[r]){ rem[k]=rem.back(); rem.pop_back(); continue; }
            ll arr=t+llabs(px[v]-ra[r])+llabs(py[v]-rb[r]);
            ll st=max(arr,rs[r]);
            if(st+rl[r]<=rf[r]){
                double gain=rl[r]+(st==rs[r]?bf*B:0.0);
                double cost=(double)(st-t)+rl[r]+cc;
                double val=gain/cost;
                if(val>best){best=val;bi=(int)k;bst=st;}
            }
            k++;
        }
        if(bi<0) continue;
        int r=rem[bi];
        res.score+=rl[r]+(bst==rs[r]?B:0);
        res.asg[v].push_back(r);
        px[v]=rx[r];py[v]=ry[r];
        rem[bi]=rem.back(); rem.pop_back();
        pq.push({bst+rl[r],v});
    }
    return res;
}

int main(){
    auto t0=chrono::steady_clock::now();
    scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T);
    ra.resize(N);rb.resize(N);rx.resize(N);ry.resize(N);rs.resize(N);rf.resize(N);rl.resize(N);
    for(int i=0;i<N;i++){
        scanf("%lld %lld %lld %lld %lld %lld",&ra[i],&rb[i],&rx[i],&ry[i],&rs[i],&rf[i]);
        rl[i]=llabs(ra[i]-rx[i])+llabs(rb[i]-ry[i]);
    }
    vector<pair<double,double>> params;
    double bfs[]={1.0,0.5,2.0,0.25,4.0};
    double ccs[]={0,5,20,1,50,100};
    for(double c:ccs) for(double b:bfs) params.push_back({b,c});
    Res best; best.score=-1;
    double maxRun=0;
    for(auto &p:params){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(best.score>=0 && el+maxRun*1.3>8.3) break;
        auto a=chrono::steady_clock::now();
        Res r=run(p.first,p.second);
        double d=chrono::duration<double>(chrono::steady_clock::now()-a).count();
        maxRun=max(maxRun,d);
        if(r.score>best.score) best=move(r);
    }
    string out;
    for(int v=0;v<F;v++){
        out+=to_string(best.asg[v].size());
        for(int r:best.asg[v]){out+=' ';out+=to_string(r);}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
}
