#include <bits/stdc++.h>
using namespace std;
int R,C,A,L,V,B,T,rs,cs;
vector<int> wr,wc;
vector<pair<int,int>> tg;
vector<vector<int>> rowT;
vector<vector<char>> cov;
int tid[1000][1000];
static inline int gain(int t,int r,int c){
    int g=0;
    int lo=max(0,r-V),hi=min(R-1,r+V);
    for(int u=lo;u<=hi;u++){
        for(int id:rowT[u]){
            int v=tg[id].second;
            int d=abs(c-v);d=min(d,C-d);
            int dr=r-u;
            if(dr*dr+d*d<=V*V&&!cov[t][id])g++;
        }
    }
    return g;
}
struct Node{int r,c,a,par;signed char act;long long sc;};
int main(){
    scanf("%d %d %d",&R,&C,&A);
    scanf("%d %d %d %d",&L,&V,&B,&T);
    scanf("%d %d",&rs,&cs);
    tg.resize(L);rowT.assign(R,{});
    for(int i=0;i<L;i++){scanf("%d %d",&tg[i].first,&tg[i].second);rowT[tg[i].first].push_back(i);}
    wr.resize((size_t)A*R*C);wc.resize((size_t)A*R*C);
    for(size_t i=0;i<(size_t)A*R*C;i++){scanf("%d %d",&wr[i],&wc[i]);}
    cov.assign(T+1,vector<char>(L,0));
    vector<vector<signed char>> out(T,vector<signed char>(B,0));
    clock_t st=clock();
    for(int b=0;b<B;b++){
        double el=(double)(clock()-st)/CLOCKS_PER_SEC;
        double perBalloonLeft=(8.0-el)/(B-b);
        int W=3;
        if(perBalloonLeft<0.002)W=1;
        else if(perBalloonLeft>0.02)W=6;
        vector<vector<Node>> lay(T+1);
        lay[0].push_back({rs,cs,0,-1,0,0});
        for(int t=1;t<=T;t++){
            vector<Node> cand;
            auto&prev=lay[t-1];
            for(int i=0;i<(int)prev.size();i++){
                const Node&n=prev[i];
                for(int d=-1;d<=1;d++){
                    int na=n.a+d;
                    if(na<0||na>A)continue;
                    if(n.a==0&&d<0)continue;
                    if(n.a>0&&na<1)continue;
                    int nr=n.r,nc=n.c;long long g=0;
                    if(na>0){
                        size_t k=((size_t)(na-1)*R+n.r)*C+n.c;
                        nr=n.r+wr[k];
                        if(nr<0||nr>=R)continue;
                        nc=((n.c+wc[k])%C+C)%C;
                        g=gain(t,nr,nc);
                    }
                    cand.push_back({nr,nc,na,i,(signed char)d,n.sc+g});
                }
            }
            sort(cand.begin(),cand.end(),[](const Node&x,const Node&y){return x.sc>y.sc;});
            set<tuple<int,int,int>> seen;
            for(auto&n:cand){
                if((int)lay[t].size()>=W)break;
                if(n.a>0&&!seen.insert({n.r,n.c,n.a}).second)continue;
                lay[t].push_back(n);
            }
            if(lay[t].empty())break;
        }
        // find best final
        int bt=0;
        for(int t=T;t>=0;t--)if(!lay[t].empty()){bt=t;break;}
        int bi=0;
        for(int i=0;i<(int)lay[bt].size();i++)if(lay[bt][i].sc>lay[bt][bi].sc)bi=i;
        if(lay[bt][bi].sc<=0)continue;
        vector<int> idx(bt+1);
        int cur=bi;
        for(int t=bt;t>=1;t--){
            out[t-1][b]=lay[t][cur].act;
            idx[t]=cur;
            cur=lay[t][cur].par;
        }
        // replay to mark coverage
        int r=rs,c=cs,a=0;
        for(int t=1;t<=bt;t++){
            a+=out[t-1][b];
            if(a>0){
                size_t k=((size_t)(a-1)*R+r)*C+c;
                r+=wr[k];
                c=((c+wc[k])%C+C)%C;
                int lo=max(0,r-V),hi=min(R-1,r+V);
                for(int u=lo;u<=hi;u++)for(int id:rowT[u]){
                    int d=abs(c-tg[id].second);d=min(d,C-d);
                    int dr=r-u;
                    if(dr*dr+d*d<=V*V)cov[t][id]=1;
                }
            }
        }
        // remaining turns after bt: output 0 (altitude stays)
    }
    string s;
    for(int t=0;t<T;t++){
        s.clear();
        for(int b=0;b<B;b++){
            if(b)s+=' ';
            int v=out[t][b];
            if(v==-1)s+="-1";else s+=char('0'+v);
        }
        puts(s.c_str());
    }
}
