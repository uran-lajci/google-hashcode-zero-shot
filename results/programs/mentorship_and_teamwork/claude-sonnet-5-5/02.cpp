#include <bits/stdc++.h>
using namespace std;
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int C,P;cin>>C>>P;
    map<string,int> sid;
    auto gid=[&](const string&s){auto it=sid.find(s);if(it!=sid.end())return it->second;int k=sid.size();sid[s]=k;return k;};
    vector<string> cn(C);
    vector<map<int,int>> sk(C);
    vector<vector<int>> bySkill;
    for(int i=0;i<C;i++){
        int n;cin>>cn[i]>>n;
        while(n--){string s;int l;cin>>s>>l;sk[i][gid(s)]=l;}
    }
    struct Pr{string name;int d,s,b;vector<pair<int,int>> r;};
    vector<Pr> pr(P);
    for(auto&p:pr){
        int r;cin>>p.name>>p.d>>p.s>>p.b>>r;
        while(r--){string s;int l;cin>>s>>l;p.r.push_back({gid(s),l});}
    }
    int S=sid.size();
    bySkill.assign(S,{});
    for(int i=0;i<C;i++)for(auto&kv:sk[i])bySkill[kv.first].push_back(i);
    auto lvl=[&](int c,int s){auto it=sk[c].find(s);return it==sk[c].end()?0:it->second;};
    vector<int> freeT(C,0);
    vector<int> pend(P);
    iota(pend.begin(),pend.end(),0);
    sort(pend.begin(),pend.end(),[&](int a,int b){
        return pr[a].b+pr[a].s<pr[b].b+pr[b].s;});
    vector<pair<int,vector<int>>> out;
    auto st=chrono::steady_clock::now();
    vector<int> mark(C,0);int stamp=0;
    bool prog=true;
    while(prog){
        prog=false;
        vector<int> rest;
        for(size_t pi=0;pi<pend.size();pi++){
            int pid=pend[pi];
            double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
            if(el>8.0){rest.push_back(pid);continue;}
            Pr&p=pr[pid];
            int R=p.r.size();
            vector<int> team(R,-1);
            stamp++;
            bool ok=true;
            // order roles by level desc
            vector<int> ord(R);iota(ord.begin(),ord.end(),0);
            sort(ord.begin(),ord.end(),[&](int a,int b){return p.r[a].second>p.r[b].second;});
            for(int ri:ord){
                int s=p.r[ri].first,l=p.r[ri].second;
                int best=-1;long long bk=1e18;
                for(int c:bySkill[s]){
                    if(mark[c]==stamp)continue;
                    int cl=lvl(c,s);
                    if(cl<l)continue;
                    long long key=(long long)(cl-l)*1000000+freeT[c];
                    if(key<bk){bk=key;best=c;}
                }
                if(best<0){ok=false;break;}
                team[ri]=best;mark[best]=stamp;
            }
            if(!ok){rest.push_back(pid);continue;}
            int start=0;
            for(int c:team)start=max(start,freeT[c]);
            int end=start+p.d;
            int sc=p.s-max(0,end-p.b);
            if(sc<=0){rest.push_back(pid);continue;}
            for(int i=0;i<R;i++){
                int c=team[i];
                int s=p.r[i].first,l=p.r[i].second;
                if(lvl(c,s)<=l){
                    if(!sk[c].count(s))bySkill[s].push_back(c);
                    sk[c][s]=l+1;
                }
                freeT[c]=end;
            }
            out.push_back({pid,team});
            prog=true;
        }
        pend=rest;
    }
    cout<<out.size()<<"\n";
    for(auto&o:out){
        cout<<pr[o.first].name<<"\n";
        for(size_t i=0;i<o.second.size();i++){
            if(i)cout<<" ";
            cout<<cn[o.second[i]];
        }
        cout<<"\n";
    }
}
