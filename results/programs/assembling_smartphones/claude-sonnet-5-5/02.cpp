#include <bits/stdc++.h>
using namespace std;
int W,H,R,M,T,L;
int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    vector<int> mx(M),my(M);
    vector<char> mount((size_t)W*H,0),occ((size_t)W*H,0);
    for(int i=0;i<M;i++){scanf("%d %d",&mx[i],&my[i]);mount[(size_t)my[i]*W+mx[i]]=1;}
    vector<int> S(T);vector<vector<pair<int,int>>> pts(T);vector<long long> len(T,0);
    for(int i=0;i<T;i++){
        int p;scanf("%d %d",&S[i],&p);pts[i].resize(p);
        for(auto&q:pts[i])scanf("%d %d",&q.first,&q.second);
        for(int j=1;j<p;j++)len[i]+=abs(pts[i][j].first-pts[i][j-1].first)+abs(pts[i][j].second-pts[i][j-1].second);
    }
    vector<char> taken(T,0),usedM(M,0);
    struct Arm{int mount;vector<int> st;int task=-1,idx=0,stuck=0;vector<int> done;string ins;int doneLen=0;};
    vector<Arm> arms;
    for(int r=0;r<R;r++){
        double best=-1;int bt=-1,bm=-1;
        for(int t=0;t<T;t++){
            if(taken[t])continue;
            int bd=INT_MAX,bmm=-1;
            for(int m=0;m<M;m++){
                if(usedM[m])continue;
                int d=abs(mx[m]-pts[t][0].first)+abs(my[m]-pts[t][0].second);
                if(d<bd){bd=d;bmm=m;}
            }
            if(bmm<0)continue;
            long long c=bd+len[t];
            if(c>L)continue;
            double v=(double)S[t]/(c+1);
            if(v>best){best=v;bt=t;bm=bmm;}
        }
        if(bt<0)break;
        taken[bt]=1;usedM[bm]=1;
        Arm a;a.mount=bm;a.st.push_back(my[bm]*W+mx[bm]);occ[a.st[0]]=1;a.task=bt;a.idx=0;
        arms.push_back(a);
    }
    int dx[4]={1,-1,0,0},dy[4]={0,0,1,-1};char dc[4]={'R','L','U','D'};
    auto pick=[&](Arm&a,int step){
        // after finishing: choose new task
        int cur=a.st.back();int cx=cur%W,cy=cur/W;
        double best=-1;int bt=-1;
        for(int t=0;t<T;t++){
            if(taken[t])continue;
            long long c=abs(cx-pts[t][0].first)+abs(cy-pts[t][0].second)+len[t];
            if(step+c>L)continue;
            double v=(double)S[t]/(c+1);
            if(v>best){best=v;bt=t;}
        }
        if(bt>=0){taken[bt]=1;}
        a.task=bt;a.idx=0;
    };
    auto check=[&](Arm&a,int step){
        while(a.task>=0){
            int cur=a.st.back();
            while(a.idx<(int)pts[a.task].size()&&pts[a.task][a.idx].first==cur%W&&pts[a.task][a.idx].second==cur/W)a.idx++;
            if(a.idx==(int)pts[a.task].size()){
                a.done.push_back(a.task);a.doneLen=step;
                pick(a,step);
            } else break;
        }
    };
    for(int step=0;step<L;step++){
        bool any=false;
        for(size_t ai=0;ai<arms.size();ai++){
            Arm&a=arms[ai];
            if(a.task<0)continue;
            any=true;
            int cur=a.st.back();int cx=cur%W,cy=cur/W;
            int tx=pts[a.task][a.idx].first,ty=pts[a.task][a.idx].second;
            int curd=abs(cx-tx)+abs(cy-ty);
            int bd=-1,bdist=INT_MAX;
            int vd[4],vdist[4];int nv=0;
            for(int d=0;d<4;d++){
                int nx=cx+dx[d],ny=cy+dy[d];
                if(nx<0||ny<0||nx>=W||ny>=H)continue;
                int nc=ny*W+nx;
                bool ok;
                if(a.st.size()>=2&&nc==a.st[a.st.size()-2])ok=true;
                else ok=!mount[nc]&&!occ[nc];
                if(!ok)continue;
                int dist=abs(nx-tx)+abs(ny-ty);
                vd[nv]=d;vdist[nv]=dist;nv++;
                if(dist<bdist){bdist=dist;bd=d;}
            }
            int chosen=-1;
            if(bd>=0&&bdist<curd)chosen=bd;
            else if(a.stuck>=2&&nv>0)chosen=vd[(a.stuck+ai)%nv];
            if(chosen<0){a.stuck++;a.ins.push_back('W');}
            else{
                int nx=cx+dx[chosen],ny=cy+dy[chosen];int nc=ny*W+nx;
                if(a.st.size()>=2&&nc==a.st[a.st.size()-2]){occ[a.st.back()]=0;a.st.pop_back();}
                else{a.st.push_back(nc);occ[nc]=1;}
                a.ins.push_back(dc[chosen]);
                a.stuck=0;
                check(a,step+1);
            }
        }
        if(!any)break;
    }
    int cnt=0;
    for(auto&a:arms)if(!a.done.empty())cnt++;
    printf("%d\n",cnt);
    for(auto&a:arms){
        if(a.done.empty())continue;
        printf("%d %d %d %d\n",mx[a.mount],my[a.mount],(int)a.done.size(),a.doneLen);
        for(size_t i=0;i<a.done.size();i++)printf(i?" %d":"%d",a.done[i]);
        printf("\n");
        for(int i=0;i<a.doneLen;i++)printf(i?" %c":"%c",a.ins[i]);
        printf("\n");
    }
}
