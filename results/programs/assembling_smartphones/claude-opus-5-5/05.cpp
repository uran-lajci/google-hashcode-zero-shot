#include <bits/stdc++.h>
using namespace std;

int W,H,R,M,T,L,N;
vector<char> isMount;
vector<int> occ, lastFreed;
struct Task{ long long S; vector<int> px,py; vector<long long> suf; long long len; };
vector<Task> tasks;
vector<char> claimed;

struct Arm{
    int mount; vector<int> body; int task=-1, idx=0;
    deque<int> plan; int blocked=0; string ins; vector<int> completed;
    int lastAb=-1, pickCool=0, planCool=0;
};
vector<Arm> arms;

static inline int manh(int c,int x,int y){ int cx=c%W, cy=c/W; return abs(cx-x)+abs(cy-y);}

vector<int> seenSt, Gv, par, srcI, closedSt; int stampv=0;
const int PEN=6; const int CAP=200000;

bool planPath(Arm& A,int aid,int target,int mode){
    ++stampv;
    int tx=target%W, ty=target/W;
    typedef pair<int,int> PI;
    priority_queue<PI,vector<PI>,greater<PI>> pq;
    int n=A.body.size();
    for(int i=0;i<n;i++){
        int c=A.body[i]; int g=n-1-i;
        if(seenSt[c]!=stampv||g<Gv[c]){ seenSt[c]=stampv; Gv[c]=g; par[c]=-1; srcI[c]=i; pq.push({g+manh(c,tx,ty),c}); }
    }
    int nodes=0;
    static const int dx[4]={1,-1,0,0}, dy[4]={0,0,1,-1};
    while(!pq.empty()){
        auto [f,c]=pq.top(); pq.pop();
        if(closedSt[c]==stampv) continue;
        closedSt[c]=stampv;
        if(c==target){
            vector<int> path; int cur=c;
            while(par[cur]!=-1){ path.push_back(cur); cur=par[cur]; }
            int si=srcI[cur];
            A.plan.clear();
            for(int k=0;k<n-1-si;k++) A.plan.push_back(-1);
            for(int k=(int)path.size()-1;k>=0;k--) A.plan.push_back(path[k]);
            return true;
        }
        if(++nodes>CAP) return false;
        int cx=c%W, cy=c/W;
        for(int d=0;d<4;d++){
            int nx=cx+dx[d], ny=cy+dy[d];
            if(nx<0||ny<0||nx>=W||ny>=H) continue;
            int nb=ny*W+nx;
            if(isMount[nb]) continue;
            int o=occ[nb];
            if(o==aid) continue;
            int cost=1;
            if(o>=0 && nb!=target){ if(mode==0) continue; cost+=PEN; }
            int ng=Gv[c]+cost;
            if(seenSt[nb]!=stampv||ng<Gv[nb]){
                seenSt[nb]=stampv; Gv[nb]=ng; par[nb]=c; srcI[nb]=-1;
                pq.push({ng+abs(nx-tx)+abs(ny-ty),nb});
            }
        }
    }
    return false;
}

void pickTask(Arm& A,long long remain){
    int g=A.body.back(); int mnt=A.mount; long long bl=A.body.size()-1;
    double best=-1; int bt=-1;
    for(int t=0;t<T;t++){
        if(claimed[t]||t==A.lastAb) continue;
        const Task& tk=tasks[t];
        long long e1=manh(g,tk.px[0],tk.py[0]);
        long long e2=bl+manh(mnt,tk.px[0],tk.py[0]);
        long long est=min(e1,e2)+tk.len;
        if(est>remain) continue;
        double r=(double)tk.S/(est+1.0);
        if(r>best){best=r;bt=t;}
    }
    if(bt>=0){ A.task=bt; A.idx=0; claimed[bt]=1; A.plan.clear(); A.blocked=0; }
}

void progress(Arm& A,int t,long long remain){
    while(true){
        if(A.task<0){
            if(A.pickCool>t) return;
            pickTask(A,remain);
            if(A.task<0){ A.pickCool=t+10; return; }
        }
        Task& tk=tasks[A.task];
        int g=A.body.back(); int gx=g%W, gy=g/W;
        int P=tk.px.size(); int old=A.idx;
        while(A.idx<P && tk.px[A.idx]==gx && tk.py[A.idx]==gy) A.idx++;
        if(A.idx!=old) A.plan.clear();
        if(A.idx==P){ A.completed.push_back(A.task); A.task=-1; A.plan.clear(); A.blocked=0; continue; }
        return;
    }
}

char dirChar(int from,int to){
    int fx=from%W, fy=from/W, tx=to%W, ty=to/W;
    if(tx==fx+1) return 'R'; if(tx==fx-1) return 'L';
    if(ty==fy+1) return 'U'; return 'D';
}

int main(){
    auto st=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L)!=6) return 0;
    N=W*H;
    isMount.assign(N,0);
    vector<int> mounts(M);
    for(int i=0;i<M;i++){int x,y;scanf("%d %d",&x,&y);mounts[i]=y*W+x;isMount[mounts[i]]=1;}
    tasks.resize(T);
    for(int t=0;t<T;t++){
        long long S;int P;scanf("%lld %d",&S,&P);
        tasks[t].S=S; tasks[t].px.resize(P); tasks[t].py.resize(P);
        for(int k=0;k<P;k++) scanf("%d %d",&tasks[t].px[k],&tasks[t].py[k]);
        tasks[t].suf.assign(P,0);
        for(int k=P-2;k>=0;k--) tasks[t].suf[k]=tasks[t].suf[k+1]+abs(tasks[t].px[k]-tasks[t].px[k+1])+abs(tasks[t].py[k]-tasks[t].py[k+1]);
        tasks[t].len=tasks[t].suf[0];
    }
    claimed.assign(T,0);
    // mount selection
    vector<int> vm;
    for(int i=0;i<M;i++){
        int c=mounts[i]; int cx=c%W, cy=c/W; bool ok=false;
        int dx[4]={1,-1,0,0}, dy[4]={0,0,1,-1};
        for(int d=0;d<4;d++){int nx=cx+dx[d],ny=cy+dy[d]; if(nx<0||ny<0||nx>=W||ny>=H) continue; if(!isMount[ny*W+nx]) ok=true;}
        if(ok) vm.push_back(c);
    }
    int VM=vm.size();
    vector<float> val((size_t)VM*T);
    for(int i=0;i<VM;i++) for(int t=0;t<T;t++){
        long long d=manh(vm[i],tasks[t].px[0],tasks[t].py[0])+tasks[t].len;
        val[(size_t)i*T+t]= d+1<=L ? (float)((double)tasks[t].S/(d+1.0)) : 0.f;
    }
    vector<float> bestv(T,0); vector<char> sel(VM,0);
    vector<int> chosen;
    for(int r=0;r<R && r<VM;r++){
        double bg=0; int bi=-1;
        for(int i=0;i<VM;i++){ if(sel[i]) continue;
            double g=0; const float* v=&val[(size_t)i*T];
            for(int t=0;t<T;t++){ float d=v[t]-bestv[t]; if(d>0) g+=d; }
            if(g>bg){bg=g;bi=i;}
        }
        if(bi<0) break;
        sel[bi]=1; chosen.push_back(vm[bi]);
        for(int t=0;t<T;t++) bestv[t]=max(bestv[t],val[(size_t)bi*T+t]);
    }
    if(chosen.empty() && VM>0) chosen.push_back(vm[0]);
    occ.assign(N,-1); lastFreed.assign(N,-1);
    seenSt.assign(N,0); Gv.assign(N,0); par.assign(N,-1); srcI.assign(N,-1); closedSt.assign(N,0);
    int A=chosen.size();
    arms.resize(A);
    for(int a=0;a<A;a++){ arms[a].mount=chosen[a]; arms[a].body={chosen[a]}; occ[chosen[a]]=a; arms[a].ins.reserve(L); }
    bool planningAllowed=true;
    for(int t=0;t<L;t++){
        if(planningAllowed){
            double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
            if(el>8.0) planningAllowed=false;
        }
        for(int a=0;a<A;a++){
            Arm& r=arms[a];
            if(r.task>=0){
                Task& tk=tasks[r.task];
                long long need=manh(r.body.back(),tk.px[r.idx],tk.py[r.idx])+tk.suf[r.idx];
                if(need>(long long)(L-t) || r.blocked>60){
                    claimed[r.task]=0; r.lastAb=r.task; r.task=-1; r.plan.clear(); r.blocked=0;
                }
            }
            if(r.task<0) progress(r,t,L-t);
            char ch='W';
            auto doRetract=[&](){
                int g=r.body.back(); int p=r.body[r.body.size()-2];
                ch=dirChar(g,p); occ[g]=-1; lastFreed[g]=t; r.body.pop_back();
            };
            if(r.task<0){
                r.plan.clear();
                if(r.body.size()>1) doRetract();
            } else {
                if(r.plan.empty() && planningAllowed && r.planCool<=t){
                    Task& tk=tasks[r.task];
                    int target=tk.py[r.idx]*W+tk.px[r.idx];
                    bool ok=planPath(r,a,target,0)||planPath(r,a,target,1);
                    if(!ok){ r.planCool=t+3; r.blocked++; if(r.body.size()>1) r.plan.push_back(-1); }
                }
                if(!r.plan.empty()){
                    int act=r.plan.front();
                    if(act==-1){
                        if(r.body.size()>1){ doRetract(); }
                        r.plan.pop_front();
                    } else {
                        if(occ[act]==-1 && lastFreed[act]!=t && !isMount[act]){
                            ch=dirChar(r.body.back(),act);
                            occ[act]=a; r.body.push_back(act); r.plan.pop_front(); r.blocked=0;
                        } else {
                            r.blocked++;
                            if(r.blocked%12==0){
                                r.plan.clear();
                                int k=min((int)r.body.size()-1,1+(t+a)%4);
                                for(int i=0;i<k;i++) r.plan.push_back(-1);
                            } else if(r.blocked%(3+a%3)==0){
                                r.plan.clear();
                            }
                        }
                    }
                }
            }
            r.ins.push_back(ch);
            progress(r,t,(long long)L-t-1);
        }
    }
    vector<int> outA;
    for(int a=0;a<A;a++) if(!arms[a].completed.empty()) outA.push_back(a);
    string out;
    out+=to_string(outA.size())+"\n";
    for(int a:outA){
        Arm& r=arms[a];
        string s=r.ins;
        while(s.size()>1 && s.back()=='W') s.pop_back();
        if(s.empty()) s="W";
        int x=r.mount%W, y=r.mount/W;
        out+=to_string(x)+" "+to_string(y)+" "+to_string(r.completed.size())+" "+to_string(s.size())+"\n";
        for(size_t i=0;i<r.completed.size();i++){ if(i) out+=' '; out+=to_string(r.completed[i]); }
        out+="\n";
        for(size_t i=0;i<s.size();i++){ if(i) out+=' '; out+=s[i]; }
        out+="\n";
    }
    fputs(out.c_str(),stdout);
    return 0;
}
