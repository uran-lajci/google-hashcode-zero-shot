#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,C,F,N; ll B,T;
vector<ll> ra,rb,rx,ry,rs,rf,rd;

struct Result{ ll score; vector<vector<int>> plan; };

Result runGreedy(double w,double c,double bf){
    vector<ll> px(F,0),py(F,0),pt(F,0);
    vector<char> used(N,0);
    vector<vector<int>> plan(F);
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int i=0;i<F;i++) pq.push({0,i});
    ll score=0;
    while(!pq.empty()){
        auto [t,car]=pq.top();pq.pop();
        ll x=px[car],y=py[car];
        int best=-1;double bv=-1;ll bend=0;bool bbon=false;
        for(int i=0;i<N;i++){
            if(used[i])continue;
            ll arrive=t+llabs(x-ra[i])+llabs(y-rb[i]);
            ll st=max(arrive,rs[i]);
            ll en=st+rd[i];
            if(en>rf[i]||en>T)continue;
            bool bon=(st==rs[i]);
            double gain=rd[i]+(bon?B*bf:0.0);
            double denom=(arrive-t)+w*(st-arrive)+rd[i]+c;
            double v=gain/denom;
            if(v>bv){bv=v;best=i;bend=en;bbon=bon;}
        }
        if(best<0)continue;
        used[best]=1;
        plan[car].push_back(best);
        score+=rd[best]+(bbon?B:0);
        px[car]=rx[best];py[car]=ry[best];pt[car]=bend;
        pq.push({bend,car});
    }
    return {score,plan};
}

int main(){
    auto t0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %lld %lld",&R,&C,&F,&N,&B,&T)!=6)return 0;
    ra.resize(N);rb.resize(N);rx.resize(N);ry.resize(N);rs.resize(N);rf.resize(N);rd.resize(N);
    for(int i=0;i<N;i++){
        scanf("%lld %lld %lld %lld %lld %lld",&ra[i],&rb[i],&rx[i],&ry[i],&rs[i],&rf[i]);
        rd[i]=llabs(ra[i]-rx[i])+llabs(rb[i]-ry[i]);
    }
    vector<array<double,3>> params;
    double ws[]={1.0,0.5,2.0,1.5,0.75,3.0,0.25};
    double cs[]={1.0,10.0,50.0,0.5,200.0,1000.0};
    double bfs[]={1.0,2.0,0.5,0.0,4.0};
    for(double bf:bfs)for(double w:ws)for(double c:cs)params.push_back({w,c,bf});
    Result best{-1,{}};
    double lastDur=0;
    for(auto&p:params){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(best.score>=0 && el+lastDur*1.3>7.5)break;
        auto s=chrono::steady_clock::now();
        Result r=runGreedy(p[0],p[1],p[2]);
        lastDur=chrono::duration<double>(chrono::steady_clock::now()-s).count();
        if(r.score>best.score)best=move(r);
    }
    string out;
    for(int i=0;i<F;i++){
        out+=to_string(best.plan[i].size());
        for(int x:best.plan[i]){out+=' ';out+=to_string(x);}
        out+='\n';
    }
    fputs(out.c_str(),stdout);
    return 0;
}
