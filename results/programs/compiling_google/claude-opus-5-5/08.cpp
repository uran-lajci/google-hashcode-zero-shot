#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int C,T,S;
vector<ll> cc, rr, cp;
vector<vector<int>> deps;
vector<string> names;
vector<int> tIdx; vector<ll> tD, tG;

vector<ll> fin; vector<int> srv; vector<ll> freeT;
vector<int> steps; vector<ll> oldFree;
vector<int> stampV; int stampC=0;
vector<int> gStamp; vector<ll> gLocal, gRemote; int gC=0;
ll work=0;

void scheduleFile(int f){
    int bestS=0; ll bf=LLONG_MAX;
    int am=0; for(int s=1;s<S;s++) if(freeT[s]<freeT[am]) am=s;
    if(deps[f].empty()){
        bestS=am; bf=freeT[am]+cc[f];
    } else {
        gC++;
        static vector<int> touched; touched.clear();
        for(int d:deps[f]){
            int s=srv[d];
            if(gStamp[s]!=gC){gStamp[s]=gC;gLocal[s]=0;gRemote[s]=0;touched.push_back(s);}
            gLocal[s]=max(gLocal[s],fin[d]);
            gRemote[s]=max(gRemote[s],fin[d]+rr[d]);
        }
        work+=deps[f].size()+touched.size();
        int top=-1; ll t1=0,t2=0;
        for(int s:touched){
            if(top<0||gRemote[s]>t1){t2=t1;t1=gRemote[s];top=s;}
            else if(gRemote[s]>t2)t2=gRemote[s];
        }
        auto eval=[&](int s){
            ll ready;
            if(gStamp[s]==gC){
                ready=max(gLocal[s], s==top?t2:t1);
            } else ready=t1;
            ll st=max(freeT[s],ready);
            ll fn=st+cc[f];
            if(fn<bf||(fn==bf&&freeT[s]<freeT[bestS])){bf=fn;bestS=s;}
        };
        for(int s:touched) eval(s);
        eval(am);
    }
    oldFree.push_back(freeT[bestS]);
    steps.push_back(f);
    srv[f]=bestS; fin[f]=bf; freeT[bestS]=bf;
}
void undoTo(size_t mark){
    while(steps.size()>mark){
        int f=steps.back(); steps.pop_back();
        freeT[srv[f]]=oldFree.back(); oldFree.pop_back();
        fin[f]=-1;
    }
}

ll runOrder(const vector<int>& order, ll budget){
    fin.assign(C,-1); srv.assign(C,0); freeT.assign(S,0);
    steps.clear(); oldFree.clear(); work=0;
    vector<int> stk, clos;
    for(int ti:order){
        if(work>budget) break;
        int tf=tIdx[ti]; ll d=tD[ti];
        if(fin[tf]>=0) continue;
        if(cp[tf]>d) continue;
        stampC++; clos.clear(); stk.clear();
        stk.push_back(tf); stampV[tf]=stampC;
        while(!stk.empty()){
            int x=stk.back(); stk.pop_back(); clos.push_back(x);
            for(int y:deps[x]){ work++; if(fin[y]<0&&stampV[y]!=stampC){stampV[y]=stampC;stk.push_back(y);} }
        }
        sort(clos.begin(),clos.end());
        size_t mark=steps.size(); bool ok=true;
        for(int f:clos){
            scheduleFile(f);
            if(fin[f]>d){ok=false;break;}
        }
        if(!ok) undoTo(mark);
    }
    ll score=0;
    for(int i=0;i<T;i++){int f=tIdx[i]; if(fin[f]>=0&&fin[f]<=tD[i]) score+=tD[i]-fin[f]+tG[i];}
    return score;
}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>T>>S;
    cc.resize(C);rr.resize(C);deps.resize(C);names.resize(C);cp.resize(C);
    unordered_map<string,int> mp; mp.reserve(C*2);
    for(int i=0;i<C;i++){
        cin>>names[i]>>cc[i]>>rr[i]; mp[names[i]]=i;
        int n;cin>>n; deps[i].resize(n);
        ll m=0;
        for(int j=0;j<n;j++){string s;cin>>s;deps[i][j]=mp[s]; m=max(m,cp[deps[i][j]]);}
        cp[i]=m+cc[i];
    }
    tIdx.resize(T);tD.resize(T);tG.resize(T);
    for(int i=0;i<T;i++){string s;cin>>s>>tD[i]>>tG[i];tIdx[i]=mp[s];}
    stampV.assign(C,0); gStamp.assign(S,0); gLocal.assign(S,0); gRemote.assign(S,0);

    vector<vector<int>> orders;
    vector<int> base(T); iota(base.begin(),base.end(),0);
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return tD[a]<tD[b];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return tG[a]>tG[b];}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){
        double va=(double)(tG[a]+tD[a]-cp[tIdx[a]])/cp[tIdx[a]];
        double vb=(double)(tG[b]+tD[b]-cp[tIdx[b]])/cp[tIdx[b]];
        return va>vb;}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){return cp[tIdx[a]]<cp[tIdx[b]];}); orders.push_back(o);}

    ll bestScore=-1; vector<int> bestSteps, bestSrv;
    ll budget=150000000LL;
    for(auto&o:orders){
        ll sc=runOrder(o,budget);
        if(sc>bestScore){
            bestScore=sc; bestSteps=steps;
            bestSrv.clear(); for(int f:steps) bestSrv.push_back(srv[f]);
        }
    }
    string out;
    if(bestSteps.empty()){
        out="1\n"+names[0]+" 0\n";
    } else {
        out=to_string(bestSteps.size())+"\n";
        for(size_t i=0;i<bestSteps.size();i++){
            out+=names[bestSteps[i]]; out+=' '; out+=to_string(bestSrv[i]); out+='\n';
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
}
