#include <bits/stdc++.h>
using namespace std;

static const int INF = 1000000000;
int W,H,R,M,T,L;
vector<int> lastUse, owner_, bodyIdx;
struct Task{ long long S; vector<int> pts; int len; };
vector<Task> tasks;
struct Arm{ int mount; vector<int> body; int t; vector<char> cmds; vector<int> tk; bool done; vector<char> failed; };
vector<Arm> arms;
struct LogE{int c,lu,ow,bi;};
vector<LogE> logv;
chrono::steady_clock::time_point st;
bool timeUp=false;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-st).count();}

inline void setCell(int c,int lu,int ow,int bi){
    logv.push_back({c,lastUse[c],owner_[c],bodyIdx[c]});
    lastUse[c]=lu;owner_[c]=ow;bodyIdx[c]=bi;
}
char dirOf(int a,int b){
    int ax=a%W,ay=a/W,bx=b%W,by=b/W;
    if(bx==ax+1)return 'R'; if(bx==ax-1)return 'L'; if(by==ay+1)return 'U'; return 'D';
}
void retractTo(Arm&a,int j){
    while((int)a.body.size()-1>j){
        int c=a.body.back(); int p=a.body[a.body.size()-2];
        a.cmds.push_back(dirOf(c,p));
        setCell(c,a.t,-1,-1);
        a.body.pop_back(); a.t++;
    }
}
vector<int> dist_, par_, stamp_; int curStamp=0;
int manh(int a,int b){return abs(a%W-b%W)+abs(a/W-b/W);}

bool reach(int id,int target){
    Arm&a=arms[id];
    int len=a.body.size();
    if(owner_[target]==id){
        int j=bodyIdx[target];
        if(a.t+(len-1-j)>L) return false;
        retractTo(a,j); return true;
    }
    if(lastUse[target]>=INF) return false;
    curStamp++;
    typedef pair<long long,int> PI;
    priority_queue<PI,vector<PI>,greater<PI>> pq;
    auto key=[&](int g,int c){return (long long)(g+manh(c,target))*2000000LL - g;};
    for(int j=0;j<len;j++){
        int g=a.t+(len-1-j); if(g>L) continue;
        int c=a.body[j];
        stamp_[c]=curStamp; dist_[c]=g; par_[c]=-1;
        pq.push({key(g,c)*0+ (long long)(g+manh(c,target))*20000LL - g, c});
    }
    // use consistent key
    auto K2=[&](int g,int c){return (long long)(g+manh(c,target))*20000LL - g;};
    long long pops=0; bool found=false;
    while(!pq.empty()){
        auto [k,u]=pq.top();pq.pop();
        int g=dist_[u];
        if(K2(g,u)!=k) continue;
        if(u==target){found=true;break;}
        if(((++pops)&4095)==0){ if(elapsed()>8.5){timeUp=true;return false;} }
        if(pops>3000000) return false;
        int ux=u%W,uy=u/W;
        int nb[4];int nc=0;
        if(ux+1<W)nb[nc++]=u+1; if(ux>0)nb[nc++]=u-1; if(uy+1<H)nb[nc++]=u+W; if(uy>0)nb[nc++]=u-W;
        for(int q=0;q<nc;q++){
            int v=nb[q]; int lu=lastUse[v]; if(lu>=INF) continue;
            int arr=max(g+1,lu+2); if(arr>L) continue;
            if(stamp_[v]!=curStamp||arr<dist_[v]){
                if(stamp_[v]==curStamp && par_[v]==-1) continue;
                stamp_[v]=curStamp;dist_[v]=arr;par_[v]=u;
                pq.push({K2(arr,v),v});
            }
        }
    }
    if(!found) return false;
    vector<int> path; int c=target;
    while(par_[c]!=-1){path.push_back(c);c=par_[c];}
    reverse(path.begin(),path.end());
    int j=bodyIdx[c];
    retractTo(a,j);
    for(int v:path){
        int arr=dist_[v];
        while(a.t<arr-1){a.cmds.push_back('W');a.t++;}
        a.cmds.push_back(dirOf(a.body.back(),v));
        a.t=arr;
        setCell(v,INF,id,(int)a.body.size());
        a.body.push_back(v);
    }
    return true;
}

int main(){
    st=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L)!=6)return 0;
    vector<int> mounts(M);
    for(int i=0;i<M;i++){int x,y;scanf("%d %d",&x,&y);mounts[i]=y*W+x;}
    tasks.resize(T);
    for(int i=0;i<T;i++){
        long long s;int p;scanf("%lld %d",&s,&p);tasks[i].S=s;tasks[i].pts.resize(p);
        for(int k=0;k<p;k++){int x,y;scanf("%d %d",&x,&y);tasks[i].pts[k]=y*W+x;}
        int l=0;for(int k=1;k<p;k++)l+=manh(tasks[i].pts[k-1],tasks[i].pts[k]);
        tasks[i].len=l;
    }
    int N=W*H;
    lastUse.assign(N,-1);owner_.assign(N,-1);bodyIdx.assign(N,-1);
    dist_.assign(N,0);par_.assign(N,-1);stamp_.assign(N,0);
    for(int m:mounts)lastUse[m]=INF;
    // choose mounts
    vector<int> chosen; vector<char> used(M,0); vector<double> cur(T,0);
    int A=min(R,M);
    for(int r=0;r<A;r++){
        double best=-1;int bm=-1;
        for(int m=0;m<M;m++){ if(used[m])continue;
            double gsum=0;
            for(int t=0;t<T;t++){
                double g=1.0/(manh(mounts[m],tasks[t].pts[0])+tasks[t].len+5);
                if(g>cur[t]) gsum+=tasks[t].S*(g-cur[t]);
            }
            if(gsum>best){best=gsum;bm=m;}
        }
        used[bm]=1;chosen.push_back(bm);
        for(int t=0;t<T;t++){double g=1.0/(manh(mounts[bm],tasks[t].pts[0])+tasks[t].len+5);cur[t]=max(cur[t],g);}
    }
    arms.resize(A);
    for(int i=0;i<A;i++){
        Arm&a=arms[i];a.mount=mounts[chosen[i]];a.body={a.mount};a.t=0;a.done=false;a.failed.assign(T,0);
        owner_[a.mount]=i;bodyIdx[a.mount]=0;
    }
    vector<char> assigned(T,0);
    while(!timeUp){
        if(elapsed()>8.5)break;
        int id=-1;
        for(int i=0;i<A;i++)if(!arms[i].done&&(id<0||arms[i].t<arms[id].t))id=i;
        if(id<0)break;
        Arm&a=arms[id];
        if(a.t>=L){a.done=true;continue;}
        int gp=a.body.back();
        vector<pair<double,int>> cand;
        for(int t=0;t<T;t++){
            if(assigned[t]||a.failed[t])continue;
            int est=manh(gp,tasks[t].pts[0])+tasks[t].len;
            if(a.t+est>L)continue;
            cand.push_back({-(double)tasks[t].S/(est+1),t});
        }
        int k=min((int)cand.size(),6);
        partial_sort(cand.begin(),cand.begin()+k,cand.end());
        bool ok=false;
        for(int ci=0;ci<k&&!timeUp;ci++){
            int t=cand[ci].second;
            logv.clear();
            vector<int> bodySnap=a.body; size_t cl=a.cmds.size(); int tSnap=a.t;
            bool good=true;
            for(int p:tasks[t].pts){ if(!reach(id,p)){good=false;break;} }
            if(good){assigned[t]=1;a.tk.push_back(t);ok=true;break;}
            for(int i=(int)logv.size()-1;i>=0;i--){auto&e=logv[i];lastUse[e.c]=e.lu;owner_[e.c]=e.ow;bodyIdx[e.c]=e.bi;}
            a.body=bodySnap;a.cmds.resize(cl);a.t=tSnap;
            a.failed[t]=1;
        }
        if(timeUp)break;
        if(!ok){
            logv.clear();
            while(a.body.size()>1&&a.t<L){
                int c=a.body.back(),p=a.body[a.body.size()-2];
                a.cmds.push_back(dirOf(c,p));
                lastUse[c]=a.t;owner_[c]=-1;bodyIdx[c]=-1;a.body.pop_back();a.t++;
            }
            a.done=true;
        }
    }
    vector<int> out;
    for(int i=0;i<A;i++)if(!arms[i].tk.empty()&&!arms[i].cmds.empty())out.push_back(i);
    string s;
    s+=to_string(out.size())+"\n";
    for(int i:out){
        Arm&a=arms[i];
        s+=to_string(a.mount%W)+" "+to_string(a.mount/W)+" "+to_string(a.tk.size())+" "+to_string(a.cmds.size())+"\n";
        for(size_t j=0;j<a.tk.size();j++){if(j)s+=' ';s+=to_string(a.tk[j]);}
        s+="\n";
        for(size_t j=0;j<a.cmds.size();j++){if(j)s+=' ';s+=a.cmds[j];}
        s+="\n";
    }
    fwrite(s.data(),1,s.size(),stdout);
    return 0;
}
