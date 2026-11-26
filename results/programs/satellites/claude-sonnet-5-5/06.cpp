#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static int wrapL(ll x){ ll m=1296000; x=((x+648000)%m+m)%m; return (int)(x-648000);}
int main(){
    auto st=chrono::steady_clock::now();
    int T,S;
    scanf("%d %d",&T,&S);
    vector<int> sp(S),sl(S),sv(S),sw(S),sd(S);
    for(int i=0;i<S;i++)scanf("%d %d %d %d %d",&sp[i],&sl[i],&sv[i],&sw[i],&sd[i]);
    int C;scanf("%d",&C);
    vector<int> V(C),rem(C);
    vector<vector<pair<int,int>>> rng(C);
    map<pair<int,int>,int> lid;
    vector<int> LP,LL;
    struct Ent{int c,idx;};
    vector<vector<Ent>> lent;
    vector<int> tmin,tmax;
    vector<vector<char>> done(C);
    for(int c=0;c<C;c++){
        int L,R;scanf("%d %d %d",&V[c],&L,&R);
        vector<int> ids;
        for(int i=0;i<L;i++){
            int p,l;scanf("%d %d",&p,&l);
            auto k=make_pair(p,l);
            auto it=lid.find(k);
            int id;
            if(it==lid.end()){
                id=LP.size();lid[k]=id;LP.push_back(p);LL.push_back(l);
                lent.emplace_back();tmin.push_back(INT_MAX);tmax.push_back(-1);
            } else id=it->second;
            bool dup=false;
            for(int x:ids)if(x==id)dup=true;
            if(!dup)ids.push_back(id);
        }
        for(int i=0;i<R;i++){int a,b;scanf("%d %d",&a,&b);rng[c].push_back({a,b});}
        rem[c]=ids.size();
        done[c].assign(ids.size(),0);
        for(size_t i=0;i<ids.size();i++){
            lent[ids[i]].push_back({c,(int)i});
            if(V[c]>0)for(auto&r:rng[c]){
                tmin[ids[i]]=min(tmin[ids[i]],r.first);
                tmax[ids[i]]=max(tmax[ids[i]],r.second);
            }
        }
    }
    int N=LP.size();
    const int CS=1000,NR=649,NC=1296;
    vector<int> start(NR*NC+1,0),items(N);
    auto cellOf=[&](int i){return ((LP[i]+324000)/CS)*NC+(LL[i]+648000)/CS;};
    for(int i=0;i<N;i++)start[cellOf(i)+1]++;
    for(int i=0;i<NR*NC;i++)start[i+1]+=start[i];
    {
        vector<int> pos(start.begin(),start.end()-1);
        for(int i=0;i<N;i++)items[pos[cellOf(i)]++]=i;
    }
    vector<int> oa(S,0),ob(S,0),tl(S,0);
    vector<array<int,4>> out;
    auto fl=[&](int x){return x>=0?x/CS:-((-x+CS-1)/CS);};
    for(int t=0;t<T;t++){
        if((t&63)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
            if(el>8.5)break;
        }
        for(int s=0;s<S;s++){
            int ph=sp[s],la=sl[s],d=sd[s],w=sw[s];
            ll lim=(ll)w*(t-tl[s]);
            int r0=max(0,(ph-d+324000)/CS),r1=min(NR-1,(ph+d+324000)/CS);
            if(ph-d+324000<0)r0=0;
            int c0=fl(la-d+648000),c1=fl(la+d+648000);
            double best=0;int bi=-1,ba=0,bb=0;
            for(int r=r0;r<=r1;r++){
                for(int c=c0;c<=c1;c++){
                    int cc=((c%NC)+NC)%NC;
                    int cell=r*NC+cc;
                    for(int k=start[cell];k<start[cell+1];k++){
                        int i=items[k];
                        if(t<tmin[i]||t>tmax[i])continue;
                        int dp=LP[i]-ph;
                        if(dp>d||dp<-d)continue;
                        int dl=wrapL((ll)LL[i]-la);
                        if(dl>d||dl<-d)continue;
                        if(abs((ll)dp-oa[s])>lim||abs((ll)dl-ob[s])>lim)continue;
                        double wt=0;
                        for(auto&e:lent[i]){
                            if(V[e.c]==0||rem[e.c]==0||done[e.c][e.idx])continue;
                            bool in=false;
                            for(auto&r2:rng[e.c])if(t>=r2.first&&t<=r2.second){in=true;break;}
                            if(in)wt+=(double)V[e.c]/rem[e.c];
                        }
                        if(wt>best){best=wt;bi=i;ba=dp;bb=dl;}
                    }
                }
            }
            if(bi>=0){
                out.push_back({LP[bi],LL[bi],t,s});
                oa[s]=ba;ob[s]=bb;tl[s]=t;
                for(auto&e:lent[bi]){
                    if(rem[e.c]==0||done[e.c][e.idx])continue;
                    for(auto&r2:rng[e.c])if(t>=r2.first&&t<=r2.second){
                        done[e.c][e.idx]=1;rem[e.c]--;break;
                    }
                }
            }
            // advance satellite
            int np=sp[s]+sv[s];
            ll nl=(ll)sl[s]-15;
            if(np>324000){np=648000-np;sv[s]=-sv[s];nl-=648000;}
            else if(np<-324000){np=-648000-np;sv[s]=-sv[s];nl-=648000;}
            sp[s]=np;sl[s]=wrapL(nl);
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out)printf("%d %d %d %d\n",o[0],o[1],o[2],o[3]);
}
