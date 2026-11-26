#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    int C,T,S;
    scanf("%d %d %d",&C,&T,&S);
    unordered_map<string,int> id;
    id.reserve(C*2);
    vector<ll> cc(C),rr(C);
    vector<vector<int>> deps(C);
    char buf[64];
    for(int i=0;i<C;i++){
        ll c,r;
        scanf("%s %lld %lld",buf,&c,&r);
        id[buf]=i;cc[i]=c;rr[i]=r;
        int n;scanf("%d",&n);
        deps[i].resize(n);
        for(int j=0;j<n;j++){scanf("%s",buf);deps[i][j]=id[buf];}
    }
    struct Tg{int f;ll d,g;};
    vector<Tg> tg(T);
    for(int i=0;i<T;i++){
        scanf("%s %lld %lld",buf,&tg[i].d,&tg[i].g);
        tg[i].f=id[buf];
    }
    stable_sort(tg.begin(),tg.end(),[](const Tg&a,const Tg&b){return a.d<b.d;});
    const ll INF=(ll)4e18/4;
    vector<ll> own((size_t)C*S,INF);
    vector<ll> mf(C,INF);
    vector<ll> fr(S,0);
    vector<pair<int,int>> out;
    vector<int> vis(C,-1);
    ll budget=300000000LL;
    for(int ti=0;ti<T;ti++){
        int tf=tg[ti].f;
        ll dl=tg[ti].d;
        if(mf[tf]<INF){continue;}
        if(budget<=0)break;
        // collect missing
        vector<int> miss,st;
        st.push_back(tf);vis[tf]=ti;
        while(!st.empty()){
            int x=st.back();st.pop_back();
            miss.push_back(x);
            for(int d:deps[x]){
                if(mf[d]<INF||vis[d]==ti)continue;
                vis[d]=ti;st.push_back(d);
            }
        }
        sort(miss.begin(),miss.end());
        vector<ll> saveFr=fr;
        vector<pair<int,int>> placed;
        vector<ll> oldmf;
        bool ok=true;
        for(int f:miss){
            budget-=(ll)S*(deps[f].size()+1);
            ll best=INF;int bs=0;
            for(int s=0;s<S;s++){
                ll t=fr[s];
                for(int d:deps[f]){
                    ll a=min(own[(size_t)d*S+s],mf[d]+rr[d]);
                    if(a>t)t=a;
                }
                t+=cc[f];
                if(t<best){best=t;bs=s;}
            }
            placed.push_back({f,bs});
            oldmf.push_back(mf[f]);
            own[(size_t)f*S+bs]=best;
            mf[f]=min(mf[f],best);
            fr[bs]=best;
            if(best>dl){ok=false;break;}
        }
        if(ok){
            for(auto&p:placed)out.push_back(p);
        } else {
            for(size_t i=0;i<placed.size();i++){
                own[(size_t)placed[i].first*S+placed[i].second]=INF;
                mf[placed[i].first]=oldmf[i];
            }
            fr=saveFr;
        }
    }
    if(out.empty()){
        // must output at least one step: compile a file with no deps
        for(int i=0;i<C;i++)if(deps[i].empty()){out.push_back({i,0});break;}
    }
    vector<string> names(C);
    for(auto&kv:id)names[kv.second]=kv.first;
    printf("%d\n",(int)out.size());
    for(auto&p:out)printf("%s %d\n",names[p.first].c_str(),p.second);
}
