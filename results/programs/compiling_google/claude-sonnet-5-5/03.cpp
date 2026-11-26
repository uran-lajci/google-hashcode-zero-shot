#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    int C,T,S;
    scanf("%d %d %d",&C,&T,&S);
    vector<ll> c(C),r(C);
    vector<vector<int>> deps(C);
    unordered_map<string,int> id;
    id.reserve(C*2);
    vector<string> names(C);
    char buf[64];
    for(int i=0;i<C;i++){
        scanf("%s %lld %lld",buf,&c[i],&r[i]);
        names[i]=buf;
        id[names[i]]=i;
        int n;scanf("%d",&n);
        deps[i].resize(n);
        for(int j=0;j<n;j++){
            scanf("%s",buf);
            deps[i][j]=id[buf];
        }
    }
    struct Tg{int f;ll d,g;};
    vector<Tg> tg(T);
    for(auto&t:tg){
        scanf("%s %lld %lld",buf,&t.d,&t.g);
        t.f=id[buf];
    }
    stable_sort(tg.begin(),tg.end(),[](const Tg&a,const Tg&b){return a.d<b.d;});
    vector<char> comp(C,0);
    vector<int> srv(C,-1),stamp(C,0);
    vector<ll> fin(C,0),fr(S,0);
    vector<pair<int,int>> steps;
    int cur=0;
    for(auto&t:tg){
        if(comp[t.f]){
            continue;
        }
        cur++;
        vector<int> need,st;
        st.push_back(t.f);
        stamp[t.f]=cur;
        while(!st.empty()){
            int x=st.back();st.pop_back();
            need.push_back(x);
            for(int d:deps[x]){
                if(!comp[d]&&stamp[d]!=cur){
                    stamp[d]=cur;
                    st.push_back(d);
                }
            }
        }
        sort(need.begin(),need.end());
        size_t base=steps.size();
        vector<pair<int,ll>> undo;
        for(int f:need){
            int am=0;
            for(int s=1;s<S;s++) if(fr[s]<fr[am]) am=s;
            vector<int> cand;
            cand.push_back(am);
            for(int d:deps[f]) cand.push_back(srv[d]);
            sort(cand.begin(),cand.end());
            cand.erase(unique(cand.begin(),cand.end()),cand.end());
            ll bf=LLONG_MAX;int bs=-1;
            for(int s:cand){
                ll rd=fr[s];
                for(int d:deps[f]){
                    ll a=(srv[d]==s)?fin[d]:fin[d]+r[d];
                    if(a>rd) rd=a;
                }
                ll e=rd+c[f];
                if(e<bf){bf=e;bs=s;}
            }
            undo.push_back({bs,fr[bs]});
            fr[bs]=bf;
            fin[f]=bf;srv[f]=bs;comp[f]=1;
            steps.push_back({f,bs});
        }
        if(fin[t.f]>t.d){
            for(int i=(int)need.size()-1;i>=0;i--){
                comp[need[i]]=0;
                fr[undo[i].first]=undo[i].second;
            }
            steps.resize(base);
        }
    }
    if(steps.empty()){
        // must output at least one step: compile a file with no deps
        for(int i=0;i<C;i++) if(deps[i].empty()){steps.push_back({i,0});break;}
    }
    printf("%d\n",(int)steps.size());
    for(auto&p:steps) printf("%s %d\n",names[p.first].c_str(),p.second);
}
