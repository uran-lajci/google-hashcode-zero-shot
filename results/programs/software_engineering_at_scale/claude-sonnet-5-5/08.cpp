#include <bits/stdc++.h>
using namespace std;
int main(){
    int L,G,S,B,F,N;
    scanf("%d %d %d %d %d %d",&L,&G,&S,&B,&F,&N);
    unordered_map<string,int> id;
    vector<int> bin(S);
    vector<int> cnt(B,0);
    char buf[64];
    for(int i=0;i<S;i++){
        int b;
        scanf("%s %d",buf,&b);
        id[buf]=i;bin[i]=b;cnt[b]++;
    }
    vector<string> fname(F);
    vector<int> D(F),U(F);
    vector<vector<int>> fb(F);
    for(int i=0;i<F;i++){
        int m;
        scanf("%s %d %d %d",buf,&m,&D[i],&U[i]);
        fname[i]=buf;
        for(int j=0;j<m;j++){
            scanf("%s",buf);
            fb[i].push_back(bin[id[buf]]);
        }
        sort(fb[i].begin(),fb[i].end());
        fb[i].erase(unique(fb[i].begin(),fb[i].end()),fb[i].end());
    }
    vector<double> pr(F);
    vector<int> ord;
    for(int i=0;i<F;i++){
        double c=0;bool ok=true;
        for(int b:fb[i]){c+=D[i]+cnt[b]; if(D[i]+cnt[b]>=L) ok=false;}
        pr[i]=U[i]/c;
        if(ok) ord.push_back(i);
    }
    sort(ord.begin(),ord.end(),[&](int a,int b){
        if(pr[a]!=pr[b]) return pr[a]>pr[b];
        return a<b;});
    typedef pair<int,int> P;
    priority_queue<P,vector<P>,greater<P>> pq;
    for(int i=0;i<G;i++) pq.push({0,i});
    vector<priority_queue<int,vector<int>,greater<int>>> ends(B);
    vector<vector<pair<int,int>>> tasks(G);
    for(int f:ord){
        for(int b:fb[f]){
            auto [t,e]=pq.top();
            if(t>=L) goto done;
            pq.pop();
            auto &h=ends[b];
            while(!h.empty()&&h.top()<=t) h.pop();
            int dur=D[f]+cnt[b]+(int)h.size();
            if(t+dur>L){
                // skip this task; engineer remains free
                pq.push({t,e});
                continue;
            }
            h.push(t+dur);
            tasks[e].push_back({f,b});
            pq.push({t+dur,e});
        }
    }
    done:;
    int E=0;
    for(int i=0;i<G;i++) if(!tasks[i].empty()) E++;
    printf("%d\n",E);
    for(int i=0;i<G;i++) if(!tasks[i].empty()){
        printf("%d\n",(int)tasks[i].size());
        for(auto&p:tasks[i]) printf("impl %s %d\n",fname[p.first].c_str(),p.second);
    }
}
