#include <bits/stdc++.h>
using namespace std;
int main(){
    int L,G,S,B,F,N;
    scanf("%d %d %d %d %d %d",&L,&G,&S,&B,&F,&N);
    unordered_map<string,int> sid;
    vector<int> sb(S);
    vector<int> R(B,0);
    vector<string> sname(S);
    char buf[64];
    for(int i=0;i<S;i++){
        int b;
        scanf("%s %d",buf,&b);
        sname[i]=buf;
        sid[buf]=i;
        sb[i]=b;
        R[b]++;
    }
    vector<string> fname(F);
    vector<int> D(F),U(F);
    vector<vector<int>> fb(F);
    vector<double> key(F);
    for(int i=0;i<F;i++){
        int m;
        scanf("%s %d %d %d",buf,&m,&D[i],&U[i]);
        fname[i]=buf;
        for(int j=0;j<m;j++){
            scanf("%s",buf);
            fb[i].push_back(sb[sid[buf]]);
        }
        sort(fb[i].begin(),fb[i].end());
        fb[i].erase(unique(fb[i].begin(),fb[i].end()),fb[i].end());
        double c=0;
        for(int b:fb[i]) c+=D[i]+R[b];
        key[i]=U[i]/c;
    }
    vector<int> ord(F);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return key[a]>key[b];});
    typedef pair<int,int> P;
    priority_queue<P,vector<P>,greater<P>> pq;
    for(int i=0;i<G;i++) pq.push({0,i});
    vector<priority_queue<int,vector<int>,greater<int>>> ends(B);
    vector<vector<pair<int,int>>> tasks(G);
    for(int f:ord){
        if(pq.top().first>=L) break;
        // skip if cannot finish even first task
        int st0=pq.top().first;
        bool ok=true;
        for(int b:fb[f]) if(st0+D[f]+R[b]>L){ok=false;break;}
        if(!ok) continue;
        for(int b:fb[f]){
            P p=pq.top();pq.pop();
            int st=p.first;
            auto &h=ends[b];
            while(!h.empty()&&h.top()<=st) h.pop();
            int C=h.size();
            int d=D[f]+R[b]+C;
            h.push(st+d);
            tasks[p.second].push_back({f,b});
            pq.push({st+d,p.second});
        }
    }
    int E=0;
    for(int i=0;i<G;i++) if(!tasks[i].empty()) E++;
    printf("%d\n",E);
    for(int i=0;i<G;i++) if(!tasks[i].empty()){
        printf("%d\n",(int)tasks[i].size());
        for(auto&t:tasks[i]) printf("impl %s %d\n",fname[t.first].c_str(),t.second);
    }
}
