#include <bits/stdc++.h>
using namespace std;
int main(){
    int L,G,S,B,F,N;
    cin>>L>>G>>S>>B>>F>>N;
    unordered_map<string,int> sid;
    vector<int> sb(S);
    vector<int> R(B,0);
    for(int i=0;i<S;i++){
        string s;int b;cin>>s>>b;
        sid[s]=i;sb[i]=b;R[b]++;
    }
    struct Feat{string name;int D,U;vector<int> bins;double key;};
    vector<Feat> fs(F);
    for(auto&f:fs){
        int m;cin>>f.name>>m>>f.D>>f.U;
        set<int> bs;
        for(int j=0;j<m;j++){
            string s;cin>>s;
            bs.insert(sb[sid[s]]);
        }
        f.bins.assign(bs.begin(),bs.end());
        double c=0;
        for(int b:f.bins)c+=f.D+R[b];
        f.key=f.U/c;
    }
    vector<int> ord(F);
    for(int i=0;i<F;i++)ord[i]=i;
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return fs[a].key>fs[b].key;});
    typedef pair<long long,int> P;
    priority_queue<P,vector<P>,greater<P>> pq;
    for(int i=0;i<G;i++)pq.push({0,i});
    vector<vector<string>> tasks(G);
    vector<priority_queue<long long,vector<long long>,greater<long long>>> bh(B);
    for(int fi:ord){
        Feat&f=fs[fi];
        long long t0=pq.top().first;
        if(t0>=L)break;
        long long mx=0;
        for(int b:f.bins)mx=max(mx,(long long)f.D+R[b]);
        if(t0+mx>L)continue;
        for(int b:f.bins){
            P p=pq.top();pq.pop();
            long long st=p.first;
            auto&h=bh[b];
            while(!h.empty()&&h.top()<=st)h.pop();
            long long dur=f.D+R[b]+(long long)h.size();
            long long en=st+dur;
            h.push(en);
            tasks[p.second].push_back("impl "+f.name+" "+to_string(b));
            pq.push({en,p.second});
        }
    }
    int E=0;
    for(int i=0;i<G;i++)if(!tasks[i].empty())E++;
    string out=to_string(E)+"\n";
    for(int i=0;i<G;i++){
        if(tasks[i].empty())continue;
        out+=to_string(tasks[i].size())+"\n";
        for(auto&s:tasks[i])out+=s+"\n";
    }
    fputs(out.c_str(),stdout);
}
