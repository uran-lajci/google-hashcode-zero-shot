#include <bits/stdc++.h>
using namespace std;

int H,W,R,Pb,Pr; long long B; int br,bc;
vector<string> g;
vector<int> rwv,lwv,pre;
vector<int> cnt0;

template<class F> void forRanges(int a,int b,F f){
    int limL=max(b-R,0),limR=min(b+R,W-1);
    for(int x=a;x<=min(a+R,H-1);x++){
        if(g[x][b]=='#')break;
        limR=min(limR,rwv[x*W+b]-1);limL=max(limL,lwv[x*W+b]+1);
        f(x,limL,limR);
    }
    limL=max(b-R,0);limR=min(b+R,W-1);
    for(int x=a;x>=max(a-R,0);x--){
        if(g[x][b]=='#')break;
        limR=min(limR,rwv[x*W+b]-1);limL=max(limL,lwv[x*W+b]+1);
        if(x<a)f(x,limL,limR);
    }
}

struct Result{ long long score; vector<int> bb; vector<int> routers; };

Result run(double k){
    int N=H*W;
    vector<int> dist(N),src(N,br*W+bc);
    for(int r=0;r<H;r++)for(int c=0;c<W;c++)dist[r*W+c]=max(abs(r-br),abs(c-bc));
    vector<char> covered(N,0);
    long long remaining=B; long long covCnt=0;
    vector<int> order,routers;
    priority_queue<pair<double,int>> pq;
    {
        vector<pair<double,int>> v;
        for(int i=0;i<N;i++) if(cnt0[i]>0){
            v.push_back({cnt0[i]/(Pr+k*Pb*dist[i]),-i});
        }
        pq=priority_queue<pair<double,int>>(less<pair<double,int>>(),std::move(v));
    }
    vector<int> q; q.reserve(1<<16);
    while(!pq.empty()){
        if(remaining<Pr)break;
        auto t=pq.top();pq.pop();
        int i=-t.second;
        long long cost=Pr+(long long)Pb*dist[i];
        if(cost>remaining)continue;
        int a=i/W,b=i%W;
        int nc=0;
        forRanges(a,b,[&](int x,int lo,int hi){
            const char*row=g[x].data();int base=x*W;
            for(int y=lo;y<=hi;y++) if(row[y]=='.'&&!covered[base+y])nc++;
        });
        if(nc==0)continue;
        double key=nc/(Pr+k*Pb*dist[i]);
        if(!pq.empty()&&key<pq.top().first-1e-12){pq.push({key,-i});continue;}
        // accept
        int d=dist[i];
        if(d>0){
            int s=src[i];int sr=s/W,sc=s%W;
            vector<int> path;int cr=a,cc=b;
            while(!(cr==sr&&cc==sc)){
                path.push_back(cr*W+cc);
                cr+= (sr>cr)-(sr<cr); cc+=(sc>cc)-(sc<cc);
            }
            reverse(path.begin(),path.end());
            q.clear();
            for(int p:path){order.push_back(p);dist[p]=0;src[p]=p;q.push_back(p);}
            size_t h=0;
            while(h<q.size()){
                int cur=q[h++];int r0=cur/W,c0=cur%W;int nd=dist[cur]+1;
                for(int dr=-1;dr<=1;dr++){int r1=r0+dr;if(r1<0||r1>=H)continue;
                    for(int dc=-1;dc<=1;dc++){int c1=c0+dc;if(c1<0||c1>=W)continue;
                        int nb=r1*W+c1; if(dist[nb]>nd){dist[nb]=nd;src[nb]=src[cur];q.push_back(nb);}
                    }}
            }
        }
        forRanges(a,b,[&](int x,int lo,int hi){
            const char*row=g[x].data();int base=x*W;
            for(int y=lo;y<=hi;y++) if(row[y]=='.'&&!covered[base+y]){covered[base+y]=1;covCnt++;}
        });
        routers.push_back(i);
        remaining-=cost;
    }
    Result res; res.score=1000*covCnt+remaining; res.bb=order; res.routers=routers;
    return res;
}

int main(){
    auto T0=chrono::steady_clock::now();
    if(!(cin>>H>>W>>R))return 0;
    cin>>Pb>>Pr>>B>>br>>bc;
    g.resize(H);
    for(int i=0;i<H;i++){cin>>g[i]; if((int)g[i].size()<W)g[i].resize(W,'-');}
    int N=H*W;
    rwv.assign(N,0);lwv.assign(N,0);pre.assign(H*(W+1),0);
    for(int r=0;r<H;r++){
        for(int c=W-1;c>=0;c--) rwv[r*W+c]= g[r][c]=='#'?c:(c+1<W?rwv[r*W+c+1]:W);
        for(int c=0;c<W;c++) lwv[r*W+c]= g[r][c]=='#'?c:(c>0?lwv[r*W+c-1]:-1);
        for(int c=0;c<W;c++) pre[r*(W+1)+c+1]=pre[r*(W+1)+c]+(g[r][c]=='.');
    }
    cnt0.assign(N,0);
    for(int a=0;a<H;a++)for(int b=0;b<W;b++){
        if(g[a][b]=='#')continue;
        int s=0;
        forRanges(a,b,[&](int x,int lo,int hi){ s+=pre[x*(W+1)+hi+1]-pre[x*(W+1)+lo]; });
        cnt0[a*W+b]=s;
    }
    vector<double> ks={1.0,0.6,1.6,0.3,2.5};
    Result best; best.score=-1;
    double firstDur=0;
    for(size_t t=0;t<ks.size();t++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-T0).count();
        if(t>0 && el+firstDur*1.5>8.5)break;
        auto s0=chrono::steady_clock::now();
        Result r=run(ks[t]);
        double dur=chrono::duration<double>(chrono::steady_clock::now()-s0).count();
        firstDur=max(firstDur,dur);
        if(r.score>best.score)best=std::move(r);
    }
    string out;
    out+=to_string(best.bb.size())+"\n";
    for(int p:best.bb){out+=to_string(p/W)+" "+to_string(p%W)+"\n";}
    out+=to_string(best.routers.size())+"\n";
    for(int p:best.routers){out+=to_string(p/W)+" "+to_string(p%W)+"\n";}
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
