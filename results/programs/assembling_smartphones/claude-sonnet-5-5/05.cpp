#include <bits/stdc++.h>
using namespace std;
int W,H,R,M,T,L;
struct Task{long long s;vector<pair<int,int>> p;};
int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    vector<pair<int,int>> mp(M);
    vector<char> isM((size_t)W*H,0);
    for(auto&m:mp){scanf("%d %d",&m.first,&m.second);isM[(size_t)m.second*W+m.first]=1;}
    vector<Task> tk(T);
    vector<long long> plen(T,0);
    for(int i=0;i<T;i++){
        int P;scanf("%lld %d",&tk[i].s,&P);
        tk[i].p.resize(P);
        for(auto&q:tk[i].p)scanf("%d %d",&q.first,&q.second);
        for(int j=1;j<P;j++)plen[i]+=abs(tk[i].p[j].first-tk[i].p[j-1].first)+abs(tk[i].p[j].second-tk[i].p[j-1].second);
    }
    vector<char> claimed(T,0),mused(M,0);
    struct Arm{
        int mx,my,x,y,task,pi,best,since;
        vector<pair<int,int>> st;
        string ins;
        vector<int> done;
        vector<int> doneLen;
        bool active;
    };
    vector<Arm> arms;
    for(int a=0;a<R;a++){
        double bs=-1;int bm=-1,bt=-1;
        for(int m=0;m<M;m++){
            if(mused[m])continue;
            for(int t=0;t<T;t++){
                if(claimed[t])continue;
                long long d=abs(mp[m].first-tk[t].p[0].first)+abs(mp[m].second-tk[t].p[0].second)+plen[t];
                if(d>L)continue;
                double v=(double)tk[t].s/(d+1);
                if(v>bs){bs=v;bm=m;bt=t;}
            }
        }
        if(bm<0){
            // no task fits; skip arm
            break;
        }
        mused[bm]=1;claimed[bt]=1;
        Arm ar;ar.mx=ar.x=mp[bm].first;ar.my=ar.y=mp[bm].second;ar.task=bt;ar.pi=0;ar.best=INT_MAX;ar.since=0;ar.active=true;
        arms.push_back(ar);
    }
    vector<char> occ((size_t)W*H,0);
    int dx[4]={1,-1,0,0},dy[4]={0,0,1,-1};
    char dc[4]={'R','L','U','D'};
    for(int t=0;t<L;t++){
        vector<pair<int,int>> pend;
        bool any=false;
        for(auto&ar:arms){
            if(!ar.active)continue;
            any=true;
            int tx=tk[ar.task].p[ar.pi].first,ty=tk[ar.task].p[ar.pi].second;
            int cd=abs(ar.x-tx)+abs(ar.y-ty);
            int bd=-1;
            for(int d=0;d<4;d++){
                int nx=ar.x+dx[d],ny=ar.y+dy[d];
                if(nx<0||ny<0||nx>=W||ny>=H)continue;
                size_t id=(size_t)ny*W+nx;
                if(isM[id]||occ[id])continue;
                int nd=abs(nx-tx)+abs(ny-ty);
                if(nd<cd){bd=d;break;}
            }
            if(bd>=0){
                int nx=ar.x+dx[bd],ny=ar.y+dy[bd];
                occ[(size_t)ny*W+nx]=1;
                ar.st.push_back({nx,ny});
                ar.x=nx;ar.y=ny;
                ar.ins+=dc[bd];
            } else if(!ar.st.empty()){
                pair<int,int> tip=ar.st.back();ar.st.pop_back();
                pair<int,int> prev=ar.st.empty()?make_pair(ar.mx,ar.my):ar.st.back();
                int d=0;
                for(;d<4;d++)if(tip.first+dx[d]==prev.first&&tip.second+dy[d]==prev.second)break;
                ar.ins+=dc[d];
                pend.push_back(tip);
                ar.x=prev.first;ar.y=prev.second;
            } else ar.ins+='W';
            // progress
            bool abandon=false;
            while(ar.pi<(int)tk[ar.task].p.size()&&tk[ar.task].p[ar.pi].first==ar.x&&tk[ar.task].p[ar.pi].second==ar.y){
                ar.pi++;ar.best=INT_MAX;ar.since=0;
            }
            if(ar.pi==(int)tk[ar.task].p.size()){
                ar.done.push_back(ar.task);
                ar.doneLen.push_back((int)ar.ins.size());
                ar.active=false;
            } else {
                int nd=abs(ar.x-tk[ar.task].p[ar.pi].first)+abs(ar.y-tk[ar.task].p[ar.pi].second);
                if(nd<ar.best){ar.best=nd;ar.since=0;}
                else ar.since++;
                if(ar.since>nd+100)abandon=true;
            }
            if(!ar.active||abandon){
                // pick next task
                double bs=-1;int bt=-1;
                int rem=L-t-1;
                for(int k=0;k<T;k++){
                    if(claimed[k])continue;
                    long long d=abs(ar.x-tk[k].p[0].first)+abs(ar.y-tk[k].p[0].second)+plen[k];
                    if(d>rem)continue;
                    double v=(double)tk[k].s/(d+1);
                    if(v>bs){bs=v;bt=k;}
                }
                if(bt<0){ar.active=false;}
                else{
                    claimed[bt]=1;ar.task=bt;ar.pi=0;ar.best=INT_MAX;ar.since=0;ar.active=true;
                    while(ar.pi<(int)tk[bt].p.size()&&tk[bt].p[ar.pi].first==ar.x&&tk[bt].p[ar.pi].second==ar.y)ar.pi++;
                    if(ar.pi==(int)tk[bt].p.size()){
                        ar.done.push_back(bt);
                        ar.doneLen.push_back((int)ar.ins.size());
                        ar.active=false;
                    }
                }
            }
        }
        for(auto&c:pend)occ[(size_t)c.second*W+c.first]=0;
        if(!any)break;
    }
    int A=0;
    for(auto&ar:arms)if(!ar.done.empty()&&ar.doneLen.back()>=1)A++;
    printf("%d\n",A);
    for(auto&ar:arms){
        if(ar.done.empty()||ar.doneLen.back()<1)continue;
        // only tasks with doneLen>=1
        vector<int> ts;
        for(size_t i=0;i<ar.done.size();i++)ts.push_back(ar.done[i]);
        int K=ar.doneLen.back();
        printf("%d %d %d %d\n",ar.mx,ar.my,(int)ts.size(),K);
        for(size_t i=0;i<ts.size();i++)printf(i?" %d":"%d",ts[i]);
        printf("\n");
        for(int i=0;i<K;i++){if(i)putchar(' ');putchar(ar.ins[i]);}
        printf("\n");
    }
}
