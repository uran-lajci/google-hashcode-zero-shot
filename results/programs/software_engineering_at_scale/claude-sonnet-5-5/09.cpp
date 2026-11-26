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
    struct Feat{string name;int D;long long U;vector<int> bins;double key;};
    vector<Feat> fs(F);
    for(int i=0;i<F;i++){
        int M,D,U;
        scanf("%s %d %d %d",buf,&M,&D,&U);
        fs[i].name=buf;fs[i].D=D;fs[i].U=U;
        for(int j=0;j<M;j++){
            scanf("%s",buf);
            fs[i].bins.push_back(bin[id[buf]]);
        }
        sort(fs[i].bins.begin(),fs[i].bins.end());
        fs[i].bins.erase(unique(fs[i].bins.begin(),fs[i].bins.end()),fs[i].bins.end());
        long long c=0;
        for(int b:fs[i].bins)c+=D+R[b];
        fs[i].key=(double)U/c;
    }
    vector<int> ord(F);
    for(int i=0;i<F;i++)ord[i]=i;
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return fs[a].key>fs[b].key;});
    typedef pair<long long,int> P;
    priority_queue<P,vector<P>,greater<P>> pq;
    for(int i=0;i<G;i++)pq.push({0,i});
    vector<vector<pair<int,int>>> tasks(G);
    for(int fi:ord){
        Feat&f=fs[fi];
        bool ok=true;
        for(int b:f.bins)if(f.D+R[b]>=L){ok=false;break;}
        if(!ok)continue;
        for(int b:f.bins){
            P p=pq.top();
            if(p.first>=L)break;
            pq.pop();
            tasks[p.second].push_back({fi,b});
            p.first+=f.D+R[b];
            pq.push(p);
        }
    }
    int E=0;
    for(int i=0;i<G;i++)if(!tasks[i].empty())E++;
    string out=to_string(E)+"\n";
    for(int i=0;i<G;i++){
        if(tasks[i].empty())continue;
        out+=to_string(tasks[i].size())+"\n";
        for(auto&t:tasks[i]){
            out+="impl "+fs[t.first].name+" "+to_string(t.second)+"\n";
        }
    }
    fputs(out.c_str(),stdout);
}
