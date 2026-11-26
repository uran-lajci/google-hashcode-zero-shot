#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int T,S;
struct Sat{ll p,l,v,w,d;ll lp,ll_,lt;};
struct Col{int V;int rem;vector<pair<int,int>> rg;};
int main(){
    scanf("%d",&T);scanf("%d",&S);
    vector<Sat> st(S);
    for(auto&s:st){scanf("%lld %lld %lld %lld %lld",&s.p,&s.l,&s.v,&s.w,&s.d);s.lp=0;s.ll_=0;s.lt=0;}
    int C;scanf("%d",&C);
    vector<Col> cols(C);
    vector<int> needCol;
    vector<int> locA,locB;
    unordered_map<ll,int> mp;
    vector<vector<int>> locNeeds;
    vector<char> done;
    for(int c=0;c<C;c++){
        int V,L,R;scanf("%d %d %d",&V,&L,&R);
        cols[c].V=V;cols[c].rem=L;
        for(int i=0;i<L;i++){
            int a,b;scanf("%d %d",&a,&b);
            ll key=(ll)(a+400000)*3000000+(b+700000);
            auto it=mp.find(key);
            int id;
            if(it==mp.end()){id=locA.size();mp[key]=id;locA.push_back(a);locB.push_back(b);locNeeds.emplace_back();}
            else id=it->second;
            int n=needCol.size();
            needCol.push_back(c);done.push_back(0);
            locNeeds[id].push_back(n);
        }
        for(int i=0;i<R;i++){int a,b;scanf("%d %d",&a,&b);cols[c].rg.push_back({a,b});}
    }
    int NL=locA.size();
    const int LC=1296;
    vector<vector<int>> grid(649*LC);
    vector<int> alive(NL);
    for(int i=0;i<NL;i++){
        alive[i]=locNeeds[i].size();
        int cl=(locA[i]+324000)/1000;
        int cb=(locB[i]+648000)/1000;
        grid[cl*LC+cb].push_back(i);
    }
    auto inRange=[&](int c,int t){
        for(auto&r:cols[c].rg)if(r.first<=t&&t<=r.second)return true;
        return false;
    };
    struct Ph{int a,b,t,s;};
    vector<Ph> out;
    const ll OFF=648000+2000000;
    for(int t=0;t<T;t++){
        for(int s=0;s<S;s++){
            Sat&q=st[s];
            if(NL>0){
                ll r=min<ll>(q.w*(t-q.lt),400);
                r=min(r,2*q.d);
                ll cp=q.p+q.lp,cl=q.l+q.ll_;
                ll alo=max<ll>(-306000,cp-r),ahi=min<ll>(306000,cp+r);
                if(alo<=ahi){
                    ll blo=cl-r,bhi=cl+r;
                    ll k0=(blo+OFF)/1000,k1=(bhi+OFF)/1000;
                    int c0=(alo+324000)/1000,c1=(ahi+324000)/1000;
                    int best=-1;double bs=0;ll bda=0,bdb=0;
                    for(int ca=c0;ca<=c1;ca++)for(ll k=k0;k<=k1;k++){
                        auto&g=grid[ca*LC+(k%LC)];
                        for(int id:g){
                            if(!alive[id])continue;
                            ll da=locA[id]-q.p;
                            ll db=((locB[id]-q.l)%1296000+1296000+648000)%1296000-648000;
                            if(llabs(da)>q.d||llabs(db)>q.d)continue;
                            if(llabs(da-q.lp)>r||llabs(db-q.ll_)>r)continue;
                            double sc=0;
                            for(int n:locNeeds[id]){
                                if(done[n])continue;
                                int c=needCol[n];
                                if(inRange(c,t))sc+=(double)cols[c].V/cols[c].rem+1e-6;
                            }
                            if(sc>bs){bs=sc;best=id;bda=da;bdb=db;}
                        }
                    }
                    if(best>=0){
                        for(int n:locNeeds[best]){
                            if(done[n])continue;
                            int c=needCol[n];
                            if(inRange(c,t)){done[n]=1;cols[c].rem--;alive[best]--;}
                        }
                        out.push_back({locA[best],locB[best],t,s});
                        q.lp=bda;q.ll_=bdb;q.lt=t;
                    }
                }
            }
            // advance
            ll np=q.p+q.v;
            if(np>324000){np=648000-np;q.v=-q.v;}
            else if(np<-324000){np=-648000-np;q.v=-q.v;}
            q.p=np;
            q.l=((q.l-15+648000)%1296000+1296000)%1296000-648000;
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out)printf("%d %d %d %d\n",o.a,o.b,o.t,o.s);
}
