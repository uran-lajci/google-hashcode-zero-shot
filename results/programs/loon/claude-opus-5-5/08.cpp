#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline int readInt(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return 0; c=gc(); } bool neg=false; if(c=='-'){neg=true;c=gc();} int x=0; while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();} return neg?-x:x; }

int R,C,A,L,V,B,T,rs,cs;
int RC,S,N;

int main(){
    auto startTime=chrono::steady_clock::now();
    auto elapsed=[&](){return chrono::duration<double>(chrono::steady_clock::now()-startTime).count();};
    R=readInt();C=readInt();A=readInt();
    L=readInt();V=readInt();B=readInt();T=readInt();
    rs=readInt();cs=readInt();
    vector<int> tr_(L),tc_(L);
    for(int i=0;i<L;i++){tr_[i]=readInt();tc_[i]=readInt();}
    RC=R*C; S=RC; N=RC+1;
    auto printZeros=[&](){
        string line;
        for(int b=0;b<B;b++){ if(b) line+=' '; line+='0'; }
        line+='\n';
        string out; out.reserve((size_t)line.size()*T);
        for(int t=0;t<T;t++) out+=line;
        fwrite(out.data(),1,out.size(),stdout);
    };
    double mem=(double)T*A*N + (double)T*N*4 + (double)A*N*4 + (double)(A+2)*N*4*3 + (double)T*L*2;
    if(mem>3.2e9){ printZeros(); return 0; }
    vector<int> trans((size_t)A*N);
    for(int a=0;a<A;a++){
        for(int r=0;r<R;r++)for(int c=0;c<C;c++){
            int dr=readInt(),dc=readInt();
            int nr=r+dr;
            int d;
            if(nr<0||nr>=R) d=S;
            else { int nc=((c+dc)%C+C)%C; d=nr*C+nc; }
            trans[(size_t)a*N+r*C+c]=d;
        }
        trans[(size_t)a*N+S]=S;
    }
    // target -> covering cells
    vector<int> tStart(L+1,0); vector<int> tCells;
    for(int l=0;l<L;l++){
        tStart[l]=tCells.size();
        int r=tr_[l],c=tc_[l];
        for(int dr=-V;dr<=V;dr++){
            int u=r+dr; if(u<0||u>=R) continue;
            int rem=V*V-dr*dr; int m=(int)floor(sqrt((double)rem)); while((m+1)*(m+1)<=rem)m++; while(m*m>rem)m--;
            if(2*m+1>=C){ for(int v=0;v<C;v++) tCells.push_back(u*C+v); }
            else for(int dc=-m;dc<=m;dc++){ int v=((c+dc)%C+C)%C; tCells.push_back(u*C+v); }
        }
    }
    tStart[L]=tCells.size();
    // cell -> targets
    vector<int> cStart(N+1,0);
    for(int x:tCells) cStart[x+1]++;
    for(int i=0;i<N;i++) cStart[i+1]+=cStart[i];
    vector<int> cTargets(tCells.size());
    { vector<int> fill(cStart.begin(),cStart.end()-1);
      for(int l=0;l<L;l++) for(int k=tStart[l];k<tStart[l+1];k++) cTargets[fill[tCells[k]]++]=l; }
    vector<int> gain((size_t)T*N);
    for(int t=0;t<T;t++){ int* g=&gain[(size_t)t*N]; for(int i=0;i<RC;i++) g[i]=cStart[i+1]-cStart[i]; g[S]=0; }
    vector<uint16_t> cnt((size_t)T*L,0);
    vector<vector<int>> pos(B, vector<int>(T,S));
    vector<vector<int8_t>> adj(B, vector<int8_t>(T,0));
    vector<int8_t> choice((size_t)T*A*N);
    vector<char> launch(T);
    const int NEG=INT_MIN/4;
    vector<int> W((size_t)(A+2)*N,NEG), Vn((size_t)A*N), Vc((size_t)A*N);

    auto cover=[&](int b,int sign){
        for(int t=0;t<T;t++){
            int p=pos[b][t]; if(p==S) continue;
            uint16_t* ct=&cnt[(size_t)t*L]; int* g=&gain[(size_t)t*N];
            for(int k=cStart[p];k<cStart[p+1];k++){
                int l=cTargets[k];
                if(sign>0){ if(ct[l]++==0) for(int q=tStart[l];q<tStart[l+1];q++) g[tCells[q]]--; }
                else { if(--ct[l]==0) for(int q=tStart[l];q<tStart[l+1];q++) g[tCells[q]]++; }
            }
        }
    };
    int startCell=rs*C+cs;
    auto solve=[&](int b){
        fill(Vn.begin(),Vn.end(),0);
        int Gn=0;
        for(int t=T-1;t>=0;t--){
            const int* g=&gain[(size_t)t*N];
            for(int a=0;a<A;a++){
                const int* tr=&trans[(size_t)a*N]; const int* vn=&Vn[(size_t)a*N]; int* w=&W[(size_t)(a+1)*N];
                for(int i=0;i<N;i++){ int d=tr[i]; w[i]=g[d]+vn[d]; }
            }
            int8_t* ch=&choice[(size_t)t*A*N];
            for(int a=0;a<A;a++){
                const int* w0=&W[(size_t)(a+1)*N]; const int* wl=&W[(size_t)a*N]; const int* wh=&W[(size_t)(a+2)*N];
                int* vc=&Vc[(size_t)a*N]; int8_t* c=ch+(size_t)a*N;
                for(int i=0;i<N;i++){
                    int best=w0[i]; int8_t cc=0;
                    if(wl[i]>best){best=wl[i];cc=-1;}
                    if(wh[i]>best){best=wh[i];cc=1;}
                    vc[i]=best; c[i]=cc;
                }
            }
            int wlv=W[(size_t)1*N+startCell];
            launch[t]= wlv>Gn;
            Gn=max(Gn,wlv);
            swap(Vn,Vc);
        }
        bool grounded=true; int alt=0,p=startCell;
        for(int t=0;t<T;t++){
            if(grounded){
                if(launch[t]){ grounded=false; alt=1; adj[b][t]=1; p=trans[startCell]; pos[b][t]=p; }
                else { adj[b][t]=0; pos[b][t]=S; }
            } else {
                int c=choice[(size_t)t*A*N+(size_t)(alt-1)*N+p];
                alt+=c; adj[b][t]=(int8_t)c; p=trans[(size_t)(alt-1)*N+p]; pos[b][t]=p;
            }
        }
        cover(b,1);
    };
    double unitsPer=(double)T*A*N;
    double budget=4.0e9, used=0;
    for(int b=0;b<B;b++){
        if(used+unitsPer>budget) break;
        if(elapsed()>9.3) break;
        solve(b); used+=unitsPer;
    }
    // improvement rounds
    if(used+unitsPer<=budget){
        int b=0; int rounds=0;
        while(used+unitsPer<=budget && rounds<20*B && elapsed()<9.0){
            cover(b,-1);
            solve(b); used+=unitsPer;
            b=(b+1)%B; rounds++;
        }
    }
    string out; out.reserve((size_t)T*B*3);
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            if(b) out+=' ';
            int v=adj[b][t];
            if(v<0) out+="-1"; else if(v==1) out+='1'; else out+='0';
        }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
