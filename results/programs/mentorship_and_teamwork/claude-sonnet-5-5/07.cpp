#include <bits/stdc++.h>
using namespace std;
int main(){
    int C,P;
    scanf("%d %d",&C,&P);
    unordered_map<string,int> id;
    auto gid=[&](const string&s){auto it=id.find(s);if(it!=id.end())return it->second;int k=id.size();id[s]=k;return k;};
    vector<string> cn(C);
    vector<unordered_map<int,int>> lv(C);
    vector<vector<int>> bySkill;
    char buf[64];
    for(int i=0;i<C;i++){
        int n;scanf("%s %d",buf,&n);cn[i]=buf;
        for(int j=0;j<n;j++){
            int l;scanf("%s %d",buf,&l);
            int s=gid(buf);
            lv[i][s]=l;
            if((int)bySkill.size()<=s)bySkill.resize(s+1);
            bySkill[s].push_back(i);
        }
    }
    struct Pr{string name;int d,s,b;vector<pair<int,int>> r;};
    vector<Pr> pr(P);
    for(int i=0;i<P;i++){
        int r;
        scanf("%s %d %d %d %d",buf,&pr[i].d,&pr[i].s,&pr[i].b,&r);
        pr[i].name=buf;
        for(int j=0;j<r;j++){
            int l;scanf("%s %d",buf,&l);
            pr[i].r.push_back({gid(buf),l});
        }
    }
    bySkill.resize(id.size()+1);
    vector<int> ord(P);
    iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){
        return (double)pr[a].s/pr[a].d/pr[a].r.size()>(double)pr[b].s/pr[b].d/pr[b].r.size();
    });
    vector<long long> fr(C,0);
    vector<int> done(P,0);
    vector<pair<int,vector<int>>> out;
    auto lvl=[&](int c,int s){auto it=lv[c].find(s);return it==lv[c].end()?0:it->second;};
    auto t0=clock();
    long long T=0;
    while(true){
        bool any=false;
        for(int pi:ord){
            if(done[pi])continue;
            Pr&p=pr[pi];
            if(T+p.d>=(long long)p.b+p.s)continue;
            int R=p.r.size();
            vector<int> team(R,-1);
            set<int> used;
            bool ok=true;
            vector<int> ro(R);
            iota(ro.begin(),ro.end(),0);
            sort(ro.begin(),ro.end(),[&](int a,int b){return p.r[a].second>p.r[b].second;});
            map<int,int> maxl;
            for(int k:ro){
                int s=p.r[k].first,need=p.r[k].second;
                int best=-1,bl=1000;
                for(int c:bySkill[s]){
                    if(fr[c]>T||used.count(c))continue;
                    int l=lvl(c,s);
                    bool q=l>=need||(l==need-1&&maxl.count(s)&&maxl[s]>=need);
                    if(!q)continue;
                    if(l<bl){bl=l;best=c;}
                }
                if(best<0){ok=false;break;}
                team[k]=best;used.insert(best);
                maxl[s]=max(maxl[s],lvl(best,s));
            }
            if(!ok)continue;
            for(int k=0;k<R;k++){
                int c=team[k],s=p.r[k].first;
                if(lvl(c,s)<=p.r[k].second)lv[c][s]=lvl(c,s)+1;
                fr[c]=T+p.d;
            }
            done[pi]=1;
            out.push_back({pi,team});
            any=true;
        }
        long long nt=LLONG_MAX;
        for(int c=0;c<C;c++)if(fr[c]>T)nt=min(nt,fr[c]);
        if(nt==LLONG_MAX)break;
        T=nt;
        if((double)(clock()-t0)/CLOCKS_PER_SEC>8)break;
        (void)any;
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out){
        printf("%s\n",pr[o.first].name.c_str());
        for(size_t i=0;i<o.second.size();i++)printf("%s%s",i?" ":"",cn[o.second[i]].c_str());
        printf("\n");
    }
}
