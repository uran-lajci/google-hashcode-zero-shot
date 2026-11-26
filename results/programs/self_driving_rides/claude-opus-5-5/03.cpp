#include <bits/stdc++.h>
using namespace std;

int R,C,F,N; long long B,T;
vector<long long> ra,rb,rx,ry,rs,rf,rl;

struct Params{double bf,ww,c;};

long long runGreedy(const Params&p, vector<vector<int>>&out){
    out.assign(F,{});
    vector<int> active(N); iota(active.begin(),active.end(),0);
    // priority queue: (time, vehicle)
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<pair<long long,int>>> pq;
    vector<long long> px(F,0),py(F,0);
    for(int i=0;i<F;i++)pq.push({0,i});
    long long total=0;
    while(!pq.empty()&&!active.empty()){
        auto [t,v]=pq.top();pq.pop();
        // prune globally impossible rides
        for(size_t k=0;k<active.size();){
            int r=active[k];
            if(t+rl[r]>rf[r]){active[k]=active.back();active.pop_back();}
            else k++;
        }
        double best=-1;int bi=-1;long long bstart=0;
        for(size_t k=0;k<active.size();k++){
            int r=active[k];
            long long d=llabs(px[v]-ra[r])+llabs(py[v]-rb[r]);
            long long arr=t+d;
            long long st=max(arr,rs[r]);
            long long en=st+rl[r];
            if(en>rf[r])continue;
            double val=rl[r]+(st==rs[r]?p.bf*B:0.0);
            double cost=d+p.ww*(st-arr)+rl[r]+p.c;
            double sc=val/cost;
            if(sc>best||(sc==best&&r<bi)){best=sc;bi=r;bstart=st;}
        }
        if(bi<0)continue; // vehicle retires
        int r=bi;
        total+=rl[r]+(bstart==rs[r]?B:0);
        out[v].push_back(r);
        px[v]=rx[r];py[v]=ry[r];
        for(size_t k=0;k<active.size();k++)if(active[k]==r){active[k]=active.back();active.pop_back();break;}
        pq.push({bstart+rl[r],v});
    }
    return total;
}

int main(){
    auto t0=chrono::steady_clock::now();
    if(!(cin>>R>>C>>F>>N>>B>>T))return 0;
    ra.resize(N);rb.resize(N);rx.resize(N);ry.resize(N);rs.resize(N);rf.resize(N);rl.resize(N);
    for(int i=0;i<N;i++){
        cin>>ra[i]>>rb[i]>>rx[i]>>ry[i]>>rs[i]>>rf[i];
        rl[i]=llabs(ra[i]-rx[i])+llabs(rb[i]-ry[i]);
    }
    vector<Params> ps;
    double bfs[]={1.0,0.5,2.0,0.25,4.0};
    double wws[]={1.0,0.5,2.0,0.25,0.0};
    double cs[]={1.0,10.0,100.0};
    for(double c:cs)for(double bf:bfs)for(double ww:wws)ps.push_back({bf,ww,c});
    vector<vector<int>> best,cur; long long bestS=-1;
    double maxRun=0;
    for(size_t i=0;i<ps.size();i++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(i>0 && el+maxRun*1.3>8.0)break;
        auto s=chrono::steady_clock::now();
        long long sc=runGreedy(ps[i],cur);
        double d=chrono::duration<double>(chrono::steady_clock::now()-s).count();
        maxRun=max(maxRun,d);
        if(sc>bestS){bestS=sc;best=cur;}
    }
    string o;
    for(int v=0;v<F;v++){
        o+=to_string(best[v].size());
        for(int r:best[v]){o+=' ';o+=to_string(r);}
        o+='\n';
    }
    fwrite(o.data(),1,o.size(),stdout);
}
