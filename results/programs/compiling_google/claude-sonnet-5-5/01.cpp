#include <bits/stdc++.h>
using namespace std;
int main(){
    int C,T,S;
    scanf("%d %d %d",&C,&T,&S);
    vector<string> name(C);
    vector<long long> c(C),r(C);
    vector<vector<int>> dep(C);
    unordered_map<string,int> id;
    char buf[64];
    for(int i=0;i<C;i++){
        long long a,b;
        scanf("%s %lld %lld",buf,&a,&b);
        name[i]=buf;c[i]=a;r[i]=b;id[name[i]]=i;
        int n;scanf("%d",&n);
        for(int j=0;j<n;j++){scanf("%s",buf);dep[i].push_back(id[buf]);}
    }
    const long long INF=LLONG_MAX/4;
    vector<long long> dl(C,INF);
    for(int i=0;i<T;i++){
        long long d,g;
        scanf("%s %lld %lld",buf,&d,&g);
        int k=id[buf];
        dl[k]=min(dl[k],d);
    }
    for(int i=C-1;i>=0;i--){
        if(dl[i]>=INF)continue;
        for(int d:dep[i])dl[d]=min(dl[d],dl[i]-c[i]);
    }
    vector<int> ord;
    for(int i=0;i<C;i++)if(dl[i]<INF)ord.push_back(i);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return dl[a]<dl[b];});
    vector<long long> fin(C,-1),free_(S,0);
    vector<int> srv(C,-1);
    vector<pair<int,int>> out;
    for(int f:ord){
        bool ok=true;
        for(int d:dep[f])if(srv[d]<0){ok=false;break;}
        if(!ok)continue;
        long long best=INF;int bs=-1;
        for(int s=0;s<S;s++){
            long long st=free_[s];
            for(int d:dep[f]){
                long long av=(srv[d]==s)?fin[d]:fin[d]+r[d];
                if(av>st)st=av;
            }
            long long e=st+c[f];
            if(e<best){best=e;bs=s;}
        }
        if(best>dl[f])continue;
        srv[f]=bs;fin[f]=best;free_[bs]=best;
        out.push_back({f,bs});
    }
    if(out.empty()){
        out.push_back({ord.empty()?0:ord[0],0});
    }
    printf("%d\n",(int)out.size());
    for(auto&p:out)printf("%s %d\n",name[p.first].c_str(),p.second);
}
