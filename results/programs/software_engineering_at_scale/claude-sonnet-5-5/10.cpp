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
    vector<double> key(F);
    for(int i=0;i<F;i++){
        double w=0;
        for(int b:fb[i]) w+=D[i]+R[b];
        key[i]=U[i]/w;
    }
    vector<int> ord(F);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return key[a]>key[b];});
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> eq;
    for(int i=0;i<G;i++) eq.push({0,i});
    vector<priority_queue<int,vector<int>,greater<int>>> bh(B);
    vector<vector<pair<int,int>>> tasks(G);
    bool done=false;
    for(int f:ord){
        if(done)break;
        for(int b:fb[f]){
            auto [t,e]=eq.top();
            if(t>=L){done=true;break;}
            eq.pop();
            while(!bh[b].empty()&&bh[b].top()<=t) bh[b].pop();
            int C=bh[b].size();
            int end=t+D[f]+R[b]+C;
            bh[b].push(end);
            tasks[e].push_back({f,b});
            eq.push({end,e});
        }
    }
    int E=0;
    for(int i=0;i<G;i++) if(!tasks[i].empty()) E++;
    printf("%d\n",E);
    for(int i=0;i<G;i++){
        if(tasks[i].empty())continue;
        printf("%d\n",(int)tasks[i].size());
        for(auto&p:tasks[i]) printf("impl %s %d\n",fname[p.first].c_str(),p.second);
    }
}
