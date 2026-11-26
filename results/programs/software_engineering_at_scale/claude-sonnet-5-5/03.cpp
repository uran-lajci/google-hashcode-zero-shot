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
    vector<double> pr(F);
    for(int i=0;i<F;i++){
        int m;
        scanf("%s %d %d %d",buf,&m,&D[i],&U[i]);
        fname[i]=buf;
        vector<int> bs;
        for(int j=0;j<m;j++){
            scanf("%s",buf);
            bs.push_back(bin[id[buf]]);
        }
        sort(bs.begin(),bs.end());
        bs.erase(unique(bs.begin(),bs.end()),bs.end());
        fb[i]=bs;
        double c=0;
        for(int b:bs)c+=D[i]+R[b];
        pr[i]=U[i]/c;
    }
    vector<int> ord(F);
    for(int i=0;i<F;i++)ord[i]=i;
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return pr[a]>pr[b];});
    typedef pair<int,int> P;
    priority_queue<P,vector<P>,greater<P>> pq;
    for(int i=0;i<G;i++)pq.push({0,i});
    vector<vector<pair<int,int>>> tasks(G);
    for(int f:ord){
        int k=fb[f].size();
        if(k>200||k>G)continue;
        vector<P> popped,mod;
        int mx=0;
        for(int j=0;j<k;j++){
            P p=pq.top();pq.pop();
            popped.push_back(p);
            int e=p.first+D[f]+R[fb[f][j]];
            mod.push_back({e,p.second});
            mx=max(mx,e);
        }
        if(mx<L){
            for(int j=0;j<k;j++){
                tasks[mod[j].second].push_back({f,fb[f][j]});
                pq.push(mod[j]);
            }
        } else {
            for(auto&p:popped)pq.push(p);
        }
    }
    int E=0;
    for(int i=0;i<G;i++)if(!tasks[i].empty())E++;
    printf("%d\n",E);
    for(int i=0;i<G;i++){
        if(tasks[i].empty())continue;
        printf("%d\n",(int)tasks[i].size());
        for(auto&t:tasks[i])printf("impl %s %d\n",fname[t.first].c_str(),t.second);
    }
}
