#include <bits/stdc++.h>
using namespace std;
int C,T,S;
vector<long long> ct,rt;
vector<vector<int>> dep;
vector<vector<pair<int,long long>>> pr;
vector<long long> fr;
long long avl(int f,int s){
    long long b=LLONG_MAX;
    for(auto&p:pr[f]){
        long long v=p.first==s?p.second:p.second+rt[f];
        b=min(b,v);
    }
    return b;
}
int main(){
    scanf("%d %d %d",&C,&T,&S);
    unordered_map<string,int> id;
    ct.resize(C);rt.resize(C);dep.resize(C);
    char buf[64];
    for(int i=0;i<C;i++){
        long long c,r;
        scanf("%s %lld %lld",buf,&c,&r);
        id[buf]=i;ct[i]=c;rt[i]=r;
        int n;scanf("%d",&n);
        for(int j=0;j<n;j++){scanf("%s",buf);dep[i].push_back(id[buf]);}
    }
    vector<array<long long,3>> tg(T);
    for(int i=0;i<T;i++){
        long long d,g;
        scanf("%s %lld %lld",buf,&d,&g);
        tg[i]={d,g,id[buf]};
    }
    vector<long long> cp(C);
    for(int i=0;i<C;i++){
        long long m=0;
        for(int d:dep[i])m=max(m,cp[d]);
        cp[i]=m+ct[i];
    }
    sort(tg.begin(),tg.end());
    pr.assign(C,{});
    fr.assign(S,0);
    vector<pair<int,int>> out;
    vector<int> mark(C,0);
    int stamp=0;
    for(auto&t:tg){
        int f=t[2];
        if(cp[f]>t[0])continue;
        stamp++;
        vector<int> need,stk={f};
        mark[f]=stamp;
        while(!stk.empty()){
            int x=stk.back();stk.pop_back();
            if(!pr[x].empty())continue;
            need.push_back(x);
            for(int d:dep[x])if(mark[d]!=stamp){mark[d]=stamp;stk.push_back(d);}
        }
        sort(need.begin(),need.end());
        vector<long long> savedFr=fr;
        vector<pair<int,int>> added;
        bool ok=true;
        long long fin=0;
        for(int x:need){
            long long best=LLONG_MAX;int bs=0;
            for(int s=0;s<S;s++){
                long long st=fr[s];
                for(int d:dep[x])st=max(st,avl(d,s));
                long long e=st+ct[x];
                if(e<best){best=e;bs=s;}
            }
            pr[x].push_back({bs,best});
            fr[bs]=best;
            added.push_back({x,bs});
            if(x==f)fin=best;
            if(best>t[0]){ok=false;break;}
        }
        if(ok){
            for(auto&a:added)out.push_back(a);
        }else{
            for(auto&a:added)pr[a.first].clear();
            fr=savedFr;
        }
    }
    vector<string> nm(C);
    for(auto&k:id)nm[k.second]=k.first;
    if(out.empty()){
        // must output at least one step
        int f=tg[0][2];
        vector<int> need;
        vector<char> seen(C,0);
        vector<int> stk={f};seen[f]=1;
        while(!stk.empty()){
            int x=stk.back();stk.pop_back();need.push_back(x);
            for(int d:dep[x])if(!seen[d]){seen[d]=1;stk.push_back(d);}
        }
        sort(need.begin(),need.end());
        printf("%d\n",(int)need.size());
        for(int x:need)printf("%s 0\n",nm[x].c_str());
        return 0;
    }
    printf("%d\n",(int)out.size());
    for(auto&a:out)printf("%s %d\n",nm[a.first].c_str(),a.second);
}
