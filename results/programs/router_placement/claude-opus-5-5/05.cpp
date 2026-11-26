#include <bits/stdc++.h>
using namespace std;

int H,W,R; long long Pb,Pr,B;
vector<char> wallv, need;
int dr8[8]={-1,-1,-1,0,0,1,1,1}, dc8[8]={-1,0,1,-1,1,-1,0,1};

static bool okq[11][11];
template<class F> void forCover(int a,int b,F f){
    for(int sx=-1;sx<=1;sx+=2)for(int sy=-1;sy<=1;sy+=2){
        for(int i=0;i<=R;i++){
            int x=a+sx*i;
            for(int j=0;j<=R;j++){
                int y=b+sy*j;
                bool ok;
                if(x<0||x>=H||y<0||y>=W) ok=false;
                else ok=!wallv[x*W+y] && (i==0||okq[i-1][j]) && (j==0||okq[i][j-1]);
                okq[i][j]=ok;
                if(ok){
                    if((i>0||sx==1)&&(j>0||sy==1)) f(x*W+y);
                }
            }
        }
    }
}
int gainOf(int c){
    int g=0; forCover(c/W,c%W,[&](int id){ if(need[id]) g++; }); return g;
}
struct E{ double key; int cell; unsigned ver; };
struct Cmp{ bool operator()(const E&a,const E&b)const{ if(a.key!=b.key) return a.key<b.key; return a.cell>b.cell; } };

int main(){
    scanf("%d %d %d",&H,&W,&R);
    scanf("%lld %lld %lld",&Pb,&Pr,&B);
    int br,bc; scanf("%d %d",&br,&bc);
    int N=H*W;
    wallv.assign(N,0); need.assign(N,0);
    {
        static char buf[2000];
        for(int r=0;r<H;r++){
            scanf("%s",buf);
            for(int c=0;c<W;c++){ char ch=buf[c]; wallv[r*W+c]=(ch=='#'); need[r*W+c]=(ch=='.'); }
        }
    }
    const int INF=INT_MAX/2;
    vector<int> dist(N,INF);
    int b0=br*W+bc; dist[b0]=0;
    {
        vector<int> q; q.reserve(N); q.push_back(b0);
        for(size_t h=0;h<q.size();h++){
            int u=q[h]; int ur=u/W, uc=u%W;
            for(int k=0;k<8;k++){ int nr=ur+dr8[k], nc=uc+dc8[k];
                if(nr<0||nr>=H||nc<0||nc>=W) continue; int v=nr*W+nc;
                if(dist[v]>dist[u]+1){ dist[v]=dist[u]+1; q.push_back(v);} }
        }
    }
    vector<int> sg(N,0); vector<unsigned> ver(N,0); vector<char> placed(N,0);
    vector<E> heapv; heapv.reserve(N);
    int nonwall=0;
    for(int c=0;c<N;c++){
        if(wallv[c]) continue; nonwall++;
        int g=gainOf(c); sg[c]=g;
        if(g>0) heapv.push_back({g/(double)(Pr+Pb*dist[c]),c,ver[c]});
    }
    Cmp cmp;
    make_heap(heapv.begin(),heapv.end(),cmp);
    auto hpush=[&](E e){ heapv.push_back(e); push_heap(heapv.begin(),heapv.end(),cmp); };
    auto hpop=[&](){ pop_heap(heapv.begin(),heapv.end(),cmp); heapv.pop_back(); };
    auto clean=[&](){ while(!heapv.empty()){ const E&t=heapv.front(); if(t.ver!=ver[t.cell]||placed[t.cell]) hpop(); else break; } };
    long long rem=B;
    vector<pair<int,int>> backbone, routers;
    vector<int> q; q.reserve(N);
    while(rem>=Pr){
        clean();
        if(heapv.empty()) break;
        E t=heapv.front(); hpop();
        int c=t.cell;
        int g=gainOf(c);
        if(g==0){ sg[c]=0; ver[c]++; continue; }
        int d=dist[c]; long long cost=Pr+Pb*(long long)d;
        double key=g/(double)cost;
        clean();
        if(!heapv.empty() && heapv.front().key>key+1e-12){
            sg[c]=g; ver[c]++; hpush({key,c,ver[c]}); continue;
        }
        if(cost>rem){ sg[c]=g; ver[c]++; continue; }
        // place
        vector<int> path; int cur=c;
        while(dist[cur]>0){
            path.push_back(cur);
            int cr=cur/W, cc=cur%W, nxt=-1;
            for(int k=0;k<8;k++){ int nr=cr+dr8[k], nc=cc+dc8[k];
                if(nr<0||nr>=H||nc<0||nc>=W) continue; int v=nr*W+nc;
                if(dist[v]==dist[cur]-1){ nxt=v; break; } }
            cur=nxt;
        }
        reverse(path.begin(),path.end());
        q.clear();
        for(int p:path){ backbone.push_back({p/W,p%W}); dist[p]=0; q.push_back(p); }
        placed[c]=1; ver[c]++;
        routers.push_back({c/W,c%W});
        rem-=cost;
        forCover(c/W,c%W,[&](int id){ need[id]=0; });
        for(size_t h=0;h<q.size();h++){
            int u=q[h]; int ur=u/W, uc=u%W;
            for(int k=0;k<8;k++){ int nr=ur+dr8[k], nc=uc+dc8[k];
                if(nr<0||nr>=H||nc<0||nc>=W) continue; int v=nr*W+nc;
                if(dist[v]>dist[u]+1){
                    dist[v]=dist[u]+1; q.push_back(v);
                    if(!wallv[v]&&!placed[v]&&sg[v]>0){
                        ver[v]++; heapv.push_back({sg[v]/(double)(Pr+Pb*dist[v]),v,ver[v]});
                        push_heap(heapv.begin(),heapv.end(),cmp);
                    }
                } }
        }
        if(heapv.size()>(size_t)3*nonwall+1000){
            vector<E> nh; nh.reserve(nonwall);
            for(int x=0;x<N;x++){
                if(wallv[x]||placed[x]||sg[x]<=0) continue;
                ver[x]++; nh.push_back({sg[x]/(double)(Pr+Pb*dist[x]),x,ver[x]});
            }
            heapv.swap(nh);
            make_heap(heapv.begin(),heapv.end(),cmp);
        }
    }
    string out;
    out.reserve((backbone.size()+routers.size())*12+64);
    out+=to_string(backbone.size()); out+='\n';
    for(auto&p:backbone){ out+=to_string(p.first); out+=' '; out+=to_string(p.second); out+='\n'; }
    out+=to_string(routers.size()); out+='\n';
    for(auto&p:routers){ out+=to_string(p.first); out+=' '; out+=to_string(p.second); out+='\n'; }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
