#include <bits/stdc++.h>
using namespace std;
int main(){
    int C,T,S;
    scanf("%d %d %d",&C,&T,&S);
    vector<string> nm(C);
    vector<long long> c(C),r(C);
    vector<vector<int>> dep(C);
    unordered_map<string,int> id;
    id.reserve(C*2);
    char buf[64];
    for(int i=0;i<C;i++){
        long long a,b;
        scanf("%63s %lld %lld",buf,&a,&b);
        nm[i]=buf;c[i]=a;r[i]=b;id[nm[i]]=i;
        int n;scanf("%d",&n);
        for(int j=0;j<n;j++){
            scanf("%63s",buf);
            dep[i].push_back(id[buf]);
        }
    }
    const long long INF=LLONG_MAX/4;
    vector<long long> key(C,INF);
    for(int i=0;i<T;i++){
        long long d,g;
        scanf("%63s %lld %lld",buf,&d,&g);
        int f=id[buf];
        key[f]=min(key[f],d);
    }
    for(int i=C-1;i>=0;i--){
        if(key[i]<INF){
            for(int d:dep[i])key[d]=min(key[d],key[i]);
        }
    }
    vector<int> order;
    for(int i=0;i<C;i++)if(key[i]<INF)order.push_back(i);
    sort(order.begin(),order.end(),[&](int a,int b){
        if(key[a]!=key[b])return key[a]<key[b];
        return a<b;
    });
    vector<long long> avail((size_t)C*S,INF);
    vector<long long> fr(S,0);
    vector<pair<int,int>> out;
    for(int f:order){
        long long best=INF;int bs=0;
        for(int s=0;s<S;s++){
            long long st=fr[s];
            if(st>=best)continue;
            for(int d:dep[f]){
                long long a=avail[(size_t)d*S+s];
                if(a>st){st=a;if(st>=best)break;}
            }
            long long fin=st+c[f];
            if(fin<best){best=fin;bs=s;}
        }
        fr[bs]=best;
        for(int s=0;s<S;s++){
            long long v=(s==bs)?best:best+r[f];
            long long &a=avail[(size_t)f*S+s];
            if(v<a)a=v;
        }
        out.push_back({f,bs});
    }
    printf("%d\n",(int)out.size());
    for(auto&p:out)printf("%s %d\n",nm[p.first].c_str(),p.second);
}
