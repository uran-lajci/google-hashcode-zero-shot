#include <bits/stdc++.h>
using namespace std;

int L,G,S,B,F,N;
vector<string> sname, fname;
vector<int> sbin;
vector<int> Dv, Uv;
vector<vector<int>> jobs; // binaries per feature sorted by R desc
vector<int> R;

struct Result{ long long score; vector<vector<pair<int,int>>> tasks; };

Result simulate(const vector<int>& order, bool record){
    Result res; res.score=0;
    if(record) res.tasks.assign(G,{});
    vector<int> cnt(B,0);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> ev, ends;
    for(int e=0;e<G;e++) ev.push({0,e});
    vector<int> finishMax(F,0), assigned(F,0);
    vector<char> doomed(F,0), opened(F,0);
    size_t p=0; int cur=-1; size_t jp=0;
    while(!ev.empty()){
        auto [t,e]=ev.top(); ev.pop();
        if(t>=L) continue;
        while(!ends.empty() && ends.top().first<=t){ cnt[ends.top().second]--; ends.pop(); }
        bool got=false;
        while(true){
            if(cur>=0 && jp<jobs[cur].size()){
                int b=jobs[cur][jp];
                int dur=Dv[cur]+R[b]+cnt[b];
                if(t+dur<L){
                    jp++;
                    cnt[b]++;
                    ends.push({t+dur,b});
                    ev.push({t+dur,e});
                    assigned[cur]++;
                    finishMax[cur]=max(finishMax[cur],t+dur);
                    if(record) res.tasks[e].push_back({cur,b});
                    got=true; break;
                } else { doomed[cur]=1; cur=-1; continue; }
            }
            // open next feature
            cur=-1;
            bool found=false;
            while(p<order.size()){
                int f=order[p++];
                int mx=0;
                for(int b:jobs[f]) mx=max(mx,Dv[f]+R[b]+cnt[b]);
                if(t+mx<L){ cur=f; jp=0; opened[f]=1; found=true; break; }
            }
            if(!found) break;
        }
        (void)got;
    }
    for(int f=0;f<F;f++){
        if(opened[f] && !doomed[f] && assigned[f]==(int)jobs[f].size() && finishMax[f]<L)
            res.score+=(long long)Uv[f]*(L-finishMax[f]);
    }
    return res;
}

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    cin>>L>>G>>S>>B>>F>>N;
    sname.resize(S); sbin.resize(S);
    unordered_map<string,int> sid; sid.reserve(S*2);
    R.assign(B,0);
    for(int i=0;i<S;i++){ cin>>sname[i]>>sbin[i]; sid[sname[i]]=i; R[sbin[i]]++; }
    fname.resize(F); Dv.resize(F); Uv.resize(F); jobs.resize(F);
    for(int i=0;i<F;i++){
        int M; cin>>fname[i]>>M>>Dv[i]>>Uv[i];
        vector<int> bs; bs.reserve(M);
        for(int j=0;j<M;j++){ string s; cin>>s; bs.push_back(sbin[sid[s]]); }
        sort(bs.begin(),bs.end()); bs.erase(unique(bs.begin(),bs.end()),bs.end());
        sort(bs.begin(),bs.end(),[&](int a,int b){ if(R[a]!=R[b]) return R[a]>R[b]; return a<b;});
        jobs[i]=bs;
    }
    vector<double> work(F), lat(F);
    vector<int> valid;
    for(int f=0;f<F;f++){
        double w=0,l=0;
        for(int b:jobs[f]){ double d=Dv[f]+R[b]; w+=d; l=max(l,d);}
        work[f]=w; lat[f]=l;
        if(l<L) valid.push_back(f);
    }
    vector<pair<double,double>> variants={{1,0},{1,1},{0.5,1},{1.5,1},{0,1},{1,2},{0.75,1},{2,1}};
    long long best=-1; vector<int> bestOrder;
    auto startT=chrono::steady_clock::now();
    for(auto [a,c]:variants){
        double el=chrono::duration<double>(chrono::steady_clock::now()-startT).count();
        if(el>6.0) break;
        vector<double> key(F,0);
        for(int f:valid) key[f]=log((double)Uv[f])+c*log((double)(L-lat[f]))-a*log(work[f]);
        vector<int> ord=valid;
        stable_sort(ord.begin(),ord.end(),[&](int x,int y){ return key[x]>key[y]; });
        Result r=simulate(ord,false);
        if(r.score>best){ best=r.score; bestOrder=ord; }
    }
    Result r=simulate(bestOrder,true);
    string out;
    int E=0;
    for(int e=0;e<G;e++) if(!r.tasks[e].empty()) E++;
    out+=to_string(E)+"\n";
    for(int e=0;e<G;e++){
        if(r.tasks[e].empty()) continue;
        out+=to_string(r.tasks[e].size())+"\n";
        for(auto [f,b]:r.tasks[e]){
            out+="impl "; out+=fname[f]; out+=' '; out+=to_string(b); out+='\n';
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
