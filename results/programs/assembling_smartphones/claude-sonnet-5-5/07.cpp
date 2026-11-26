#include <bits/stdc++.h>
using namespace std;

int W,H,R,M,T,L;
vector<char> isMount;
vector<int> lastBusy, curLast, bodyIdx, vis, distv, par;
vector<int> body, undoLog, touched;
string instr;
int stampv=0;
long long workCnt=0;
const long long WORKCAP=250000000LL;
const int INF=1000000000;

char dirc(int a,int b){
    int d=b-a;
    if(d==W) return 'U';
    if(d==-W) return 'D';
    if(d==1) return 'R';
    return 'L';
}
void popBody(){
    int s=instr.size();
    int top=body.back();
    int nb=body[body.size()-2];
    instr.push_back(dirc(top,nb));
    undoLog.push_back(top);
    curLast[top]=max(curLast[top],s);
    touched.push_back(top);
    bodyIdx[top]=-1;
    body.pop_back();
}
void pushBody(int c){
    instr.push_back(dirc(body.back(),c));
    undoLog.push_back(-1);
    bodyIdx[c]=body.size();
    body.push_back(c);
}
vector<int> q;
bool goTo(int target){
    int t=instr.size();
    int len=body.size();
    stampv++;
    q.clear();
    size_t qh=0;
    int sp=len-1;
    int lim=L-t;
    int found=-1;
    while(true){
        int u,d;
        bool useQ;
        if(qh<q.size()){
            int dq=distv[q[qh]];
            if(sp>=0 && (len-1-sp)<dq) useQ=false; else useQ=true;
        } else if(sp>=0) useQ=false; else break;
        if(useQ){u=q[qh++];d=distv[u];}
        else {u=body[sp];d=len-1-sp;sp--;}
        if(d>lim) break;
        if(u==target){found=u;break;}
        if(++workCnt>WORKCAP) return false;
        int x=u%W,y=u/W;
        int nbs[4];int nc=0;
        if(x+1<W) nbs[nc++]=u+1;
        if(x>0) nbs[nc++]=u-1;
        if(y+1<H) nbs[nc++]=u+W;
        if(y>0) nbs[nc++]=u-W;
        for(int i=0;i<nc;i++){
            int v=nbs[i];
            if(isMount[v]||bodyIdx[v]>=0||vis[v]==stampv) continue;
            if(lastBusy[v]>=t-1) continue;
            vis[v]=stampv;distv[v]=d+1;par[v]=u;
            q.push_back(v);
        }
    }
    if(found<0) return false;
    vector<int> chain;
    int c=target;
    while(bodyIdx[c]<0){chain.push_back(c);c=par[c];}
    int j=bodyIdx[c];
    while((int)body.size()-1>j) popBody();
    for(int i=(int)chain.size()-1;i>=0;i--) pushBody(chain[i]);
    return true;
}
void rollback(size_t chk){
    while(instr.size()>chk){
        int u=undoLog.back();undoLog.pop_back();
        instr.pop_back();
        if(u>=0){
            bodyIdx[u]=body.size();
            body.push_back(u);
        } else {
            bodyIdx[body.back()]=-1;
            body.pop_back();
        }
    }
}
int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    int N=W*H;
    isMount.assign(N,0);
    vector<int> mounts(M);
    for(int i=0;i<M;i++){int x,y;scanf("%d %d",&x,&y);mounts[i]=y*W+x;isMount[mounts[i]]=1;}
    vector<int> sc(T);
    vector<vector<int>> pts(T);
    vector<long long> tl(T,0);
    for(int i=0;i<T;i++){
        int p;scanf("%d %d",&sc[i],&p);
        pts[i].resize(p);
        for(int k=0;k<p;k++){int x,y;scanf("%d %d",&x,&y);pts[i][k]=y*W+x;}
        for(int k=1;k<p;k++){
            int a=pts[i][k-1],b=pts[i][k];
            tl[i]+=abs(a%W-b%W)+abs(a/W-b/W);
        }
    }
    lastBusy.assign(N,-5);curLast.assign(N,-5);bodyIdx.assign(N,-1);
    vis.assign(N,0);distv.assign(N,0);par.assign(N,0);
    vector<char> assigned(T,0), usedMount(M,0);
    struct Res{int m;vector<int> tasks;string ins;};
    vector<Res> results;
    for(int r=0;r<R;r++){
        int bm=-1;double bv=0;
        for(int m=0;m<M;m++){
            if(usedMount[m]) continue;
            int mx=mounts[m]%W,my=mounts[m]/W;
            double best=0;
            for(int i=0;i<T;i++){
                if(assigned[i]) continue;
                int f=pts[i][0];
                long long c=abs(f%W-mx)+abs(f/W-my)+tl[i];
                if(c>L) continue;
                double v=(double)sc[i]/(c+1);
                if(v>best) best=v;
            }
            if(best>bv){bv=best;bm=m;}
        }
        if(bm<0) break;
        usedMount[bm]=1;
        int mc=mounts[bm];
        body.clear();body.push_back(mc);bodyIdx[mc]=0;
        instr.clear();undoLog.clear();touched.clear();
        vector<int> done;
        vector<char> failed(T,0);
        while(true){
            int g=body.back();
            int t=instr.size();
            int bi=-1;double bvv=0;
            for(int i=0;i<T;i++){
                if(assigned[i]||failed[i]) continue;
                int f=pts[i][0];
                long long c=abs(f%W-g%W)+abs(f/W-g/W)+tl[i];
                if(c>L-t) continue;
                double v=(double)sc[i]/(c+1);
                if(v>bvv){bvv=v;bi=i;}
            }
            if(bi<0) break;
            size_t chk=instr.size();
            bool ok=true;
            for(int p:pts[bi]){ if(!goTo(p)){ok=false;break;} }
            if(ok){assigned[bi]=1;done.push_back(bi);}
            else {rollback(chk);failed[bi]=1;}
        }
        for(int c:body){curLast[c]=INF;touched.push_back(c);}
        for(int c:touched){
            lastBusy[c]=max(lastBusy[c],curLast[c]);
            curLast[c]=-5;
        }
        for(int c:body) bodyIdx[c]=-1;
        if(!done.empty()){
            results.push_back({mc,done,instr});
        }
    }
    if(results.empty()){
        printf("1\n%d %d 1 1\n0\nW\n",mounts[0]%W,mounts[0]/W);
        return 0;
    }
    string out;
    out+=to_string(results.size())+"\n";
    for(auto&rs:results){
        out+=to_string(rs.m%W)+" "+to_string(rs.m/W)+" "+to_string(rs.tasks.size())+" "+to_string(rs.ins.size())+"\n";
        for(size_t i=0;i<rs.tasks.size();i++){
            if(i) out+=' ';
            out+=to_string(rs.tasks[i]);
        }
        out+="\n";
        for(size_t i=0;i<rs.ins.size();i++){
            if(i) out+=' ';
            out+=rs.ins[i];
        }
        out+="\n";
    }
    fputs(out.c_str(),stdout);
}
