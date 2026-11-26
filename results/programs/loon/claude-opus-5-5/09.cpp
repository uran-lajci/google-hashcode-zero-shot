#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline int readInt(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return 0; c=gc(); } int s=1; if(c=='-'){s=-1;c=gc();} int x=0; while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();} return x*s; }

int R,C,A,L,V,B,T,N;
vector<int> nxt; // A*N
vector<int> g;   // T*N
vector<int> cov; // T*L
vector<int> tgt; // target cell ids
vector<int> cellStart, cellList; // cell -> targets in range
vector<vector<int>> tCells; // target -> cells in range

void cellsInRange(int r,int c,vector<int>&out){
    out.clear();
    for(int dr=-V;dr<=V;dr++){
        int u=r+dr; if(u<0||u>=R) continue;
        int rem=V*V-dr*dr; int d=(int)floor(sqrt((double)rem)); while((d+1)*(d+1)<=rem)d++; while(d*d>rem)d--;
        if(2*d+1>=C){ for(int v=0;v<C;v++) out.push_back(u*C+v); }
        else for(int dc=-d;dc<=d;dc++){ int v=((c+dc)%C+C)%C; out.push_back(u*C+v);}
    }
}

void addCov(int t,int k,int delta){
    int &cv=cov[(size_t)t*L+k];
    if(delta>0){ if(cv==0){ for(int q:tCells[k]) g[(size_t)t*N+q]--; } cv++; }
    else { cv--; if(cv==0){ for(int q:tCells[k]) g[(size_t)t*N+q]++; } }
}

void applyPath(const vector<int>&pos,int delta){
    for(int t=0;t<T;t++){ int q=pos[t]; if(q<0) continue;
        for(int i=cellStart[q];i<cellStart[q+1];i++) addCov(t,cellList[i],delta); }
}

int main(){
    auto t0=chrono::steady_clock::now();
    auto el=[&](){return chrono::duration<double>(chrono::steady_clock::now()-t0).count();};
    R=readInt();C=readInt();A=readInt();L=readInt();V=readInt();B=readInt();T=readInt();
    int rs=readInt(),cs=readInt();
    N=R*C;
    double sz=(double)T*A*N;
    if(sz>2.0e9 || (double)A*N>1.5e8 || (double)T*N>2.0e8 || (double)T*L>3e8){
        string line; for(int b=0;b<B;b++){ if(b) line+=' '; line+='0'; } line+='\n';
        for(int t=0;t<T;t++) fputs(line.c_str(),stdout);
        return 0;
    }
    tgt.resize(L);
    for(int i=0;i<L;i++){int r=readInt(),c=readInt(); tgt[i]=r*C+c;}
    nxt.assign((size_t)A*N,-1);
    for(int a=0;a<A;a++) for(int r=0;r<R;r++) for(int c=0;c<C;c++){
        int dr=readInt(),dc=readInt(); int nr=r+dr;
        if(nr<0||nr>=R) nxt[(size_t)a*N+r*C+c]=-1;
        else nxt[(size_t)a*N+r*C+c]=nr*C+(((c+dc)%C)+C)%C;
    }
    int start=rs*C+cs;
    tCells.resize(L);
    vector<int> cnt(N+1,0);
    for(int k=0;k<L;k++){ cellsInRange(tgt[k]/C,tgt[k]%C,tCells[k]); for(int q:tCells[k]) cnt[q]++; }
    cellStart.assign(N+1,0);
    for(int q=0;q<N;q++) cellStart[q+1]=cellStart[q]+cnt[q];
    cellList.resize(cellStart[N]);
    { vector<int> fill(cellStart.begin(),cellStart.end()-1);
      for(int k=0;k<L;k++) for(int q:tCells[k]) cellList[fill[q]++]=k; }
    g.resize((size_t)T*N);
    for(int t=0;t<T;t++) for(int q=0;q<N;q++) g[(size_t)t*N+q]=cnt[q];
    cov.assign((size_t)T*L,0);

    vector<vector<int>> paths(B, vector<int>(T,-1));
    vector<vector<int>> moves(B, vector<int>(T,0));
    size_t AN=(size_t)A*N;
    vector<int> Vn(AN),Vc(AN),X(AN);
    vector<int8_t> ch((size_t)T*AN);
    vector<int8_t> gch(T);
    double perBal=0; int done=0;
    bool stop=false;
    for(int pass=0;!stop;pass++){
        for(int b=0;b<B;b++){
            double now=el();
            if(done>0 && now+perBal*1.3>9.3){stop=true;break;}
            double st=now;
            applyPath(paths[b],-1);
            fill(Vn.begin(),Vn.end(),0);
            int Gn=0;
            for(int t=T-1;t>=0;t--){
                const int* gt=&g[(size_t)t*N];
                for(int a=0;a<A;a++){
                    const int* nx=&nxt[(size_t)a*N]; const int* vn=&Vn[(size_t)a*N]; int* x=&X[(size_t)a*N];
                    for(int p=0;p<N;p++){ int q=nx[p]; x[p]= q<0?0:gt[q]+vn[q]; }
                }
                int launch=X[start];
                if(launch>Gn){Gn=launch;gch[t]=1;} else gch[t]=0;
                int8_t* cht=&ch[(size_t)t*AN];
                for(int a=0;a<A;a++){
                    int* vc=&Vc[(size_t)a*N]; const int* x0=&X[(size_t)a*N]; int8_t* cc=&cht[(size_t)a*N];
                    const int* xm= a>0? &X[(size_t)(a-1)*N]:nullptr;
                    const int* xp= a<A-1? &X[(size_t)(a+1)*N]:nullptr;
                    for(int p=0;p<N;p++){
                        int v=x0[p]; int8_t c=0;
                        if(xm && xm[p]>v){v=xm[p];c=-1;}
                        if(xp && xp[p]>v){v=xp[p];c=1;}
                        vc[p]=v; cc[p]=c;
                    }
                }
                swap(Vn,Vc);
            }
            // reconstruct
            vector<int>&pos=paths[b]; vector<int>&mv=moves[b];
            int state=0; // 0 ground,1 air,2 lost
            int p=start,a=0;
            for(int t=0;t<T;t++){
                if(state==0){
                    if(gch[t]){ mv[t]=1; a=0; int q=nxt[p]; if(q<0){state=2;pos[t]=-1;} else {state=1;p=q;pos[t]=q;} }
                    else { mv[t]=0; pos[t]=-1; }
                } else if(state==1){
                    int c=ch[(size_t)t*AN+(size_t)a*N+p]; mv[t]=c; a+=c;
                    int q=nxt[(size_t)a*N+p]; if(q<0){state=2;pos[t]=-1;} else {p=q;pos[t]=q;}
                } else { mv[t]=0; pos[t]=-1; }
            }
            applyPath(pos,1);
            done++;
            double d=el()-st; perBal=max(perBal,d);
        }
        if(pass>50) break;
    }
    string out; out.reserve((size_t)T*B*3+T);
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){ if(b) out+=' '; out+=to_string(moves[b][t]); }
        out+='\n';
    }
    fputs(out.c_str(),stdout);
    return 0;
}
