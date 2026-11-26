#include <bits/stdc++.h>
using namespace std;

int W,H,R,M,T,L;
vector<int> mx,my;
vector<int> TS;
vector<vector<int>> TP;
vector<int> regionId, isMountArr, inStack, visArr, distArr, parArr, qbuf;
int stampv=0;
long long workCnt=0;
const long long WORK_CAP=700000000LL;

static inline int manh(int a,int b){return abs(a%W-b%W)+abs(a/W-b/W);}

struct Arm{
    int k; int mount;
    vector<int> st; vector<char> mv; vector<int> tasks;
};

char dirOf(int a,int b){
    int ax=a%W,ay=a/W,bx=b%W,by=b/W;
    if(bx==ax+1) return 'R';
    if(bx==ax-1) return 'L';
    if(by==ay+1) return 'U';
    return 'D';
}
void pushCell(Arm&A,int c){
    A.mv.push_back(dirOf(A.st.back(),c));
    A.st.push_back(c); inStack[c]=(int)A.st.size();
}
void popCell(Arm&A){
    int c=A.st.back(); A.st.pop_back(); inStack[c]=0;
    A.mv.push_back(dirOf(c,A.st.back()));
}

bool moveTo(Arm&A,int target){
    int limit=L-(int)A.mv.size();
    int len=A.st.size();
    if(A.st.back()==target) return true;
    if(inStack[target]){
        int j=inStack[target]-1;
        int r=len-1-j;
        if(r>limit) return false;
        for(int i=0;i<r;i++) popCell(A);
        return true;
    }
    if(workCnt>WORK_CAP) return false;
    stampv++;
    int qh=0,qt=0;
    qbuf[qt++]=target; visArr[target]=stampv; distArr[target]=0;
    int best=INT_MAX,bj=-1,bu=-1;
    while(qh<qt){
        int u=qbuf[qh++]; workCnt++;
        int d=distArr[u];
        if(best<=d+1) break;
        if(d+1>limit) break;
        int ux=u%W,uy=u/W;
        int nb[4];int nn=0;
        if(ux+1<W) nb[nn++]=u+1;
        if(ux>0) nb[nn++]=u-1;
        if(uy+1<H) nb[nn++]=u+W;
        if(uy>0) nb[nn++]=u-W;
        for(int i=0;i<nn;i++){
            int v=nb[i];
            if(inStack[v]){
                int j=inStack[v]-1;
                int cost=(len-1-j)+d+1;
                if(cost<best){best=cost;bj=j;bu=u;}
            } else if(regionId[v]==A.k && !isMountArr[v] && visArr[v]!=stampv){
                visArr[v]=stampv; distArr[v]=d+1; parArr[v]=u; qbuf[qt++]=v;
            }
        }
    }
    if(best>limit) return false;
    int r=len-1-bj;
    for(int i=0;i<r;i++) popCell(A);
    int cur=bu; pushCell(A,cur);
    while(cur!=target){cur=parArr[cur]; pushCell(A,cur);}
    return true;
}

int main(){
    if(scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L)!=6) return 0;
    mx.resize(M);my.resize(M);
    int N=W*H;
    isMountArr.assign(N,0);
    for(int i=0;i<M;i++){scanf("%d %d",&mx[i],&my[i]); isMountArr[my[i]*W+mx[i]]=1;}
    TS.resize(T);TP.resize(T);
    vector<long long> tlen(T,0);
    for(int t=0;t<T;t++){
        int P; scanf("%d %d",&TS[t],&P);
        TP[t].resize(P);
        for(int i=0;i<P;i++){int x,y;scanf("%d %d",&x,&y);TP[t][i]=y*W+x;}
        for(int i=1;i<P;i++) tlen[t]+=manh(TP[t][i-1],TP[t][i]);
    }
    // mount selection
    vector<double> bestT(T,0.0);
    vector<char> chosenF(M,0);
    vector<int> chosen;
    vector<int> mcell(M);
    for(int i=0;i<M;i++) mcell[i]=my[i]*W+mx[i];
    for(int r=0;r<R;r++){
        double bg=0;int bm=-1;
        for(int m=0;m<M;m++){
            if(chosenF[m]) continue;
            double g=0;
            for(int t=0;t<T;t++){
                long long c=manh(mcell[m],TP[t][0])+tlen[t];
                if(c>L) continue;
                double f=(double)TS[t]/(double)(c+1);
                if(f>bestT[t]) g+=f-bestT[t];
            }
            if(g>bg){bg=g;bm=m;}
        }
        if(bm<0) break;
        chosenF[bm]=1; chosen.push_back(bm);
        for(int t=0;t<T;t++){
            long long c=manh(mcell[bm],TP[t][0])+tlen[t];
            if(c>L) continue;
            double f=(double)TS[t]/(double)(c+1);
            if(f>bestT[t]) bestT[t]=f;
        }
    }
    int K=chosen.size();
    regionId.assign(N,-1);
    inStack.assign(N,0);visArr.assign(N,0);distArr.assign(N,0);parArr.assign(N,0);qbuf.assign(N+5,0);
    {
        int qh=0,qt=0;
        for(int k=0;k<K;k++){int c=mcell[chosen[k]];regionId[c]=k;qbuf[qt++]=c;}
        while(qh<qt){
            int u=qbuf[qh++];int ux=u%W,uy=u/W;
            int nb[4];int nn=0;
            if(ux+1<W) nb[nn++]=u+1;
            if(ux>0) nb[nn++]=u-1;
            if(uy+1<H) nb[nn++]=u+W;
            if(uy>0) nb[nn++]=u-W;
            for(int i=0;i<nn;i++){int v=nb[i]; if(regionId[v]==-1&&!isMountArr[v]){regionId[v]=regionId[u];qbuf[qt++]=v;}}
        }
    }
    vector<vector<int>> rt(K);
    for(int t=0;t<T;t++){
        int r=regionId[TP[t][0]]; if(r<0) continue;
        bool ok=true;
        for(int p:TP[t]) if(regionId[p]!=r||isMountArr[p]){ok=false;break;}
        if(ok) rt[r].push_back(t);
    }
    vector<Arm> arms;
    for(int k=0;k<K;k++){
        Arm A; A.k=k; A.mount=mcell[chosen[k]];
        A.st.push_back(A.mount); inStack[A.mount]=1;
        vector<int> cand=rt[k];
        while(!cand.empty()){
            int rem=L-(int)A.mv.size();
            int top=A.st.back();
            double br=-1;int bi=-1;
            for(int i=0;i<(int)cand.size();i++){
                int t=cand[i];
                long long lb=manh(top,TP[t][0])+tlen[t];
                if(lb>rem) continue;
                double ratio=(double)TS[t]/(double)(lb+1);
                if(ratio>br){br=ratio;bi=i;}
            }
            if(bi<0) break;
            int t=cand[bi];
            cand[bi]=cand.back();cand.pop_back();
            vector<int> savedSt=A.st; size_t savedMv=A.mv.size();
            bool ok=true;
            for(int p:TP[t]){ if(!moveTo(A,p)){ok=false;break;} }
            if(ok) A.tasks.push_back(t);
            else{
                for(int c:A.st) inStack[c]=0;
                A.st=savedSt;
                for(int i=0;i<(int)A.st.size();i++) inStack[A.st[i]]=i+1;
                A.mv.resize(savedMv);
            }
            if(workCnt>WORK_CAP) break;
        }
        for(int c:A.st) inStack[c]=0;
        if(!A.tasks.empty()) arms.push_back(move(A));
    }
    string out;
    out+=to_string(arms.size())+"\n";
    for(auto&A:arms){
        out+=to_string(A.mount%W)+" "+to_string(A.mount/W)+" "+to_string(A.tasks.size())+" "+to_string(A.mv.size())+"\n";
        for(size_t i=0;i<A.tasks.size();i++){if(i)out+=' ';out+=to_string(A.tasks[i]);}
        out+='\n';
        for(size_t i=0;i<A.mv.size();i++){if(i)out+=' ';out+=A.mv[i];}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
