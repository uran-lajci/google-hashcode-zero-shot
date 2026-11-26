#include <bits/stdc++.h>
using namespace std;
struct P{char t;int h,w,v,occ;};
int main(){
    int H,W,D,B;
    scanf("%d %d %d %d",&H,&W,&D,&B);
    vector<P> p(B);
    for(int i=0;i<B;i++){
        char t[3];
        scanf("%s %d %d %d",t,&p[i].h,&p[i].w,&p[i].v);
        p[i].t=t[0];
        int c=0;
        for(int r=0;r<p[i].h;r++){
            char s[100];
            scanf("%s",s);
            for(int j=0;j<p[i].w;j++)if(s[j]=='#')c++;
        }
        p[i].occ=c;
    }
    // best residential: capacity per bounding-box area
    int br=-1;
    double bs=-1;
    for(int i=0;i<B;i++)if(p[i].t=='R'){
        double s=(double)p[i].v/(p[i].h*p[i].w);
        if(s>bs){bs=s;br=i;}
    }
    // best utility per type: smallest bounding-box area
    map<int,int> bu;
    for(int i=0;i<B;i++)if(p[i].t=='U'){
        auto it=bu.find(p[i].v);
        if(it==bu.end()||p[i].h*p[i].w<p[it->second].h*p[it->second].w)bu[p[i].v]=i;
    }
    vector<int> us;
    for(auto&k:bu)us.push_back(k.second);
    sort(us.begin(),us.end(),[&](int a,int b){
        int x=p[a].h*p[a].w,y=p[b].h*p[b].w;
        if(x!=y)return x<y;
        return a<b;
    });
    int K=min((int)us.size(),max(1,D));
    us.resize(K);
    vector<array<int,3>> out;
    int y=0,ui=0;
    while(y<H){
        int x=0,bh=0;
        bool res=true;
        while(true){
            int id=res?br:us[ui%K];
            if(x+p[id].w>W){
                // try a utility to fill remaining space
                break;
            }
            if(y+p[id].h>H)break;
            out.push_back({id,y,x});
            x+=p[id].w;
            bh=max(bh,p[id].h);
            if(!res)ui++;
            res=!res;
        }
        if(bh==0)break;
        y+=bh;
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out)printf("%d %d %d\n",o[0],o[1],o[2]);
}
