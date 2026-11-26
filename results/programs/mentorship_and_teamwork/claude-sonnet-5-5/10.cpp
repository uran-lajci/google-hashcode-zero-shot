#include <bits/stdc++.h>
using namespace std;
int main(){
    int C,P;
    scanf("%d %d",&C,&P);
    unordered_map<string,int> sid;
    vector<string> cn(C);
    vector<unordered_map<int,int>> sk(C);
    char b[64];
    auto gs=[&](const char*s){string x=s;auto it=sid.find(x);if(it!=sid.end())return it->second;int k=sid.size();sid[x]=k;return k;};
    for(int i=0;i<C;i++){
        int n;scanf("%s %d",b,&n);cn[i]=b;
        for(int j=0;j<n;j++){int l;scanf("%s %d",b,&l);sk[i][gs(b)]=l;}
    }
    vector<string> pn(P);
    vector<int> D(P),S(P),B(P);
    vector<vector<pair<int,int>>> R(P);
    for(int i=0;i<P;i++){
        int r;scanf("%s %d %d %d %d",b,&D[i],&S[i],&B[i],&r);pn[i]=b;
        for(int j=0;j<r;j++){int l;scanf("%s %d",b,&l);R[i].push_back({gs(b),l});}
    }
    vector<int> freeAt(C,0);
    vector<int> order(P);
    iota(order.begin(),order.end(),0);
    sort(order.begin(),order.end(),[&](int a,int c){
        double x=(double)S[a]/D[a]/R[a].size(),y=(double)S[c]/D[c]/R[c].size();
        return x>y;});
    vector<int> rem=order;
    vector<pair<int,vector<int>>> out;
    long long t=0;
    auto st=chrono::steady_clock::now();
    vector<char> used(C,0);
    while(!rem.empty()){
        if(chrono::duration<double>(chrono::steady_clock::now()-st).count()>7.5)break;
        vector<int> nrem;
        for(int p:rem){
            long long end=t+D[p];
            if(S[p]-max(0LL,end-B[p])<=0)continue;
            int r=R[p].size();
            vector<int> asg(r,-1);
            vector<int> ro(r);iota(ro.begin(),ro.end(),0);
            sort(ro.begin(),ro.end(),[&](int a,int c){return R[p][a].second>R[p][c].second;});
            vector<int> team;bool ok=true;
            auto lvl=[&](int c,int s){auto it=sk[c].find(s);return it==sk[c].end()?0:it->second;};
            for(int k:ro){
                int s=R[p][k].first,l=R[p][k].second;
                int best=-1,bl=1e9;
                bool mentor=false;
                for(int c:team)if(lvl(c,s)>=l)mentor=true;
                for(int c=0;c<C;c++){
                    if(freeAt[c]>t||used[c])continue;
                    int v=lvl(c,s);
                    if(v>=l||(v==l-1&&mentor)){
                        if(v<bl){bl=v;best=c;}
                    }
                }
                if(best<0){ok=false;break;}
                used[best]=1;team.push_back(best);asg[k]=best;
            }
            if(ok){
                // verify mentoring
                for(int k=0;k<r&&ok;k++){
                    int s=R[p][k].first,l=R[p][k].second;
                    if(lvl(asg[k],s)<l){
                        bool m=false;
                        for(int j=0;j<r;j++)if(j!=k&&lvl(asg[j],s)>=l)m=true;
                        if(!m)ok=false;
                    }
                }
            }
            for(int c:team)used[c]=0;
            if(!ok){nrem.push_back(p);continue;}
            for(int k=0;k<r;k++){
                int c=asg[k],s=R[p][k].first,l=R[p][k].second;
                if(lvl(c,s)<=l)sk[c][s]=lvl(c,s)+1;
                freeAt[c]=end;
            }
            out.push_back({p,asg});
        }
        rem=nrem;
        long long nt=LLONG_MAX;
        for(int c=0;c<C;c++)if(freeAt[c]>t)nt=min(nt,(long long)freeAt[c]);
        if(nt==LLONG_MAX)break;
        t=nt;
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out){
        printf("%s\n",pn[o.first].c_str());
        for(size_t i=0;i<o.second.size();i++)printf("%s%s",i?" ":"",cn[o.second[i]].c_str());
        printf("\n");
    }
}
