#include <bits/stdc++.h>
using namespace std;
struct Pr{string name;int d,s,b;vector<pair<int,int>> r;};
int main(){
    int C,P;
    scanf("%d %d",&C,&P);
    unordered_map<string,int> sid;
    vector<string> cn(C);
    vector<unordered_map<int,int>> sk(C);
    auto gid=[&](const string&s){auto it=sid.find(s);if(it!=sid.end())return it->second;int k=sid.size();sid[s]=k;return k;};
    char b1[64],b2[64];
    for(int i=0;i<C;i++){
        int n;scanf("%s %d",b1,&n);cn[i]=b1;
        for(int j=0;j<n;j++){int l;scanf("%s %d",b2,&l);sk[i][gid(b2)]=l;}
    }
    vector<Pr> pr(P);
    for(int i=0;i<P;i++){
        int R;scanf("%s %d %d %d %d",b1,&pr[i].d,&pr[i].s,&pr[i].b,&R);
        pr[i].name=b1;
        for(int j=0;j<R;j++){int l;scanf("%s %d",b2,&l);pr[i].r.push_back({gid(b2),l});}
    }
    int S=sid.size();
    vector<vector<int>> bySk(S);
    for(int i=0;i<C;i++)for(auto&p:sk[i])bySk[p.first].push_back(i);
    vector<int> order(P);
    iota(order.begin(),order.end(),0);
    stable_sort(order.begin(),order.end(),[&](int a,int b){
        long long x=pr[a].b-pr[a].d,y=pr[b].b-pr[b].d;
        if(x!=y)return x<y;
        return pr[a].s>pr[b].s;});
    vector<long long> fr(C,0);
    vector<char> done(P,0);
    vector<int> usedMark(C,-1);
    int stamp=0;
    vector<pair<int,vector<int>>> out;
    auto lvl=[&](int c,int s){auto it=sk[c].find(s);return it==sk[c].end()?0:it->second;};
    auto st=chrono::steady_clock::now();
    bool timeUp=false;
    const int CAP=300;
    for(int round=0;round<50&&!timeUp;round++){
        int committed=0;
        for(int pi:order){
            if(done[pi])continue;
            if(chrono::duration<double>(chrono::steady_clock::now()-st).count()>8){timeUp=true;break;}
            Pr&p=pr[pi];
            int R=p.r.size();
            stamp++;
            vector<int> asg(R,-1);
            vector<int> idx(R);
            iota(idx.begin(),idx.end(),0);
            sort(idx.begin(),idx.end(),[&](int a,int b){return p.r[a].second>p.r[b].second;});
            bool ok=true;
            vector<int> team;
            long long bestStart=0;
            // pass 1: full-level
            for(int k:idx){
                int s=p.r[k].first,l=p.r[k].second;
                int best=-1;long long bk=0;int cnt=0;
                for(int c:bySk[s]){
                    if(usedMark[c]==stamp)continue;
                    int lv=lvl(c,s);
                    if(lv<l)continue;
                    long long key=max(fr[c],0LL)*20+(lv-l);
                    if(best<0||key<bk){best=c;bk=key;}
                    if(++cnt>=CAP)break;
                }
                if(best>=0){asg[k]=best;usedMark[best]=stamp;team.push_back(best);}
            }
            // pass 2: mentee
            for(int k:idx){
                if(asg[k]>=0)continue;
                int s=p.r[k].first,l=p.r[k].second;
                bool has=false;
                for(int c:team)if(lvl(c,s)>=l){has=true;break;}
                if(!has||l==0){ok=false;break;}
                int best=-1;long long bk=0;int cnt=0;
                if(l-1==0){
                    // anyone with level 0: scan contributors, capped
                    for(int c=0;c<C&&cnt<CAP*3;c++){
                        if(usedMark[c]==stamp)continue;
                        cnt++;
                        long long key=fr[c];
                        if(best<0||key<bk){best=c;bk=key;}
                    }
                } else {
                    for(int c:bySk[s]){
                        if(usedMark[c]==stamp)continue;
                        if(lvl(c,s)!=l-1)continue;
                        long long key=fr[c];
                        if(best<0||key<bk){best=c;bk=key;}
                        if(++cnt>=CAP)break;
                    }
                }
                if(best<0){ok=false;break;}
                asg[k]=best;usedMark[best]=stamp;team.push_back(best);
            }
            if(!ok)continue;
            for(int c:team)bestStart=max(bestStart,fr[c]);
            long long fin=bestStart+p.d;
            if(fin>=p.b+p.s)continue; // zero score
            // commit
            for(int k=0;k<R;k++){
                int c=asg[k],s=p.r[k].first,l=p.r[k].second;
                if(lvl(c,s)<=l)sk[c][s]=lvl(c,s)+1;
                if(lvl(c,s)==1&&bySk[s].size()>=0){
                    // newly gained skill: register
                    if(find(bySk[s].begin(),bySk[s].end(),c)==bySk[s].end()&&bySk[s].size()<2000000)bySk[s].push_back(c);
                }
                fr[c]=fin;
            }
            done[pi]=1;committed++;
            out.push_back({pi,asg});
        }
        if(!committed)break;
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out){
        printf("%s\n",pr[o.first].name.c_str());
        for(size_t i=0;i<o.second.size();i++)printf("%s%s",i?" ":"",cn[o.second[i]].c_str());
        printf("\n");
    }
}
