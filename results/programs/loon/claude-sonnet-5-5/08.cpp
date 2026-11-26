#include <bits/stdc++.h>
using namespace std;
int R,C,A,L,V,B,T,rs,cs;
vector<int> W; // dr,dc per altitude
int main(){
    scanf("%d %d %d %d %d %d %d %d %d",&R,&C,&A,&L,&V,&B,&T,&rs,&cs);
    vector<pair<int,int>> tg(L);
    for(auto&p:tg)scanf("%d %d",&p.first,&p.second);
    sort(tg.begin(),tg.end());
    vector<int> tr(L);
    for(int i=0;i<L;i++)tr[i]=tg[i].first;
    vector<signed char> dr((size_t)A*R*C),dc((size_t)A*R*C);
    for(size_t i=0;i<(size_t)A*R*C;i++){int a,b;scanf("%d %d",&a,&b);dr[i]=a;dc[i]=b;}
    vector<vector<unsigned char>> cov(T+1,vector<unsigned char>(L,0));
    vector<vector<signed char>> out(T,vector<signed char>(B,0));
    auto gain=[&](int t,int r,int c){
        int lo=lower_bound(tr.begin(),tr.end(),r-V)-tr.begin();
        int g=0;
        for(int i=lo;i<L&&tr[i]<=r+V;i++){
            if(cov[t][i])continue;
            int d=abs(c-tg[i].second);d=min(d,C-d);
            int e=tr[i]-r;
            if(e*e+d*d<=V*V)g++;
        }
        return g;
    };
    auto mark=[&](int t,int r,int c){
        int lo=lower_bound(tr.begin(),tr.end(),r-V)-tr.begin();
        for(int i=lo;i<L&&tr[i]<=r+V;i++){
            int d=abs(c-tg[i].second);d=min(d,C-d);
            int e=tr[i]-r;
            if(e*e+d*d<=V*V)cov[t][i]=1;
        }
    };
    struct S{int alt,r,c,par,sc;signed char mv;};
    const int K=8;
    clock_t st=clock();
    for(int b=0;b<B;b++){
        double el=(double)(clock()-st)/CLOCKS_PER_SEC;
        if(el>8.0)break;
        vector<vector<S>> lay(T+1);
        lay[0].push_back({0,rs,cs,-1,0,0});
        for(int t=1;t<=T;t++){
            vector<S> cand;
            auto&pl=lay[t-1];
            for(int i=0;i<(int)pl.size();i++){
                S&s=pl[i];
                for(int m=-1;m<=1;m++){
                    int na=s.alt+m;
                    if(s.alt==0){if(m<0)continue;}
                    else if(na<1||na>A)continue;
                    if(na>A)continue;
                    int nr=s.r,nc=s.c;
                    if(na>0){
                        size_t k=((size_t)(na-1)*R+s.r)*C+s.c;
                        nr=s.r+dr[k];nc=((s.c+dc[k])%C+C)%C;
                        if(nr<0||nr>=R)continue;
                    }
                    int g=na>0?gain(t,nr,nc):0;
                    cand.push_back({na,nr,nc,i,s.sc+g,(signed char)m});
                }
            }
            stable_sort(cand.begin(),cand.end(),[](const S&a,const S&b){return a.sc>b.sc;});
            set<tuple<int,int,int>> seen;
            for(auto&s:cand){
                if((int)lay[t].size()>=K)break;
                if(seen.insert({s.alt,s.r,s.c}).second)lay[t].push_back(s);
            }
            if(lay[t].empty())break;
        }
        int bt=-1;
        for(int t=T;t>=0;t--)if(!lay[t].empty()){bt=t;break;}
        if(bt<0)continue;
        // pick best at bt; if died early, fine
        int bi=0;
        for(int i=0;i<(int)lay[bt].size();i++)if(lay[bt][i].sc>lay[bt][bi].sc)bi=i;
        int idx=bi;
        for(int t=bt;t>=1;t--){
            S&s=lay[t][idx];
            out[t-1][b]=s.mv;
            mark(t,s.r,s.c);
            idx=s.par;
        }
        // remaining turns after loss: need valid zeros; altitude unchanged is valid
    }
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++)printf("%d%c",(int)out[t][b],b+1<B?' ':'\n');
    }
}
