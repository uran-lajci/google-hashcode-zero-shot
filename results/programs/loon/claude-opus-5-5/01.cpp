#include <bits/stdc++.h>
using namespace std;

static char*buf;static size_t bufLen,bufPos=0;
static inline int readInt(){
    while(bufPos<bufLen && buf[bufPos]!='-' && (buf[bufPos]<'0'||buf[bufPos]>'9')) bufPos++;
    bool neg=false; if(bufPos<bufLen&&buf[bufPos]=='-'){neg=true;bufPos++;}
    int x=0; while(bufPos<bufLen && buf[bufPos]>='0'&&buf[bufPos]<='9'){x=x*10+(buf[bufPos]-'0');bufPos++;}
    return neg?-x:x;
}
static chrono::steady_clock::time_point T0;
static double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();}

int R,C,A,L,V,B,T,rs,cs,n;
vector<int> nxt; // A*n, value n = lost
vector<int> cellStart,cellList; // targets covered by balloon at cell
vector<int> tgtStart,tgtList;   // cells covering target
vector<int> gainv; // (T+1)*n
vector<int> covc;  // (T+1)*L
vector<vector<int>> pos; // per balloon, pos[t] t=0..T ; -1 ground, n lost
vector<vector<signed char>> mv;

void applyB(int b,int sign){
    for(int t=1;t<=T;t++){
        int c=pos[b][t]; if(c<0||c>=n) continue;
        for(int k=cellStart[c];k<cellStart[c+1];k++){
            int i=cellList[k]; int &cc=covc[(size_t)t*L+i];
            if(sign>0){
                if(cc==0){ int*g=&gainv[(size_t)t*n]; for(int q=tgtStart[i];q<tgtStart[i+1];q++) g[tgtList[q]]--; }
                cc++;
            } else {
                cc--;
                if(cc==0){ int*g=&gainv[(size_t)t*n]; for(int q=tgtStart[i];q<tgtStart[i+1];q++) g[tgtList[q]]++; }
            }
        }
    }
}

int main(){
    T0=chrono::steady_clock::now();
    {
        size_t cap=1<<20; buf=(char*)malloc(cap); bufLen=0;
        while(true){ if(bufLen==cap){cap*=2;buf=(char*)realloc(buf,cap);} size_t r=fread(buf+bufLen,1,cap-bufLen,stdin); if(r==0)break; bufLen+=r; }
    }
    R=readInt();C=readInt();A=readInt();
    L=readInt();V=readInt();B=readInt();T=readInt();
    rs=readInt();cs=readInt();
    n=R*C;
    vector<int> tr(L),tc(L);
    for(int i=0;i<L;i++){tr[i]=readInt();tc[i]=readInt();}
    nxt.assign((size_t)A*n,n);
    for(int a=0;a<A;a++)for(int r=0;r<R;r++)for(int c=0;c<C;c++){
        int dr=readInt(),dc=readInt();
        int r2=r+dr;
        if(r2>=0&&r2<R){ int c2=((c+dc)%C+C)%C; nxt[(size_t)a*n+r*C+c]=r2*C+c2; }
    }
    // disc lists
    tgtStart.assign(L+1,0);
    for(int i=0;i<L;i++){
        tgtStart[i]=tgtList.size();
        for(int dr=-V;dr<=V;dr++){
            int r=tr[i]+dr; if(r<0||r>=R) continue;
            long long rem=(long long)V*V-(long long)dr*dr;
            long long m=(long long)sqrt((double)rem); while((m+1)*(m+1)<=rem)m++; while(m*m>rem)m--;
            if(m>=C/2){ for(int c=0;c<C;c++) tgtList.push_back(r*C+c); }
            else for(int dc=-(int)m;dc<=(int)m;dc++){ int c=((tc[i]+dc)%C+C)%C; tgtList.push_back(r*C+c);}
        }
    }
    tgtStart[L]=tgtList.size();
    cellStart.assign(n+1,0);
    for(size_t k=0;k<tgtList.size();k++) cellStart[tgtList[k]+1]++;
    for(int c=0;c<n;c++) cellStart[c+1]+=cellStart[c];
    cellList.resize(tgtList.size());
    {
        vector<int> fillp(cellStart.begin(),cellStart.end()-1);
        for(int i=0;i<L;i++) for(int q=tgtStart[i];q<tgtStart[i+1];q++) cellList[fillp[tgtList[q]]++]=i;
    }
    int s=rs*C+cs;
    mv.assign(B,vector<signed char>(T,0));
    const double BUDGET=9.2;
    bool useDP = ((double)T*n <= 2.5e8) && ((double)T*A*n <= 1.5e9);
    if(useDP){
        pos.assign(B,vector<int>(T+1,-1));
        gainv.assign((size_t)(T+1)*n,0);
        for(int t=1;t<=T;t++) for(int c=0;c<n;c++) gainv[(size_t)t*n+c]=cellStart[c+1]-cellStart[c];
        covc.assign((size_t)(T+1)*L,0);
        size_t N1=n+1;
        vector<int> Fa((size_t)A*N1,0),W((size_t)A*N1,0),Gm((size_t)A*n,0);
        vector<uint8_t> dec((size_t)T*A*n);
        vector<uint8_t> gdec(T+1);
        double maxDur=0;
        bool stop=false;
        for(int pass=0;!stop;pass++){
            for(int b=0;b<B;b++){
                if(elapsed()+maxDur*1.15>BUDGET){stop=true;break;}
                double st=elapsed();
                if(pass>0) applyB(b,-1);
                // DP
                fill(Fa.begin(),Fa.end(),0);
                long long Gr=0;
                for(int t=T-1;t>=0;t--){
                    const int* g=&gainv[(size_t)(t+1)*n];
                    for(int a=0;a<A;a++){
                        int* w=&W[a*N1]; const int* f=&Fa[a*N1];
                        for(int c=0;c<n;c++) w[c]=g[c]+f[c];
                        w[n]=0;
                    }
                    long long lv=W[nxt[s]];
                    if(lv>Gr){Gr=lv;gdec[t]=1;} else gdec[t]=0;
                    if(t==0)break;
                    for(int a=0;a<A;a++){
                        int* gm=&Gm[(size_t)a*n]; const int* w=&W[a*N1]; const int* nx=&nxt[(size_t)a*n];
                        for(int c=0;c<n;c++) gm[c]=w[nx[c]];
                    }
                    for(int a=0;a<A;a++){
                        int* f=&Fa[a*N1]; uint8_t* d=&dec[((size_t)t*A+a)*n];
                        const int* g0=&Gm[(size_t)a*n];
                        for(int c=0;c<n;c++){f[c]=g0[c];d[c]=1;}
                        if(a>0){const int* gl=&Gm[(size_t)(a-1)*n];
                            for(int c=0;c<n;c++){ bool bt=gl[c]>f[c]; f[c]=bt?gl[c]:f[c]; d[c]=bt?0:d[c]; }}
                        if(a<A-1){const int* gh=&Gm[(size_t)(a+1)*n];
                            for(int c=0;c<n;c++){ bool bt=gh[c]>f[c]; f[c]=bt?gh[c]:f[c]; d[c]=bt?2:d[c]; }}
                        f[n]=0;
                    }
                }
                // reconstruct
                auto &P=pos[b]; auto &M=mv[b];
                P[0]=-1;
                int t=0; int alt=-1; int cell=-1;
                while(t<T){
                    if(alt<0){
                        if(gdec[t]){ alt=0; cell=nxt[s]; M[t]=1; }
                        else M[t]=0;
                    } else {
                        if(cell==n){M[t]=0;}
                        else{
                            int d=dec[((size_t)t*A+alt)*n+cell];
                            alt+=d-1; M[t]=d-1; cell=nxt[(size_t)alt*n+cell];
                        }
                    }
                    t++;
                    P[t]=(alt<0)?-1:cell;
                }
                applyB(b,+1);
                maxDur=max(maxDur,elapsed()-st);
            }
        }
    } else {
        // fallback greedy
        vector<int> alt(B,-1),cell(B,-1);
        vector<char> cov(L,0);
        bool stop=false;
        for(int t=0;t<T;t++){
            fill(cov.begin(),cov.end(),0);
            if(!stop && elapsed()>BUDGET) stop=true;
            for(int b=0;b<B;b++){
                if(stop){mv[b][t]=0; if(alt[b]>=0&&cell[b]!=n) cell[b]=nxt[(size_t)alt[b]*n+cell[b]]; continue;}
                if(alt[b]<0){
                    if(b==t){ alt[b]=0; cell[b]=nxt[s]; mv[b][t]=1; }
                    else { mv[b][t]=0; continue; }
                } else if(cell[b]==n){ mv[b][t]=0; continue; }
                else {
                    int bestd=0; long long bestv=LLONG_MIN;
                    for(int d=-1;d<=1;d++){
                        int a2=alt[b]+d; if(a2<0||a2>=A) continue;
                        int c2=nxt[(size_t)a2*n+cell[b]];
                        long long v;
                        if(c2==n) v=-1;
                        else { v=0; for(int k=cellStart[c2];k<cellStart[c2+1];k++) if(!cov[cellList[k]]) v++; }
                        if(v>bestv){bestv=v;bestd=d;}
                    }
                    alt[b]+=bestd; mv[b][t]=bestd; cell[b]=nxt[(size_t)alt[b]*n+cell[b]];
                }
                if(cell[b]!=n) for(int k=cellStart[cell[b]];k<cellStart[cell[b]+1];k++) cov[cellList[k]]=1;
            }
        }
    }
    string out; out.reserve((size_t)T*B*3+T);
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            if(b) out.push_back(' ');
            int m=mv[b][t];
            if(m<0){out.push_back('-');out.push_back('1');}
            else out.push_back('0'+m);
        }
        out.push_back('\n');
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
