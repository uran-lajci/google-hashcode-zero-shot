#include <bits/stdc++.h>
using namespace std;

int L,G,S,B,F,N;
vector<int> R;
vector<string> fname;
vector<int> FD; vector<long long> FU;
vector<vector<int>> fb;

struct Res{ long long score; vector<array<int,3>> asg; vector<int> done; };

Res sim(const vector<int>& order){
    // build task list
    vector<int> tf, tb;
    for(int f:order) for(int b:fb[f]){tf.push_back(f);tb.push_back(b);}
    int T=tf.size();
    vector<int> nxt(T+1), prv(T+1);
    // linked list with sentinel T
    for(int i=0;i<T;i++){nxt[i]=i+1;prv[i]=(i==0?T:i-1);}
    nxt[T]=(T>0?0:T); prv[T]=(T>0?T-1:T);
    auto rem=[&](int i){nxt[prv[i]]=nxt[i];prv[nxt[i]]=prv[i];};
    vector<priority_queue<int,vector<int>,greater<int>>> act(B);
    vector<int> remain(F,0), fin(F,0);
    for(int f:order) remain[f]=fb[f].size();
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> pq;
    for(int g=0;g<G;g++) pq.push({0,g});
    Res r; r.score=0;
    int left=T;
    const int K=20;
    while(left>0 && !pq.empty()){
        auto [t,g]=pq.top(); pq.pop();
        if(t>=L) break;
        int pick=-1;
        int c=nxt[T];
        for(int k=0;k<K && c!=T;k++,c=nxt[c]){
            int b=tb[c];
            auto &h=act[b];
            while(!h.empty()&&h.top()<=t) h.pop();
            if(h.empty()){pick=c;break;}
        }
        if(pick<0) pick=nxt[T];
        int b=tb[pick], f=tf[pick];
        auto &h=act[b];
        while(!h.empty()&&h.top()<=t) h.pop();
        int C=h.size();
        int dur=FD[f]+R[b]+C;
        int e=t+dur;
        h.push(e);
        rem(pick); left--;
        r.asg.push_back({g,f,b});
        remain[f]--; fin[f]=max(fin[f],e);
        pq.push({e,g});
    }
    for(int f:order){
        if(remain[f]==0 && fin[f]<L){ r.score+=FU[f]*(long long)(L-fin[f]); r.done.push_back(f);}
    }
    return r;
}

int main(){
    scanf("%d %d %d %d %d %d",&L,&G,&S,&B,&F,&N);
    unordered_map<string,int> sid; sid.reserve(S*2);
    vector<int> sb(S); R.assign(B,0);
    char buf[64];
    for(int i=0;i<S;i++){int b;scanf("%s %d",buf,&b);sid[buf]=i;sb[i]=b;R[b]++;}
    fname.resize(F);FD.resize(F);FU.resize(F);fb.resize(F);
    long long totTasks=0;
    for(int i=0;i<F;i++){
        int m,d;long long u;scanf("%s %d %d %lld",buf,&m,&d,&u);
        fname[i]=buf;FD[i]=d;FU[i]=u;
        vector<int> bs;
        for(int j=0;j<m;j++){scanf("%s",buf);bs.push_back(sb[sid[buf]]);}
        sort(bs.begin(),bs.end());bs.erase(unique(bs.begin(),bs.end()),bs.end());
        fb[i]=bs; totTasks+=bs.size();
    }
    vector<double> cost(F), mx(F);
    for(int i=0;i<F;i++){double c=0,m=0;for(int b:fb[i]){c+=FD[i]+R[b];m=max(m,(double)FD[i]+R[b]);}cost[i]=c;mx[i]=m;}
    double perRun=(double)totTasks*25+ (double)G*20 + B + F*5;
    long long budget=(long long)(1.5e9/perRun);
    if(budget<1)budget=1;
    Res best; best.score=-1;
    int variants=3;
    long long runsPer=max(1LL,budget/variants);
    if(runsPer>15)runsPer=15;
    for(int v=0;v<variants;v++){
        vector<double> pr(F);
        for(int i=0;i<F;i++){
            if(v==0) pr[i]=FU[i]/cost[i];
            else if(v==1) pr[i]=FU[i]/pow(cost[i],1.5);
            else pr[i]=FU[i]*max(1.0,L-mx[i])/cost[i];
        }
        vector<int> order;
        for(int i=0;i<F;i++) if(mx[i]<L) order.push_back(i);
        stable_sort(order.begin(),order.end(),[&](int a,int b){return pr[a]>pr[b];});
        for(long long it=0;it<runsPer;it++){
            Res r=sim(order);
            bool improved = r.score>best.score;
            if(improved) best=r;
            if(r.done.size()==order.size()) break;
            vector<char> ok(F,0); for(int f:r.done) ok[f]=1;
            vector<int> no; for(int f:order) if(ok[f]) no.push_back(f);
            if(no.empty()) break;
            order=no;
        }
        if(budget<=variants) { if(v+1>=budget) break; }
    }
    // output
    vector<vector<pair<int,int>>> et;
    unordered_map<int,int> eidx;
    vector<int> engs;
    for(auto&a:best.asg){ if(!eidx.count(a[0])){eidx[a[0]]=0;engs.push_back(a[0]);} }
    sort(engs.begin(),engs.end());
    for(size_t i=0;i<engs.size();i++) eidx[engs[i]]=i;
    et.resize(engs.size());
    for(auto&a:best.asg) et[eidx[a[0]]].push_back({a[1],a[2]});
    string out;
    out+=to_string(et.size())+"\n";
    for(auto&v:et){
        out+=to_string(v.size())+"\n";
        for(auto&p:v){out+="impl ";out+=fname[p.first];out+=' ';out+=to_string(p.second);out+='\n';}
    }
    fwrite(out.data(),1,out.size(),stdout);
}
