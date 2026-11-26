#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,S,U,P,M;
vector<int> sz, cp, srow, sslot, spool;
vector<ll> rc; // P*R
vector<ll> tot, gcv;
vector<vector<int>> plist;
vector<int> ppos;

ll computeGC(int p,int r1,ll d1,int r2,ll d2){
    ll t=tot[p]+d1+d2; ll mx=LLONG_MIN;
    const ll* a=&rc[(size_t)p*R];
    for(int r=0;r<R;r++){
        ll v=a[r]; if(r==r1)v+=d1; if(r==r2)v+=d2;
        if(v>mx)mx=v;
    }
    return t-mx;
}
void addTo(int p,int s){
    rc[(size_t)p*R+srow[s]]+=cp[s]; tot[p]+=cp[s];
    spool[s]=p; ppos[s]=plist[p].size(); plist[p].push_back(s);
}
void remFrom(int p,int s){
    rc[(size_t)p*R+srow[s]]-=cp[s]; tot[p]-=cp[s];
    int i=ppos[s]; int last=plist[p].back();
    plist[p][i]=last; ppos[last]=i; plist[p].pop_back();
    spool[s]=-1;
}

int main(){
    scanf("%d %d %d %d %d",&R,&S,&U,&P,&M);
    vector<char> un((size_t)R*S,0);
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);un[(size_t)r*S+s]=1;}
    sz.resize(M);cp.resize(M);srow.assign(M,-1);sslot.assign(M,-1);spool.assign(M,-1);ppos.assign(M,-1);
    for(int i=0;i<M;i++)scanf("%d %d",&sz[i],&cp[i]);
    vector<set<pair<int,int>>> seg(R);
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(un[(size_t)r*S+s]){s++;continue;}
            int e=s; while(e<S&&!un[(size_t)r*S+e])e++;
            seg[r].insert({e-s,s}); s=e;
        }
    }
    vector<int> ord(M); iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        ll x=(ll)cp[a]*sz[b], y=(ll)cp[b]*sz[a];
        if(x!=y)return x>y;
        if(sz[a]!=sz[b])return sz[a]>sz[b];
        return a<b;
    });
    set<pair<ll,int>> rows;
    vector<ll> rowcap(R,0);
    for(int r=0;r<R;r++)rows.insert({0,r});
    int failSize=INT_MAX;
    for(int s:ord){
        int z=sz[s]; if(z>=failSize)continue;
        int found=-1;
        for(auto &pr:rows){
            int r=pr.second;
            if(!seg[r].empty()&&seg[r].rbegin()->first>=z){found=r;break;}
        }
        if(found<0){failSize=z;continue;}
        int r=found;
        auto it=seg[r].lower_bound({z,-1});
        int len=it->first,st=it->second; seg[r].erase(it);
        if(len>z)seg[r].insert({len-z,st+z});
        srow[s]=r; sslot[s]=st;
        rows.erase({rowcap[r],r}); rowcap[r]+=cp[s]; rows.insert({rowcap[r],r});
    }
    // pool assignment
    rc.assign((size_t)P*R,0); tot.assign(P,0); gcv.assign(P,0); plist.assign(P,{});
    vector<int> placed;
    for(int i=0;i<M;i++)if(srow[i]>=0)placed.push_back(i);
    sort(placed.begin(),placed.end(),[&](int a,int b){if(cp[a]!=cp[b])return cp[a]>cp[b];return a<b;});
    {
        priority_queue<tuple<ll,ll,int>,vector<tuple<ll,ll,int>>,greater<tuple<ll,ll,int>>> pq;
        for(int p=0;p<P;p++)pq.push({0,0,p});
        for(int s:placed){
            auto [g,t,p]=pq.top();pq.pop();
            addTo(p,s);
            gcv[p]=computeGC(p,-1,0,-1,0);
            pq.push({gcv[p],tot[p],p});
        }
    }
    // local search
    if(P>=2 && !placed.empty()){
        set<pair<ll,int>> gs;
        for(int p=0;p<P;p++)gs.insert({gcv[p],p});
        mt19937_64 rng(12345);
        ll iters=(ll)(2.5e8/(R+20));
        if(iters>20000000)iters=20000000;
        for(ll it=0;it<iters;it++){
            int a;
            if(rng()&1)a=gs.begin()->second; else a=rng()%P;
            int b=rng()%P; if(a==b)continue;
            int type=rng()%2;
            ll ga,gb;
            if(type==0){
                if(plist[b].empty())continue;
                int s=plist[b][rng()%plist[b].size()];
                int r=srow[s]; ll c=cp[s];
                ga=computeGC(a,r,c,-1,0);
                gb=computeGC(b,r,-c,-1,0);
                if(min(ga,gb)>=min(gcv[a],gcv[b])){
                    gs.erase({gcv[a],a});gs.erase({gcv[b],b});
                    remFrom(b,s);addTo(a,s);
                    gcv[a]=ga;gcv[b]=gb;
                    gs.insert({ga,a});gs.insert({gb,b});
                }
            }else{
                if(plist[a].empty()||plist[b].empty())continue;
                int s=plist[a][rng()%plist[a].size()];
                int t=plist[b][rng()%plist[b].size()];
                if(cp[s]==cp[t]&&srow[s]==srow[t])continue;
                ll cs=cp[s],ct=cp[t]; int rs=srow[s],rt=srow[t];
                ga=computeGC(a,rs,-cs,rt,ct);
                gb=computeGC(b,rt,-ct,rs,cs);
                if(min(ga,gb)>=min(gcv[a],gcv[b])){
                    gs.erase({gcv[a],a});gs.erase({gcv[b],b});
                    remFrom(a,s);remFrom(b,t);addTo(a,t);addTo(b,s);
                    gcv[a]=ga;gcv[b]=gb;
                    gs.insert({ga,a});gs.insert({gb,b});
                }
            }
        }
    }
    string out;
    out.reserve((size_t)M*16);
    char buf[64];
    for(int i=0;i<M;i++){
        if(srow[i]<0||spool[i]<0){out+="x\n";}
        else{snprintf(buf,sizeof(buf),"%d %d %d\n",srow[i],sslot[i],spool[i]);out+=buf;}
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
