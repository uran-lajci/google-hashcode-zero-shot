#include <bits/stdc++.h>
using namespace std;

int R,C,A,L,V,B,T,rs,cs;
int RC;
vector<int> dest; // [a][p]
vector<int> gainv; // [t][p]
vector<int> tid; // target id per cell
vector<pair<int,int>> offs;
vector<int> cnt; // [t][L]

static inline int cellAt(int r,int c,int dr,int dc){
    int nr=r+dr; if(nr<0||nr>=R) return -1;
    int nc=((c+dc)%C+C)%C; return nr*C+nc;
}

void applyCov(const vector<int>& pos,int sign){
    for(int t=0;t<T;t++){
        int p=pos[t]; if(p<0) continue;
        int r=p/C,c=p%C;
        int* g=&gainv[(size_t)t*RC];
        for(auto&o:offs){
            int q=cellAt(r,c,o.first,o.second); if(q<0) continue;
            int id=tid[q]; if(id<0) continue;
            int &k=cnt[(size_t)t*L+id];
            bool ch=false;
            if(sign>0){ if(k++==0) ch=true; }
            else { if(--k==0) ch=true; }
            if(ch){
                int tr=q/C,tc=q%C;
                for(auto&o2:offs){
                    int z=cellAt(tr,tc,o2.first,o2.second); if(z<0) continue;
                    g[z]-=sign;
                }
            }
        }
    }
}

int main(){
    if(scanf("%d %d %d",&R,&C,&A)!=3) return 0;
    scanf("%d %d %d %d",&L,&V,&B,&T);
    scanf("%d %d",&rs,&cs);
    RC=R*C;
    tid.assign(RC,-1);
    for(int i=0;i<L;i++){int r,c;scanf("%d %d",&r,&c);tid[r*C+c]=i;}
    dest.assign((size_t)A*RC,-1);
    for(int a=0;a<A;a++)for(int r=0;r<R;r++)for(int c=0;c<C;c++){
        int dr,dc;scanf("%d %d",&dr,&dc);
        int nr=r+dr;
        if(nr>=0&&nr<R){int nc=((c+dc)%C+C)%C; dest[(size_t)a*RC+r*C+c]=nr*C+nc;}
    }
    vector<vector<int>> out(B, vector<int>(T,0));
    double memNeed=(double)T*RC*4+(double)T*RC*A+(double)T*L*4+(double)A*RC*16;
    auto printOut=[&](){
        string s;
        for(int t=0;t<T;t++){
            for(int b=0;b<B;b++){ if(b) s+=' '; s+=to_string(out[b][t]); }
            s+='\n';
        }
        fwrite(s.data(),1,s.size(),stdout);
    };
    if(memNeed>3.0e9){ printOut(); return 0; }
    for(int dr=-V;dr<=V;dr++){
        int rem=V*V-dr*dr; int m=(int)floor(sqrt((double)rem));
        while((m+1)*(m+1)<=rem)m++; while(m*m>rem)m--;
        if(2*m+1>=C){ for(int dc=0;dc<C;dc++) offs.push_back({dr,dc}); }
        else for(int dc=-m;dc<=m;dc++) offs.push_back({dr,dc});
    }
    // initial gain
    vector<int> g0(RC,0);
    for(int q=0;q<RC;q++) if(tid[q]>=0){
        int r=q/C,c=q%C;
        for(auto&o:offs){int z=cellAt(r,c,o.first,o.second); if(z>=0) g0[z]++;}
    }
    gainv.resize((size_t)T*RC);
    for(int t=0;t<T;t++) memcpy(&gainv[(size_t)t*RC],g0.data(),sizeof(int)*RC);
    cnt.assign((size_t)T*L,0);

    vector<uint8_t> dec((size_t)T*A*RC);
    vector<uint8_t> decG(T);
    vector<int> Vn((size_t)A*RC),Vc((size_t)A*RC),W((size_t)A*RC);
    vector<vector<int>> posAll(B, vector<int>(T,-1));
    vector<char> planned(B,0);

    double per=(double)T*RC*A; if(per<1) per=1;
    double budget=5.0e9;
    long long nDP=(long long)(budget/per);
    nDP=min<long long>(nDP,(long long)B*4);

    int sp=rs*C+cs;
    for(long long k=0;k<nDP;k++){
        int b=k%B;
        if(planned[b]) applyCov(posAll[b],-1);
        // DP
        fill(Vn.begin(),Vn.end(),0);
        int VgN=0;
        for(int t=T-1;t>=0;t--){
            const int* g=&gainv[(size_t)t*RC];
            for(int a=0;a<A;a++){
                const int* d=&dest[(size_t)a*RC];
                const int* vn=&Vn[(size_t)a*RC];
                int* w=&W[(size_t)a*RC];
                for(int p=0;p<RC;p++){int q=d[p]; w[p]= q<0?0:g[q]+vn[q];}
            }
            uint8_t* dt=&dec[(size_t)t*A*RC];
            for(int a=0;a<A;a++){
                int* vc=&Vc[(size_t)a*RC];
                uint8_t* da=dt+(size_t)a*RC;
                const int* w0=&W[(size_t)a*RC];
                const int* wl=a>0?&W[(size_t)(a-1)*RC]:nullptr;
                const int* wh=a<A-1?&W[(size_t)(a+1)*RC]:nullptr;
                for(int p=0;p<RC;p++){
                    int bv=w0[p]; uint8_t ba=a;
                    if(wh&&wh[p]>bv){bv=wh[p];ba=a+1;}
                    if(wl&&wl[p]>bv){bv=wl[p];ba=a-1;}
                    vc[p]=bv; da[p]=ba;
                }
            }
            int lw=W[sp];
            if(lw>VgN){decG[t]=1;VgN=lw;} else decG[t]=0;
            swap(Vn,Vc);
        }
        // reconstruct
        vector<int>& o=out[b]; vector<int>& ps=posAll[b];
        bool launched=false,alive=true; int a=0,p=sp;
        for(int t=0;t<T;t++){
            ps[t]=-1; o[t]=0;
            if(!launched){
                if(decG[t]){
                    o[t]=1; launched=true; a=0; p=dest[sp];
                    if(p<0) alive=false; else ps[t]=p;
                }
            } else if(alive){
                int a2=dec[(size_t)t*A*RC+(size_t)a*RC+p];
                o[t]=a2-a; a=a2; p=dest[(size_t)a*RC+p];
                if(p<0) alive=false; else ps[t]=p;
            }
        }
        applyCov(ps,+1);
        planned[b]=1;
    }
    printOut();
    return 0;
}
