#include <bits/stdc++.h>
using namespace std;
int W,H,R,M,T,L;
vector<char> isMount;
vector<vector<pair<int,int>>> occ;
vector<int> stampA,distA;vector<char> pdir;int stampC=0;
vector<char> bodyMark;
vector<int> body,bst;vector<char> ins;
vector<array<int,3>> lg;
bool stopAll=false;
auto t0=chrono::steady_clock::now();
double el(){return chrono::duration<double>(chrono::steady_clock::now()-t0).count();}
vector<int> sc;vector<vector<int>> pts;
char dirc(int a,int b){
    if(b==a+1)return 'R';
    if(b==a-1)return 'L';
    if(b==a+W)return 'U';
    return 'D';
}
bool freeAt(int c,int t){
    for(auto&p:occ[c])if(p.second>=t)return false;
    return true;
}
bool retractTo(int j){
    while((int)body.size()-1>j){
        int n=body.size();
        int cur=body[n-1],prev=body[n-2];
        if((int)ins.size()>=L)return false;
        lg.push_back({cur,bst[n-1],(int)ins.size()});
        bodyMark[cur]=0;
        ins.push_back(dirc(cur,prev));
        body.pop_back();bst.pop_back();
    }
    return true;
}
bool bfs(int tg){
    int s=body.back();
    int time0=ins.size();
    ++stampC;
    vector<int> q;q.push_back(s);
    stampA[s]=stampC;distA[s]=0;
    size_t h=0;bool found=false;
    while(h<q.size()&&!found){
        int c=q[h++];
        int d=distA[c];
        if(time0+d+1>L)continue;
        int x=c%W,y=c/W;
        for(int k=0;k<4;k++){
            int nx=x+(k==0)-(k==1),ny=y+(k==2)-(k==3);
            if(nx<0||ny<0||nx>=W||ny>=H)continue;
            int nc=ny*W+nx;
            if(isMount[nc]||bodyMark[nc]||stampA[nc]==stampC)continue;
            if(!freeAt(nc,time0+d))continue;
            stampA[nc]=stampC;distA[nc]=d+1;pdir[nc]="RLUD"[k];
            q.push_back(nc);
            if(nc==tg){found=true;break;}
        }
    }
    if(!found)return false;
    vector<int> path;
    int c=tg;
    while(c!=s){
        path.push_back(c);
        char d=pdir[c];
        if(d=='R')c-=1;else if(d=='L')c+=1;else if(d=='U')c-=W;else c+=W;
    }
    reverse(path.begin(),path.end());
    for(int p:path){
        char d=dirc(body.back(),p);
        bst.push_back(ins.size());
        ins.push_back(d);
        body.push_back(p);
        bodyMark[p]=1;
    }
    return true;
}
bool doTask(int id){
    vector<int> sb=body,sbst=bst;vector<char> sins=ins;size_t slg=lg.size();
    bool ok=true;
    for(int tg:pts[id]){
        if(el()>8.0){stopAll=true;ok=false;break;}
        if(body.back()==tg)continue;
        int n=body.size();
        int tx=tg%W,ty=tg/W;
        int bj=0;long long bv=LLONG_MAX;
        for(int j=0;j<n;j++){
            long long v=(n-1-j)+abs(body[j]%W-tx)+abs(body[j]/W-ty);
            if(v<=bv){bv=v;bj=j;}
        }
        bool done=false;
        if(retractTo(bj)&&bfs(tg))done=true;
        else if(!stopAll){
            if(retractTo(0)&&bfs(tg))done=true;
        }
        if(!done){ok=false;break;}
    }
    if(!ok){
        for(int c:body)bodyMark[c]=0;
        body=sb;bst=sbst;ins=sins;lg.resize(slg);
        for(int c:body)bodyMark[c]=1;
    }
    return ok;
}
int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    isMount.assign(W*H,0);
    vector<int> mounts(M);
    for(auto&m:mounts){int x,y;scanf("%d %d",&x,&y);m=y*W+x;isMount[m]=1;}
    sc.resize(T);pts.resize(T);
    vector<long long> len(T);
    for(int i=0;i<T;i++){
        int p;scanf("%d %d",&sc[i],&p);
        pts[i].resize(p);
        for(auto&q:pts[i]){int x,y;scanf("%d %d",&x,&y);q=y*W+x;}
        long long l=1;
        for(int k=1;k<p;k++)l+=abs(pts[i][k]%W-pts[i][k-1]%W)+abs(pts[i][k]/W-pts[i][k-1]/W);
        len[i]=l;
    }
    occ.assign(W*H,{});
    stampA.assign(W*H,0);distA.assign(W*H,0);pdir.assign(W*H,0);
    bodyMark.assign(W*H,0);
    vector<char> used(T,0),mused(M,0);
    struct Res{int mount;vector<int> tasks;vector<char> ins;};
    vector<Res> res;
    for(int a=0;a<R&&!stopAll&&el()<8.0;a++){
        // first task
        int bt=-1;double bs=-1;
        for(int i=0;i<T;i++)if(!used[i]){
            double v=(double)sc[i]/(double)(len[i]);
            if(v>bs){bs=v;bt=i;}
        }
        if(bt<0)break;
        int bm=-1;long long bd=LLONG_MAX;
        int fx=pts[bt][0]%W,fy=pts[bt][0]/W;
        for(int m=0;m<M;m++)if(!mused[m]){
            long long d=abs(mounts[m]%W-fx)+abs(mounts[m]/W-fy);
            if(d<bd){bd=d;bm=m;}
        }
        if(bm<0)break;
        mused[bm]=1;
        body.assign(1,mounts[bm]);bst.assign(1,0);ins.clear();lg.clear();
        bodyMark[mounts[bm]]=1;
        vector<int> tasks;
        vector<char> failed(T,0);
        bool first=true;
        while(!stopAll){
            int time=ins.size();
            int g=body.back();
            int best=-1;double bv=-1;
            for(int i=0;i<T;i++)if(!used[i]&&!failed[i]){
                long long c=len[i]+abs(g%W-pts[i][0]%W)+abs(g/W-pts[i][0]/W);
                if(time+c>L)continue;
                double v=(double)sc[i]/(double)(c+1);
                if(first&&i==bt)v=1e18;
                if(v>bv){bv=v;best=i;}
            }
            if(best<0)break;
            first=false;
            if(doTask(best)){used[best]=1;tasks.push_back(best);}
            else failed[best]=1;
        }
        // commit
        for(int c:body)bodyMark[c]=0;
        for(auto&e:lg)occ[e[0]].push_back({e[1],e[2]});
        for(size_t k=1;k<body.size();k++)occ[body[k]].push_back({bst[k],INT_MAX});
        if(!tasks.empty()&&!ins.empty())res.push_back({mounts[bm],tasks,ins});
    }
    printf("%d\n",(int)res.size());
    for(auto&r:res){
        printf("%d %d %d %d\n",r.mount%W,r.mount/W,(int)r.tasks.size(),(int)r.ins.size());
        for(size_t i=0;i<r.tasks.size();i++)printf(i?" %d":"%d",r.tasks[i]);
        printf("\n");
        for(size_t i=0;i<r.ins.size();i++){if(i)putchar(' ');putchar(r.ins[i]);}
        printf("\n");
    }
}
