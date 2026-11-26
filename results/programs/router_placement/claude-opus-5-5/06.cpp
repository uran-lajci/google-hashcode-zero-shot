#include <bits/stdc++.h>
using namespace std;

int H,W,R,Pb,Pr; long long B; int br,bc;
vector<string> g;
vector<uint8_t> rf,lf;
vector<int> tp;
vector<char> cov,isT;
vector<int> distv,srcv,ubv;

template<class F> void forSeg(int a,int b,F f){
    for(int dir=1;dir>=-1;dir-=2){
        int rl=1000,ll=1000;
        for(int i=0;i<=R;i++){
            int r=a+dir*i; if(r<0||r>=H)break;
            rl=min(rl,(int)rf[r*W+b]); ll=min(ll,(int)lf[r*W+b]);
            if(rl==0||ll==0)break;
            if(i==0&&dir==-1)continue;
            f(r,b-ll+1,b+rl-1);
        }
    }
}
struct E{float k;int c;};
struct Cmp{bool operator()(const E&a,const E&b)const{ if(a.k!=b.k)return a.k<b.k; return a.c>b.c;}};

int main(){
    scanf("%d %d %d",&H,&W,&R);
    scanf("%d %d %lld",&Pb,&Pr,&B);
    scanf("%d %d",&br,&bc);
    g.resize(H);
    {static char buf[2000];
     for(int i=0;i<H;i++){scanf("%s",buf);g[i]=buf; if((int)g[i].size()<W)g[i].resize(W,'-');}}
    int N=H*W;
    rf.assign(N,0);lf.assign(N,0);
    for(int r=0;r<H;r++){
        for(int c=W-1;c>=0;c--){ if(g[r][c]=='#')rf[r*W+c]=0; else { int v=1+(c+1<W?rf[r*W+c+1]:0); rf[r*W+c]=min(v,R+1);} }
        for(int c=0;c<W;c++){ if(g[r][c]=='#')lf[r*W+c]=0; else { int v=1+(c>0?lf[r*W+c-1]:0); lf[r*W+c]=min(v,R+1);} }
    }
    tp.assign((size_t)H*(W+1),0);
    isT.assign(N,0);
    for(int r=0;r<H;r++)for(int c=0;c<W;c++){isT[r*W+c]=g[r][c]=='.';tp[(size_t)r*(W+1)+c+1]=tp[(size_t)r*(W+1)+c]+isT[r*W+c];}
    cov.assign(N,0);
    distv.assign(N,INT_MAX);srcv.assign(N,-1);ubv.assign(N,0);
    vector<char> bb(N,0);
    int seed=br*W+bc; bb[seed]=1;
    // initial BFS
    {
        vector<int> q;q.reserve(N);q.push_back(seed);distv[seed]=0;srcv[seed]=seed;
        for(size_t h=0;h<q.size();h++){int u=q[h];int ur=u/W,uc=u%W;
            for(int dr=-1;dr<=1;dr++)for(int dc=-1;dc<=1;dc++){if(!dr&&!dc)continue;int nr=ur+dr,nc=uc+dc;if(nr<0||nr>=H||nc<0||nc>=W)continue;int v=nr*W+nc;
                if(distv[v]>distv[u]+1){distv[v]=distv[u]+1;srcv[v]=srcv[u];q.push_back(v);}}}
    }
    auto key=[&](int c)->float{return (float)ubv[c]/(float)(Pr+(long long)Pb*distv[c]);};
    vector<E> init;init.reserve(N);
    for(int r=0;r<H;r++)for(int c=0;c<W;c++){
        if(g[r][c]=='#')continue;
        int s=0;
        forSeg(r,c,[&](int rr,int c1,int c2){s+=tp[(size_t)rr*(W+1)+c2+1]-tp[(size_t)rr*(W+1)+c1];});
        ubv[r*W+c]=s;
        if(s>0)init.push_back({key(r*W+c),r*W+c});
    }
    priority_queue<E,vector<E>,Cmp> pq(Cmp(),std::move(init));
    long long spent=0;
    vector<int> bbList,routers;
    vector<int> q;q.reserve(N);
    while(!pq.empty()){
        E e=pq.top();pq.pop();int c=e.c;
        if(ubv[c]<=0)continue;
        if(e.k!=key(c))continue;
        int a=c/W,b=c%W;
        int u=0;
        forSeg(a,b,[&](int rr,int c1,int c2){int base=rr*W;for(int x=c1;x<=c2;x++)if(isT[base+x]&&!cov[base+x])u++;});
        if(u!=ubv[c]){ubv[c]=u;if(u>0)pq.push({key(c),c});continue;}
        long long d=distv[c];
        long long cost=Pr+Pb*d;
        if(1000LL*u<=cost)break;
        if(cost>B-spent)continue;
        // accept
        spent+=cost;
        routers.push_back(c);
        forSeg(a,b,[&](int rr,int c1,int c2){int base=rr*W;for(int x=c1;x<=c2;x++)cov[base+x]=1;});
        ubv[c]=0;
        // path
        q.clear();
        int s=srcv[c];int sr=s/W,sc=s%W;
        while(sr!=a||sc!=b){
            sr+=(a>sr)-(a<sr);sc+=(b>sc)-(b<sc);
            int v=sr*W+sc;
            if(!bb[v]){bb[v]=1;bbList.push_back(v);}
            distv[v]=0;srcv[v]=v;q.push_back(v);
        }
        for(size_t h=0;h<q.size();h++){int x=q[h];int xr=x/W,xc=x%W;
            for(int dr=-1;dr<=1;dr++)for(int dc=-1;dc<=1;dc++){if(!dr&&!dc)continue;int nr=xr+dr,nc=xc+dc;if(nr<0||nr>=H||nc<0||nc>=W)continue;int v=nr*W+nc;
                if(distv[v]>distv[x]+1){distv[v]=distv[x]+1;srcv[v]=srcv[x];q.push_back(v);
                    if(ubv[v]>0)pq.push({key(v),v});}}}
    }
    string out;
    out.reserve(bbList.size()*12+routers.size()*12+32);
    out+=to_string(bbList.size());out+='\n';
    for(int v:bbList){out+=to_string(v/W);out+=' ';out+=to_string(v%W);out+='\n';}
    out+=to_string(routers.size());out+='\n';
    for(int v:routers){out+=to_string(v/W);out+=' ';out+=to_string(v%W);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
