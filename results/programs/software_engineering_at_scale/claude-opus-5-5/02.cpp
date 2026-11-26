#include <bits/stdc++.h>
using namespace std;

int L,G,S,B,F,N;
vector<int> Rb;               // services per binary
vector<string> fname;
vector<int> FD, FU;
vector<vector<int>> fb;       // binaries per feature

struct Res { long long score; vector<vector<pair<int,int>>> tasks; };

long long simulate(const vector<int>& order, vector<vector<pair<int,int>>>* rec){
    vector<priority_queue<int,vector<int>,greater<int>>> act(B);
    vector<vector<int>> bucket(L+1);
    bucket[0].resize(G);
    for(int i=0;i<G;i++) bucket[0][i]=i;
    vector<char> dead(F,0);
    vector<int> rem(F,0), fe(F,0);
    vector<char> opened(F,0);
    deque<pair<int,int>> pending;
    size_t ptr=0;
    bool allDone=false;
    if(rec){ rec->assign(G,{}); }
    for(int t=0;t<L && !allDone;t++){
        auto &bk=bucket[t];
        sort(bk.begin(),bk.end());
        for(int e:bk){
            bool assigned=false;
            while(!assigned){
                if(!pending.empty()){
                    auto [f,b]=pending.front(); pending.pop_front();
                    if(dead[f]) continue;
                    auto &h=act[b];
                    while(!h.empty() && h.top()<=t) h.pop();
                    int C=h.size();
                    int dur=FD[f]+Rb[b]+C;
                    if(t+dur>=L){ dead[f]=1; continue; }
                    h.push(t+dur);
                    fe[f]=max(fe[f],t+dur);
                    rem[f]--;
                    bucket[t+dur].push_back(e);
                    if(rec) (*rec)[e].push_back({f,b});
                    assigned=true;
                } else if(ptr<order.size()){
                    int f=order[ptr++];
                    bool ok=true;
                    for(int b:fb[f]){
                        auto &h=act[b];
                        while(!h.empty() && h.top()<=t) h.pop();
                        int C=h.size();
                        if(t+FD[f]+Rb[b]+C>=L){ok=false;break;}
                    }
                    if(!ok) continue;
                    opened[f]=1;
                    rem[f]=fb[f].size();
                    for(int b:fb[f]) pending.push_back({f,b});
                } else { allDone=true; break; }
            }
            if(allDone) break;
        }
        vector<int>().swap(bk);
    }
    long long sc=0;
    for(int f=0;f<F;f++){
        if(opened[f] && !dead[f] && rem[f]==0 && fe[f]<L) sc+=(long long)FU[f]*(L-fe[f]);
    }
    return sc;
}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>L>>G>>S>>B>>F>>N;
    unordered_map<string,int> sid; sid.reserve(S*2);
    vector<int> sb(S);
    Rb.assign(B,0);
    for(int i=0;i<S;i++){string s;int b;cin>>s>>b;sid[s]=i;sb[i]=b;Rb[b]++;}
    fname.resize(F);FD.resize(F);FU.resize(F);fb.resize(F);
    long long totalTasks=0;
    for(int i=0;i<F;i++){
        int M;cin>>fname[i]>>M>>FD[i]>>FU[i];
        vector<int> bs;
        for(int j=0;j<M;j++){string s;cin>>s;bs.push_back(sb[sid[s]]);}
        sort(bs.begin(),bs.end());bs.erase(unique(bs.begin(),bs.end()),bs.end());
        int D=FD[i];
        sort(bs.begin(),bs.end(),[&](int a,int b){
            if(Rb[a]!=Rb[b]) return Rb[a]>Rb[b]; return a<b;});
        fb[i]=bs; totalTasks+=bs.size();
        (void)D;
    }
    vector<double> sumc(F),maxc(F);
    for(int f=0;f<F;f++){
        double s=0,m=0;
        for(int b:fb[f]){double c=FD[f]+Rb[b];s+=c;m=max(m,c);}
        sumc[f]=s;maxc[f]=m;
    }
    auto makeOrder=[&](const vector<double>& key){
        vector<int> o(F);iota(o.begin(),o.end(),0);
        stable_sort(o.begin(),o.end(),[&](int a,int b){return key[a]>key[b];});
        return o;
    };
    vector<vector<double>> keys;
    for(int k=0;k<5;k++){
        vector<double> key(F);
        for(int f=0;f<F;f++){
            double U=FU[f],sc=sumc[f],mc=maxc[f],nb=fb[f].size();
            double rem=max(1.0,(double)L-mc);
            switch(k){
                case 0:key[f]=U/sc;break;
                case 1:key[f]=U*rem/sc;break;
                case 2:key[f]=U/(sc*mc);break;
                case 3:key[f]=U*rem/nb;break;
                case 4:key[f]=U*rem/(sc*sqrt(mc));break;
            }
        }
        keys.push_back(key);
    }
    long long best=-1; vector<int> bestOrder; vector<double> bestKey;
    for(auto&key:keys){
        auto o=makeOrder(key);
        long long s=simulate(o,nullptr);
        if(s>best){best=s;bestOrder=o;bestKey=key;}
    }
    double work=(double)totalTasks+G+L+F;
    int iters=(int)min(60.0,3e7/work);
    mt19937 rng(12345);
    normal_distribution<double> nd(0.0,1.0);
    for(int it=0;it<iters;it++){
        double sigma=0.3*(1.0-(double)it/max(1,iters))+0.02;
        vector<double> key(F);
        for(int f=0;f<F;f++) key[f]=bestKey[f]*exp(sigma*nd(rng));
        auto o=makeOrder(key);
        long long s=simulate(o,nullptr);
        if(s>best){best=s;bestOrder=o;bestKey=key;}
    }
    vector<vector<pair<int,int>>> rec;
    simulate(bestOrder,&rec);
    string out;
    int E=0;
    for(int e=0;e<G;e++) if(!rec[e].empty()) E++;
    out+=to_string(E)+"\n";
    for(int e=0;e<G;e++){
        if(rec[e].empty()) continue;
        out+=to_string(rec[e].size())+"\n";
        for(auto&[f,b]:rec[e]){
            out+="impl "+fname[f]+" "+to_string(b)+"\n";
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
