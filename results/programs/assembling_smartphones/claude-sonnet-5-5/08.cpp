#include <bits/stdc++.h>
using namespace std;
int W,H,R,M,T,L;
struct Arm{ vector<int> path; int task=-1; int idx=0; vector<int> done; string mv; bool fin=false; int mount; };
int main(){
    scanf("%d %d %d %d %d %d",&W,&H,&R,&M,&T,&L);
    vector<int> mx(M),my(M);
    vector<char> isMount((size_t)W*H,0);
    for(int i=0;i<M;i++){scanf("%d %d",&mx[i],&my[i]);isMount[(size_t)my[i]*W+mx[i]]=1;}
    vector<int> sc(T);
    vector<vector<pair<int,int>>> pts(T);
    vector<long long> internal(T,0);
    for(int i=0;i<T;i++){
        int p;scanf("%d %d",&sc[i],&p);
        pts[i].resize(p);
        for(auto&q:pts[i])scanf("%d %d",&q.first,&q.second);
        for(int j=1;j<p;j++)internal[i]+=abs(pts[i][j].first-pts[i][j-1].first)+abs(pts[i][j].second-pts[i][j-1].second);
    }
    vector<char> taken(T,0);
    vector<short> owner((size_t)W*H,-1);
    vector<int> order(T);
    iota(order.begin(),order.end(),0);
    stable_sort(order.begin(),order.end(),[&](int a,int b){return sc[a]>sc[b];});
    vector<Arm> arms;
    vector<char> usedM(M,0);
    for(int k=0;k<T&&(int)arms.size()<R;k++){
        int t=order[k];
        int best=-1;long long bd=LLONG_MAX;
        for(int m=0;m<M;m++){
            if(usedM[m])continue;
            long long d=abs(mx[m]-pts[t][0].first)+abs(my[m]-pts[t][0].second);
            if(d<bd){bd=d;best=m;}
        }
        if(best<0)break;
        usedM[best]=1;
        Arm a;a.mount=best;a.path.push_back(my[best]*W+mx[best]);
        a.task=t;a.idx=0;taken[t]=1;
        arms.push_back(a);
    }
    // fill remaining arms with other mounts
    for(int m=0;m<M&&(int)arms.size()<R;m++){
        if(usedM[m])continue;
        usedM[m]=1;
        Arm a;a.mount=m;a.path.push_back(my[m]*W+mx[m]);
        arms.push_back(a);
    }
    int dxs[4]={1,-1,0,0},dys[4]={0,0,1,-1};
    char ch[4]={'R','L','U','D'};
    auto assign=[&](Arm&a,int s){
        int cx=a.path.back()%W,cy=a.path.back()/W;
        int best=-1;double bv=-1;
        for(int t=0;t<T;t++){
            if(taken[t])continue;
            long long c=abs(cx-pts[t][0].first)+abs(cy-pts[t][0].second)+internal[t];
            if(c>L-s)continue;
            double v=(double)sc[t]/(c+1);
            if(v>bv){bv=v;best=t;}
        }
        if(best<0){a.fin=true;a.task=-1;return;}
        taken[best]=1;a.task=best;a.idx=0;
    };
    auto advance=[&](Arm&a,int s){
        while(true){
            if(a.task<0){
                if(a.fin)return;
                assign(a,s);
                if(a.task<0)return;
            }
            int c=a.path.back();
            auto&q=pts[a.task][a.idx];
            if(q.second*W+q.first==c){
                a.idx++;
                if(a.idx==(int)pts[a.task].size()){
                    a.done.push_back(a.task);a.task=-1;
                }
            } else return;
        }
    };
    for(auto&a:arms)advance(a,0);
    for(int s=0;s<L;s++){
        for(int i=0;i<(int)arms.size();i++){
            Arm&a=arms[i];
            char mvc='W';
            if(a.task>=0){
                int c=a.path.back();
                int cx=c%W,cy=c/W;
                auto&q=pts[a.task][a.idx];
                int dx=q.first-cx,dy=q.second-cy;
                int cand[2];int nc=0;
                int hx=dx>0?0:(dx<0?1:-1),vy=dy>0?2:(dy<0?3:-1);
                if(abs(dx)>=abs(dy)){ if(hx>=0)cand[nc++]=hx; if(vy>=0)cand[nc++]=vy;}
                else { if(vy>=0)cand[nc++]=vy; if(hx>=0)cand[nc++]=hx;}
                bool moved=false;
                for(int k=0;k<nc&&!moved;k++){
                    int d=cand[k];
                    int nx=cx+dxs[d],ny=cy+dys[d];
                    if(nx<0||ny<0||nx>=W||ny>=H)continue;
                    int nid=ny*W+nx;
                    if(isMount[nid]||owner[nid]!=-1)continue;
                    owner[nid]=i;a.path.push_back(nid);mvc=ch[d];moved=true;
                }
                if(!moved&&a.path.size()>1){
                    int prev=a.path[a.path.size()-2];
                    owner[c]=-1;
                    a.path.pop_back();
                    int px=prev%W,py=prev/W;
                    for(int d=0;d<4;d++)if(cx+dxs[d]==px&&cy+dys[d]==py)mvc=ch[d];
                }
            }
            a.mv.push_back(mvc);
            advance(a,s+1);
        }
    }
    vector<int> out;
    for(int i=0;i<(int)arms.size();i++)if(!arms[i].done.empty())out.push_back(i);
    printf("%d\n",(int)out.size());
    for(int i:out){
        Arm&a=arms[i];
        printf("%d %d %d %d\n",mx[a.mount],my[a.mount],(int)a.done.size(),(int)a.mv.size());
        for(size_t k=0;k<a.done.size();k++)printf(k?" %d":"%d",a.done[k]);
        printf("\n");
        for(size_t k=0;k<a.mv.size();k++){if(k)putchar(' ');putchar(a.mv[k]);}
        printf("\n");
    }
}
