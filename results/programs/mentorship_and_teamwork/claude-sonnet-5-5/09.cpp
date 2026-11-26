#include <bits/stdc++.h>
using namespace std;
int main(){
    int C,P;
    scanf("%d %d",&C,&P);
    unordered_map<string,int> sid;
    vector<string> cname(C);
    vector<unordered_map<int,int>> lvl(C);
    vector<vector<int>> bySkill;
    char buf[64],buf2[64];
    auto getS=[&](const string&s){
        auto it=sid.find(s);
        if(it!=sid.end())return it->second;
        int k=sid.size();sid[s]=k;bySkill.push_back({});return k;
    };
    for(int i=0;i<C;i++){
        int n;scanf("%s %d",buf,&n);cname[i]=buf;
        for(int j=0;j<n;j++){
            int l;scanf("%s %d",buf2,&l);
            int s=getS(buf2);lvl[i][s]=l;bySkill[s].push_back(i);
        }
    }
    struct Pr{string name;int D,S,B;vector<pair<int,int>> r;};
    vector<Pr> pr(P);
    for(int i=0;i<P;i++){
        int R;scanf("%s %d %d %d %d",buf,&pr[i].D,&pr[i].S,&pr[i].B,&R);
        pr[i].name=buf;
        for(int j=0;j<R;j++){
            int l;scanf("%s %d",buf2,&l);
            pr[i].r.push_back({getS(buf2),l});
        }
    }
    vector<int> ord(P);
    iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        long long x=(long long)pr[a].B+pr[a].S-pr[a].D;
        long long y=(long long)pr[b].B+pr[b].S-pr[b].D;
        if(x!=y)return x<y;
        return a<b;});
    vector<long long> avail(C,0);
    vector<int> rem=ord;
    vector<pair<int,vector<int>>> out;
    long long ops=0;const long long BUDGET=300000000LL;
    long long T=0;
    auto L=[&](int c,int s){auto it=lvl[c].find(s);return it==lvl[c].end()?0:it->second;};
    vector<char> used(C,0);
    while(!rem.empty()&&ops<BUDGET){
        bool any=false;
        vector<int> nrem;
        for(size_t k=0;k<rem.size();k++){
            int p=rem[k];
            if(ops>=BUDGET){nrem.push_back(p);continue;}
            Pr&q=pr[p];
            if((long long)T+q.D>=(long long)q.B+q.S){continue;} // worthless
            int R=q.r.size();
            vector<int> team(R,-1);
            bool ok=true;
            vector<int> idx(R);
            iota(idx.begin(),idx.end(),0);
            sort(idx.begin(),idx.end(),[&](int a,int b){return q.r[a].second>q.r[b].second;});
            vector<int> usedl;
            for(int ri:idx){
                int s=q.r[ri].first,need=q.r[ri].second;
                int best=-1,bl=1e9;
                bool mentor=false;
                for(int m:usedl)if(L(m,s)>=need)mentor=true;
                for(int c:bySkill[s]){
                    ops++;
                    if(used[c]||avail[c]>T)continue;
                    int l=L(c,s);
                    if(l>=need||(mentor&&l==need-1)){
                        int key=l;
                        if(key<bl){bl=key;best=c;}
                    }
                }
                if(best<0){ok=false;break;}
                team[ri]=best;used[best]=1;usedl.push_back(best);
            }
            if(ok){
                for(int ri=0;ri<R&&ok;ri++){
                    int s=q.r[ri].first,need=q.r[ri].second;
                    if(L(team[ri],s)<need){
                        bool m=false;
                        for(int rj=0;rj<R;rj++)if(rj!=ri&&L(team[rj],s)>=need)m=true;
                        if(!m)ok=false;
                    }
                }
            }
            for(int c:usedl)used[c]=0;
            if(ok){
                any=true;
                for(int ri=0;ri<R;ri++){
                    int c=team[ri],s=q.r[ri].first;
                    if(L(c,s)<=q.r[ri].second){
                        if(!lvl[c].count(s))bySkill[s].push_back(c);
                        lvl[c][s]=L(c,s)+1;
                    }
                    avail[c]=T+q.D;
                }
                out.push_back({p,team});
            } else nrem.push_back(p);
        }
        rem=nrem;
        if(any)continue;
        long long nt=LLONG_MAX;
        for(int c=0;c<C;c++)if(avail[c]>T)nt=min(nt,avail[c]);
        if(nt==LLONG_MAX)break;
        T=nt;
        if(T>300000)break;
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out){
        printf("%s\n",pr[o.first].name.c_str());
        for(size_t i=0;i<o.second.size();i++)
            printf("%s%s",i?" ":"",cname[o.second[i]].c_str());
        printf("\n");
    }
}
