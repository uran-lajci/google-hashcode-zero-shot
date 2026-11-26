#include <bits/stdc++.h>
using namespace std;

static char*buf;static size_t bp=0,bl=0;
static int rd(){
    while(bp<bl&&!(buf[bp]=='-'||(buf[bp]>='0'&&buf[bp]<='9')))bp++;
    bool neg=false;if(bp<bl&&buf[bp]=='-'){neg=true;bp++;}
    int x=0;while(bp<bl&&buf[bp]>='0'&&buf[bp]<='9'){x=x*10+(buf[bp]-'0');bp++;}
    return neg?-x:x;
}

int R,C,A,L,V,B,T,rs,cs,N;
vector<int> nxt;
vector<vector<pair<int,int>>> rowT;
vector<int> wv;
vector<char> covered;
vector<int> gainArr;
bool dpMode=false;
chrono::steady_clock::time_point st;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-st).count();}

template<class F> void forTargets(int cell,F f){
    int r=cell/C,c=cell%C;
    for(int dr=-V;dr<=V;dr++){
        int rr=r+dr;if(rr<0||rr>=R)continue;
        int ww=wv[abs(dr)];
        for(auto&p:rowT[rr]){
            int cd=abs(c-p.first);cd=min(cd,C-cd);
            if(cd<=ww)f(p.second);
        }
    }
}
template<class F> void forDisk(int u,int v,F f){
    for(int dr=-V;dr<=V;dr++){
        int rr=u+dr;if(rr<0||rr>=R)continue;
        int ww=wv[abs(dr)];
        if(2*ww+1>=C){for(int c=0;c<C;c++)f(rr*C+c);}
        else for(int k=-ww;k<=ww;k++){int c=((v+k)%C+C)%C;f(rr*C+c);}
    }
}
vector<int> tr,tc;
int gainAt(int t,int cell){
    if(dpMode)return gainArr[(size_t)t*N+cell];
    int g=0;
    forTargets(cell,[&](int l){if(!covered[(size_t)t*L+l])g++;});
    return g;
}
void markPath(const vector<int>&pos){
    for(int t=0;t<T;t++){
        if(pos[t]<0)continue;
        forTargets(pos[t],[&](int l){
            size_t id=(size_t)t*L+l;
            if(!covered[id]){
                covered[id]=1;
                if(dpMode){
                    int*g=&gainArr[(size_t)t*N];
                    forDisk(tr[l],tc[l],[&](int cell){g[cell]--;});
                }
            }
        });
    }
}

vector<vector<signed char>> ans; // ans[t][b]

vector<int> Vc,W;vector<signed char> ch;
void dpBalloon(int b){
    size_t NA=(size_t)N*A;
    vector<int> gb(T+1,0);vector<char> launch(T,0);
    for(int t=T-1;t>=0;t--){
        if(t==T-1)fill(Vc.begin(),Vc.end(),0);
        const int*g=&gainArr[(size_t)t*N];
        for(int a=0;a<A;a++){int*U=&Vc[(size_t)a*N];for(int i=0;i<N;i++)U[i]+=g[i];}
        for(int a=0;a<A;a++){
            const int*nx=&nxt[(size_t)a*N];const int*U=&Vc[(size_t)a*N];int*Wa=&W[(size_t)a*N];
            for(int i=0;i<N;i++){int d=nx[i];Wa[i]=d<0?0:U[d];}
        }
        int gW=W[rs*C+cs];
        if(gW>gb[t+1]){gb[t]=gW;launch[t]=1;}else gb[t]=gb[t+1];
        if(t>=1){
            for(int a=0;a<A;a++){
                const int*W0=&W[(size_t)a*N];
                const int*Wm=a>0?&W[(size_t)(a-1)*N]:nullptr;
                const int*Wp=a<A-1?&W[(size_t)(a+1)*N]:nullptr;
                int*Vo=&Vc[(size_t)a*N];
                signed char*cp=&ch[(size_t)t*NA+(size_t)a*N];
                for(int i=0;i<N;i++){
                    int best=W0[i];signed char c=0;
                    if(Wm&&Wm[i]>best){best=Wm[i];c=-1;}
                    if(Wp&&Wp[i]>best){best=Wp[i];c=1;}
                    Vo[i]=best;cp[i]=c;
                }
            }
        }
    }
    vector<int> pos(T,-1);
    int alt=0,cell=rs*C+cs;bool lost=false;
    for(int t=0;t<T;t++){
        int da=0;
        if(lost){da=0;}
        else if(alt==0){
            if(launch[t]){da=1;alt=1;int d=nxt[cell];if(d<0)lost=true;else{cell=d;pos[t]=cell;}}
        }else{
            da=ch[(size_t)t*NA+(size_t)(alt-1)*N+cell];
            alt+=da;int d=nxt[(size_t)(alt-1)*N+cell];
            if(d<0)lost=true;else{cell=d;pos[t]=cell;}
        }
        ans[t][b]=(signed char)da;
    }
    markPath(pos);
}

void greedyBalloon(int b){
    vector<int> pos(T,-1);
    int alt=0,cell=rs*C+cs;bool lost=false;
    for(int t=0;t<T;t++){
        int da=0;
        if(lost){}
        else if(alt==0){
            da=1;alt=1;int d=nxt[cell];if(d<0)lost=true;else{cell=d;pos[t]=cell;}
        }else{
            long long bestS=LLONG_MIN;int bestDa=0;
            for(int d1=-1;d1<=1;d1++){
                int a1=alt+d1;if(a1<1||a1>A)continue;
                int c1=nxt[(size_t)(a1-1)*N+cell];
                long long s;
                if(c1<0)s=-1000000000LL;
                else{
                    s=(long long)gainAt(t,c1)*2;
                    if(t+1<T){
                        long long b2=-1000000;
                        for(int d2=-1;d2<=1;d2++){
                            int a2=a1+d2;if(a2<1||a2>A)continue;
                            int c2=nxt[(size_t)(a2-1)*N+c1];
                            long long s2=c2<0?-1000000:gainAt(t+1,c2);
                            b2=max(b2,s2);
                        }
                        s+=b2;
                    }
                }
                if(s>bestS){bestS=s;bestDa=d1;}
            }
            da=bestDa;alt+=da;int d=nxt[(size_t)(alt-1)*N+cell];
            if(d<0)lost=true;else{cell=d;pos[t]=cell;}
        }
        ans[t][b]=(signed char)da;
    }
    markPath(pos);
}

int main(){
    st=chrono::steady_clock::now();
    {
        size_t cap=1<<20;buf=(char*)malloc(cap);
        while(true){
            if(bl==cap){cap*=2;buf=(char*)realloc(buf,cap);}
            size_t r=fread(buf+bl,1,cap-bl,stdin);if(r==0)break;bl+=r;
        }
    }
    R=rd();C=rd();A=rd();L=rd();V=rd();B=rd();T=rd();rs=rd();cs=rd();
    N=R*C;
    tr.resize(L);tc.resize(L);rowT.assign(R,{});
    for(int i=0;i<L;i++){tr[i]=rd();tc[i]=rd();rowT[tr[i]].push_back({tc[i],i});}
    nxt.assign((size_t)A*N,-1);
    for(int a=0;a<A;a++)for(int r=0;r<R;r++)for(int c=0;c<C;c++){
        int dr=rd(),dc=rd();int nr=r+dr;
        if(nr<0||nr>=R)nxt[(size_t)a*N+r*C+c]=-1;
        else nxt[(size_t)a*N+r*C+c]=nr*C+((c+dc)%C+C)%C;
    }
    wv.assign(V+1,0);
    for(int d=0;d<=V;d++){int rem=V*V-d*d;int w=(int)sqrt((double)rem);while((w+1)*(w+1)<=rem)w++;while(w*w>rem)w--;wv[d]=w;}
    covered.assign((size_t)T*L,0);
    ans.assign(T,vector<signed char>(B,0));

    double NA=(double)N*A;
    if(NA<=5e7&&NA*T<=1.2e9&&(double)T*N<=1.5e8)dpMode=true;
    if(dpMode){
        vector<int> base(N,0);
        for(int l=0;l<L;l++)forDisk(tr[l],tc[l],[&](int cell){base[cell]++;});
        gainArr.resize((size_t)T*N);
        for(int t=0;t<T;t++)memcpy(&gainArr[(size_t)t*N],base.data(),sizeof(int)*N);
        Vc.assign((size_t)N*A,0);W.assign((size_t)N*A,0);
        ch.assign((size_t)T*N*A,0);
    }
    const double LIM=8.4;
    double est=dpMode?NA*T*3e-9:0;
    bool useDP=dpMode;
    for(int b=0;b<B;b++){
        double e=elapsed();
        if(useDP&&e+est*1.15<LIM){
            double t0=elapsed();
            dpBalloon(b);
            est=elapsed()-t0;
        }else{
            useDP=false;
            if(e<9.0)greedyBalloon(b);
            // else stays grounded (all zeros)
        }
    }
    string out;out.reserve((size_t)T*B*3+T);
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            if(b)out+=' ';
            out+=to_string((int)ans[t][b]);
        }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
}
