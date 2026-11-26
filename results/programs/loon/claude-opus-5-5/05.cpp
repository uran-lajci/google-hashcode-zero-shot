#include <bits/stdc++.h>
using namespace std;

int R,C,A,L,V,B,T,rs,cs,RC,startCell;
vector<int> nxtArr; // A*RC, index (a-1)*RC+cell
vector<int> tcStart, tcList; // cell -> targets (CSR)
vector<vector<int>> targetCells;
vector<int> gainArr; // T*RC
vector<int> cov; // T*L
vector<int8_t> choiceArr;
vector<char> launchCh;

void applyPath(const vector<int>& pos,int sgn){
    for(int t=0;t<T;t++){
        int p=pos[t]; if(p<0) continue;
        int* g=&gainArr[(size_t)t*RC];
        int* cv=&cov[(size_t)t*L];
        for(int i=tcStart[p];i<tcStart[p+1];i++){
            int k=tcList[i];
            if(sgn>0){
                if(cv[k]++==0) for(int c:targetCells[k]) g[c]--;
            } else {
                if(--cv[k]==0) for(int c:targetCells[k]) g[c]++;
            }
        }
    }
}

// returns path positions and moves
void plan(vector<int>& pos, vector<int>& moves){
    size_t ARC=(size_t)A*RC;
    vector<int> Vn(ARC,0), Vc(ARC), W(ARC);
    vector<int> V0(T+1,0);
    for(int t=T-1;t>=0;t--){
        const int* g=&gainArr[(size_t)t*RC];
        for(int a=0;a<A;a++){
            const int* nx=&nxtArr[(size_t)a*RC];
            const int* vn=&Vn[(size_t)a*RC];
            int* w=&W[(size_t)a*RC];
            for(int c=0;c<RC;c++){
                int n=nx[c];
                w[c]= n>=0 ? g[n]+vn[n] : 0;
            }
        }
        int8_t* ch=&choiceArr[(size_t)t*ARC];
        for(int a=0;a<A;a++){
            int* vc=&Vc[(size_t)a*RC];
            const int* w0=&W[(size_t)a*RC];
            int8_t* cha=ch+(size_t)a*RC;
            const int* wm= a>0? &W[(size_t)(a-1)*RC]:nullptr;
            const int* wp= a<A-1? &W[(size_t)(a+1)*RC]:nullptr;
            for(int c=0;c<RC;c++){
                int best=w0[c]; int8_t d=0;
                if(wp && wp[c]>best){best=wp[c];d=1;}
                if(wm && wm[c]>best){best=wm[c];d=-1;}
                vc[c]=best; cha[c]=d;
            }
        }
        int lv=W[startCell];
        if(lv>V0[t+1]){V0[t]=lv;launchCh[t]=1;} else {V0[t]=V0[t+1];launchCh[t]=0;}
        swap(Vn,Vc);
    }
    pos.assign(T,-1); moves.assign(T,0);
    int alt=0,cell=startCell; bool lost=false;
    for(int t=0;t<T;t++){
        if(lost){continue;}
        int d;
        if(alt==0){ d=launchCh[t]?1:0; }
        else d=choiceArr[(size_t)t*ARC+(size_t)(alt-1)*RC+cell];
        moves[t]=d; alt+=d;
        if(alt==0){pos[t]=-1;continue;}
        int n=nxtArr[(size_t)(alt-1)*RC+cell];
        if(n<0){lost=true;pos[t]=-1;continue;}
        cell=n;pos[t]=cell;
    }
}

int main(){
    auto st=chrono::steady_clock::now();
    auto el=[&](){return chrono::duration<double>(chrono::steady_clock::now()-st).count();};
    if(scanf("%d %d %d",&R,&C,&A)!=3)return 0;
    scanf("%d %d %d %d",&L,&V,&B,&T);
    scanf("%d %d",&rs,&cs);
    RC=R*C; startCell=rs*C+cs;
    vector<int> tr(L),tcc(L);
    for(int i=0;i<L;i++)scanf("%d %d",&tr[i],&tcc[i]);
    nxtArr.assign((size_t)A*RC,-1);
    for(int a=0;a<A;a++)for(int r=0;r<R;r++)for(int c=0;c<C;c++){
        int dr,dc;scanf("%d %d",&dr,&dc);
        int nr=r+dr;
        if(nr<0||nr>=R)continue;
        int nc=((c+dc)%C+C)%C;
        nxtArr[(size_t)a*RC+r*C+c]=nr*C+nc;
    }
    double states=(double)T*A*RC;
    bool fallback = states>1.3e9 || (double)T*RC>2.5e8 || (double)T*L>3e8;
    if(fallback){
        string out;
        for(int t=0;t<T;t++){
            for(int b=0;b<B;b++){ if(b)out+=' '; out+= (t==0?"1":"0"); }
            out+='\n';
        }
        fputs(out.c_str(),stdout);return 0;
    }
    targetCells.assign(L,{});
    vector<int> cnt(RC,0);
    for(int k=0;k<L;k++){
        int r=tr[k],c=tcc[k];
        for(int u=max(0,r-V);u<=min(R-1,r+V);u++){
            int dr=u-r; int rem=V*V-dr*dr;
            int md=(int)floor(sqrt((double)rem)); while(md*md>rem)md--; while((md+1)*(md+1)<=rem)md++;
            if(2*md+1>=C){ for(int v=0;v<C;v++)targetCells[k].push_back(u*C+v);}
            else for(int d=-md;d<=md;d++){int v=((c+d)%C+C)%C;targetCells[k].push_back(u*C+v);}
        }
        for(int x:targetCells[k])cnt[x]++;
    }
    tcStart.assign(RC+1,0);
    for(int i=0;i<RC;i++)tcStart[i+1]=tcStart[i]+cnt[i];
    tcList.assign(tcStart[RC],0);
    {vector<int> fill(tcStart.begin(),tcStart.end()-1);
     for(int k=0;k<L;k++)for(int x:targetCells[k])tcList[fill[x]++]=k;}
    gainArr.assign((size_t)T*RC,0);
    for(int t=0;t<T;t++)for(int c=0;c<RC;c++)gainArr[(size_t)t*RC+c]=cnt[c];
    cov.assign((size_t)T*L,0);
    choiceArr.assign((size_t)T*A*RC,0);
    launchCh.assign(T,0);

    vector<vector<int>> P(B),M(B);
    vector<char> planned(B,0);
    const double LIM=9.2;
    double maxDur=0;
    bool stop=false;
    for(int b=0;b<B&&!stop;b++){
        double s=el();
        if(s+maxDur*1.2>LIM){stop=true;break;}
        plan(P[b],M[b]); applyPath(P[b],1); planned[b]=1;
        maxDur=max(maxDur,el()-s);
    }
    if(!stop){
        while(!stop){
            for(int b=0;b<B;b++){
                double s=el();
                if(s+maxDur*1.2>LIM){stop=true;break;}
                applyPath(P[b],-1);
                plan(P[b],M[b]); applyPath(P[b],1);
                maxDur=max(maxDur,el()-s);
            }
        }
    }
    string out;
    for(int t=0;t<T;t++){
        for(int b=0;b<B;b++){
            if(b)out+=' ';
            int d=planned[b]?M[b][t]:0;
            out+=to_string(d);
        }
        out+='\n';
    }
    fputs(out.c_str(),stdout);
}
