#include <bits/stdc++.h>
using namespace std;
struct B{char t;int h,w,v;vector<pair<int,int>> cells;};
int H,W,D,NB;
vector<B> bs;
struct Pl{int b,r,c;};
vector<Pl> gen(int k,int m,const vector<int>&res,const vector<int>&ut){
    vector<char> grid((size_t)H*W,0);
    vector<int> rowPtr(H,0);
    int startRow=0;
    long long ops=0,budget=50000000LL;
    vector<char> failed(NB,0);
    vector<Pl> out;
    bool stop=false;
    auto place=[&](int p)->bool{
        const B&b=bs[p];
        for(int r=startRow;r<=H-b.h;r++){
            for(int c=rowPtr[r];c<=W-b.w;c++){
                if(++ops>budget){stop=true;return false;}
                bool ok=true;
                for(auto&q:b.cells){
                    if(grid[(size_t)(r+q.first)*W+c+q.second]){ok=false;break;}
                }
                if(ok){
                    for(auto&q:b.cells)grid[(size_t)(r+q.first)*W+c+q.second]=1;
                    for(int i=r;i<r+b.h;i++){
                        while(rowPtr[i]<W&&grid[(size_t)i*W+rowPtr[i]])rowPtr[i]++;
                    }
                    while(startRow<H&&rowPtr[startRow]>=W)startRow++;
                    out.push_back({p,r,c});
                    return true;
                }
            }
        }
        return false;
    };
    int ri=0,cyc=0;
    int kk=min<int>(k,ut.size());
    bool utAlive=true;
    while(!stop){
        for(int i=0;i<m&&!stop;i++){
            bool placed=false;
            while(ri<(int)res.size()){
                if(place(res[ri])){placed=true;break;}
                if(stop)break;
                ri++;
            }
            if(!placed){stop=true;}
        }
        if(stop)break;
        if(utAlive){
            bool any=false;
            for(int a=0;a<kk;a++){
                int t=(cyc++)%kk;
                if(failed[ut[t]])continue;
                if(place(ut[t])){any=true;break;}
                if(stop)break;
                failed[ut[t]]=1;
            }
            if(!any&&!stop){
                bool all=true;
                for(int a=0;a<kk;a++)if(!failed[ut[a]])all=false;
                if(all)utAlive=false;
            }
        }
    }
    return out;
}
long long evalL(const vector<Pl>&pl){
    int BK=64;
    int bh=(H+BK-1)/BK+1,bw=(W+BK-1)/BK+1;
    vector<vector<int>> bucket((size_t)bh*bw);
    for(int i=0;i<(int)pl.size();i++){
        if(bs[pl[i].b].t=='U')bucket[(size_t)(pl[i].r/BK)*bw+pl[i].c/BK].push_back(i);
    }
    vector<int> stamp(1001,-1);
    long long total=0;
    int reach=(D+50)/BK+1;
    for(int i=0;i<(int)pl.size();i++){
        const B&a=bs[pl[i].b];
        if(a.t!='R')continue;
        int r1=pl[i].r,c1=pl[i].c,r2=r1+a.h-1,c2=c1+a.w-1;
        int br=r1/BK,bc=c1/BK;
        int cnt=0;
        for(int x=max(0,br-reach);x<=min(bh-1,br+reach);x++)
            for(int y=max(0,bc-reach);y<=min(bw-1,bc+reach);y++){
                for(int j:bucket[(size_t)x*bw+y]){
                    const B&u=bs[pl[j].b];
                    int ur1=pl[j].r,uc1=pl[j].c,ur2=ur1+u.h-1,uc2=uc1+u.w-1;
                    int dr=max(0,max(ur1-r2,r1-ur2)),dc=max(0,max(uc1-c2,c1-uc2));
                    if(dr+dc<=D&&stamp[u.v]!=i){stamp[u.v]=i;cnt++;}
                }
            }
        total+=(long long)cnt*a.v;
    }
    return total;
}
int main(){
    scanf("%d %d %d %d",&H,&W,&D,&NB);
    bs.resize(NB);
    for(int i=0;i<NB;i++){
        char t[4];
        scanf("%s %d %d %d",t,&bs[i].h,&bs[i].w,&bs[i].v);
        bs[i].t=t[0];
        for(int r=0;r<bs[i].h;r++){
            char s[64];
            scanf("%s",s);
            for(int c=0;c<bs[i].w;c++)if(s[c]=='#')bs[i].cells.push_back({r,c});
        }
    }
    vector<int> res;
    map<int,int> bestU;
    for(int i=0;i<NB;i++){
        if(bs[i].t=='R')res.push_back(i);
        else{
            auto it=bestU.find(bs[i].v);
            if(it==bestU.end()||bs[i].cells.size()<bs[it->second].cells.size())bestU[bs[i].v]=i;
        }
    }
    sort(res.begin(),res.end(),[&](int a,int b){
        long long x=(long long)bs[a].v*bs[b].cells.size(),y=(long long)bs[b].v*bs[a].cells.size();
        if(x!=y)return x>y;
        return a<b;
    });
    vector<int> ut;
    for(auto&p:bestU)ut.push_back(p.second);
    sort(ut.begin(),ut.end(),[&](int a,int b){
        if(bs[a].cells.size()!=bs[b].cells.size())return bs[a].cells.size()<bs[b].cells.size();
        return a<b;
    });
    int ks[]={1,2,3,5,8},ms[]={1,3};
    vector<Pl> best;
    long long bestS=-1;
    for(int k:ks)for(int m:ms){
        auto pl=gen(k,m,res,ut);
        long long s=evalL(pl);
        if(s>bestS){bestS=s;best=pl;}
    }
    printf("%d\n",(int)best.size());
    for(auto&p:best)printf("%d %d %d\n",p.b,p.r,p.c);
}
