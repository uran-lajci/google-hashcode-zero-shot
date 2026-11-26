#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static int wrapL(ll x){ x=((x+648000)%1296000+1296000)%1296000-648000; return (int)x; }
int main(){
    int T,S;
    scanf("%d %d",&T,&S);
    vector<int> sp(S),sl(S),sv(S),sw(S),sd(S);
    for(int i=0;i<S;i++) scanf("%d %d %d %d %d",&sp[i],&sl[i],&sv[i],&sw[i],&sd[i]);
    int C; scanf("%d",&C);
    vector<int> val(C),rem(C);
    vector<vector<pair<int,int>>> rng(C);
    map<pair<int,int>,int> idm;
    vector<int> lp,ll_;
    vector<vector<pair<int,int>>> ent; // (collection, entry flag index)
    vector<char> flag;
    for(int c=0;c<C;c++){
        int V,L,R; scanf("%d %d %d",&V,&L,&R);
        val[c]=V;
        int cnt=0;
        for(int i=0;i<L;i++){
            int a,b; scanf("%d %d",&a,&b);
            auto key=make_pair(a,b);
            auto it=idm.find(key);
            int id;
            if(it==idm.end()){
                id=lp.size(); idm[key]=id; lp.push_back(a); ll_.push_back(b); ent.emplace_back();
            } else id=it->second;
            if(!ent[id].empty() && ent[id].back().first==c) continue;
            ent[id].push_back({c,(int)flag.size()});
            flag.push_back(0);
            cnt++;
        }
        rem[c]=cnt;
        for(int i=0;i<R;i++){int a,b; scanf("%d %d",&a,&b); rng[c].push_back({a,b});}
    }
    int N=lp.size();
    const int CS=5000;
    const int NR=612000/CS+2, NC=1296000/CS+1;
    vector<vector<int>> cells((size_t)NR*NC);
    for(int i=0;i<N;i++){
        int r=(lp[i]+306000)/CS;
        int cc=(ll_[i]+648000)/CS;
        cells[(size_t)r*NC+cc].push_back(i);
    }
    vector<char> dead(N,0);
    vector<int> cp=sp,cl=sl,cv=sv,pp(S,0),pl(S,0),pt(S,0);
    ll total=(ll)S*T;
    int stride=(int)max(1LL,(total+14999999)/15000000);
    vector<array<int,4>> out;
    for(int t=0;t<T;t++){
        for(int s=0;s<S;s++){
            int d=sd[s];
            int st=min(stride,max(1,d/500));
            if(t%st==0){
                int lo=cp[s]-d,hi=cp[s]+d;
                int r0=max(lo,-306000),r1=min(hi,306000);
                if(r0<=r1){
                    int ra=(r0+306000)/CS,rb=(r1+306000)/CS;
                    int ca=(int)(((ll)cl[s]-d+648000));
                    int cb=(int)(((ll)cl[s]+d+648000));
                    // lon cell range with wrap
                    double best=0; int bi=-1;
                    int dt=t-pt[s];
                    ll lim=(ll)sw[s]*dt;
                    for(int r=ra;r<=rb;r++){
                        for(int k=floor((double)ca/CS);k<=floor((double)cb/CS);k++){
                            int cc=((k%NC)+NC)%NC;
                            // careful: NC=260+1 cells not exactly periodic; handle by raw lon check below
                            auto &v=cells[(size_t)r*NC+cc];
                            for(int id:v){
                                if(dead[id]) continue;
                                int dphi=lp[id]-cp[s];
                                if(dphi<-d||dphi>d) continue;
                                int dl=wrapL((ll)ll_[id]-cl[s]);
                                if(dl<-d||dl>d) continue;
                                if(abs((ll)dphi-pp[s])>lim||abs((ll)dl-pl[s])>lim) continue;
                                double w=0; bool alive=false;
                                for(auto &e:ent[id]){
                                    if(flag[e.second]) continue;
                                    int c=e.first;
                                    if(rem[c]<=0) continue;
                                    alive=true;
                                    bool in=false;
                                    for(auto &q:rng[c]) if(q.first<=t&&t<=q.second){in=true;break;}
                                    if(in) w+=(double)val[c]/rem[c];
                                }
                                if(!alive){dead[id]=1;continue;}
                                if(w>best){best=w;bi=id;}
                            }
                        }
                    }
                    if(bi>=0){
                        int id=bi;
                        for(auto &e:ent[id]){
                            if(flag[e.second]) continue;
                            int c=e.first;
                            if(rem[c]<=0) continue;
                            bool in=false;
                            for(auto &q:rng[c]) if(q.first<=t&&t<=q.second){in=true;break;}
                            if(in){flag[e.second]=1;rem[c]--;}
                        }
                        pp[s]=lp[id]-cp[s];
                        pl[s]=wrapL((ll)ll_[id]-cl[s]);
                        pt[s]=t;
                        out.push_back({lp[id],ll_[id],t,s});
                    }
                }
            }
            // advance
            int nl=cp[s]+cv[s];
            if(nl>324000){
                cp[s]=648000-nl; cv[s]=-cv[s]; cl[s]=wrapL((ll)cl[s]-15-648000);
            } else if(nl<-324000){
                cp[s]=-648000-nl; cv[s]=-cv[s]; cl[s]=wrapL((ll)cl[s]-15-648000);
            } else { cp[s]=nl; cl[s]=wrapL((ll)cl[s]-15); }
        }
    }
    printf("%d\n",(int)out.size());
    for(auto &o:out) printf("%d %d %d %d\n",o[0],o[1],o[2],o[3]);
}
