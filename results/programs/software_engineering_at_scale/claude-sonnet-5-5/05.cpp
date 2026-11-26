#include <bits/stdc++.h>
using namespace std;
int main(){
    int L,G,S,B,F,N;
    scanf("%d %d %d %d %d %d",&L,&G,&S,&B,&F,&N);
    unordered_map<string,int> id;
    vector<int> bin(S);
    char buf[64];
    vector<int> R(B,0);
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
        double c=0;
        for(int b:fb[i])c+=D[i]+R[b];
        pr[i]=(double)U[i]/c;
        ord.push_back(i);
    }
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return pr[a]>pr[b];});
    vector<pair<int,int>> tasks;
    for(int f:ord)for(int b:fb[f])tasks.push_back({f,b});
    vector<char> dead(F,0);
    typedef pair<int,int> pii;
    priority_queue<pii,vector<pii>,greater<pii>> pq;
    for(int g=0;g<G;g++)pq.push({0,g});
    vector<priority_queue<int,vector<int>,greater<int>>> act(B);
    vector<vector<string>> out(G);
    size_t p=0;
    while(!pq.empty()&&p<tasks.size()){
        auto [t,g]=pq.top();pq.pop();
        if(t>=L)break;
        bool got=false;
        while(p<tasks.size()){
            auto [f,b]=tasks[p++];
            if(dead[f])continue;
            auto &h=act[b];
            while(!h.empty()&&h.top()<=t)h.pop();
            int C=h.size();
            int dur=D[f]+R[b]+C;
            if(t+dur>=L){dead[f]=1;continue;}
            h.push(t+dur);
            out[g].push_back("impl "+fname[f]+" "+to_string(b));
            pq.push({t+dur,g});
            got=true;
            break;
        }
        if(!got)break;
    }
    int E=0;
    for(int g=0;g<G;g++)if(!out[g].empty())E++;
    printf("%d\n",E);
    for(int g=0;g<G;g++)if(!out[g].empty()){
        printf("%d\n",(int)out[g].size());
        for(auto&s:out[g])printf("%s\n",s.c_str());
    }
}
