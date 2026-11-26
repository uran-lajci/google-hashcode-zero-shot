#include <bits/stdc++.h>
using namespace std;

int H,W,R,Pb,Pr; long long B; int br,bc;
vector<string> g;
vector<unsigned char> rrun, lrun; // non-wall runs to right/left, capped R+1
vector<int> gainv;
vector<char> covered;

template<class F> inline void forCover(int a,int b,F f){
    for(int dir=1;dir>=-1;dir-=2){
        int limL=R,limR=R;
        for(int i=0;i<=R;i++){
            int r=a+dir*i; if(r<0||r>=H) break;
            int idx=r*W+b;
            int lr=(int)rrun[idx]-1, ll=(int)lrun[idx]-1;
            if(lr<0) break;
            limR=min(limR,lr); limL=min(limL,ll);
            if(dir==-1&&i==0) continue;
            f(r,b-limL,b+limR);
        }
    }
}

struct E{double v;int cell;int ver;};
struct Cmp{bool operator()(const E&a,const E&b)const{ if(a.v!=b.v) return a.v<b.v; return a.cell>b.cell;}};

int main(){
    scanf("%d %d %d",&H,&W,&R);
    scanf("%d %d %lld",&Pb,&Pr,&B);
    scanf("%d %d",&br,&bc);
    g.resize(H);
    {
        static char buf[2000];
        for(int i=0;i<H;i++){ scanf("%s",buf); g[i]=buf; if((int)g[i].size()<W) g[i].resize(W,'-'); }
    }
    int N=H*W;
    rrun.assign(N,0); lrun.assign(N,0);
    for(int r=0;r<H;r++){
        for(int c=W-1;c>=0;c--){
            if(g[r][c]=='#') rrun[r*W+c]=0;
            else { int nx=(c+1<W)?rrun[r*W+c+1]:0; rrun[r*W+c]=(unsigned char)min(R+1,nx+1);}
        }
        for(int c=0;c<W;c++){
            if(g[r][c]=='#') lrun[r*W+c]=0;
            else { int nx=(c>0)?lrun[r*W+c-1]:0; lrun[r*W+c]=(unsigned char)min(R+1,nx+1);}
        }
    }
    vector<int> pre((size_t)H*(W+1),0);
    for(int r=0;r<H;r++) for(int c=0;c<W;c++) pre[r*(W+1)+c+1]=pre[r*(W+1)+c]+(g[r][c]=='.');
    gainv.assign(N,0); covered.assign(N,0);
    for(int a=0;a<H;a++) for(int b=0;b<W;b++){
        if(g[a][b]=='#') continue;
        int s=0;
        forCover(a,b,[&](int r,int l,int rr){ s+=pre[r*(W+1)+rr+1]-pre[r*(W+1)+l]; });
        gainv[a*W+b]=s;
    }
    vector<int> dist(N), src(N);
    int s0=br*W+bc;
    for(int r=0;r<H;r++) for(int c=0;c<W;c++){
        dist[r*W+c]=max(abs(r-br),abs(c-bc)); src[r*W+c]=s0;
    }
    vector<char> isBB(N,0); isBB[s0]=1;
    vector<int> ver(N,0);
    priority_queue<E,vector<E>,Cmp> pq;
    auto costOf=[&](int p)->long long{ return (long long)Pr+(long long)Pb*dist[p]; };
    for(int p=0;p<N;p++){
        if(gainv[p]>0) pq.push({(double)gainv[p]/costOf(p),p,0});
    }
    long long spent=0;
    vector<int> bbList, routers;
    vector<char> placed(N,0);
    vector<int> q; q.reserve(N);
    while(!pq.empty()){
        if(B-spent<Pr) break;
        E e=pq.top(); pq.pop();
        int p=e.cell;
        if(e.ver!=ver[p]||placed[p]) continue;
        int gn=gainv[p]; if(gn<=0) continue;
        long long cst=costOf(p);
        double cur=(double)gn/cst;
        if(cur<e.v*(1-1e-12)){ ver[p]++; pq.push({cur,p,ver[p]}); continue; }
        if(spent+cst>B) continue;
        if(1000LL*gn<=cst) continue;
        // accept
        spent+=cst; placed[p]=1; routers.push_back(p);
        // backbone path
        vector<int> path;
        if(!isBB[p]){
            int s=src[p]; int sr=s/W, sc=s%W; int r=p/W, c=p%W;
            while(!(r==sr&&c==sc)){
                path.push_back(r*W+c);
                if(r<sr) r++; else if(r>sr) r--;
                if(c<sc) c++; else if(c>sc) c--;
            }
            reverse(path.begin(),path.end());
            for(int x:path){ isBB[x]=1; bbList.push_back(x); }
        }
        // coverage update
        int pa=p/W, pbq=p%W;
        forCover(pa,pbq,[&](int r,int l,int rr){
            for(int c=l;c<=rr;c++){
                int id=r*W+c;
                if(g[r][c]=='.'&&!covered[id]){
                    covered[id]=1;
                    forCover(r,c,[&](int r2,int l2,int rr2){
                        int base=r2*W;
                        for(int c2=l2;c2<=rr2;c2++) gainv[base+c2]--;
                    });
                }
            }
        });
        // BFS dist update
        if(!path.empty()){
            q.clear();
            for(int x:path){ dist[x]=0; src[x]=x; q.push_back(x); if(gainv[x]>0&&!placed[x]){ver[x]++; pq.push({(double)gainv[x]/costOf(x),x,ver[x]});} }
            size_t h=0;
            while(h<q.size()){
                int u=q[h++]; int ur=u/W, uc=u%W; int nd=dist[u]+1;
                for(int dr=-1;dr<=1;dr++){ int vr=ur+dr; if(vr<0||vr>=H) continue;
                    for(int dc=-1;dc<=1;dc++){ if(!dr&&!dc) continue; int vc=uc+dc; if(vc<0||vc>=W) continue;
                        int v=vr*W+vc;
                        if(dist[v]>nd){
                            dist[v]=nd; src[v]=src[u]; q.push_back(v);
                            if(gainv[v]>0&&!placed[v]&&g[vr][vc]!='#'){ ver[v]++; pq.push({(double)gainv[v]/costOf(v),v,ver[v]}); }
                        }
                    }
                }
            }
        }
    }
    string out;
    out.reserve(32*(bbList.size()+routers.size())+64);
    out+=to_string(bbList.size()); out+='\n';
    for(int x:bbList){ out+=to_string(x/W); out+=' '; out+=to_string(x%W); out+='\n'; }
    out+=to_string(routers.size()); out+='\n';
    for(int x:routers){ out+=to_string(x/W); out+=' '; out+=to_string(x%W); out+='\n'; }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
