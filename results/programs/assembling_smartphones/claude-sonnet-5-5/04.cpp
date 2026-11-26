#include <bits/stdc++.h>
using namespace std;
int W,H,R,M,T,L;
vector<int> sc;vector<vector<int>> pts;
vector<char> mnt,occ;
vector<int> st;string mv;
vector<int> vis,par;int stamp=0;
long long work=0;
char dirc(int a,int b){int d=b-a;if(d==1)return 'R';if(d==-1)return 'L';if(d==W)return 'U';return 'D';}
bool goTo(int t){
    while(true){
        int g=st.back();
        if(g==t)return true;
        if((int)mv.size()>=L)return false;
        stamp++;
        vector<int> q;q.push_back(g);vis[g]=stamp;
        bool found=false;
        size_t h=0;
        while(h<q.size()&&!found){
            int c=q[h++];work++;
            int x=c%W,y=c/W;
            int nb[4];int k=0;
            if(x+1<W)nb[k++]=c+1;
            if(x>0)nb[k++]=c-1;
            if(y+1<H)nb[k++]=c+W;
            if(y>0)nb[k++]=c-W;
            for(int i=0;i<k;i++){
                int n=nb[i];
                if(vis[n]==stamp||mnt[n]||occ[n])continue;
                vis[n]=stamp;par[n]=c;q.push_back(n);
                if(n==t){found=true;break;}
            }
        }
        if(found){
            vector<int> path;
            for(int c=t;c!=g;c=par[c])path.push_back(c);
            reverse(path.begin(),path.end());
            if((int)(mv.size()+path.size())>L)return false;
            for(int c:path){mv+=dirc(st.back(),c);st.push_back(c);occ[c]=1;}
            return true;
        }
        if(st.size()<=1)return false;
        if(work>400000000LL)return false;
        int top=st.back();occ[top]=0;st.pop_back();
        mv+=dirc(top,st.back());
    }
}
int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    mnt.assign(W*H,0);occ.assign(W*H,0);vis.assign(W*H,0);par.assign(W*H,0);
    vector<int> mounts;
    for(int i=0;i<M;i++){int x,y;scanf("%d %d",&x,&y);mounts.push_back(y*W+x);mnt[y*W+x]=1;}
    sc.resize(T);pts.resize(T);
    vector<long long> inner(T,0);
    for(int i=0;i<T;i++){
        int P;scanf("%d %d",&sc[i],&P);
        for(int j=0;j<P;j++){int x,y;scanf("%d %d",&x,&y);pts[i].push_back(y*W+x);}
        for(int j=1;j<P;j++){
            int a=pts[i][j-1],b=pts[i][j];
            inner[i]+=abs(a%W-b%W)+abs(a/W-b/W);
        }
    }
    auto md=[&](int a,int b){return abs(a%W-b%W)+abs(a/W-b/W);};
    // choose mount
    int bm=mounts[0];double bestr=-1;
    for(int m:mounts){
        for(int i=0;i<T;i++){
            long long c=md(m,pts[i][0])+inner[i];
            if(c>L)continue;
            double r=(double)sc[i]/(c+1);
            if(r>bestr){bestr=r;bm=m;}
        }
    }
    st.push_back(bm);occ[bm]=1;
    vector<char> dead(T,0);
    vector<int> done;
    while(true){
        int g=st.back();
        int bi=-1;double br=-1;
        for(int i=0;i<T;i++){
            if(dead[i])continue;
            long long c=md(g,pts[i][0])+inner[i];
            if((long long)mv.size()+c>L){continue;}
            double r=(double)sc[i]/(c+1);
            if(r>br){br=r;bi=i;}
        }
        if(bi<0||work>400000000LL)break;
        vector<int> sst=st;size_t sm=mv.size();
        bool ok=true;
        for(int p:pts[bi]){if(!goTo(p)){ok=false;break;}}
        if(ok){done.push_back(bi);}
        else{
            for(int c:st)occ[c]=0;
            st=sst;mv.resize(sm);
            for(int c:st)occ[c]=1;
        }
        dead[bi]=1;
    }
    if(done.empty()){done.push_back(0);}
    if(mv.empty())mv="W";
    printf("1\n%d %d %d %d\n",bm%W,bm/W,(int)done.size(),(int)mv.size());
    for(size_t i=0;i<done.size();i++)printf("%d%c",done[i],i+1<done.size()?' ':'\n');
    for(size_t i=0;i<mv.size();i++)printf("%c%c",mv[i],i+1<mv.size()?' ':'\n');
}
