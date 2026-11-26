#include <bits/stdc++.h>
using namespace std;
struct P{char t;int h,w,v;vector<string> g;int cnt;};
int H,W,D,B;
vector<P> p;
vector<vector<char>> occ;
bool fits(const P&b,int r,int c){
    if(r+b.h>H||c+b.w>W)return false;
    for(int i=0;i<b.h;i++)for(int j=0;j<b.w;j++)
        if(b.g[i][j]=='#'&&occ[r+i][c+j])return false;
    return true;
}
void put(const P&b,int r,int c){
    for(int i=0;i<b.h;i++)for(int j=0;j<b.w;j++)
        if(b.g[i][j]=='#')occ[r+i][c+j]=1;
}
int main(){
    scanf("%d %d %d %d",&H,&W,&D,&B);
    p.resize(B);
    for(int i=0;i<B;i++){
        char t;int h,w,v;
        scanf(" %c %d %d %d",&t,&h,&w,&v);
        p[i].t=t;p[i].h=h;p[i].w=w;p[i].v=v;p[i].g.resize(h);p[i].cnt=0;
        for(int k=0;k<h;k++){
            char buf[100];scanf("%s",buf);p[i].g[k]=buf;
            for(char ch:p[i].g[k])if(ch=='#')p[i].cnt++;
        }
    }
    int bestR=-1;double bs=-1;
    for(int i=0;i<B;i++)if(p[i].t=='R'){
        double s=(double)p[i].v/(p[i].h*p[i].w);
        if(s>bs){bs=s;bestR=i;}
    }
    map<int,int> ut;
    for(int i=0;i<B;i++)if(p[i].t=='U'){
        auto it=ut.find(p[i].v);
        if(it==ut.end()||p[i].h*p[i].w<p[it->second].h*p[it->second].w)ut[p[i].v]=i;
    }
    // choose best utility by smallest area (single type pattern)
    int bestU=-1;int ba=1<<30;
    for(auto&kv:ut){int a=p[kv.second].h*p[kv.second].w;if(a<ba){ba=a;bestU=kv.second;}}
    occ.assign(H,vector<char>(W,0));
    vector<array<int,3>> out;
    const P&R=p[bestR];const P&U=p[bestU];
    // pattern: block of rows: residential rows then utility row, repeated
    int r=0;
    bool useU=false;
    int period=0;
    while(r<H){
        if(useU){
            int c=0;
            bool any=false;
            while(c+U.w<=W&&r+U.h<=H){
                out.push_back({bestU,r,c});put(U,r,c);c+=U.w;any=true;
            }
            r+=U.h;
            useU=false;
            (void)any;
        }else{
            int c=0;
            while(c+R.w<=W&&r+R.h<=H){
                out.push_back({bestR,r,c});put(R,r,c);c+=R.w;
            }
            r+=R.h;
            period++;
            if(period%1==0)useU=true;
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&a:out)printf("%d %d %d\n",a[0],a[1],a[2]);
}
