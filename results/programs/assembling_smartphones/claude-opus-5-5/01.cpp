#include <bits/stdc++.h>
using namespace std;

int W,H,R,M,T,L;
vector<char> isMount;
vector<int> occ;
struct Task{ long long S; vector<int> pts; int len; };
vector<Task> tasks;
vector<int> taskState; // 0 avail,1 assigned,2 done
vector<int> taskFail;

struct Arm{
    int mount; vector<int> body; int task=-1; int idx=0;
    deque<int> plan; string instr; vector<int> done; int lastComplete=0;
    int taskStart=0, taskEst=0, blocked=0, failCnt=0; bool strict=false;
};
vector<Arm> arms;

inline int manh(int a,int b){ return abs(a%W-b%W)+abs(a/W-b/W); }

vector<int> gval, stampv, par; int curStamp=0;
struct HN{int f,g,c;};
struct HC{ bool operator()(const HN&a,const HN&b)const{ if(a.f!=b.f) return a.f>b.f; return a.g<b.g; } };

bool astar(int ai,int target,bool strict){
    Arm &A=arms[ai];
    curStamp++;
    priority_queue<HN,vector<HN>,HC> pq;
    int tip=(int)A.body.size()-1;
    int tx=target%W, ty=target/W;
    for(int k=tip;k>=0;k--){
        int c=A.body[k]; int g=tip-k;
        stampv[c]=curStamp; gval[c]=g; par[c]=-(k+2);
        int h=abs(c%W-tx)+abs(c/W-ty);
        pq.push({g+h,g,c});
    }
    long long exp=0; const long long LIM=250000;
    while(!pq.empty()){
        HN cur=pq.top(); pq.pop();
        if(cur.g>gval[cur.c]) continue;
        int c=cur.c;
        if(c==target){
            vector<int> path; int x=c;
            while(par[x]>=0){ path.push_back(x); x=par[x]; }
            int k=-par[x]-2;
            A.plan.clear();
            for(int i=0;i<tip-k;i++) A.plan.push_back(-1);
            for(int i=(int)path.size()-1;i>=0;i--) A.plan.push_back(path[i]);
            return true;
        }
        if(++exp>LIM) return false;
        int cx=c%W, cy=c/W;
        for(int d=0;d<4;d++){
            int nx=cx+(d==0)-(d==1), ny=cy+(d==2)-(d==3);
            if(nx<0||ny<0||nx>=W||ny>=H) continue;
            int n=ny*W+nx;
            if(isMount[n]) continue;
            int o=occ[n]; int cost;
            if(o==-1) cost=1;
            else if(o==ai) continue;
            else { if(strict) continue; cost=6; }
            int ng=cur.g+cost;
            if(stampv[n]==curStamp && gval[n]<=ng) continue;
            stampv[n]=curStamp; gval[n]=ng; par[n]=c;
            pq.push({ng+abs(nx-tx)+abs(ny-ty),ng,n});
        }
    }
    return false;
}

int pickTask(int ai,int time){
    Arm &A=arms[ai]; int tip=A.body.back();
    int best=-1; double bv=-1;
    for(int t=0;t<T;t++){
        if(taskState[t]!=0||taskFail[t]>=3) continue;
        int est=manh(tip,tasks[t].pts[0])+tasks[t].len;
        if(est>L-time) continue;
        double v=(double)tasks[t].S/(est+1);
        if(v>bv){bv=v;best=t;}
    }
    return best;
}

void advance(int ai,int time,bool allowPick){
    Arm &A=arms[ai];
    while(true){
        if(A.task!=-1){
            Task &tk=tasks[A.task];
            int tip=A.body.back();
            bool moved=false;
            while(A.idx<(int)tk.pts.size() && tk.pts[A.idx]==tip){A.idx++;moved=true;}
            if(moved) A.plan.clear();
            if(A.idx==(int)tk.pts.size()){
                A.done.push_back(A.task); taskState[A.task]=2; A.lastComplete=time;
                A.task=-1; A.plan.clear(); continue;
            }
            break;
        } else {
            if(!allowPick) break;
            int t=pickTask(ai,time);
            if(t<0) break;
            A.task=t; A.idx=0; A.taskStart=time; taskState[t]=1;
            A.taskEst=manh(A.body.back(),tasks[t].pts[0])+tasks[t].len;
            A.failCnt=0; A.blocked=0; A.strict=false; A.plan.clear();
        }
    }
}

char dirChar(int from,int to){
    int fx=from%W, fy=from/W, tx=to%W, ty=to/W;
    if(tx==fx+1) return 'R'; if(tx==fx-1) return 'L';
    if(ty==fy+1) return 'U'; return 'D';
}

int main(){
    auto t0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L)!=6) return 0;
    isMount.assign((size_t)W*H,0);
    vector<int> mounts(M);
    for(int i=0;i<M;i++){int x,y;scanf("%d %d",&x,&y); mounts[i]=y*W+x; isMount[mounts[i]]=1;}
    tasks.resize(T);
    for(int t=0;t<T;t++){
        long long S;int P;scanf("%lld %d",&S,&P);
        tasks[t].S=S; tasks[t].pts.resize(P);
        for(int i=0;i<P;i++){int x,y;scanf("%d %d",&x,&y);tasks[t].pts[i]=y*W+x;}
        int len=0; for(int i=1;i<P;i++) len+=manh(tasks[t].pts[i-1],tasks[t].pts[i]);
        tasks[t].len=len;
    }
    taskState.assign(T,0); taskFail.assign(T,0);
    // mount selection
    vector<double> bestv(T,0.0); vector<char> chosen(M,0); vector<int> sel;
    for(int r=0;r<R;r++){
        int bm=-1; double bg=-1;
        for(int m=0;m<M;m++){
            if(chosen[m]) continue;
            double g=0;
            for(int t=0;t<T;t++){
                int c=manh(mounts[m],tasks[t].pts[0])+tasks[t].len;
                if(c>L) continue;
                double v=(double)tasks[t].S/(c+1);
                if(v>bestv[t]) g+=v-bestv[t];
            }
            if(g>bg){bg=g;bm=m;}
        }
        if(bm<0) break;
        chosen[bm]=1; sel.push_back(bm);
        for(int t=0;t<T;t++){
            int c=manh(mounts[bm],tasks[t].pts[0])+tasks[t].len;
            if(c>L) continue;
            double v=(double)tasks[t].S/(c+1);
            bestv[t]=max(bestv[t],v);
        }
    }
    occ.assign((size_t)W*H,-1);
    gval.assign((size_t)W*H,0); stampv.assign((size_t)W*H,0); par.assign((size_t)W*H,0);
    int A=sel.size(); arms.resize(A);
    for(int i=0;i<A;i++){arms[i].mount=mounts[sel[i]]; arms[i].body.push_back(arms[i].mount); occ[arms[i].mount]=i;}
    int tEnd=L;
    vector<int> frees;
    for(int t=0;t<L;t++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
        if(el>8.3){tEnd=t;break;}
        frees.clear();
        for(int q=0;q<A;q++){
            int ai=(q+t)%A; Arm &a=arms[ai];
            advance(ai,t,true);
            int move=-2; // -2 wait, -1 retract, else cell
            if(a.task==-1){
                if(a.body.size()>1) move=-1;
            } else {
                if(t-a.taskStart>2*a.taskEst+30){
                    taskState[a.task]=0; taskFail[a.task]++; a.task=-1; a.plan.clear();
                    if(a.body.size()>1) move=-1;
                } else {
                    if(a.plan.empty()){
                        int target=tasks[a.task].pts[a.idx];
                        bool ok=astar(ai,target,a.strict);
                        if(!ok && a.strict) ok=astar(ai,target,false);
                        a.strict=false;
                        if(!ok){
                            a.failCnt++;
                            if(a.failCnt>=5){taskState[a.task]=0; taskFail[a.task]++; a.task=-1; a.plan.clear();}
                        }
                    }
                    if(a.task!=-1 && !a.plan.empty()){
                        int f=a.plan.front();
                        if(f==-1){ move=-1; a.plan.pop_front(); }
                        else if(occ[f]==-1 && !isMount[f]){ move=f; a.plan.pop_front(); a.blocked=0; }
                        else { a.blocked++; if(a.blocked>=2){a.plan.clear(); a.strict=true; a.blocked=0;} }
                    }
                }
            }
            if(move==-1 && a.body.size()>1){
                int c=a.body.back(); a.body.pop_back(); frees.push_back(c);
                a.instr.push_back(dirChar(c,a.body.back()));
            } else if(move>=0){
                int tip=a.body.back();
                occ[move]=ai; a.body.push_back(move);
                a.instr.push_back(dirChar(tip,move));
            } else a.instr.push_back('W');
        }
        for(int c:frees) occ[c]=-1;
    }
    for(int i=0;i<A;i++) advance(i,tEnd,false);
    string out; int cnt=0;
    for(int i=0;i<A;i++){
        Arm &a=arms[i]; if(a.done.empty()) continue;
        int last=-1; for(int j=0;j<(int)a.instr.size();j++) if(a.instr[j]!='W') last=j;
        int K=max(last+1,a.lastComplete); if(K<1) K=1; if(K>(int)a.instr.size()) K=a.instr.size();
        if(K<1) continue;
        cnt++;
        out+=to_string(a.mount%W)+" "+to_string(a.mount/W)+" "+to_string(a.done.size())+" "+to_string(K)+"\n";
        for(size_t j=0;j<a.done.size();j++){ if(j) out+=' '; out+=to_string(a.done[j]); }
        out+="\n";
        for(int j=0;j<K;j++){ if(j) out+=' '; out+=a.instr[j]; }
        out+="\n";
    }
    printf("%d\n%s",cnt,out.c_str());
    return 0;
}
