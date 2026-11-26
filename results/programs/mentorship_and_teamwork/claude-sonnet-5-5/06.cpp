#include <bits/stdc++.h>
using namespace std;
int main(){
    int C,P;
    scanf("%d %d",&C,&P);
    unordered_map<string,int> sid;
    auto gid=[&](const string&s){auto it=sid.find(s);if(it!=sid.end())return it->second;int k=sid.size();sid[s]=k;return k;};
    vector<string> cn(C);
    vector<unordered_map<int,int>> sk(C);
    char buf[64];
    for(int i=0;i<C;i++){
        int n;scanf("%s %d",buf,&n);cn[i]=buf;
        for(int j=0;j<n;j++){int l;scanf("%s %d",buf,&l);sk[i][gid(buf)]=l;}
    }
    struct Pr{string name;int d,s,b;vector<pair<int,int>> r;};
    vector<Pr> pr(P);
    for(auto&p:pr){
        int r;scanf("%s %d %d %d %d",buf,&p.d,&p.s,&p.b,&r);p.name=buf;
        for(int j=0;j<r;j++){int l;scanf("%s %d",buf,&l);p.r.push_back({gid(buf),l});}
    }
    vector<int> order(P);
    iota(order.begin(),order.end(),0);
    stable_sort(order.begin(),order.end(),[&](int a,int b){
        return (long long)(pr[a].b)*pr[b].d*0+pr[a].b-pr[a].d < pr[b].b-pr[b].d;});
    vector<int> freeAt(C,0);
    vector<int> remaining=order;
    vector<pair<int,vector<int>>> out;
    auto st=chrono::steady_clock::now();
    auto el=[&](){return chrono::duration<double>(chrono::steady_clock::now()-st).count();};
    auto lv=[&](int c,int s){auto it=sk[c].find(s);return it==sk[c].end()?0:it->second;};
    long long T=0;
    while(!remaining.empty()&&el()<8.0){
        vector<int> avail;
        for(int c=0;c<C;c++)if(freeAt[c]<=T)avail.push_back(c);
        vector<int> nrem;
        vector<char> used(C,0);
        for(size_t idx=0;idx<remaining.size();idx++){
            int pi=remaining[idx];
            Pr&p=pr[pi];
            if(p.s-max(0LL,T+p.d-p.b)<=0)continue;
            if(el()>8.0){nrem.push_back(pi);continue;}
            int R=p.r.size();
            vector<int> asg(R,-1);
            vector<int> ro(R);
            iota(ro.begin(),ro.end(),0);
            sort(ro.begin(),ro.end(),[&](int a,int b){return p.r[a].second>p.r[b].second;});
            bool ok=true;
            set<int> inT;
            int scan=0;
            for(int ri:ro){
                int s=p.r[ri].first,l=p.r[ri].second;
                int best=-1,bl=1e9;
                scan=0;
                for(int c:avail){
                    if(used[c]||inT.count(c))continue;
                    if(++scan>3000)break;
                    int x=lv(c,s);
                    if(x>=l&&x<bl){bl=x;best=c;if(x==l)break;}
                }
                if(best<0){
                    // mentee
                    scan=0;
                    bool m=false;
                    for(int t:inT)if(lv(t,s)>=l){m=true;break;}
                    if(m){
                        for(int c:avail){
                            if(used[c]||inT.count(c))continue;
                            if(++scan>3000)break;
                            if(lv(c,s)==l-1){best=c;break;}
                        }
                    }
                }
                if(best<0){ok=false;break;}
                asg[ri]=best;inT.insert(best);
            }
            if(ok){
                // verify mentoring
                for(int ri=0;ri<R&&ok;ri++){
                    int c=asg[ri],s=p.r[ri].first,l=p.r[ri].second;
                    if(lv(c,s)>=l)continue;
                    bool m=false;
                    for(int rj=0;rj<R;rj++)if(rj!=ri&&lv(asg[rj],s)>=l){m=true;break;}
                    if(!m)ok=false;
                }
            }
            if(!ok){nrem.push_back(pi);continue;}
            for(int ri=0;ri<R;ri++){
                int c=asg[ri],s=p.r[ri].first,l=p.r[ri].second;
                used[c]=1;
                freeAt[c]=T+p.d;
                if(lv(c,s)<=l)sk[c][s]=lv(c,s)+1;
            }
            out.push_back({pi,asg});
        }
        remaining=nrem;
        long long nt=LLONG_MAX;
        for(int c=0;c<C;c++)if(freeAt[c]>T)nt=min<long long>(nt,freeAt[c]);
        if(nt==LLONG_MAX)break;
        T=nt;
        if(T>200000)break;
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out){
        printf("%s\n",pr[o.first].name.c_str());
        for(size_t i=0;i<o.second.size();i++)printf("%s%s",i?" ":"",cn[o.second[i]].c_str());
        printf("\n");
    }
}
