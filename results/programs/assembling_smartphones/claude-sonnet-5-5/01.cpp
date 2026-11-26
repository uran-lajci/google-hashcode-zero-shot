#include <bits/stdc++.h>
using namespace std;
int W,H,R,M,T,L;
struct Arm{int mx,my;vector<int> path;vector<int> done;int cur=-1,idx=0;string ins;size_t lastLen=0;};
vector<int> sc;vector<vector<pair<int,int>>> pts;
vector<char> isM,occ,assigned;
int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    vector<pair<int,int>> mp(M);
    isM.assign((size_t)W*H,0);occ.assign((size_t)W*H,0);
    for(auto&m:mp){scanf("%d %d",&m.first,&m.second);isM[(size_t)m.second*W+m.first]=1;}
    sc.resize(T);pts.resize(T);assigned.assign(T,0);
    for(int i=0;i<T;i++){int p;scanf("%d %d",&sc[i],&p);pts[i].resize(p);for(auto&q:pts[i])scanf("%d %d",&q.first,&q.second);}
    vector<int> ord(T);iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return sc[a]>sc[b];});
    vector<char> used(M,0);vector<Arm> arms;
    for(int k=0;k<T&&(int)arms.size()<R;k++){
        int t=ord[k];int best=-1,bd=INT_MAX;
        for(int m=0;m<M;m++)if(!used[m]){int d=abs(mp[m].first-pts[t][0].first)+abs(mp[m].second-pts[t][0].second);if(d<bd){bd=d;best=m;}}
        if(best<0)break;used[best]=1;Arm a;a.mx=mp[best].first;a.my=mp[best].second;arms.push_back(a);
    }
    for(int m=0;m<M&&(int)arms.size()<R;m++)if(!used[m]){used[m]=1;Arm a;a.mx=mp[m].first;a.my=mp[m].second;arms.push_back(a);}
    auto grip=[&](Arm&a){if(a.path.empty())return make_pair(a.mx,a.my);int c=a.path.back();return make_pair(c%W,c/W);};
    auto advance=[&](Arm&a){
        while(true){
            if(a.cur!=-1){
                auto g=grip(a);
                while(a.idx<(int)pts[a.cur].size()&&pts[a.cur][a.idx]==g)a.idx++;
                if(a.idx==(int)pts[a.cur].size()){a.done.push_back(a.cur);a.lastLen=a.ins.size();a.cur=-1;}
                else break;
            }
            auto g=grip(a);int rem=L-(int)a.ins.size();
            int best=-1;double bs=-1;
            for(int t=0;t<T;t++)if(!assigned[t]){
                long long c=abs(g.first-pts[t][0].first)+abs(g.second-pts[t][0].second);
                for(size_t i=1;i<pts[t].size();i++)c+=abs(pts[t][i].first-pts[t][i-1].first)+abs(pts[t][i].second-pts[t][i-1].second);
                if(c>rem)continue;
                double s=(double)sc[t]/(c+1);
                if(s>bs){bs=s;best=t;}
            }
            if(best<0)break;
            assigned[best]=1;a.cur=best;a.idx=0;
        }
    };
    for(auto&a:arms)advance(a);
    const int dx[4]={1,-1,0,0},dy[4]={0,0,1,-1};const char dc[4]={'R','L','U','D'};
    for(int t=0;t<L;t++){
        for(auto&a:arms){
            auto g=grip(a);
            char out='W';
            if(a.cur!=-1){
                auto tg=pts[a.cur][a.idx];
                int ddx=tg.first-g.first,ddy=tg.second-g.second;
                vector<int> cand;
                auto dirx=[&](int s){return s>0?0:1;};
                auto diry=[&](int s){return s>0?2:3;};
                if(abs(ddx)>=abs(ddy)){
                    if(ddx)cand.push_back(dirx(ddx));
                    if(ddy)cand.push_back(diry(ddy));
                }else{
                    if(ddy)cand.push_back(diry(ddy));
                    if(ddx)cand.push_back(dirx(ddx));
                }
                if(ddx==0){cand.push_back(0);cand.push_back(1);}
                if(ddy==0){cand.push_back(2);cand.push_back(3);}
                bool moved=false;
                for(int d:cand){
                    int nx=g.first+dx[d],ny=g.second+dy[d];
                    if(nx<0||ny<0||nx>=W||ny>=H)continue;
                    size_t c=(size_t)ny*W+nx;
                    if(isM[c]||occ[c])continue;
                    occ[c]=1;a.path.push_back(c);out=dc[d];moved=true;break;
                }
                if(!moved&&!a.path.empty()){
                    int c=a.path.back();a.path.pop_back();occ[c]=0;
                    auto ng=grip(a);
                    int ddx2=ng.first-g.first,ddy2=ng.second-g.second;
                    for(int d=0;d<4;d++)if(dx[d]==ddx2&&dy[d]==ddy2)out=dc[d];
                }
            }
            a.ins.push_back(out);
            advance(a);
        }
    }
    vector<Arm*> use;
    for(auto&a:arms)if(!a.done.empty()&&a.lastLen>0)use.push_back(&a);
    if(use.empty()){
        // fallback: valid trivial output (single arm doing nothing useful is invalid if no task); try arm with one task
        printf("0\n");return 0;
    }
    printf("%d\n",(int)use.size());
    for(auto*a:use){
        printf("%d %d %d %d\n",a->mx,a->my,(int)a->done.size(),(int)a->lastLen);
        for(size_t i=0;i<a->done.size();i++)printf("%d%c",a->done[i],i+1==a->done.size()?'\n':' ');
        for(size_t i=0;i<a->lastLen;i++)printf("%c%c",a->ins[i],i+1==a->lastLen?'\n':' ');
    }
}
