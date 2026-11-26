#include <bits/stdc++.h>
using namespace std;
int W,H,R,M,T,L;
struct Arm{
    vector<int> path; int task=-1,idx=0,best=0,stag=0,last=-1;
    vector<char> ins; vector<int> done;
    int mount;
};
vector<int> S;
vector<vector<int>> pts;
vector<long long> len_;
vector<char> taken, isMount;
vector<int> occ;
int dist(int a,int b){return abs(a%W-b%W)+abs(a/W-b/W);}
int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    isMount.assign((size_t)W*H,0);
    occ.assign((size_t)W*H,0);
    vector<int> mounts(M);
    for(auto&m:mounts){int x,y;scanf("%d %d",&x,&y);m=y*W+x;isMount[m]=1;}
    S.resize(T);pts.resize(T);len_.assign(T,0);taken.assign(T,0);
    for(int i=0;i<T;i++){
        int p;scanf("%d %d",&S[i],&p);
        pts[i].resize(p);
        for(auto&c:pts[i]){int x,y;scanf("%d %d",&x,&y);c=y*W+x;}
        for(int j=1;j<p;j++)len_[i]+=dist(pts[i][j-1],pts[i][j]);
    }
    vector<Arm> arms;
    vector<char> usedM(M,0);
    auto pick=[&](Arm&a,int step){
        int g=a.path.back();
        double bv=-1;int bt=-1;
        for(int t=0;t<T;t++){
            if(taken[t])continue;
            long long c=dist(g,pts[t][0])+len_[t];
            if(c>L-step)continue;
            double v=(double)S[t]/(c+1);
            if(v>bv){bv=v;bt=t;}
        }
        if(bt>=0){
            taken[bt]=1;a.task=bt;a.idx=0;
            a.best=dist(g,pts[bt][0]);a.stag=0;
        } else a.task=-1;
    };
    // initial assignment
    for(int r=0;r<R;r++){
        double bv=-1;int bm=-1,bt=-1;
        for(int m=0;m<M;m++){
            if(usedM[m])continue;
            for(int t=0;t<T;t++){
                if(taken[t])continue;
                long long c=dist(mounts[m],pts[t][0])+len_[t];
                if(c>L)continue;
                double v=(double)S[t]/(c+1);
                if(v>bv){bv=v;bm=m;bt=t;}
            }
        }
        if(bm<0)break;
        usedM[bm]=1;taken[bt]=1;
        Arm a;a.mount=mounts[bm];a.path.push_back(mounts[bm]);
        a.task=bt;a.idx=0;a.best=dist(mounts[bm],pts[bt][0]);
        arms.push_back(a);
    }
    int dx[4]={1,-1,0,0},dy[4]={0,0,1,-1};
    char dc[4]={'R','L','U','D'};
    for(int t=0;t<L;t++){
        bool any=false;
        for(auto&a:arms)if(a.task>=0)any=true;
        if(!any)break;
        for(auto&a:arms){
            if(a.task<0){a.ins.push_back('W');continue;}
            int g=a.path.back();
            int tgt=pts[a.task][a.idx];
            int cur=dist(g,tgt);
            int gx=g%W,gy=g/W;
            int bd=cur,bk=-1;
            for(int k=0;k<4;k++){
                int nx=gx+dx[k],ny=gy+dy[k];
                if(nx<0||ny<0||nx>=W||ny>=H)continue;
                int c=ny*W+nx;
                bool retract=a.path.size()>1&&a.path[a.path.size()-2]==c;
                if(!retract&&(isMount[c]||occ[c]))continue;
                int d=dist(c,tgt);
                if(d<bd){bd=d;bk=k;}
            }
            if(bk<0&&a.path.size()>1){
                int c=a.path[a.path.size()-2];
                for(int k=0;k<4;k++)if(gx+dx[k]+(gy+dy[k])*W==c&&gx+dx[k]>=0&&gx+dx[k]<W)bk=k;
            }
            if(bk<0){a.ins.push_back('W');}
            else{
                int c=(gy+dy[bk])*W+gx+dx[bk];
                if(a.path.size()>1&&a.path[a.path.size()-2]==c){
                    if(!isMount[g])occ[g]=0;
                    a.path.pop_back();
                } else {
                    a.path.push_back(c);occ[c]=1;
                }
                a.ins.push_back(dc[bk]);
            }
            // visits
            while(a.task>=0&&a.path.back()==pts[a.task][a.idx]){
                a.idx++;
                a.best=1<<30;
                if(a.idx==(int)pts[a.task].size()){
                    a.done.push_back(a.task);a.last=t;
                    pick(a,t+1);
                    if(a.task<0)break;
                }
            }
            if(a.task>=0){
                int d=dist(a.path.back(),pts[a.task][a.idx]);
                if(d<a.best){a.best=d;a.stag=0;}
                else if(++a.stag>300){a.task=-1;}
            }
        }
    }
    vector<Arm*> out;
    for(auto&a:arms)if(!a.done.empty())out.push_back(&a);
    if(out.empty()){
        printf("1\n%d %d 1 1\n0\nW\n",mounts[0]%W,mounts[0]/W);
        return 0;
    }
    printf("%d\n",(int)out.size());
    for(auto*a:out){
        int K=a->last+1;
        printf("%d %d %d %d\n",a->mount%W,a->mount/W,(int)a->done.size(),K);
        for(size_t i=0;i<a->done.size();i++)printf("%d%c",a->done[i],i+1<a->done.size()?' ':'\n');
        string s;
        for(int i=0;i<K;i++){s+=a->ins[i];if(i+1<K)s+=' ';}
        printf("%s\n",s.c_str());
    }
}
