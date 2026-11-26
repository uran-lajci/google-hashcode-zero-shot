#include <bits/stdc++.h>
using namespace std;
int main(){
    int L,G,S,B,F,N;
    scanf("%d %d %d %d %d %d",&L,&G,&S,&B,&F,&N);
    unordered_map<string,int> id;
    vector<int> bin(S);
    vector<int> R(B,0);
    char buf[64];
    vector<string> sn(S);
    for(int i=0;i<S;i++){
        int b;scanf("%s %d",buf,&b);
        id[buf]=i;bin[i]=b;R[b]++;
    }
    vector<string> fname(F);
    vector<int> D(F),U(F);
    vector<vector<int>> fb(F);
    for(int i=0;i<F;i++){
        int m;scanf("%s %d %d %d",buf,&m,&D[i],&U[i]);
        fname[i]=buf;
        for(int k=0;k<m;k++){
            scanf("%s",buf);
            fb[i].push_back(bin[id[buf]]);
        }
        sort(fb[i].begin(),fb[i].end());
        fb[i].erase(unique(fb[i].begin(),fb[i].end()),fb[i].end());
    }
    vector<double> pri(F,-1);
    vector<int> ord;
    for(int i=0;i<F;i++){
        bool ok=true;
        double c=0;
        for(int b:fb[i]){
            if(D[i]+R[b]>=L)ok=false;
            c+=D[i]+R[b];
        }
        if(!ok)continue;
        pri[i]=(double)U[i]/c;
        ord.push_back(i);
    }
    sort(ord.begin(),ord.end(),[&](int a,int b){
        if(pri[a]!=pri[b])return pri[a]>pri[b];
        return a<b;
    });
    typedef pair<int,int> pii;
    priority_queue<pii,vector<pii>,greater<pii>> pq;
    for(int g=0;g<G;g++)pq.push({0,g});
    vector<vector<int>> active(B);
    vector<vector<string>> tasks(G);
    bool stop=false;
    for(int f:ord){
        if(stop)break;
        for(int b:fb[f]){
            pii t=pq.top();
            if(t.first>=L){stop=true;break;}
            pq.pop();
            auto&a=active[b];
            size_t w=0;
            for(size_t k=0;k<a.size();k++)if(a[k]>t.first)a[w++]=a[k];
            a.resize(w);
            int C=a.size();
            int dur=D[f]+R[b]+C;
            int e=t.first+dur;
            a.push_back(e);
            tasks[t.second].push_back("impl "+fname[f]+" "+to_string(b));
            pq.push({e,t.second});
        }
    }
    int E=0;
    for(int g=0;g<G;g++)if(!tasks[g].empty())E++;
    string out=to_string(E)+"\n";
    for(int g=0;g<G;g++)if(!tasks[g].empty()){
        out+=to_string(tasks[g].size())+"\n";
        for(auto&s:tasks[g])out+=s+"\n";
    }
    fputs(out.c_str(),stdout);
}
