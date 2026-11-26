#include <bits/stdc++.h>
using namespace std;

int H,W,R,Pb,Pr; long long B; int br,bc;
vector<string> g;
vector<int> nwR,nwL;
vector<char> covered,isRouter,isBack;
vector<int> distv;
vector<int> backList, routers;
const int INF=INT_MAX/2;
static const int DR[8]={-1,-1,-1,0,0,1,1,1};
static const int DC[8]={-1,0,1,-1,1,-1,0,1};
chrono::steady_clock::time_point T0;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();}

inline bool isT(int r,int c){return g[r][c]=='.';}

int gainCount(int a,int b,bool mark){
    int cnt=0;
    int lo=b-R,hi=b+R;
    for(int x=a;x<=a+R&&x<H;x++){
        if(g[x][b]=='#')break;
        int id=x*W+b;
        hi=min(hi,nwR[id]-1); lo=max(lo,nwL[id]+1);
        for(int y=lo;y<=hi;y++){int k=x*W+y; if(g[x][y]=='.'&&!covered[k]){cnt++; if(mark)covered[k]=1;}}
    }
    lo=b-R;hi=b+R;
    {int id=a*W+b; hi=min(hi,nwR[id]-1); lo=max(lo,nwL[id]+1);}
    for(int x=a-1;x>=a-R&&x>=0;x--){
        if(g[x][b]=='#')break;
        int id=x*W+b;
        hi=min(hi,nwR[id]-1); lo=max(lo,nwL[id]+1);
        for(int y=lo;y<=hi;y++){int k=x*W+y; if(g[x][y]=='.'&&!covered[k]){cnt++; if(mark)covered[k]=1;}}
    }
    return cnt;
}

void propagate(deque<int>&q){
    while(!q.empty()){
        int u=q.front();q.pop_front();
        int r=u/W,c=u%W,du=distv[u]+1;
        for(int k=0;k<8;k++){
            int nr=r+DR[k],nc=c+DC[k];
            if(nr<0||nr>=H||nc<0||nc>=W)continue;
            int v=nr*W+nc;
            if(du<distv[v]){distv[v]=du;q.push_back(v);}
        }
    }
}

// connect cell id to backbone along gradient; returns number of new cells
int connect(int id){
    vector<int> path; int cur=id;
    while(distv[cur]>0){
        path.push_back(cur);
        int r=cur/W,c=cur%W,d=distv[cur];
        int nxt=-1;
        for(int k=0;k<8;k++){
            int nr=r+DR[k],nc=c+DC[k];
            if(nr<0||nr>=H||nc<0||nc>=W)continue;
            int v=nr*W+nc; if(distv[v]==d-1){nxt=v;break;}
        }
        cur=nxt;
    }
    deque<int> q;
    for(int i=(int)path.size()-1;i>=0;i--){
        int v=path[i]; isBack[v]=1; backList.push_back(v); distv[v]=0; q.push_back(v);
    }
    propagate(q);
    return path.size();
}

void fullBFS(){
    fill(distv.begin(),distv.end(),INF);
    deque<int> q;
    int s=br*W+bc; distv[s]=0; q.push_back(s);
    for(int v:backList){distv[v]=0;q.push_back(v);}
    propagate(q);
}

long long spent=0;
typedef pair<double,int> PDI;
priority_queue<PDI> pq;
vector<PDI> deferred;

int greedy(){
    int added=0;
    while(!pq.empty()){
        if(spent+Pr>B){ // nothing affordable
            while(!pq.empty()){deferred.push_back(pq.top());pq.pop();}
            break;
        }
        PDI t=pq.top();pq.pop();
        int id=t.second;
        if(isRouter[id])continue;
        long long cost=Pr+(long long)Pb*distv[id];
        if(spent+cost>B){deferred.push_back(t);continue;}
        int gn=gainCount(id/W,id%W,false);
        if(gn==0)continue;
        double key=(double)gn/cost;
        if(1000LL*gn<=cost){deferred.push_back(PDI(key,id));continue;}
        if(!pq.empty()&&key<pq.top().first-1e-12){pq.push(PDI(key,id));continue;}
        int nc=connect(id);
        spent+=Pr+(long long)Pb*nc;
        isRouter[id]=1; routers.push_back(id);
        gainCount(id/W,id%W,true);
        added++;
    }
    return added;
}

bool rebuild(){
    vector<int> oldBack=backList;
    vector<char> oldIsBack=isBack;
    backList.clear();
    fill(isBack.begin(),isBack.end(),0);
    isBack[br*W+bc]=1;
    fullBFS();
    vector<int> rem=routers;
    while(!rem.empty()){
        int bi=0;
        for(int i=1;i<(int)rem.size();i++) if(distv[rem[i]]<distv[rem[bi]])bi=i;
        int id=rem[bi]; rem[bi]=rem.back(); rem.pop_back();
        connect(id);
        if(elapsed()>9.0)break;
    }
    if(!rem.empty()||backList.size()>=oldBack.size()){
        backList=oldBack; isBack=oldIsBack; fullBFS(); return false;
    }
    spent=(long long)routers.size()*Pr+(long long)backList.size()*Pb;
    return true;
}

int main(){
    T0=chrono::steady_clock::now();
    scanf("%d %d %d",&H,&W,&R);
    scanf("%d %d %lld",&Pb,&Pr,&B);
    scanf("%d %d",&br,&bc);
    g.resize(H);
    {
        static char buf[2000];
        for(int i=0;i<H;i++){scanf("%s",buf);g[i]=buf;if((int)g[i].size()<W)g[i].resize(W,'-');}
    }
    int N=H*W;
    nwR.assign(N,W);nwL.assign(N,-1);
    for(int r=0;r<H;r++){
        int last=-1;
        for(int c=0;c<W;c++){if(g[r][c]=='#')last=c;nwL[r*W+c]=last;}
        last=W;
        for(int c=W-1;c>=0;c--){if(g[r][c]=='#')last=c;nwR[r*W+c]=last;}
    }
    covered.assign(N,0);isRouter.assign(N,0);isBack.assign(N,0);distv.assign(N,INF);
    isBack[br*W+bc]=1;
    fullBFS();
    // prefix sums for initial gain
    vector<int> pre((size_t)H*(W+1),0);
    for(int r=0;r<H;r++)for(int c=0;c<W;c++)pre[(size_t)r*(W+1)+c+1]=pre[(size_t)r*(W+1)+c]+(g[r][c]=='.');
    {
        vector<PDI> init;
        for(int a=0;a<H;a++)for(int b=0;b<W;b++){
            if(g[a][b]=='#')continue;
            int cnt=0;
            int lo=b-R,hi=b+R;
            for(int x=a;x<=a+R&&x<H;x++){
                if(g[x][b]=='#')break;
                int id=x*W+b; hi=min(hi,nwR[id]-1); lo=max(lo,nwL[id]+1);
                cnt+=pre[(size_t)x*(W+1)+hi+1]-pre[(size_t)x*(W+1)+lo];
            }
            lo=b-R;hi=b+R;
            {int id=a*W+b; hi=min(hi,nwR[id]-1); lo=max(lo,nwL[id]+1);}
            for(int x=a-1;x>=a-R&&x>=0;x--){
                if(g[x][b]=='#')break;
                int id=x*W+b; hi=min(hi,nwR[id]-1); lo=max(lo,nwL[id]+1);
                cnt+=pre[(size_t)x*(W+1)+hi+1]-pre[(size_t)x*(W+1)+lo];
            }
            if(cnt>0){
                double cost=Pr+(double)Pb*distv[a*W+b];
                init.push_back(PDI(cnt/cost,a*W+b));
            }
        }
        pq=priority_queue<PDI>(less<PDI>(),move(init));
    }
    pre.clear();pre.shrink_to_fit();
    greedy();
    for(int it=0;it<6;it++){
        if(elapsed()>7.5)break;
        bool ok=rebuild();
        if(!ok)break;
        if(elapsed()>8.0)break;
        pq=priority_queue<PDI>(less<PDI>(),move(deferred));
        deferred.clear();
        int a=greedy();
        if(a==0)break;
    }
    // safety check budget
    long long total=(long long)backList.size()*Pb+(long long)routers.size()*Pr;
    if(total>B){ // should not happen; output empty
        printf("0\n0\n");return 0;
    }
    string out;
    out.reserve((backList.size()+routers.size())*12+32);
    out+=to_string(backList.size());out+='\n';
    for(int v:backList){out+=to_string(v/W);out+=' ';out+=to_string(v%W);out+='\n';}
    out+=to_string(routers.size());out+='\n';
    for(int v:routers){out+=to_string(v/W);out+=' ';out+=to_string(v%W);out+='\n';}
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
