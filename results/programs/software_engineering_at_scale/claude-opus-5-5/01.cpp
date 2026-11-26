#include <bits/stdc++.h>
using namespace std;

struct Feat{
    string name; int D; long long U;
    vector<int> bins; long long work; int maxlen;
};
struct Task{ int type; int f; int b; }; // type 0 = impl, 1 = wait (f = days)

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    int L,G,S,B,F,N;
    if(!(cin>>L>>G>>S>>B>>F>>N)) return 0;
    unordered_map<string,int> sid; sid.reserve(S*2+10);
    vector<int> sb(S);
    vector<int> R(B,0);
    for(int i=0;i<S;i++){string nm;int b;cin>>nm>>b;sid[nm]=i;sb[i]=b;R[b]++;}
    vector<Feat> fs(F);
    vector<int> mark(B,-1);
    for(int i=0;i<F;i++){
        int M; cin>>fs[i].name>>M>>fs[i].D>>fs[i].U;
        for(int j=0;j<M;j++){
            string nm;cin>>nm;
            auto it=sid.find(nm);
            if(it==sid.end()) continue;
            int b=sb[it->second];
            if(mark[b]!=i){mark[b]=i;fs[i].bins.push_back(b);}
        }
        auto &ft=fs[i];
        sort(ft.bins.begin(),ft.bins.end(),[&](int a,int c){ if(R[a]!=R[c]) return R[a]>R[c]; return a<c;});
        ft.work=0; ft.maxlen=0;
        for(int b:ft.bins){ ft.work+=ft.D+R[b]; ft.maxlen=max(ft.maxlen,ft.D+R[b]); }
    }
    vector<int> order;
    vector<double> key(F,0);
    for(int i=0;i<F;i++){
        if(fs[i].bins.empty()) continue;
        if(fs[i].maxlen>=L) continue;
        key[i]=(double)fs[i].U*(double)(L-fs[i].maxlen)/(double)fs[i].work;
        order.push_back(i);
    }
    stable_sort(order.begin(),order.end(),[&](int a,int b){ if(key[a]!=key[b]) return key[a]>key[b]; return a<b;});

    vector<int> cnt(B,0);
    vector<vector<int>> bucket(L+1);
    vector<int> curBin(G,-1), lastFree(G,0);
    vector<vector<Task>> tasks(G);
    vector<char> doomed(F,0);
    deque<pair<int,int>> pending;
    size_t ptr=0;
    vector<int> idle(G); for(int i=0;i<G;i++) idle[i]=i;

    for(int t=0;t<L;t++){
        vector<int>& nw=bucket[t];
        for(int e:nw){ cnt[curBin[e]]--; lastFree[e]=t; }
        if(!nw.empty()){
            sort(nw.begin(),nw.end());
            vector<int> merged; merged.reserve(idle.size()+nw.size());
            merge(idle.begin(),idle.end(),nw.begin(),nw.end(),back_inserter(merged));
            idle.swap(merged);
            vector<int>().swap(nw);
        }
        if(idle.empty()) continue;
        if(pending.empty() && ptr>=order.size()) break;
        vector<int> newIdle;
        size_t k=0;
        for(;k<idle.size();k++){
            int e=idle[k];
            int gf=-1,gb=-1,gd=0;
            while(true){
                while(!pending.empty()){
                    auto pr=pending.front();
                    if(doomed[pr.first]){pending.pop_front();continue;}
                    int dur=fs[pr.first].D+R[pr.second]+cnt[pr.second];
                    if(t+dur>=L){doomed[pr.first]=1;pending.pop_front();continue;}
                    gf=pr.first;gb=pr.second;gd=dur;pending.pop_front();break;
                }
                if(gf>=0) break;
                bool opened=false;
                while(ptr<order.size()){
                    int f=order[ptr++];
                    int mx=0;
                    for(int b:fs[f].bins) mx=max(mx,fs[f].D+R[b]+cnt[b]);
                    if(t+mx<L){
                        for(int b:fs[f].bins) pending.push_back({f,b});
                        opened=true;break;
                    }
                }
                if(!opened) break;
            }
            if(gf<0) break;
            if(t>lastFree[e]) tasks[e].push_back({1,t-lastFree[e],0});
            tasks[e].push_back({0,gf,gb});
            cnt[gb]++;
            curBin[e]=gb;
            int end=t+gd;
            if(end<L) bucket[end].push_back(e);
        }
        for(;k<idle.size();k++) newIdle.push_back(idle[k]);
        idle.swap(newIdle);
    }

    string out;
    int E=0;
    for(int e=0;e<G;e++) if(!tasks[e].empty()) E++;
    out+=to_string(E); out+='\n';
    for(int e=0;e<G;e++){
        if(tasks[e].empty()) continue;
        out+=to_string(tasks[e].size()); out+='\n';
        for(auto &tk:tasks[e]){
            if(tk.type==1){ out+="wait "; out+=to_string(tk.f); out+='\n'; }
            else { out+="impl "; out+=fs[tk.f].name; out+=' '; out+=to_string(tk.b); out+='\n'; }
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
