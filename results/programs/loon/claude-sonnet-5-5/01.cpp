#include <bits/stdc++.h>
using namespace std;
int R,C,A,L,V,B,T,rs,cs;
vector<int> wr,wc;
vector<pair<int,int>> tg;
int main(){
    scanf("%d %d %d %d %d %d %d %d %d",&R,&C,&A,&L,&V,&B,&T,&rs,&cs);
    tg.resize(L);
    for(auto&p:tg)scanf("%d %d",&p.first,&p.second);
    wr.resize((size_t)A*R*C);wc.resize((size_t)A*R*C);
    for(size_t i=0;i<wr.size();i++){scanf("%d %d",&wr[i],&wc[i]);}
    vector<vector<signed char>> adj(T,vector<signed char>(B,0));
    // cover count per turn per target
    vector<vector<int>> cov(T+1,vector<int>(L,0));
    auto st=chrono::steady_clock::now();
    auto covers=[&](int r,int c,int k){
        int dr=r-tg[k].first;
        int dc=abs(c-tg[k].second);dc=min(dc,C-dc);
        return dr*dr+dc*dc<=V*V;
    };
    struct Node{int a,r,c,par;signed char d;float v;};
    int W=3;
    for(int b=0;b<B;b++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
        if(el>7.0)break;
        vector<vector<Node>> lay(T+1);
        lay[0].push_back({0,rs,cs,-1,0,0});
        for(int t=1;t<=T;t++){
            vector<Node> cand;
            for(int i=0;i<(int)lay[t-1].size();i++){
                Node&n=lay[t-1][i];
                for(int d=-1;d<=1;d++){
                    int na=n.a+d;
                    if(na<0||na>A)continue;
                    if(n.a==0&&d<0)continue;
                    if(n.a>=1&&na<1)continue;
                    if(na==0){cand.push_back({0,n.r,n.c,i,(signed char)d,n.v});continue;}
                    size_t idx=((size_t)(na-1)*R+n.r)*C+n.c;
                    int nr=n.r+wr[idx];
                    if(nr<0||nr>=R)continue;
                    int nc=((n.c+wc[idx])%C+C)%C;
                    int g=0;
                    for(int k=0;k<L;k++)if(cov[t][k]==0&&covers(nr,nc,k))g++;
                    cand.push_back({na,nr,nc,i,(signed char)d,n.v+g});
                }
            }
            sort(cand.begin(),cand.end(),[](const Node&x,const Node&y){return x.v>y.v;});
            set<tuple<int,int,int>> seen;
            for(auto&n:cand){
                if((int)lay[t].size()>=W)break;
                if(seen.insert({n.a,n.r,n.c}).second)lay[t].push_back(n);
            }
            if(lay[t].empty())break;
        }
        int bt=-1;
        for(int t=T;t>=0;t--)if(!lay[t].empty()){bt=t;break;}
        if(bt<0)continue;
        int bi=0;
        for(int i=0;i<(int)lay[bt].size();i++)if(lay[bt][i].v>lay[bt][bi].v)bi=i;
        // reconstruct
        vector<Node> path(bt+1);
        int cur=bi;
        for(int t=bt;t>=0;t--){path[t]=lay[t][cur];cur=lay[t][cur].par;}
        for(int t=1;t<=bt;t++){
            adj[t-1][b]=path[t].d;
            if(path[t].a>0)for(int k=0;k<L;k++)if(covers(path[t].r,path[t].c,k))cov[t][k]++;
        }
        // after being lost, remaining turns stay valid with 0 adjustments
    }
    string out;
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            out+=to_string((int)adj[t][b]);
            out+=(b+1<B?' ':'\n');
        }
    }
    fputs(out.c_str(),stdout);
}
