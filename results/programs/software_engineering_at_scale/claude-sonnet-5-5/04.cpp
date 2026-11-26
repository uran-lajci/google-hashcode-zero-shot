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
    vector<double> key(F);
    for(int i=0;i<F;i++){
        int m;
        scanf("%s %d %d %d",buf,&m,&D[i],&U[i]);
        fname[i]=buf;
        for(int k=0;k<m;k++){
            scanf("%s",buf);
            fb[i].push_back(bin[id[buf]]);
        }
        sort(fb[i].begin(),fb[i].end());
        fb[i].erase(unique(fb[i].begin(),fb[i].end()),fb[i].end());
        double c=0;
        for(int b:fb[i])c+=D[i]+R[b];
        key[i]=U[i]/c;
    }
    vector<int> ord(F);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return key[a]>key[b];});
    vector<long long> tm(G,0);
    vector<vector<pair<int,int>>> q(G);
    for(int f:ord){
        // tentative
        map<int,long long> tmp;
        bool ok=true;
        for(int b:fb[f]){
            int e=b%G;
            long long &t=tmp[e];
            if(t==0)t=tm[e];
            t+=D[f]+R[b];
            if(t>=L){ok=false;break;}
        }
        if(!ok)continue;
        for(int b:fb[f]){
            int e=b%G;
            tm[e]+=D[f]+R[b];
            q[e].push_back({f,b});
        }
    }
    int E=0;
    for(int e=0;e<G;e++)if(!q[e].empty())E++;
    printf("%d\n",E);
    for(int e=0;e<G;e++){
        if(q[e].empty())continue;
        printf("%d\n",(int)q[e].size());
        for(auto&p:q[e])printf("impl %s %d\n",fname[p.first].c_str(),p.second);
    }
}
