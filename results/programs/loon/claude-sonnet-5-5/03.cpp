#include <bits/stdc++.h>
using namespace std;
struct S{int r,c,a,par;int sc;signed char mv;};
int main(){
    int R,C,A,L,V,B,T,rs,cs;
    scanf("%d %d %d",&R,&C,&A);
    scanf("%d %d %d %d",&L,&V,&B,&T);
    scanf("%d %d",&rs,&cs);
    vector<int> tr(L),tc(L);
    for(int i=0;i<L;i++)scanf("%d %d",&tr[i],&tc[i]);
    vector<signed char> wr((size_t)A*R*C),wc((size_t)A*R*C);
    for(size_t i=0;i<(size_t)A*R*C;i++){int a,b;scanf("%d %d",&a,&b);wr[i]=a;wc[i]=b;}
    vector<vector<unsigned char>> cov(T+1,vector<unsigned char>(L,0));
    vector<vector<signed char>> out(T,vector<signed char>(B,0));
    auto t0=chrono::steady_clock::now();
    int V2=V*V;
    for(int b=0;b<B;b++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        double remain=8.0-el;
        int W;
        if(remain<=0)W=1;
        else{
            double per=remain/(B-b);
            W=(int)(per/ (0.4e-6*T*3*max(1,L/4)));
            if(W<1)W=1;
            if(W>40)W=40;
        }
        if(remain<=0)W=1;
        vector<vector<S>> lay(T+1);
        lay[0].push_back({rs,cs,0,-1,0,0});
        for(int t=1;t<=T;t++){
            vector<S>&cur=lay[t];
            vector<int> unc;
            for(int i=0;i<L;i++)if(!cov[t][i])unc.push_back(i);
            vector<S> cand;
            vector<S>&pr=lay[t-1];
            for(int pi=0;pi<(int)pr.size();pi++){
                S&s=pr[pi];
                for(int d=-1;d<=1;d++){
                    int na=s.a+d;
                    if(na<0||na>A)continue;
                    if(s.a==0){
                        if(d<0)continue;
                        if(d==0){cand.push_back({s.r,s.c,0,pi,s.sc,0});continue;}
                    }else if(na<1)continue;
                    size_t idx=((size_t)(na-1)*R+s.r)*C+s.c;
                    int nr=s.r+wr[idx];
                    if(nr<0||nr>=R)continue;
                    int nc=((s.c+wc[idx])%C+C)%C;
                    int g=0;
                    for(int i:unc){
                        int dr=nr-tr[i];
                        int dc=abs(nc-tc[i]);dc=min(dc,C-dc);
                        if(dr*dr+dc*dc<=V2)g++;
                    }
                    cand.push_back({nr,nc,na,pi,s.sc+g,(signed char)d});
                }
            }
            sort(cand.begin(),cand.end(),[](const S&x,const S&y){
                if(x.sc!=y.sc)return x.sc>y.sc;
                if(x.a!=y.a)return x.a<y.a;
                if(x.r!=y.r)return x.r<y.r;
                return x.c<y.c;});
            set<tuple<int,int,int>> seen;
            for(auto&s:cand){
                if((int)cur.size()>=W)break;
                if(seen.insert({s.r,s.c,s.a}).second)cur.push_back(s);
            }
            if(cur.empty()){
                // dead: balloon lost
                for(int k=t+1;k<=T;k++){}
                lay.resize(t);
                break;
            }
        }
        int last=(int)lay.size()-1;
        if(last<1)continue;
        // best at last layer (first, sorted by score)
        int idx=0;
        vector<array<int,3>> path(last+1);
        for(int t=last;t>=1;t--){
            S&s=lay[t][idx];
            out[t-1][b]=s.mv;
            path[t]={s.r,s.c,s.a};
            idx=s.par;
        }
        for(int t=1;t<=last;t++){
            if(path[t][2]==0)continue;
            for(int i=0;i<L;i++){
                if(cov[t][i])continue;
                int dr=path[t][0]-tr[i];
                int dc=abs(path[t][1]-tc[i]);dc=min(dc,C-dc);
                if(dr*dr+dc*dc<=V2)cov[t][i]=1;
            }
        }
    }
    string o;
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            if(b)o+=' ';
            o+=to_string((int)out[t][b]);
        }
        o+='\n';
    }
    fputs(o.c_str(),stdout);
}
