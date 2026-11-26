#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
size_t bufLen=0,bufPos=0;
inline int gc(){
    if(bufPos==bufLen){bufLen=fread(buf,1,sizeof(buf),stdin);bufPos=0;if(bufLen==0)return -1;}
    return buf[bufPos++];
}
inline long long readInt(){
    int c=gc();while(c!='-'&&(c<'0'||c>'9')){if(c==-1)return 0;c=gc();}
    bool neg=false;if(c=='-'){neg=true;c=gc();}
    long long x=0;while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();}
    return neg?-x:x;
}

int R,C,A,L,V,B,T;

template<class F> void coverCells(int r,int c,F f){
    for(int dr=-V;dr<=V;dr++){
        int rr=r+dr; if(rr<0||rr>=R) continue;
        int rem=V*V-dr*dr;
        int w=(int)floor(sqrt((double)rem));
        while((long long)(w+1)*(w+1)<=rem) w++;
        while((long long)w*w>rem) w--;
        if(2*w+1>=C){
            for(int cc=0;cc<C;cc++){
                int d=abs(cc-c); d=min(d,C-d);
                if(d<=w) f(rr*C+cc);
            }
        } else {
            for(int dc=-w;dc<=w;dc++){
                int cc=((c+dc)%C+C)%C;
                f(rr*C+cc);
            }
        }
    }
}

int main(){
    R=readInt();C=readInt();A=readInt();
    L=readInt();V=readInt();B=readInt();T=readInt();
    int rs=readInt(),cs=readInt();
    vector<int> tr(L),tc(L);
    for(int i=0;i<L;i++){tr[i]=readInt();tc[i]=readInt();}
    long long cells=(long long)R*C;
    const double BUDGET=4.5e9;
    long long perB=(long long)T*cells*A;
    bool full = (long long)A*cells<=200000000LL && (long long)(T+1)*cells*A<=400000000LL
        && (long long)T*cells<=400000000LL && (double)perB<=BUDGET;
    vector<signed char> out((size_t)T*B,0);
    int nDP=0;
    if(full){
        int N=(int)cells;
        vector<int> mv((size_t)N*A);
        for(int a=0;a<A;a++)for(int r=0;r<R;r++)for(int c=0;c<C;c++){
            int dr=readInt(),dc=readInt();
            int nr=r+dr; int id=r*C+c;
            if(nr<0||nr>=R) mv[(size_t)id*A+a]=-1;
            else mv[(size_t)id*A+a]=nr*C+(((c+dc)%C)+C)%C;
        }
        vector<int> tid(N,-1);
        for(int i=0;i<L;i++) tid[tr[i]*C+tc[i]]=i;
        vector<int> cnt(N,0);
        for(int i=0;i<L;i++) coverCells(tr[i],tc[i],[&](int x){cnt[x]++;});
        vector<int16_t> gain((size_t)T*N);
        for(int t=0;t<T;t++) for(int x=0;x<N;x++) gain[(size_t)t*N+x]=(int16_t)cnt[x];
        vector<uint8_t> covered((size_t)T*L,0);
        vector<int> F((size_t)(T+1)*N*A,0);
        vector<int> G(T+1,0);
        long long maxDP=(long long)(BUDGET/(double)perB);
        nDP=(int)min<long long>(B,maxDP);
        int start=rs*C+cs;
        vector<int> W(A);
        vector<int> pos(T);
        int planned=0;
        for(int b=0;b<nDP;b++){
            G[T]=0;
            for(int t=T-1;t>=0;t--){
                const int16_t* g=&gain[(size_t)t*N];
                const int* Fn=&F[(size_t)(t+1)*N*A];
                int* Fc=&F[(size_t)t*N*A];
                int Ws=0;
                for(int x=0;x<N;x++){
                    const int* m=&mv[(size_t)x*A];
                    for(int a=0;a<A;a++){
                        int d=m[a];
                        W[a]= d<0?0:(g[d]+Fn[(size_t)d*A+a]);
                    }
                    int* fc=Fc+(size_t)x*A;
                    for(int a=0;a<A;a++){
                        int v=W[a];
                        if(a>0&&W[a-1]>v)v=W[a-1];
                        if(a+1<A&&W[a+1]>v)v=W[a+1];
                        fc[a]=v;
                    }
                    if(x==start) Ws=W[0];
                }
                G[t]=max(G[t+1],Ws);
            }
            if(G[0]<=0) break;
            planned++;
            // reconstruct
            bool launched=false,lost=false; int cell=start,alt=0;
            for(int t=0;t<T;t++){
                pos[t]=-1;
                const int16_t* g=&gain[(size_t)t*N];
                const int* Fn=&F[(size_t)(t+1)*N*A];
                int adj=0;
                if(lost){adj=0;}
                else if(!launched){
                    int d=mv[(size_t)start*A+0];
                    int w0= d<0?0:(g[d]+Fn[(size_t)d*A+0]);
                    if(w0>G[t+1]){
                        adj=1;launched=true;alt=0;
                        if(d<0) lost=true; else {cell=d;pos[t]=d;}
                    }
                } else {
                    int best=-1,ba=alt;
                    for(int na=alt-1;na<=alt+1;na++){
                        if(na<0||na>=A) continue;
                        int d=mv[(size_t)cell*A+na];
                        int w= d<0?0:(g[d]+Fn[(size_t)d*A+na]);
                        if(w>best||(w==best&&na==alt)){best=w;ba=na;}
                    }
                    adj=ba-alt;alt=ba;
                    int d=mv[(size_t)cell*A+alt];
                    if(d<0) lost=true; else {cell=d;pos[t]=d;}
                }
                out[(size_t)t*B+b]=(signed char)adj;
            }
            // update coverage
            for(int t=0;t<T;t++){
                if(pos[t]<0) continue;
                int pr=pos[t]/C,pc=pos[t]%C;
                int16_t* g=&gain[(size_t)t*N];
                uint8_t* cv=&covered[(size_t)t*L];
                coverCells(pr,pc,[&](int x){
                    int id=tid[x];
                    if(id>=0&&!cv[id]){
                        cv[id]=1;
                        coverCells(tr[id],tc[id],[&](int y){g[y]--;});
                    }
                });
            }
        }
        nDP=planned;
    }
    // fallback for remaining balloons: launch at turn 0, stay at altitude 1
    for(int b=nDP;b<B;b++){
        for(int t=0;t<T;t++) out[(size_t)t*B+b]=(t==0)?1:0;
    }
    string s; s.reserve((size_t)T*B*3+T);
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            if(b) s.push_back(' ');
            int v=out[(size_t)t*B+b];
            if(v<0){s.push_back('-');s.push_back('1');}
            else s.push_back('0'+v);
        }
        s.push_back('\n');
    }
    fwrite(s.data(),1,s.size(),stdout);
    return 0;
}
