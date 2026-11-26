#include <bits/stdc++.h>
using namespace std;
int W,H,R,M,T,L;
int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    vector<pair<int,int>> mp(M);
    vector<char> isMount((size_t)W*H,0);
    for(auto&m:mp){scanf("%d %d",&m.first,&m.second);isMount[(size_t)m.second*W+m.first]=1;}
    vector<int> S(T);
    vector<vector<pair<int,int>>> pts(T);
    vector<long long> inner(T,0);
    for(int i=0;i<T;i++){
        int p;scanf("%d %d",&S[i],&p);
        pts[i].resize(p);
        for(auto&q:pts[i])scanf("%d %d",&q.first,&q.second);
        for(int j=1;j<p;j++)inner[i]+=abs(pts[i][j].first-pts[i][j-1].first)+abs(pts[i][j].second-pts[i][j-1].second);
    }
    int best=0;
    for(int i=1;i<T;i++)if(S[i]>S[best])best=i;
    int bm=0;long long bd=1e18;
    for(int i=0;i<M;i++){
        long long d=abs(mp[i].first-pts[best][0].first)+abs(mp[i].second-pts[best][0].second);
        if(d<bd){bd=d;bm=i;}
    }
    int cx=mp[bm].first,cy=mp[bm].second;
    vector<char> body((size_t)W*H,0);
    vector<pair<int,int>> stk;
    stk.push_back({cx,cy});
    body[(size_t)cy*W+cx]=1;
    vector<char> moves;
    vector<int> done;
    vector<char> used(T,0);
    vector<int> stamp((size_t)W*H,0),par((size_t)W*H,-1);
    int st=0;
    const int dx[4]={1,-1,0,0},dy[4]={0,0,1,-1};
    const char dc[4]={'R','L','U','D'};
    vector<int> q;
    bool stop=false;
    auto navigate=[&](int tx,int ty)->bool{
        while(!(cx==tx&&cy==ty)){
            if((int)moves.size()>=L)return false;
            st++;
            q.clear();
            int s=cy*W+cx,t=ty*W+tx;
            q.push_back(s);stamp[s]=st;par[s]=-1;
            bool found=false;
            for(size_t h=0;h<q.size()&&!found;h++){
                int c=q[h],x=c%W,y=c/W;
                for(int d=0;d<4;d++){
                    int nx=x+dx[d],ny=y+dy[d];
                    if(nx<0||ny<0||nx>=W||ny>=H)continue;
                    int n=ny*W+nx;
                    if(stamp[n]==st||isMount[n]||body[n])continue;
                    stamp[n]=st;par[n]=c;q.push_back(n);
                    if(n==t){found=true;break;}
                }
            }
            if(!found){
                if(stk.size()<=1)return false;
                auto lst=stk.back();stk.pop_back();
                body[(size_t)lst.second*W+lst.first]=0;
                auto pr=stk.back();
                for(int d=0;d<4;d++)if(lst.first+dx[d]==pr.first&&lst.second+dy[d]==pr.second){moves.push_back(dc[d]);break;}
                cx=pr.first;cy=pr.second;
                continue;
            }
            vector<int> path;
            for(int c=t;c!=s;c=par[c])path.push_back(c);
            reverse(path.begin(),path.end());
            for(int c:path){
                if((int)moves.size()>=L)return false;
                int x=c%W,y=c/W;
                for(int d=0;d<4;d++)if(cx+dx[d]==x&&cy+dy[d]==y){moves.push_back(dc[d]);break;}
                cx=x;cy=y;
                stk.push_back({x,y});body[c]=1;
            }
        }
        return true;
    };
    while(!stop){
        int rem=L-(int)moves.size();
        int bi=-1;double bs=-1;
        for(int i=0;i<T;i++){
            if(used[i])continue;
            long long c=abs(cx-pts[i][0].first)+abs(cy-pts[i][0].second)+inner[i];
            if(c>rem)continue;
            double r=(double)S[i]/(c+1);
            if(r>bs){bs=r;bi=i;}
        }
        if(bi<0)break;
        used[bi]=1;
        size_t cp=moves.size();
        bool ok=true;
        for(auto&p:pts[bi]){
            if(!navigate(p.first,p.second)){ok=false;break;}
        }
        if(ok)done.push_back(bi);
        else{moves.resize(cp);stop=true;}
    }
    if(done.empty()){
        done.push_back(0);
        if(moves.empty())moves.push_back('W');
    }
    if(moves.empty())moves.push_back('W');
    printf("1\n%d %d %d %d\n",mp[bm].first,mp[bm].second,(int)done.size(),(int)moves.size());
    for(size_t i=0;i<done.size();i++)printf("%d%c",done[i],i+1<done.size()?' ':'\n');
    for(size_t i=0;i<moves.size();i++)printf("%c%c",moves[i],i+1<moves.size()?' ':'\n');
}
