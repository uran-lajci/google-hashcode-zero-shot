#include <bits/stdc++.h>
using namespace std;
struct P{char t;int h,w,v;vector<string> g;int area;};
int main(){
    int H,W,D,B;
    scanf("%d %d %d %d",&H,&W,&D,&B);
    vector<P> p(B);
    for(int i=0;i<B;i++){
        char c[4];
        scanf("%s %d %d %d",c,&p[i].h,&p[i].w,&p[i].v);
        p[i].t=c[0];
        p[i].g.resize(p[i].h);
        p[i].area=0;
        for(int r=0;r<p[i].h;r++){
            char buf[100];
            scanf("%s",buf);
            p[i].g[r]=buf;
            for(char ch:p[i].g[r])if(ch=='#')p[i].area++;
        }
    }
    int bestR=-1;
    double bd=-1;
    for(int i=0;i<B;i++)if(p[i].t=='R'){
        double d=(double)p[i].v/p[i].area;
        if(d>bd){bd=d;bestR=i;}
    }
    vector<int> utils;
    for(int i=0;i<B;i++)if(p[i].t=='U')utils.push_back(i);
    sort(utils.begin(),utils.end(),[&](int a,int b){
        if(p[a].area!=p[b].area)return p[a].area<p[b].area;
        return a<b;
    });
    vector<vector<char>> occ(H,vector<char>(W,0));
    vector<array<int,3>> out;
    auto fits=[&](int id,int r,int c){
        if(r+p[id].h>H||c+p[id].w>W)return false;
        for(int i=0;i<p[id].h;i++)for(int j=0;j<p[id].w;j++)
            if(p[id].g[i][j]=='#'&&occ[r+i][c+j])return false;
        return true;
    };
    auto put=[&](int id,int r,int c){
        for(int i=0;i<p[id].h;i++)for(int j=0;j<p[id].w;j++)
            if(p[id].g[i][j]=='#')occ[r+i][c+j]=1;
        out.push_back({id,r,c});
    };
    int U=utils.size();
    int k=0;
    // pass A: sparse utility lattice
    int sr=max(1,D), sc=max(1,D);
    int step=0;
    for(int r=0;r<H;r+=sr+2){
        for(int c=0;c<W;c+=sc+2){
            int id=utils[(k++)%U];
            // shift lattice rows to vary types
            int cc=c+(step%2)*(sc/2);
            if(fits(id,r,cc))put(id,r,cc);
        }
        step++;
    }
    // pass B: fill with residential
    for(int r=0;r<H;r++)for(int c=0;c<W;c++){
        if(fits(bestR,r,c))put(bestR,r,c);
    }
    printf("%d\n",(int)out.size());
    for(auto&a:out)printf("%d %d %d\n",a[0],a[1],a[2]);
}
