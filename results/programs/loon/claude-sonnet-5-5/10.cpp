#include <bits/stdc++.h>
using namespace std;
int R,C,A,L,V,B,T,rs,cs;
vector<int> wr,wc;
vector<pair<int,int>> tg;
vector<vector<char>> cov;
vector<vector<int>> rowT;
int gain(int t,int r,int c){
    int g=0;
    for(int rr=max(0,r-V);rr<=min(R-1,r+V);rr++)
        for(int id:rowT[rr]){
            if(cov[t][id])continue;
            int dc=abs(c-tg[id].second);dc=min(dc,C-dc);
            int dr=r-rr;
            if(dr*dr+dc*dc<=V*V)g++;
        }
    return g;
}
void mark(int t,int r,int c){
    for(int rr=max(0,r-V);rr<=min(R-1,r+V);rr++)
        for(int id:rowT[rr]){
            int dc=abs(c-tg[id].second);dc=min(dc,C-dc);
            int dr=r-rr;
            if(dr*dr+dc*dc<=V*V)cov[t][id]=1;
        }
}
struct S{int a,r,c;long long v;int par;int d;};
int main(){
    auto st=chrono::steady_clock::now();
    scanf("%d %d %d %d %d %d %d %d %d",&R,&C,&A,&L,&V,&B,&T,&rs,&cs);
    tg.resize(L);rowT.assign(R,{});
    for(int i=0;i<L;i++){scanf("%d %d",&tg[i].first,&tg[i].second);rowT[tg[i].first].push_back(i);}
    wr.resize((size_t)A*R*C);wc.resize((size_t)A*R*C);
    for(size_t i=0;i<(size_t)A*R*C;i++)scanf("%d %d",&wr[i],&wc[i]);
    cov.assign(T+1,vector<char>(L,0));
    vector<vector<signed char>> ans(T,vector<signed char>(B,0));
    const int K=6;
    for(int b=0;b<B;b++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
        if(el>7.0)break;
        vector<vector<S>> lay(T+1);
        lay[0].push_back({0,rs,cs,0,-1,0});
        for(int t=1;t<=T;t++){
            vector<S> cand;
            for(int i=0;i<(int)lay[t-1].size();i++){
                S&s=lay[t-1][i];
                for(int d=-1;d<=1;d++){
                    int na=s.a+d;
                    if(na<0||na>A)continue;
                    if(s.a==0&&d<0)continue;
                    if(s.a>0&&na<1)continue;
                    int nr=s.r,nc=s.c;
                    if(na>=1){
                        size_t k=((size_t)(na-1)*R+s.r)*C+s.c;
                        nr=s.r+wr[k];
                        if(nr<0||nr>=R)continue;
                        nc=((s.c+wc[k])%C+C)%C;
                    }
                    long long v=s.v;
                    if(na>=1){
                        int g=gain(t,nr,nc);
                        v+=g*1000;
                        if(g==0){
                            int best=1<<30;
                            for(auto&p:tg){
                                int dc=abs(nc-p.second);dc=min(dc,C-dc);
                                best=min(best,abs(nr-p.first)+dc);
                            }
                            v-=min(best,999);
                        }
                    }
                    cand.push_back({na,nr,nc,v,i,d});
                }
            }
            sort(cand.begin(),cand.end(),[](const S&x,const S&y){
                if(x.v!=y.v)return x.v>y.v;
                if(x.a!=y.a)return x.a<y.a;
                if(x.r!=y.r)return x.r<y.r;
                return x.c<y.c;});
            for(auto&s:cand){
                bool dup=false;
                for(auto&q:lay[t])if(q.a==s.a&&q.r==s.r&&q.c==s.c){dup=true;break;}
                if(dup)continue;
                lay[t].push_back(s);
                if((int)lay[t].size()>=K)break;
            }
            if(lay[t].empty()){
                for(int u=t+1;u<=T;u++)lay[u].clear();
                break;
            }
        }
        int bi=-1;
        long long bv=-1;
        for(int i=0;i<(int)lay[T].size();i++)if(lay[T][i].v>bv){bv=lay[T][i].v;bi=i;}
        if(bi<0)continue;
        int idx=bi;
        for(int t=T;t>=1;t--){
            S&s=lay[t][idx];
            ans[t-1][b]=s.d;
            if(s.a>=1)mark(t,s.r,s.c);
            idx=s.par;
        }
    }
    string out;
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            out+=to_string((int)ans[t][b]);
            out+=(b+1<B?' ':'\n');
        }
    }
    fputs(out.c_str(),stdout);
}
