#include <bits/stdc++.h>
using namespace std;

int W,H,R,M,T,L,N;
struct Task{long long S; vector<int> pts; int len;};
vector<Task> tasks;
vector<int> mountsAll;
vector<char> isMount;

long long work=0;
long long WORK_LIMIT=350000000LL;

struct Arm{
    int mount;
    vector<int> path;
    int task=-1, ptIdx=0;
    vector<int> plan; int planPos=0; int planTarget=-1;
    int blocked=0;
    vector<int> done;
    string moves;
    vector<char> banned;
};

vector<int> owner_, stampA, parA, freedAt;
int curStamp=0;
vector<int> taskState;
vector<Arm> arms;
long long simScore;
double alphaV;
vector<int> bcur,bnxt;

inline int manh(int a,int b){return abs(a%W-b%W)+abs(a/W-b/W);}

bool bfs(int self,int target,bool perm,int maxd){
    Arm&A=arms[self];
    ++curStamp;
    bcur.clear();bnxt.clear();
    int len=A.path.size();
    for(int d=0;;d++){
        if(d<=len-1){
            int c=A.path[len-1-d];
            if(stampA[c]!=curStamp){stampA[c]=curStamp;parA[c]=-(len-1-d)-2;bcur.push_back(c);}
        }
        if(bcur.empty()){ if(d>=len-1) return false; else continue;}
        if(d>maxd) return false;
        for(int c:bcur){
            work++;
            if(c==target){
                vector<int> seq; int x=c;
                while(parA[x]>=0){seq.push_back(x);x=parA[x];}
                int i=-parA[x]-2;
                A.plan.clear();
                for(int j=len-2;j>=i;j--) A.plan.push_back(A.path[j]);
                for(int k=(int)seq.size()-1;k>=0;k--) A.plan.push_back(seq[k]);
                A.planPos=0; A.planTarget=target;
                return true;
            }
            int cx=c%W, cy=c/W;
            int nb[4]; int cnt=0;
            if(cx+1<W) nb[cnt++]=c+1;
            if(cx>0) nb[cnt++]=c-1;
            if(cy+1<H) nb[cnt++]=c+W;
            if(cy>0) nb[cnt++]=c-W;
            for(int k=0;k<cnt;k++){
                int n=nb[k];
                if(stampA[n]==curStamp||isMount[n]) continue;
                int o=owner_[n];
                if(o==-1||(perm&&o!=self)){
                    stampA[n]=curStamp;parA[n]=c;bnxt.push_back(n);
                }
            }
        }
        swap(bcur,bnxt);bnxt.clear();
    }
}

void selectTask(int a,int remaining){
    Arm&A=arms[a];
    int len=A.path.size();
    vector<pair<int,int>> samp;
    int stp=max(1,len/32);
    for(int i=len-1;i>=0;i-=stp) samp.push_back({A.path[i],len-1-i});
    if(samp.back().second!=len-1) samp.push_back({A.path[0],len-1});
    int best=-1; double bv=-1;
    for(int t=0;t<T;t++){
        if(taskState[t]!=0||A.banned[t]) continue;
        work+=samp.size();
        int p0=tasks[t].pts[0];
        int e=INT_MAX;
        for(auto&s:samp){int v=s.second+manh(s.first,p0); if(v<e)e=v;}
        long long est=(long long)e+tasks[t].len;
        if(est>remaining) continue;
        double v=(double)tasks[t].S/pow((double)est+1.0,alphaV);
        if(v>bv){bv=v;best=t;}
    }
    if(best>=0){
        A.task=best;A.ptIdx=0;A.plan.clear();A.planPos=0;A.planTarget=-1;A.blocked=0;
        taskState[best]=1;
    }
}

void progress(int a,int remaining){
    Arm&A=arms[a];
    for(int guard=0;guard<T+5;guard++){
        if(A.task==-1){
            selectTask(a,remaining);
            if(A.task==-1) return;
        }
        int t=A.task; int g=A.path.back();
        auto&P=tasks[t].pts;
        while(A.ptIdx<(int)P.size()&&P[A.ptIdx]==g) A.ptIdx++;
        if(A.ptIdx==(int)P.size()){
            A.done.push_back(t);taskState[t]=2;simScore+=tasks[t].S;
            A.task=-1;A.plan.clear();A.planPos=0;A.planTarget=-1;A.blocked=0;
            continue;
        }
        if(A.planTarget!=P[A.ptIdx]){A.plan.clear();A.planPos=0;A.planTarget=-1;}
        return;
    }
}

char dirChar(int from,int to){
    if(to==from+1) return 'R';
    if(to==from-1) return 'L';
    if(to==from+W) return 'U';
    return 'D';
}

bool doRetract(int a,int s){
    Arm&A=arms[a];
    if(A.path.size()<2) return false;
    int g=A.path.back(); int p=A.path[A.path.size()-2];
    owner_[g]=-1; freedAt[g]=s; A.path.pop_back();
    A.moves.push_back(dirChar(g,p));
    return true;
}

// try to execute next plan step; return 1 moved, 0 blocked
int tryMove(int a,int s){
    Arm&A=arms[a];
    if(A.planPos>=(int)A.plan.size()) return 0;
    int n=A.plan[A.planPos];
    int g=A.path.back();
    if(A.path.size()>=2&&n==A.path[A.path.size()-2]){
        doRetract(a,s); A.planPos++; return 1;
    }
    if(manh(g,n)!=1) {A.plan.clear();A.planPos=0;A.planTarget=-1;return 0;}
    if(owner_[n]==-1&&!isMount[n]&&freedAt[n]!=s){
        owner_[n]=a;A.path.push_back(n);A.moves.push_back(dirChar(g,n));A.planPos++;return 1;
    }
    return 0;
}

void abandonTask(int a){
    Arm&A=arms[a];
    if(A.task>=0){taskState[A.task]=0;A.banned[A.task]=1;}
    A.task=-1;A.plan.clear();A.planPos=0;A.planTarget=-1;A.blocked=0;
}

long long simulate(const vector<int>& mts,double alpha,long long limit){
    alphaV=alpha;
    simScore=0;
    fill(owner_.begin(),owner_.end(),-1);
    fill(freedAt.begin(),freedAt.end(),-1);
    taskState.assign(T,0);
    int A=mts.size();
    arms.assign(A,Arm());
    for(int i=0;i<A;i++){arms[i].mount=mts[i];arms[i].path={mts[i]};owner_[mts[i]]=i;arms[i].banned.assign(T,0);}
    for(int i=0;i<A;i++) progress(i,L);
    for(int s=0;s<L;s++){
        if(work>limit) break;
        int remAfter=L-1-s;
        for(int k=0;k<A;k++){
            int a=(k+s)%A;
            Arm&B=arms[a];
            work++;
            bool moved=false;
            if(B.task==-1){
                moved=doRetract(a,s);
            }else{
                int t=B.task; int target=tasks[t].pts[B.ptIdx];
                int maxd=L-s;
                if(B.planPos>=(int)B.plan.size()||B.planTarget!=target){
                    if(!bfs(a,target,false,maxd)&&!bfs(a,target,true,maxd)){
                        abandonTask(a);
                        moved=doRetract(a,s);
                        goto after;
                    }
                }
                if(tryMove(a,s)){moved=true;B.blocked=0;}
                else{
                    B.blocked++;
                    if(B.blocked>25){
                        abandonTask(a);
                        moved=doRetract(a,s);
                    }else if(B.blocked%3==0||B.planPos>=(int)B.plan.size()){
                        if(!bfs(a,target,false,maxd)) bfs(a,target,true,maxd);
                        if(tryMove(a,s)) moved=true;
                    }
                }
            }
            after:
            if(!moved) B.moves.push_back('W');
            progress(a,remAfter);
        }
    }
    return simScore;
}

int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    N=W*H;
    isMount.assign(N,0);
    mountsAll.resize(M);
    for(int i=0;i<M;i++){int x,y;scanf("%d %d",&x,&y);mountsAll[i]=y*W+x;isMount[y*W+x]=1;}
    tasks.resize(T);
    for(int t=0;t<T;t++){
        int P;scanf("%lld %d",&tasks[t].S,&P);
        tasks[t].pts.resize(P);
        for(int i=0;i<P;i++){int x,y;scanf("%d %d",&x,&y);tasks[t].pts[i]=y*W+x;}
        int ln=0;for(int i=1;i<P;i++){int a=tasks[t].pts[i-1],b=tasks[t].pts[i];ln+=abs(a%W-b%W)+abs(a/W-b/W);}
        tasks[t].len=ln;
    }
    owner_.assign(N,-1);stampA.assign(N,0);parA.assign(N,0);freedAt.assign(N,-1);
    // available mounts
    vector<int> avail;
    for(int m:mountsAll){
        int x=m%W,y=m/W;bool ok=false;
        if(x+1<W&&!isMount[m+1])ok=true;
        if(x>0&&!isMount[m-1])ok=true;
        if(y+1<H&&!isMount[m+W])ok=true;
        if(y>0&&!isMount[m-W])ok=true;
        if(ok)avail.push_back(m);
    }
    if(avail.empty()){printf("0\n");return 0;}
    // greedy facility location
    int K=min((int)avail.size(),R);
    vector<double> cover(T,0.0);
    vector<char> used(avail.size(),0);
    vector<int> order;
    for(int k=0;k<K;k++){
        double bg=-1;int bi=-1;
        for(size_t mi=0;mi<avail.size();mi++){
            if(used[mi])continue;
            int m=avail[mi];int mx=m%W,my=m/W;
            double g=0;
            for(int t=0;t<T;t++){
                int p=tasks[t].pts[0];
                int d=abs(mx-p%W)+abs(my-p/W);
                if(d+tasks[t].len>L)continue;
                double f=1.0/(tasks[t].len+d+1);
                if(f>cover[t]) g+=tasks[t].S*(f-cover[t]);
            }
            if(g>bg){bg=g;bi=mi;}
        }
        used[bi]=1;order.push_back(avail[bi]);
        int m=avail[bi];int mx=m%W,my=m/W;
        for(int t=0;t<T;t++){
            int p=tasks[t].pts[0];
            int d=abs(mx-p%W)+abs(my-p/W);
            if(d+tasks[t].len>L)continue;
            double f=1.0/(tasks[t].len+d+1);
            if(f>cover[t])cover[t]=f;
        }
    }
    vector<pair<double,double>> variants={{1.0,1.0},{0.5,1.0},{1.0,1.5},{0.75,1.0},{1.0,0.7},{0.25,1.0},{0.5,1.5}};
    long long bestScore=-1; vector<Arm> bestArms;
    long long lastUsed=0;
    for(size_t v=0;v<variants.size();v++){
        long long remain=WORK_LIMIT-work;
        if(v>0 && remain<lastUsed*11/10) break;
        if(remain<=0)break;
        int na=max(1,(int)llround(variants[v].first*K));
        na=min(na,K);
        vector<int> mts(order.begin(),order.begin()+na);
        long long before=work;
        long long sc=simulate(mts,variants[v].second,WORK_LIMIT);
        lastUsed=work-before;
        if(sc>bestScore){bestScore=sc;bestArms=arms;}
    }
    // output
    vector<int> outIdx;
    for(size_t i=0;i<bestArms.size();i++) if(!bestArms[i].done.empty()) outIdx.push_back(i);
    string out;
    out+=to_string(outIdx.size())+"\n";
    for(int i:outIdx){
        Arm&A=bestArms[i];
        string mv=A.moves;
        while(!mv.empty()&&mv.back()=='W')mv.pop_back();
        if(mv.empty())mv="W";
        out+=to_string(A.mount%W)+" "+to_string(A.mount/W)+" "+to_string(A.done.size())+" "+to_string(mv.size())+"\n";
        for(size_t j=0;j<A.done.size();j++){if(j)out+=' ';out+=to_string(A.done[j]);}
        out+="\n";
        for(size_t j=0;j<mv.size();j++){if(j)out+=' ';out+=mv[j];}
        out+="\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
