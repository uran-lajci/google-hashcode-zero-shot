#include <bits/stdc++.h>
using namespace std;
struct P{bool res;int h,w,v;vector<string> g;vector<pair<int,int>> cells;};
int H,W,D,B;
vector<P> ps;
struct Pl{int b,r,c;};
bool fits(vector<vector<char>>&occ,const P&p,int r,int c){
    if(r+p.h>H||c+p.w>W)return false;
    for(auto&q:p.cells)if(occ[r+q.first][c+q.second])return false;
    return true;
}
void put(vector<vector<char>>&occ,const P&p,int r,int c){
    for(auto&q:p.cells)occ[r+q.first][c+q.second]=1;
}
long long score(const vector<Pl>&v){
    vector<int> R,U;
    for(int i=0;i<(int)v.size();i++){if(ps[v[i].b].res)R.push_back(i);else U.push_back(i);}
    long long s=0;
    for(int i:R){
        const P&a=ps[v[i].b];
        set<int> t;
        for(int j:U){
            const P&b=ps[v[j].b];
            if(t.count(b.v))continue;
            int ar=v[i].r,ac=v[i].c,br=v[j].r,bc=v[j].c;
            int dr=max(0,max(ar-(br+b.h-1),br-(ar+a.h-1)));
            int dc=max(0,max(ac-(bc+b.w-1),bc-(ac+a.w-1)));
            if(dr+dc>D)continue;
            bool ok=false;
            for(auto&x:a.cells){
                for(auto&y:b.cells){
                    if(abs(ar+x.first-br-y.first)+abs(ac+x.second-bc-y.second)<=D){ok=true;break;}
                }
                if(ok)break;
            }
            if(ok)t.insert(b.v);
        }
        s+=(long long)a.v*t.size();
    }
    return s;
}
int main(){
    scanf("%d %d %d %d",&H,&W,&D,&B);
    ps.resize(B);
    for(auto&p:ps){
        char t;scanf(" %c %d %d %d",&t,&p.h,&p.w,&p.v);
        p.res=(t=='R');
        p.g.resize(p.h);
        for(int i=0;i<p.h;i++){
            char buf[64];scanf("%s",buf);p.g[i]=buf;
            for(int j=0;j<p.w;j++)if(buf[j]=='#')p.cells.push_back({i,j});
        }
    }
    int bestR=-1;double br=-1;
    for(int i=0;i<B;i++)if(ps[i].res){
        double r=(double)ps[i].v/ps[i].cells.size();
        if(r>br){br=r;bestR=i;}
    }
    // utility per type: smallest cell count
    map<int,int> ut;
    for(int i=0;i<B;i++)if(!ps[i].res){
        auto it=ut.find(ps[i].v);
        if(it==ut.end()||ps[i].cells.size()<ps[it->second].cells.size())ut[ps[i].v]=i;
    }
    vector<int> U;
    vector<pair<size_t,int>> tmp;
    for(auto&k:ut)tmp.push_back({ps[k.second].cells.size(),k.second});
    sort(tmp.begin(),tmp.end());
    for(auto&k:tmp)U.push_back(k.second);
    vector<Pl> best;long long bs=-1;
    int maxU=min<int>(U.size(),4);
    auto start=chrono::steady_clock::now();
    for(int nu=1;nu<=maxU;nu++){
        for(int k=2;k<=6;k++){
            if(chrono::duration<double>(chrono::steady_clock::now()-start).count()>6.0)break;
            vector<vector<char>> occ(H,vector<char>(W,0));
            vector<Pl> v;int cnt=0,ui=0;
            for(int r=0;r<H;r++)for(int c=0;c<W;c++){
                bool useU=(cnt%k==k-1);
                int b=useU?U[ui%nu]:bestR;
                if(fits(occ,ps[b],r,c)){
                    put(occ,ps[b],r,c);v.push_back({b,r,c});
                    cnt++;if(useU)ui++;
                }
            }
            if(v.size()>6000)continue;
            long long s=score(v);
            if(s>bs){bs=s;best=v;}
        }
    }
    if(bs<0){
        // fallback: one residential and one utility
        vector<vector<char>> occ(H,vector<char>(W,0));
        int u=U[0];
        for(int r=0;r<H&&best.size()<2;r++)for(int c=0;c<W&&best.size()<2;c++){
            int b=best.empty()?bestR:u;
            if(fits(occ,ps[b],r,c)){put(occ,ps[b],r,c);best.push_back({b,r,c});}
        }
    }
    printf("%d\n",(int)best.size());
    for(auto&p:best)printf("%d %d %d\n",p.b,p.r,p.c);
}
