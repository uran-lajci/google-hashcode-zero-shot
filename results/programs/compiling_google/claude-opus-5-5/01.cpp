#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF=LLONG_MAX/4;

int C,T,S;
vector<string> nm;
vector<ll> ct,rt;
vector<vector<int>> deps;
vector<int> tf; vector<ll> td,tg;
vector<ll> lb;

vector<vector<pair<int,ll>>> copiesV;
vector<ll> gAvail, sFree;
struct LogE{int node,s; ll oldFree,oldG;};
vector<LogE> logv;
vector<int> stamp; int stampId=0;
vector<ll> rbuf,abuf;

ll ops;

void scheduleNode(int f){
    for(int s=0;s<S;s++) rbuf[s]=0;
    for(int d:deps[f]){
        ll g=gAvail[d];
        for(int s=0;s<S;s++) abuf[s]=g;
        for(auto&p:copiesV[d]) abuf[p.first]=min(abuf[p.first],p.second);
        for(int s=0;s<S;s++) if(abuf[s]>rbuf[s]) rbuf[s]=abuf[s];
    }
    ops+=(ll)S*(deps[f].size()+1);
    int bs=0; ll be=INF;
    for(int s=0;s<S;s++){
        ll e=max(sFree[s],rbuf[s])+ct[f];
        if(e<be){be=e;bs=s;}
    }
    logv.push_back({f,bs,sFree[bs],gAvail[f]});
    copiesV[f].push_back({bs,be});
    gAvail[f]=min(gAvail[f],be+rt[f]);
    sFree[bs]=be;
}

void undoTo(size_t k){
    while(logv.size()>k){
        LogE e=logv.back();logv.pop_back();
        copiesV[e.node].pop_back();
        gAvail[e.node]=e.oldG;
        sFree[e.s]=e.oldFree;
        ops++;
    }
}

ll runOrder(const vector<int>& order, ll budget, vector<pair<int,int>>& steps){
    ops=0;
    for(int i=0;i<C;i++){copiesV[i].clear();gAvail[i]=INF;}
    for(int s=0;s<S;s++) sFree[s]=0;
    logv.clear();
    vector<int> stk,anc;
    for(int ti:order){
        if(ops>budget) break;
        int f=tf[ti];
        if(lb[f]>td[ti]) continue;
        if(!copiesV[f].empty()) continue;
        stampId++;
        anc.clear(); stk.clear();
        stk.push_back(f); stamp[f]=stampId;
        while(!stk.empty()){
            int x=stk.back();stk.pop_back();anc.push_back(x);
            for(int d:deps[x]){
                ops++;
                if(stamp[d]!=stampId && copiesV[d].empty()){stamp[d]=stampId;stk.push_back(d);}
            }
        }
        sort(anc.begin(),anc.end());
        size_t ck=logv.size();
        for(int x:anc) scheduleNode(x);
        ll fin=INF;
        for(auto&p:copiesV[f]) fin=min(fin,p.second);
        if(fin>td[ti]) undoTo(ck);
    }
    ll score=0;
    for(int i=0;i<T;i++){
        ll fin=INF;
        for(auto&p:copiesV[tf[i]]) fin=min(fin,p.second);
        if(fin<=td[i]) score+=tg[i]+td[i]-fin;
    }
    steps.clear();
    for(auto&e:logv) steps.push_back({e.node,e.s});
    return score;
}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>T>>S;
    nm.resize(C);ct.resize(C);rt.resize(C);deps.resize(C);
    unordered_map<string,int> id; id.reserve(C*2);
    for(int i=0;i<C;i++){
        cin>>nm[i]>>ct[i]>>rt[i];
        id[nm[i]]=i;
        int n;cin>>n;
        deps[i].resize(n);
        for(int j=0;j<n;j++){string s;cin>>s;deps[i][j]=id[s];}
    }
    tf.resize(T);td.resize(T);tg.resize(T);
    for(int i=0;i<T;i++){string s;cin>>s>>td[i]>>tg[i];tf[i]=id[s];}
    lb.assign(C,0);
    for(int i=0;i<C;i++){
        ll m=0;for(int d:deps[i]) m=max(m,lb[d]);
        lb[i]=m+ct[i];
    }
    copiesV.assign(C,{});gAvail.assign(C,INF);sFree.assign(S,0);
    stamp.assign(C,0);rbuf.assign(S,0);abuf.assign(S,0);

    vector<vector<double>> keys(4,vector<double>(T));
    for(int i=0;i<T;i++){
        keys[0][i]=(double)td[i];
        keys[1][i]=-(double)tg[i];
        keys[2][i]=-(double)tg[i]/(double)lb[tf[i]];
        keys[3][i]=(double)(td[i]-lb[tf[i]]);
    }
    ll totalBudget=1500000000LL;
    ll used=0;
    ll bestScore=-1; vector<pair<int,int>> bestSteps, steps;
    mt19937 rng(12345);
    ll lastCost=0;
    int it=0;
    while(true){
        vector<double> key(T);
        if(it<4) key=keys[it];
        else{
            int k=rng()%4;
            uniform_real_distribution<double> U(-0.4,0.4);
            for(int i=0;i<T;i++){
                double b=keys[k][i];
                double noise=exp(U(rng));
                key[i]= b>=0? b*noise : b*noise;
            }
        }
        vector<int> order(T);
        iota(order.begin(),order.end(),0);
        stable_sort(order.begin(),order.end(),[&](int a,int b){return key[a]<key[b];});
        ll budget = (it==0)? totalBudget*2/3 : totalBudget-used;
        if(it>0 && (lastCost*1.2>budget || budget<=0)) break;
        ll sc=runOrder(order,budget,steps);
        lastCost=max(lastCost,ops);
        used+=ops;
        if(sc>bestScore){bestScore=sc;bestSteps=steps;}
        it++;
        if(it>=60) break;
    }
    if(bestSteps.empty()){
        int f=0;
        for(int i=0;i<C;i++) if(deps[i].empty()){f=i;break;}
        bestSteps.push_back({f,0});
    }
    string out;
    out+=to_string(bestSteps.size())+"\n";
    for(auto&p:bestSteps){out+=nm[p.first];out+=' ';out+=to_string(p.second);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
}
