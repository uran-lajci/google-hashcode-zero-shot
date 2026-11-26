#include <bits/stdc++.h>
using namespace std;
int main(){
    int D,I,S,V,F;
    scanf("%d %d %d %d %d",&D,&I,&S,&V,&F);
    vector<int> E(S);
    vector<string> name(S);
    unordered_map<string,int> id;
    char buf[64];
    for(int i=0;i<S;i++){
        int b,e,l;
        scanf("%d %d %s %d",&b,&e,buf,&l);
        E[i]=e;
        name[i]=buf;
        id[name[i]]=i;
    }
    vector<long long> cnt(S,0),st(S,0);
    for(int v=0;v<V;v++){
        int p;
        scanf("%d",&p);
        for(int j=0;j<p;j++){
            scanf("%s",buf);
            int s=id[buf];
            if(j==0)st[s]++;
            if(j<p-1)cnt[s]++;
        }
    }
    vector<vector<int>> inc(I);
    for(int s=0;s<S;s++)if(cnt[s]>0)inc[E[s]].push_back(s);
    int A=0;
    for(int i=0;i<I;i++)if(!inc[i].empty())A++;
    printf("%d\n",A);
    for(int i=0;i<I;i++){
        if(inc[i].empty())continue;
        auto&v=inc[i];
        sort(v.begin(),v.end(),[&](int a,int b){
            if(st[a]!=st[b])return st[a]>st[b];
            if(cnt[a]!=cnt[b])return cnt[a]>cnt[b];
            return a<b;
        });
        printf("%d\n%d\n",i,(int)v.size());
        for(int s:v)printf("%s 1\n",name[s].c_str());
    }
}
