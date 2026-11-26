#include <bits/stdc++.h>
using namespace std;

int L,G,S,B,F,N;
vector<string> sname, fname;
vector<int> sbin;
vector<int> FD, FU;
vector<vector<int>> fb; // binaries per feature
vector<int> R;

long long simulate(const vector<int>& order, bool record, vector<vector<pair<int,int>>>* out){
    vector<char> dead(F,0);
    vector<int> fin(F,0), rem(F,0);
    for(int f=0;f<F;f++) rem[f]=(int)fb[f].size();
    vector<int> active(B,0);
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> eq, fq;
    for(int i=0;i<G;i++) eq.push({0,i});
    deque<pair<int,int>> pending;
    size_t ptr=0;
    while(!eq.empty()){
        auto [t,e]=eq.top(); eq.pop();
        while(!fq.empty() && fq.top().first<=t){ active[fq.top().second]--; fq.pop(); }
        while(true){
            while(pending.empty() && ptr<order.size()){
                int f=order[ptr++];
                int worst=0;
                for(int b:fb[f]) worst=max(worst,FD[f]+R[b]+active[b]);
                if((long long)t+worst<L){
                    for(int b:fb[f]) pending.push_back({f,b});
                }
            }
            if(pending.empty()) break;
            auto [f,b]=pending.front(); pending.pop_front();
            if(dead[f]) continue;
            int d=FD[f]+R[b]+active[b];
            if((long long)t+d>=L){ dead[f]=1; continue; }
            active[b]++;
            fq.push({t+d,b});
            fin[f]=max(fin[f],t+d);
            rem[f]--;
            if(record) (*out)[e].push_back({f,b});
            eq.push({t+d,e});
            break;
        }
    }
    long long sc=0;
    for(int f=0;f<F;f++) if(!dead[f]&&rem[f]==0&&fin[f]<L) sc+=(long long)FU[f]*(L-fin[f]);
    return sc;
}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>L>>G>>S>>B>>F>>N;
    sname.resize(S); sbin.resize(S);
    unordered_map<string,int> sid; sid.reserve(S*2);
    R.assign(B,0);
    for(int i=0;i<S;i++){cin>>sname[i]>>sbin[i]; sid[sname[i]]=i; R[sbin[i]]++;}
    fname.resize(F); FD.resize(F); FU.resize(F); fb.resize(F);
    for(int f=0;f<F;f++){
        int M; cin>>fname[f]>>M>>FD[f]>>FU[f];
        vector<int> bs;
        for(int j=0;j<M;j++){string s;cin>>s; auto it=sid.find(s); if(it!=sid.end()) bs.push_back(sbin[it->second]);}
        sort(bs.begin(),bs.end()); bs.erase(unique(bs.begin(),bs.end()),bs.end());
        fb[f]=bs;
    }
    vector<double> W(F), MX(F);
    for(int f=0;f<F;f++){
        double w=0,m=0;
        for(int b:fb[f]){w+=FD[f]+R[b]; m=max(m,(double)FD[f]+R[b]);}
        W[f]=max(w,1.0); MX[f]=max(m,1.0);
    }
    vector<vector<double>> keys;
    for(double a:{0.5,1.0,1.5,2.0}){
        vector<double> k(F); for(int f=0;f<F;f++) k[f]=FU[f]/pow(W[f],a); keys.push_back(k);
    }
    {vector<double> k(F); for(int f=0;f<F;f++) k[f]=FU[f]/MX[f]; keys.push_back(k);}
    {vector<double> k(F); for(int f=0;f<F;f++) k[f]=FU[f]*max(0.0,L-MX[f])/W[f]; keys.push_back(k);}
    long long best=-1; vector<int> bestOrder;
    for(auto&k:keys){
        vector<int> ord;
        for(int f=0;f<F;f++) if(!fb[f].empty()) ord.push_back(f);
        stable_sort(ord.begin(),ord.end(),[&](int a,int b){return k[a]>k[b];});
        long long sc=simulate(ord,false,nullptr);
        if(sc>best){best=sc;bestOrder=ord;}
    }
    vector<vector<pair<int,int>>> out(G);
    simulate(bestOrder,true,&out);
    int E=0; for(int i=0;i<G;i++) if(!out[i].empty()) E++;
    string res; res.reserve(1<<20);
    res+=to_string(E)+"\n";
    for(int i=0;i<G;i++){
        if(out[i].empty()) continue;
        res+=to_string(out[i].size())+"\n";
        for(auto&p:out[i]){ res+="impl "; res+=fname[p.first]; res+=" "; res+=to_string(p.second); res+="\n"; }
    }
    fwrite(res.data(),1,res.size(),stdout);
    return 0;
}
