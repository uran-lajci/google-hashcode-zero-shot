#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    int C,T,S;
    scanf("%d %d %d",&C,&T,&S);
    vector<string> nm(C);
    unordered_map<string,int> id;
    id.reserve(C*2);
    vector<ll> c(C),r(C);
    vector<vector<int>> deps(C);
    char buf[64];
    for(int i=0;i<C;i++){
        ll a,b;
        scanf("%s %lld %lld",buf,&a,&b);
        nm[i]=buf;id[nm[i]]=i;c[i]=a;r[i]=b;
        int n;scanf("%d",&n);
        for(int j=0;j<n;j++){
            scanf("%s",buf);
            deps[i].push_back(id[buf]);
        }
    }
    const ll INF=LLONG_MAX/4;
    vector<ll> minD(C,INF),maxD(C,0);
    for(int i=0;i<T;i++){
        ll d,g;
        scanf("%s %lld %lld",buf,&d,&g);
        int f=id[buf];
        minD[f]=min(minD[f],d);
        maxD[f]=max(maxD[f],d);
    }
    for(int f=C-1;f>=0;f--){
        if(maxD[f]==0)continue;
        for(int d:deps[f]){
            minD[d]=min(minD[d],minD[f]);
            maxD[d]=max(maxD[d],maxD[f]);
        }
    }
    vector<int> indeg(C,0);
    vector<vector<int>> ch(C);
    for(int f=0;f<C;f++){
        if(maxD[f]==0)continue;
        for(int d:deps[f]){indeg[f]++;ch[d].push_back(f);}
    }
    priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>> pq;
    for(int f=0;f<C;f++)if(maxD[f]>0&&indeg[f]==0)pq.push({minD[f],f});
    vector<ll> freeT(S,0);
    vector<ll> avail((size_t)C*S,0);
    vector<pair<int,int>> out;
    while(!pq.empty()){
        int f=pq.top().second;pq.pop();
        ll bestF=INF;int bs=-1;ll bst=0;
        for(int s=0;s<S;s++){
            ll st=freeT[s];
            for(int d:deps[f]){
                ll a=avail[(size_t)d*S+s];
                if(a>st)st=a;
            }
            ll fin=st+c[f];
            if(fin<bestF){bestF=fin;bs=s;bst=st;}
        }
        if(bestF>maxD[f])continue;
        freeT[bs]=bestF;
        for(int s=0;s<S;s++)avail[(size_t)f*S+s]=bestF+r[f];
        avail[(size_t)f*S+bs]=bestF;
        out.push_back({f,bs});
        for(int x:ch[f]){
            if(--indeg[x]==0)pq.push({minD[x],x});
        }
    }
    if(out.empty())out.push_back({0,0});
    printf("%d\n",(int)out.size());
    for(auto&p:out)printf("%s %d\n",nm[p.first].c_str(),p.second);
}
