#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll INF=LLONG_MAX/4;

int C,T,S;
vector<ll> cc,rr,cp;
vector<vector<int>> deps;
vector<int> tf; vector<ll> td,tg;

vector<ll> freeT, early;
vector<vector<pair<int,ll>>> copiesV;
vector<pair<int,int>> steps;
struct Undo{int f,s; ll oldFree, oldEarly;};
vector<Undo> undoLog;
vector<int> candStamp; int candCur=0;
vector<int> visStamp; int visCur=0;

chrono::steady_clock::time_point st;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-st).count();}

void scheduleFile(int f){
    candCur++;
    vector<int> cand;
    ll base=0;
    for(int d:deps[f]){
        base=max(base,early[d]+rr[d]);
        for(auto&p:copiesV[d]) if(candStamp[p.first]!=candCur){candStamp[p.first]=candCur;cand.push_back(p.first);}
    }
    ll bestF=INF; int bestS=0;
    for(int s=0;s<S;s++){
        if(candStamp[s]==candCur) continue;
        ll fin=max(freeT[s],base)+cc[f];
        if(fin<bestF){bestF=fin;bestS=s;}
    }
    for(int s:cand){
        ll ready=0;
        for(int d:deps[f]){
            ll a=early[d]+rr[d];
            for(auto&p:copiesV[d]) if(p.first==s){a=min(a,p.second);}
            ready=max(ready,a);
        }
        ll fin=max(freeT[s],ready)+cc[f];
        if(fin<bestF||(fin==bestF&&s<bestS)){bestF=fin;bestS=s;}
    }
    undoLog.push_back({f,bestS,freeT[bestS],early[f]});
    freeT[bestS]=bestF;
    early[f]=min(early[f],bestF);
    copiesV[f].push_back({bestS,bestF});
    steps.push_back({f,bestS});
}
void rollbackTo(size_t pt){
    while(undoLog.size()>pt){
        Undo u=undoLog.back();undoLog.pop_back();
        freeT[u.s]=u.oldFree; early[u.f]=u.oldEarly; copiesV[u.f].pop_back(); steps.pop_back();
    }
}
ll runPass(const vector<int>& order,double limit){
    freeT.assign(S,0); early.assign(C,INF);
    for(auto&v:copiesV) v.clear();
    steps.clear(); undoLog.clear();
    vector<int> stk,clo;
    for(int ti:order){
        if(elapsed()>limit) break;
        int f=tf[ti];
        if(cp[f]>td[ti]) continue;
        if(early[f]<=td[ti]) continue;
        if(early[f]<INF) continue; // already compiled (late)
        visCur++; clo.clear(); stk.clear();
        stk.push_back(f); visStamp[f]=visCur;
        while(!stk.empty()){
            int x=stk.back();stk.pop_back();clo.push_back(x);
            for(int d:deps[x]) if(visStamp[d]!=visCur&&early[d]==INF){visStamp[d]=visCur;stk.push_back(d);}
        }
        sort(clo.begin(),clo.end());
        size_t pt=undoLog.size();
        for(int x:clo) scheduleFile(x);
        if(early[f]>td[ti]) rollbackTo(pt);
        else undoLog.clear();
    }
    ll sc=0;
    for(int i=0;i<T;i++){int f=tf[i]; if(early[f]<=td[i]) sc+=td[i]-early[f]+tg[i];}
    return sc;
}
int main(){
    st=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>T>>S;
    cc.resize(C);rr.resize(C);deps.resize(C);cp.resize(C);
    vector<string> names(C);
    unordered_map<string,int> mp; mp.reserve(C*2);
    for(int i=0;i<C;i++){
        cin>>names[i]>>cc[i]>>rr[i]; mp[names[i]]=i;
        int n;cin>>n; deps[i].resize(n);
        for(int j=0;j<n;j++){string s;cin>>s;deps[i][j]=mp[s];}
        ll m=0; for(int d:deps[i]) m=max(m,cp[d]); cp[i]=m+cc[i];
    }
    tf.resize(T);td.resize(T);tg.resize(T);
    for(int i=0;i<T;i++){string s;cin>>s>>td[i]>>tg[i];tf[i]=mp[s];}
    copiesV.assign(C,{}); candStamp.assign(S,0); visStamp.assign(C,0);

    vector<vector<double>> keys;
    {
        vector<double> k(T);
        for(int i=0;i<T;i++) k[i]=td[i]; keys.push_back(k);
        for(int i=0;i<T;i++) k[i]=-(double)tg[i]; keys.push_back(k);
        for(int i=0;i<T;i++) k[i]=-(double)(tg[i]+td[i]-cp[tf[i]])/cp[tf[i]]; keys.push_back(k);
        for(int i=0;i<T;i++) k[i]=td[i]-cp[tf[i]]; keys.push_back(k);
        for(int i=0;i<T;i++) k[i]=td[i]-0.5*tg[i]; keys.push_back(k);
    }
    const double LIMIT=8.5;
    ll bestScore=-1; vector<pair<int,int>> bestSteps; vector<double> bestKey;
    auto tryKey=[&](const vector<double>& k){
        vector<int> ord(T); iota(ord.begin(),ord.end(),0);
        stable_sort(ord.begin(),ord.end(),[&](int a,int b){return k[a]<k[b];});
        ll sc=runPass(ord,LIMIT);
        if(sc>bestScore){bestScore=sc;bestSteps=steps;bestKey=k;}
    };
    for(auto&k:keys){ if(elapsed()>LIMIT&&bestScore>=0) break; tryKey(k);}
    mt19937 rng(12345);
    double lastDur=elapsed()/max<size_t>(1,keys.size());
    while(elapsed()+lastDur*1.5<LIMIT){
        double t0=elapsed();
        vector<double> k=bestKey;
        double scale=0; for(double v:k) scale=max(scale,fabs(v)); if(scale==0) scale=1;
        uniform_real_distribution<double> U(-1,1);
        double amp=scale*0.05*(rng()%4+1)/4.0;
        for(auto&v:k) v+=U(rng)*amp;
        tryKey(k);
        lastDur=elapsed()-t0;
    }
    if(bestSteps.empty()){
        bestSteps.push_back({0,0});
    }
    string out; out.reserve(bestSteps.size()*16+16);
    out+=to_string(bestSteps.size()); out+='\n';
    for(auto&p:bestSteps){out+=names[p.first];out+=' ';out+=to_string(p.second);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
}
