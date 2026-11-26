#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    int C,T,S;
    scanf("%d %d %d",&C,&T,&S);
    vector<string> names(C);
    vector<ll> c(C),r(C);
    vector<vector<int>> dep(C);
    unordered_map<string,int> id;
    id.reserve(C*2);
    char buf[64];
    for(int i=0;i<C;i++){
        ll a,b;
        scanf("%s %lld %lld",buf,&a,&b);
        names[i]=buf;c[i]=a;r[i]=b;id[names[i]]=i;
        int n;scanf("%d",&n);
        for(int j=0;j<n;j++){scanf("%s",buf);dep[i].push_back(id[buf]);}
    }
    struct Tg{int f;ll d,g;};
    vector<Tg> tg(T);
    for(int i=0;i<T;i++){
        ll d,g;scanf("%s %lld %lld",buf,&d,&g);
        tg[i]={id[buf],d,g};
    }
    sort(tg.begin(),tg.end(),[](const Tg&a,const Tg&b){
        if(a.d!=b.d)return a.d<b.d;
        return a.g>b.g;});
    const ll INF=LLONG_MAX/4;
    vector<ll> ckp(C);
    for(int i=0;i<C;i++){
        ll m=0;
        for(int d:dep[i])m=max(m,ckp[d]);
        ckp[i]=m+c[i];
    }
    vector<ll> fin((size_t)C*S,INF);
    vector<ll> minRep(C,INF);
    vector<char> done(C,0),vis(C,0);
    vector<ll> freeT(S,0);
    vector<pair<int,int>> steps;
    for(auto&t:tg){
        if(ckp[t.f]>t.d)continue;
        if(done[t.f])continue;
        vector<int> order;
        vector<pair<int,int>> st;
        vector<int> visited;
        vis[t.f]=1;visited.push_back(t.f);
        st.push_back({t.f,0});
        while(!st.empty()){
            auto &tp=st.back();
            int u=tp.first;
            if(tp.second<(int)dep[u].size()){
                int v=dep[u][tp.second++];
                if(!done[v]&&!vis[v]){
                    vis[v]=1;visited.push_back(v);
                    st.push_back({v,0});
                }
            } else {
                order.push_back(u);
                st.pop_back();
            }
        }
        for(int v:visited)vis[v]=0;
        for(int f:order){
            ll best=INF;int bs=0;
            for(int s=0;s<S;s++){
                ll start=freeT[s];
                if(start>=best)continue;
                for(int d:dep[f]){
                    ll a=min(fin[(size_t)d*S+s],minRep[d]);
                    if(a>start)start=a;
                }
                ll e=start+c[f];
                if(e<best){best=e;bs=s;}
            }
            freeT[bs]=best;
            fin[(size_t)f*S+bs]=best;
            minRep[f]=min(minRep[f],best+r[f]);
            done[f]=1;
            steps.push_back({f,bs});
        }
    }
    if(steps.empty())steps.push_back({0,0});
    printf("%d\n",(int)steps.size());
    for(auto&p:steps)printf("%s %d\n",names[p.first].c_str(),p.second);
}
