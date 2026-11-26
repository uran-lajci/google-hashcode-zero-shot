#include <bits/stdc++.h>
using namespace std;
int R,C,A,L,V,B,T,rs,cs;
vector<int> W; // dr,dc per altitude,cell
vector<vector<int>> rowT; // targets columns by row
vector<vector<char>> cov; // [turn][target]
vector<int> tr,tc;
int cd(int a,int b){int d=abs(a-b);return min(d,C-d);}
int main(){
    scanf("%d %d %d",&R,&C,&A);
    scanf("%d %d %d %d",&L,&V,&B,&T);
    scanf("%d %d",&rs,&cs);
    tr.resize(L);tc.resize(L);
    rowT.assign(R,{});
    for(int i=0;i<L;i++){scanf("%d %d",&tr[i],&tc[i]);rowT[tr[i]].push_back(i);}
    W.resize((size_t)A*R*C*2);
    for(size_t i=0;i<W.size();i++)scanf("%d",&W[i]);
    cov.assign(T+1,vector<char>(L,0));
    long long budget=60000000LL;
    long long K=budget/((long long)B*T*3);
    if(K<1)K=1; if(K>200)K=200;
    vector<vector<int>> out(T,vector<int>(B,0));
    struct S{int a,r,c;int par;int adj;double v;};
    for(int b=0;b<B;b++){
        vector<vector<S>> lay(T+1);
        lay[0].push_back({0,rs,cs,-1,0,0});
        for(int t=1;t<=T;t++){
            unordered_map<long long,int> idx;
            vector<S> nx;
            auto&cur=lay[t-1];
            for(int i=0;i<(int)cur.size();i++){
                S&s=cur[i];
                for(int d=-1;d<=1;d++){
                    int na=s.a+d;
                    if(s.a==0&&d<0)continue;
                    if(na<0||na>A)continue;
                    if(s.a>0&&na<1)continue;
                    int nr=s.r,nc=s.c;
                    if(na>0){
                        size_t o=(((size_t)(na-1)*R+s.r)*C+s.c)*2;
                        nr=s.r+W[o];
                        if(nr<0||nr>=R)continue;
                        nc=(((s.c+W[o+1])%C)+C)%C;
                    }
                    double g=0;
                    if(na>0){
                        for(int rr=max(0,nr-V);rr<=min(R-1,nr+V);rr++)
                            for(int id:rowT[rr]){
                                if(cov[t][id])continue;
                                int dr=rr-nr,dc=cd(nc,tc[id]);
                                if(dr*dr+dc*dc<=V*V)g++;
                            }
                    }
                    double nv=s.v+g;
                    long long key=((long long)na*R+nr)*C+nc;
                    auto it=idx.find(key);
                    if(it==idx.end()){idx[key]=nx.size();nx.push_back({na,nr,nc,i,d,nv});}
                    else if(nx[it->second].v<nv)nx[it->second]={na,nr,nc,i,d,nv};
                }
            }
            if((int)nx.size()>K){
                nth_element(nx.begin(),nx.begin()+K,nx.end(),[](const S&x,const S&y){return x.v>y.v;});
                nx.resize(K);
            }
            lay[t]=nx;
            if(nx.empty())break;
        }
        int best=-1;double bv=-1;int bt=0;
        for(int t=T;t>=0&&best<0;t--){
            if(lay[t].empty())continue;
            // only full-length paths matter; dead ones can't continue
            for(int i=0;i<(int)lay[t].size();i++)if(lay[t][i].v>bv){bv=lay[t][i].v;best=i;bt=t;}
        }
        // Reconstruct
        int i=best;
        vector<int> adj(T,0);
        vector<array<int,2>> pos(T+1);
        int t=bt;
        while(t>0){
            S&s=lay[t][i];
            adj[t-1]=s.adj;
            pos[t]={s.r,s.c};
            // altitude>0 means covering at this turn
            if(s.a>0){
                for(int rr=max(0,s.r-V);rr<=min(R-1,s.r+V);rr++)
                    for(int id:rowT[rr]){
                        int dr=rr-s.r,dc=cd(s.c,tc[id]);
                        if(dr*dr+dc*dc<=V*V)cov[t][id]=1;
                    }
            }
            i=s.par;t--;
        }
        // for turns after bt (balloon lost), adjust 0 (valid as altitude stays)
        for(int k=0;k<T;k++)out[k][b]=adj[k];
    }
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++)printf("%d%c",out[t][b],b==B-1?'\n':' ');
    }
}
