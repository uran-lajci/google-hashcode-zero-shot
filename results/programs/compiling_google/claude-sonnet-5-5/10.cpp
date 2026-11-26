#include <bits/stdc++.h>
using namespace std;
int main(){
    int C,T,S;
    scanf("%d %d %d",&C,&T,&S);
    vector<long long> c(C),r(C);
    vector<vector<int>> dep(C);
    unordered_map<string,int> id;
    char buf[64];
    for(int i=0;i<C;i++){
        scanf("%s %lld %lld",buf,&c[i],&r[i]);
        id[buf]=i;
        int n;scanf("%d",&n);
        dep[i].resize(n);
        for(int j=0;j<n;j++){scanf("%s",buf);dep[i][j]=id[buf];}
    }
    vector<int> tf(T);vector<long long> td(T),tg(T);
    for(int i=0;i<T;i++){
        scanf("%s %lld %lld",buf,&td[i],&tg[i]);
        tf[i]=id[buf];
    }
    // critical path
    vector<long long> cp(C);
    for(int i=0;i<C;i++){
        long long m=0;
        for(int d:dep[i])m=max(m,cp[d]);
        cp[i]=m+c[i];
    }
    vector<int> ord(T);
    iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        if(td[a]!=td[b])return td[a]<td[b];
        return tg[a]>tg[b];
    });
    const int INF=INT_MAX;
    vector<int> av((size_t)C*S,INF);
    vector<char> done(C,0);
    vector<long long> freeT(S,0);
    vector<pair<int,int>> out;
    vector<int> stamp(C,-1);
    for(int oi=0;oi<T;oi++){
        int t=ord[oi];
        int f=tf[t];
        if(cp[f]>td[t])continue;
        vector<int> need,st;
        st.push_back(f);
        stamp[f]=oi;
        while(!st.empty()){
            int x=st.back();st.pop_back();
            if(done[x])continue;
            need.push_back(x);
            for(int d:dep[x]){
                if(!done[d]&&stamp[d]!=oi){stamp[d]=oi;st.push_back(d);}
            }
        }
        sort(need.begin(),need.end());
        for(int x:need){
            long long best=LLONG_MAX;int bs=0;
            for(int s=0;s<S;s++){
                long long start=freeT[s];
                for(int d:dep[x]){
                    long long a=av[(size_t)d*S+s];
                    if(a>start)start=a;
                }
                long long fin=start+c[x];
                if(fin<best){best=fin;bs=s;}
            }
            freeT[bs]=best;
            done[x]=1;
            out.push_back({x,bs});
            for(int s=0;s<S;s++){
                long long v=(s==bs)?best:best+r[x];
                if(v>INF-1)v=INF-1;
                av[(size_t)x*S+s]=(int)v;
            }
        }
    }
    if(out.empty()){
        // must output at least one step: compile a file with no dependencies
        for(int i=0;i<C;i++)if(dep[i].empty()){out.push_back({i,0});break;}
    }
    // names
    vector<string> nm(C);
    for(auto&p:id)nm[p.second]=p.first;
    printf("%d\n",(int)out.size());
    for(auto&p:out)printf("%s %d\n",nm[p.first].c_str(),p.second);
}
