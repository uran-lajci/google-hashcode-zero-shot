#include <bits/stdc++.h>
using namespace std;
int main(){
    int C,P;
    if(scanf("%d %d",&C,&P)!=2)return 0;
    unordered_map<string,int> sid;
    vector<string> cn(C);
    vector<unordered_map<int,int>> sk(C);
    auto gid=[&](const string&s){auto it=sid.find(s);if(it!=sid.end())return it->second;int k=sid.size();sid[s]=k;return k;};
    char b1[64],b2[64];
    for(int i=0;i<C;i++){
        int n;scanf("%s %d",b1,&n);cn[i]=b1;
        for(int j=0;j<n;j++){int l;scanf("%s %d",b2,&l);sk[i][gid(b2)]=l;}
    }
    struct Pr{string name;int d,s,b;vector<pair<int,int>> r;};
    vector<Pr> pr(P);
    for(auto&p:pr){
        int n;scanf("%s %d %d %d %d",b1,&p.d,&p.s,&p.b,&n);p.name=b1;
        for(int j=0;j<n;j++){int l;scanf("%s %d",b2,&l);p.r.push_back({gid(b2),l});}
    }
    vector<int> ord(P);
    iota(ord.begin(),ord.end(),0);
    auto key=[&](int i){return (double)pr[i].s/((double)pr[i].d*pr[i].r.size());};
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return key(a)>key(b);});
    vector<long long> fr(C,0);
    vector<int> pend=ord;
    vector<pair<int,vector<int>>> res;
    auto lvl=[&](int c,int s){auto it=sk[c].find(s);return it==sk[c].end()?0:it->second;};
    long long t=0;
    long long work=0;
    const long long LIM=150000000LL;
    while(!pend.empty()&&work<LIM){
        vector<int> freeC;
        for(int c=0;c<C;c++)if(fr[c]<=t)freeC.push_back(c);
        vector<int> np;
        vector<char> used(C,0);
        int nf=freeC.size();
        for(size_t pi=0;pi<pend.size();pi++){
            int id=pend[pi];
            Pr&p=pr[id];
            if(t+p.d>=(long long)p.b+p.s){continue;}
            if(work>=LIM){np.push_back(id);continue;}
            int R=p.r.size();
            if(nf<R){np.push_back(id);continue;}
            vector<int> team(R,-1);
            vector<int> tmp;
            bool ok=true;
            // pass 1
            for(int k=0;k<R;k++){
                int best=-1,bl=1000;
                for(int c:freeC){work++;
                    if(used[c])continue;
                    int l=lvl(c,p.r[k].first);
                    if(l>=p.r[k].second&&l<bl){bl=l;best=c;if(l==p.r[k].second)break;}
                }
                if(best>=0){team[k]=best;used[best]=1;tmp.push_back(best);}
            }
            for(int k=0;k<R&&ok;k++){
                if(team[k]>=0)continue;
                bool men=false;
                for(int j=0;j<R;j++)if(team[j]>=0&&lvl(team[j],p.r[k].first)>=p.r[k].second){men=true;break;}
                if(!men){ok=false;break;}
                int best=-1;
                for(int c:freeC){work++;
                    if(used[c])continue;
                    if(lvl(c,p.r[k].first)>=p.r[k].second-1){best=c;break;}
                }
                if(best<0){ok=false;break;}
                team[k]=best;used[best]=1;tmp.push_back(best);
            }
            if(!ok){for(int c:tmp)used[c]=0;np.push_back(id);continue;}
            // validate
            bool v=true;
            for(int k=0;k<R&&v;k++){
                int l=lvl(team[k],p.r[k].first);
                if(l>=p.r[k].second)continue;
                bool m=false;
                for(int j=0;j<R;j++)if(j!=k&&lvl(team[j],p.r[k].first)>=p.r[k].second)m=true;
                if(!m)v=false;
            }
            if(!v){for(int c:tmp)used[c]=0;np.push_back(id);continue;}
            for(int k=0;k<R;k++){
                int c=team[k];
                int l=lvl(c,p.r[k].first);
                if(l<=p.r[k].second)sk[c][p.r[k].first]=l+1;
                fr[c]=t+p.d;
            }
            nf-=R;
            res.push_back({id,team});
        }
        // remove used from freeC implicitly by fr>t
        pend=np;
        long long nt=LLONG_MAX;
        for(int c=0;c<C;c++)if(fr[c]>t)nt=min(nt,fr[c]);
        if(nt==LLONG_MAX)break;
        t=nt;
    }
    printf("%d\n",(int)res.size());
    for(auto&r:res){
        printf("%s\n",pr[r.first].name.c_str());
        for(size_t i=0;i<r.second.size();i++)printf("%s%s",i?" ":"",cn[r.second[i]].c_str());
        printf("\n");
    }
}
