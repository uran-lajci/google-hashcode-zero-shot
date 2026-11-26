#include <bits/stdc++.h>
using namespace std;

int W,H,R,M,T,L,N;
vector<char> isMount;
vector<int> owner_, bodyIdx, visS, distA, parA;
int stampG=0;
struct Task{long long score; vector<int> pts; vector<long long> suff; long long internal;};
vector<Task> tasks;
vector<char> taken;
vector<int> nearestChosen;
struct Arm{int mount; vector<int> body; int task=-1,pidx=0; bool hasPlan=false; int keepLen=1; vector<int> plan; int planIdx=0; int blocked=0; int retreat=0; string mv; vector<int> done; int lastProgress=0; int banned=-1;};
vector<Arm> arms;
static unsigned long long rs=88172645463325252ULL;
unsigned rng(){rs^=rs<<7;rs^=rs>>9;return (unsigned)rs;}
inline int manh(int a,int b){return abs(a%W-b%W)+abs(a/W-b/W);}
char dirChar(int f,int t){int fx=f%W,fy=f/W,tx=t%W,ty=t/W;if(tx==fx+1)return 'R';if(tx==fx-1)return 'L';if(ty==fy+1)return 'U';return 'D';}
int nb[4];
int neigh(int c){int k=0;int x=c%W,y=c/W;if(x+1<W)nb[k++]=c+1;if(x>0)nb[k++]=c-1;if(y+1<H)nb[k++]=c+W;if(y>0)nb[k++]=c-W;return k;}

bool planArm(int a){
    Arm&A=arms[a];int tgt=tasks[A.task].pts[A.pidx];
    A.plan.clear();A.planIdx=0;
    if(owner_[tgt]==a){A.keepLen=bodyIdx[tgt]+1;A.hasPlan=true;return true;}
    stampG++;vector<int> q;q.reserve(1024);
    visS[tgt]=stampG;distA[tgt]=0;parA[tgt]=-1;q.push_back(tgt);
    long long best=LLONG_MAX;int bestK=-1,bestU=-1;int sz=A.body.size();
    size_t h=0;const size_t cap=150000;
    while(h<q.size()){
        int u=q[h++];int d=distA[u];
        if(d+1>=best)break;
        if(q.size()>cap)break;
        int k=neigh(u);int loc[4];for(int i=0;i<k;i++)loc[i]=nb[i];
        for(int i=0;i<k;i++){int v=loc[i];
            if(owner_[v]==a){int j=bodyIdx[v];long long c=(sz-1-j)+d+1;if(c<best){best=c;bestK=j;bestU=u;}continue;}
            if(visS[v]==stampG)continue;
            if(isMount[v]||owner_[v]!=-1)continue;
            visS[v]=stampG;distA[v]=d+1;parA[v]=u;q.push_back(v);
        }
    }
    if(bestK<0)return false;
    A.keepLen=bestK+1;int u=bestU;while(u!=-1){A.plan.push_back(u);u=parA[u];}
    A.hasPlan=true;return true;
}

void chooseTask(int a,long long rem){
    Arm&A=arms[a];int g=A.body.back();double br=-1;int bt=-1;
    for(int j=0;j<T;j++){if(taken[j]||j==A.banned)continue;
        long long est=manh(g,tasks[j].pts[0])+tasks[j].internal;
        if(est>rem)continue;
        double extra=max(0,manh(A.mount,tasks[j].pts[0])-nearestChosen[j]);
        double r=(double)tasks[j].score/(est+1+0.5*extra);
        if(r>br){br=r;bt=j;}}
    if(bt>=0){taken[bt]=1;A.task=bt;A.pidx=0;A.hasPlan=false;}
}

void advance(int a,long long rem,int t){
    Arm&A=arms[a];
    for(int guard=0;guard<100000;guard++){
        if(A.task==-1){chooseTask(a,rem);A.lastProgress=t;if(A.task==-1)return;}
        Task&tk=tasks[A.task];int g=A.body.back();
        while(A.pidx<(int)tk.pts.size()&&tk.pts[A.pidx]==g){A.pidx++;A.hasPlan=false;A.lastProgress=t;}
        if(A.pidx==(int)tk.pts.size()){A.done.push_back(A.task);A.task=-1;A.hasPlan=false;continue;}
        long long re=manh(g,tk.pts[A.pidx])+tk.suff[A.pidx];
        if(re>rem||t-A.lastProgress>300){taken[A.task]=0;A.banned=A.task;A.task=-1;A.hasPlan=false;continue;}
        return;
    }
}

int main(){
    auto st=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L)!=6)return 0;
    N=W*H;isMount.assign(N,0);owner_.assign(N,-1);bodyIdx.assign(N,0);visS.assign(N,0);distA.assign(N,0);parA.assign(N,-1);
    vector<int> mounts(M);
    for(int i=0;i<M;i++){int x,y;scanf("%d %d",&x,&y);mounts[i]=y*W+x;isMount[mounts[i]]=1;}
    tasks.resize(T);
    for(int j=0;j<T;j++){int s,p;scanf("%d %d",&s,&p);tasks[j].score=s;tasks[j].pts.resize(p);
        for(int i=0;i<p;i++){int x,y;scanf("%d %d",&x,&y);tasks[j].pts[i]=y*W+x;}
        auto&P=tasks[j].pts;tasks[j].suff.assign(p,0);
        for(int i=p-2;i>=0;i--)tasks[j].suff[i]=tasks[j].suff[i+1]+manh(P[i],P[i+1]);
        tasks[j].internal=tasks[j].suff[0];}
    // mount selection
    vector<int> cand;
    for(int i=0;i<M;i++){int k=neigh(mounts[i]);bool ok=false;for(int q=0;q<k;q++)if(!isMount[nb[q]])ok=true;if(ok)cand.push_back(mounts[i]);}
    int C=cand.size();
    vector<double> w(T);for(int j=0;j<T;j++)w[j]=(double)tasks[j].score/(tasks[j].internal+1);
    vector<float> G((size_t)C*T);
    for(int i=0;i<C;i++)for(int j=0;j<T;j++)G[(size_t)i*T+j]=1.0f/(1+manh(cand[i],tasks[j].pts[0]));
    vector<double> bestv(T,0);vector<char> used(C,0);vector<int> chosen;
    int npick=min(R,C);
    for(int p=0;p<npick;p++){double bg=-1;int bi=-1;
        for(int i=0;i<C;i++){if(used[i])continue;double s=0;const float*row=&G[(size_t)i*T];
            for(int j=0;j<T;j++){double d=row[j]-bestv[j];if(d>0)s+=w[j]*d;}
            if(s>bg){bg=s;bi=i;}}
        if(bi<0)break;used[bi]=1;chosen.push_back(cand[bi]);
        for(int j=0;j<T;j++)bestv[j]=max(bestv[j],(double)G[(size_t)bi*T+j]);}
    nearestChosen.assign(T,0);
    for(int j=0;j<T;j++){int b=INT_MAX;for(int m:chosen)b=min(b,manh(m,tasks[j].pts[0]));nearestChosen[j]=(b==INT_MAX?0:b);}
    taken.assign(T,0);
    int n=chosen.size();arms.resize(n);
    for(int a=0;a<n;a++){arms[a].mount=chosen[a];arms[a].body.push_back(chosen[a]);owner_[chosen[a]]=a;bodyIdx[chosen[a]]=0;}
    for(int a=0;a<n;a++)advance(a,L,0);
    vector<int> act(n),tcell(n);
    for(int t=0;t<L;t++){
        double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
        if(el>8.8)break;
        for(int a=0;a<n;a++){Arm&A=arms[a];act[a]=0;
            if(A.task==-1){if(A.body.size()>1)act[a]=1;continue;}
            if(A.retreat>0){A.retreat--;if(A.body.size()>1)act[a]=1;continue;}
            if(!A.hasPlan){
                if(!planArm(a)){unsigned r=rng()%3;if(r==0){A.retreat=rng()%5;if(A.body.size()>1)act[a]=1;}continue;}
            }
            if((int)A.body.size()>A.keepLen)act[a]=1;
            else if(A.planIdx<(int)A.plan.size()){act[a]=2;tcell[a]=A.plan[A.planIdx];}
            else A.hasPlan=false;
        }
        vector<char> ch(n,'W');
        for(int a=0;a<n;a++)if(act[a]==1){Arm&A=arms[a];int g=A.body.back();owner_[g]=-1;A.body.pop_back();ch[a]=dirChar(g,A.body.back());}
        for(int ii=0;ii<n;ii++){int a=(ii+t)%n;if(act[a]!=2)continue;Arm&A=arms[a];int c=tcell[a];int g=A.body.back();
            if(owner_[c]==-1&&!isMount[c]&&manh(g,c)==1){owner_[c]=a;bodyIdx[c]=A.body.size();A.body.push_back(c);A.planIdx++;A.blocked=0;ch[a]=dirChar(g,c);}
            else{A.blocked++;if(A.blocked>=2){A.hasPlan=false;A.blocked=0;}}}
        for(int a=0;a<n;a++)arms[a].mv.push_back(ch[a]);
        long long rem=L-(t+1);
        for(int a=0;a<n;a++)advance(a,rem,t+1);
    }
    vector<int> out;for(int a=0;a<n;a++)if(!arms[a].done.empty())out.push_back(a);
    string res;res+=to_string(out.size())+"\n";
    for(int a:out){Arm&A=arms[a];string&s=A.mv;int k=s.size();while(k>0&&s[k-1]=='W')k--;if(k==0)k=1;
        if(s.empty())s="W";
        res+=to_string(A.mount%W)+" "+to_string(A.mount/W)+" "+to_string(A.done.size())+" "+to_string(k)+"\n";
        for(size_t i=0;i<A.done.size();i++){if(i)res+=' ';res+=to_string(A.done[i]);}res+="\n";
        for(int i=0;i<k;i++){if(i)res+=' ';res+=s[i];}res+="\n";}
    fwrite(res.data(),1,res.size(),stdout);
    return 0;
}
