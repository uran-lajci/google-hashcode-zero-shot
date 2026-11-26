#include <bits/stdc++.h>
using namespace std;
struct Proj{string name;int d,s,b;vector<pair<int,int>> r;};
int main(){
    int C,P;
    scanf("%d %d",&C,&P);
    map<string,int> sid;
    auto gid=[&](const string&s){auto it=sid.find(s);if(it!=sid.end())return it->second;int k=sid.size();sid[s]=k;return k;};
    vector<string> cn(C);
    vector<unordered_map<int,int>> sk(C);
    vector<vector<int>> bySkill;
    char buf[64],buf2[64];
    for(int i=0;i<C;i++){
        int n;scanf("%s %d",buf,&n);cn[i]=buf;
        for(int j=0;j<n;j++){int l;scanf("%s %d",buf2,&l);int id=gid(buf2);sk[i][id]=l;}
    }
    vector<Proj> pr(P);
    for(auto&p:pr){
        int r;scanf("%s %d %d %d %d",buf,&p.d,&p.s,&p.b,&r);p.name=buf;
        for(int j=0;j<r;j++){int l;scanf("%s %d",buf2,&l);p.r.push_back({gid(buf2),l});}
    }
    vector<long long> fr(C,0);
    vector<char> done(P,0);
    vector<pair<int,vector<int>>> out;
    auto lvl=[&](int c,int s){auto it=sk[c].find(s);return it==sk[c].end()?0:it->second;};
    auto st=chrono::steady_clock::now();
    auto el=[&](){return chrono::duration<double>(chrono::steady_clock::now()-st).count();};
    vector<int> order(P);
    iota(order.begin(),order.end(),0);
    sort(order.begin(),order.end(),[&](int a,int b){
        double x=(double)pr[a].s/(pr[a].d*pr[a].r.size());
        double y=(double)pr[b].s/(pr[b].d*pr[b].r.size());
        if(x!=y)return x>y;return a<b;});
    vector<char> used(C,0);
    for(int pass=0;pass<30&&el()<7.0;pass++){
        bool any=false;
        for(int pi:order){
            if(done[pi])continue;
            if(el()>8.0)break;
            Proj&p=pr[pi];
            int R=p.r.size();
            vector<int> asg(R,-1);
            vector<int> idx(R);
            iota(idx.begin(),idx.end(),0);
            sort(idx.begin(),idx.end(),[&](int a,int b){return p.r[a].second>p.r[b].second;});
            vector<int> chosen;
            bool ok=true;
            // pass A: strict
            auto pick=[&](int role,bool strict){
                int s=p.r[role].first,l=p.r[role].second;
                int best=-1;long long bk=0;
                for(int c=0;c<C;c++){
                    if(used[c])continue;
                    int v=lvl(c,s);
                    if(strict?(v<l):(v!=l-1))continue;
                    long long key=(long long)(v-l)*1000000000LL+fr[c];
                    if(best<0||key<bk){best=c;bk=key;}
                }
                return best;
            };
            for(int role:idx){
                int c=pick(role,true);
                if(c<0){
                    // need mentor
                    int s=p.r[role].first,l=p.r[role].second;
                    bool has=false;
                    for(int c2:chosen)if(lvl(c2,s)>=l)has=true;
                    if(has||l<=1){
                        // mentor exists among chosen; or level1 requires someone anyway
                    }
                    if(has)c=pick(role,false);
                }
                if(c<0){ok=false;break;}
                asg[role]=c;used[c]=1;chosen.push_back(c);
            }
            if(ok){
                // validate
                for(int role=0;role<R&&ok;role++){
                    int c=asg[role];int s=p.r[role].first,l=p.r[role].second;
                    if(lvl(c,s)>=l)continue;
                    bool m=false;
                    for(int o=0;o<R;o++)if(o!=role&&lvl(asg[o],s)>=l)m=true;
                    if(!m)ok=false;
                }
            }
            long long start=0;
            if(ok)for(int c:asg)start=max(start,fr[c]);
            if(ok){
                long long end=start+p.d;
                long long late=max(0LL,end-p.b);
                if(p.s-late<=0)ok=false;
            }
            if(ok){
                long long end=start+p.d;
                for(int role=0;role<R;role++){
                    int c=asg[role];int s=p.r[role].first,l=p.r[role].second;
                    if(lvl(c,s)<=l)sk[c][s]=lvl(c,s)+1;
                    fr[c]=end;
                }
                done[pi]=1;any=true;
                out.push_back({pi,asg});
            }
            for(int c:chosen)used[c]=0;
        }
        if(!any)break;
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out){
        printf("%s\n",pr[o.first].name.c_str());
        for(size_t i=0;i<o.second.size();i++)printf("%s%s",i?" ":"",cn[o.second[i]].c_str());
        printf("\n");
    }
}
