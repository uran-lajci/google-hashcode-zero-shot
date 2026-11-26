#include <bits/stdc++.h>
using namespace std;
int main(){
    int L,G,S,B,F,N;
    scanf("%d %d %d %d %d %d",&L,&G,&S,&B,&F,&N);
    unordered_map<string,int> id;
    vector<int> bin(S);
    vector<int> R(B,0);
    char buf[64];
    for(int i=0;i<S;i++){
        int b;
        scanf("%s %d",buf,&b);
        id[buf]=i;bin[i]=b;R[b]++;
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
        double w=0;
        for(int b:fb[i]) w+=D[i]+R[b];
        pr[i]=U[i]/w;
        ord.push_back(i);
    }
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return pr[a]>pr[b];});
    typedef pair<int,int> P;
    priority_queue<P,vector<P>,greater<P>> pq;
    for(int i=0;i<G;i++)pq.push({0,i});
    vector<priority_queue<int,vector<int>,greater<int>>> act(B);
    vector<vector<pair<int,int>>> tasks(G);
    size_t fi=0,bi=0;
    while(!pq.empty()){
        auto [t,e]=pq.top();pq.pop();
        if(t>=L)break;
        bool got=false;
        while(fi<ord.size()){
            int f=ord[fi];
            if(bi>=fb[f].size()){fi++;bi=0;continue;}
            int b=fb[f][bi++];
            auto &h=act[b];
            while(!h.empty()&&h.top()<=t)h.pop();
            int dur=D[f]+R[b]+(int)h.size();
            if(t+dur>=L)continue;
            h.push(t+dur);
            tasks[e].push_back({f,b});
            pq.push({t+dur,e});
            got=true;
            break;
        }
        if(!got)break;
    }
    int E=0;
    for(int i=0;i<G;i++)if(!tasks[i].empty())E++;
    printf("%d\n",E);
    for(int i=0;i<G;i++){
        if(tasks[i].empty())continue;
        printf("%d\n",(int)tasks[i].size());
        for(auto&p:tasks[i])printf("impl %s %d\n",fname[p.first].c_str(),p.second);
    }
}
