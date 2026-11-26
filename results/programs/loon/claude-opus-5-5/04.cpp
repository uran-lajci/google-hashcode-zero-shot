#include <bits/stdc++.h>
using namespace std;

static char buf[1<<25];
int bufLen=0,bufPos=0;
inline int gc(){ if(bufPos==bufLen){ bufLen=fread(buf,1,sizeof(buf),stdin); bufPos=0; if(bufLen<=0) return -1;} return buf[bufPos++]; }
inline int readInt(){ int c=gc(); while(c!='-'&&(c<'0'||c>'9')){ if(c==-1) return 0; c=gc(); } int s=1; if(c=='-'){s=-1;c=gc();} int x=0; while(c>='0'&&c<='9'){x=x*10+(c-'0');c=gc();} return x*s; }

int R,C,A,L,V,B,T,rs,cs,N;
vector<int> mv; // A*N
vector<int> g;  // T*N
vector<int> cov; // T*L
vector<vector<int>> targetCells, cellTargets;
vector<int8_t> choice;
vector<char> launchv;
vector<int> Fn,Fc,H;

void addPath(const vector<int>& pos,int sign){
    for(int t=0;t<T;t++){
        int p=pos[t]; if(p<0) continue;
        for(int j:cellTargets[p]){
            int &cv=cov[(size_t)t*L+j];
            if(sign>0){ if(cv++==0){ int* gt=&g[(size_t)t*N]; for(int q:targetCells[j]) gt[q]--; } }
            else { if(--cv==0){ int* gt=&g[(size_t)t*N]; for(int q:targetCells[j]) gt[q]++; } }
        }
    }
}

long long dp(vector<int>& adj, vector<int>& pos){
    size_t AN=(size_t)A*N;
    fill(Fn.begin(),Fn.end(),0);
    long long G=0;
    for(int t=T-1;t>=0;t--){
        const int* gt=&g[(size_t)t*N];
        for(size_t i=0;i<AN;i++){
            int m=mv[i];
            if(m>=0){ size_t base=i-(i%N); H[i]=gt[m]+Fn[base+m]; } else H[i]=0;
        }
        int8_t* ch=&choice[(size_t)t*AN];
        for(int a=0;a<A;a++){
            const int* h0=&H[(size_t)a*N];
            const int* hm=a>0?&H[(size_t)(a-1)*N]:nullptr;
            const int* hp=a<A-1?&H[(size_t)(a+1)*N]:nullptr;
            int* fc=&Fc[(size_t)a*N]; int8_t* cc=&ch[(size_t)a*N];
            for(int c=0;c<N;c++){
                int best=h0[c]; int8_t k=0;
                if(hm&&hm[c]>best){best=hm[c];k=-1;}
                if(hp&&hp[c]>best){best=hp[c];k=1;}
                fc[c]=best; cc[c]=k;
            }
        }
        long long lv=H[(size_t)0*N+(size_t)rs*C+cs];
        if(lv>G){launchv[t]=1;G=lv;} else launchv[t]=0;
        swap(Fn,Fc);
    }
    adj.assign(T,0); pos.assign(T,-1);
    bool ground=true,lost=false; int a=-1; int cell=rs*C+cs;
    for(int t=0;t<T;t++){
        if(ground){
            if(launchv[t]){ ground=false; a=0; adj[t]=1; cell=mv[(size_t)0*N+cell]; if(cell<0) lost=true; pos[t]=cell; }
            else { adj[t]=0; pos[t]=-1; }
        } else if(lost){ adj[t]=0; pos[t]=-1; }
        else {
            int k=choice[((size_t)t*A+a)*N+cell]; a+=k; adj[t]=k;
            cell=mv[(size_t)a*N+cell]; if(cell<0) lost=true; pos[t]=cell;
        }
    }
    return G;
}

int main(){
    R=readInt();C=readInt();A=readInt();
    L=readInt();V=readInt();B=readInt();T=readInt();
    rs=readInt();cs=readInt();
    N=R*C;
    vector<int> tr(L),tc(L);
    for(int i=0;i<L;i++){tr[i]=readInt();tc[i]=readInt();}
    mv.assign((size_t)A*N,-1);
    for(int a=0;a<A;a++) for(int r=0;r<R;r++) for(int c=0;c<C;c++){
        int dr=readInt(),dc=readInt();
        int nr=r+dr;
        if(nr>=0&&nr<R){ int nc=((c+dc)%C+C)%C; mv[(size_t)a*N+r*C+c]=nr*C+nc; }
    }
    vector<vector<int>> adjAll(B,vector<int>(T,0));
    double stateCount=(double)T*A*N;
    if(stateCount>1.2e9 || (double)T*N>3e8 || (double)T*L>3e8){
        // fallback: all zeros
        string out;
        for(int t=0;t<T;t++){ for(int b=0;b<B;b++){ if(b) out+=' '; out+='0'; } out+='\n'; }
        fwrite(out.data(),1,out.size(),stdout); return 0;
    }
    targetCells.assign(L,{}); cellTargets.assign(N,{});
    for(int j=0;j<L;j++){
        int r=tr[j],c=tc[j];
        for(int dr=-V;dr<=V;dr++){
            int u=r+dr; if(u<0||u>=R) continue;
            long long rem=(long long)V*V-(long long)dr*dr;
            if(2*V+1>=C){
                for(int v=0;v<C;v++){ int d=abs(c-v); d=min(d,C-d); if((long long)d*d<=rem) targetCells[j].push_back(u*C+v); }
            } else {
                for(int dc=-V;dc<=V;dc++){ if((long long)dc*dc<=rem){ int v=((c+dc)%C+C)%C; targetCells[j].push_back(u*C+v);} }
            }
        }
        for(int q:targetCells[j]) cellTargets[q].push_back(j);
    }
    g.assign((size_t)T*N,0);
    for(int t=0;t<T;t++){ int* gt=&g[(size_t)t*N]; for(int q=0;q<N;q++) gt[q]=cellTargets[q].size(); }
    cov.assign((size_t)T*L,0);
    choice.assign((size_t)T*A*N,0);
    launchv.assign(T,0);
    Fn.assign((size_t)A*N,0);Fc.assign((size_t)A*N,0);H.assign((size_t)A*N,0);

    double budget=4.5e9, used=0;
    vector<vector<int>> posAll(B,vector<int>(T,-1));
    vector<long long> val(B,0);
    int placed=0;
    for(int b=0;b<B;b++){
        if(used+stateCount>budget) break;
        used+=stateCount;
        val[b]=dp(adjAll[b],posAll[b]);
        addPath(posAll[b],1);
        placed++;
    }
    if(placed==B && B>1){
        int noImp=0; int b=0;
        while(used+stateCount<=budget && noImp<B){
            used+=stateCount;
            addPath(posAll[b],-1);
            vector<int> na,np;
            long long nv=dp(na,np);
            // current contribution of old path (its marginal gain) equals evaluating old path; dp is optimal so nv>=old
            long long oldv=0;
            for(int t=0;t<T;t++) if(posAll[b][t]>=0) oldv+=g[(size_t)t*N+posAll[b][t]];
            if(nv>oldv){ adjAll[b]=na; posAll[b]=np; noImp=0; } else noImp++;
            addPath(posAll[b],1);
            b=(b+1)%B;
        }
    }
    string out; out.reserve((size_t)T*B*3+T);
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){ if(b) out+=' '; out+=to_string(adjAll[b][t]); }
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
