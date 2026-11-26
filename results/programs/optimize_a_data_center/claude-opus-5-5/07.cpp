#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,S,U,P,M;
vector<int> sz, cap, srow, sslot, spool;
vector<vector<ll>> rowcap;
vector<ll> tot, gcv;
set<pair<ll,int>> gset;
map<ll,int> gcnt;
vector<vector<int>> members;
vector<int> midx;

ll computeGC(int p){
    ll mx=0; const vector<ll>&rc=rowcap[p];
    for(int r=0;r<R;r++) if(rc[r]>mx) mx=rc[r];
    return tot[p]-mx;
}
void removeG(int p){
    gset.erase({gcv[p],p});
    auto it=gcnt.find(gcv[p]); if(--it->second==0) gcnt.erase(it);
}
void insertG(int p){
    gset.insert({gcv[p],p}); gcnt[gcv[p]]++;
}
pair<ll,int> curV(){
    auto it=gcnt.begin(); return {it->first,-it->second};
}
void addMember(int p,int s){midx[s]=members[p].size();members[p].push_back(s);}
void remMember(int p,int s){
    int i=midx[s]; int last=members[p].back();
    members[p][i]=last; midx[last]=i; members[p].pop_back();
}

struct Ch{int p,r;ll d;};
// apply changes; returns whether accepted (reverts if not)
bool tryChanges(Ch* ch,int n){
    pair<ll,int> old=curV();
    int aff[4];int na=0;
    for(int i=0;i<n;i++){bool f=false;for(int j=0;j<na;j++)if(aff[j]==ch[i].p)f=true;if(!f)aff[na++]=ch[i].p;}
    for(int i=0;i<n;i++){rowcap[ch[i].p][ch[i].r]+=ch[i].d;tot[ch[i].p]+=ch[i].d;}
    for(int j=0;j<na;j++){int p=aff[j];removeG(p);gcv[p]=computeGC(p);insertG(p);}
    pair<ll,int> nv=curV();
    if(nv>=old) return true;
    for(int i=0;i<n;i++){rowcap[ch[i].p][ch[i].r]-=ch[i].d;tot[ch[i].p]-=ch[i].d;}
    for(int j=0;j<na;j++){int p=aff[j];removeG(p);gcv[p]=computeGC(p);insertG(p);}
    return false;
}

int main(){
    auto t0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %d",&R,&S,&U,&P,&M)!=5) return 0;
    vector<vector<char>> bl(R,vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bl[r][s]=1;}
    sz.resize(M);cap.resize(M);
    for(int i=0;i<M;i++)scanf("%d %d",&sz[i],&cap[i]);
    srow.assign(M,-1);sslot.assign(M,-1);spool.assign(M,-1);
    // segments
    vector<vector<pair<int,int>>> segs(R);
    vector<int> maxSeg(R,0);
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bl[r][s]){s++;continue;}
            int st=s;while(s<S&&!bl[r][s])s++;
            segs[r].push_back({st,s-st});maxSeg[r]=max(maxSeg[r],s-st);
        }
    }
    vector<int> ord(M);iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        ll l=(ll)cap[a]*sz[b], rr=(ll)cap[b]*sz[a];
        if(l!=rr) return l>rr;
        if(cap[a]!=cap[b]) return cap[a]>cap[b];
        return a<b;});
    vector<ll> rcap(R,0);
    set<pair<ll,int>> rset;
    for(int r=0;r<R;r++) if(maxSeg[r]>0) rset.insert({0,r});
    int minFail=INT_MAX;
    for(int s:ord){
        int z=sz[s];
        if(z>=minFail) continue;
        int chosen=-1;
        for(auto &pr:rset){ if(maxSeg[pr.second]>=z){chosen=pr.second;break;} }
        if(chosen<0){minFail=z;continue;}
        int r=chosen;
        int bi=-1;
        for(int i=0;i<(int)segs[r].size();i++){
            if(segs[r][i].second>=z && (bi<0||segs[r][i].second<segs[r][bi].second)) bi=i;
        }
        srow[s]=r;sslot[s]=segs[r][bi].first;
        segs[r][bi].first+=z;segs[r][bi].second-=z;
        if(segs[r][bi].second==0) segs[r].erase(segs[r].begin()+bi);
        int mx=0;for(auto&q:segs[r])mx=max(mx,q.second);maxSeg[r]=mx;
        rset.erase({rcap[r],r});rcap[r]+=cap[s];
        if(mx>0) rset.insert({rcap[r],r});
    }
    // pool assignment
    rowcap.assign(P,vector<ll>(R,0));tot.assign(P,0);gcv.assign(P,0);
    members.assign(P,{});midx.assign(M,-1);
    vector<int> placed;
    for(int i=0;i<M;i++) if(srow[i]>=0) placed.push_back(i);
    sort(placed.begin(),placed.end(),[&](int a,int b){if(cap[a]!=cap[b])return cap[a]>cap[b];return a<b;});
    for(int p=0;p<P;p++) insertG(p);
    for(int s:placed){
        int p=gset.begin()->second;
        removeG(p);
        rowcap[p][srow[s]]+=cap[s];tot[p]+=cap[s];spool[s]=p;addMember(p,s);
        gcv[p]=computeGC(p);insertG(p);
    }
    vector<vector<int>> bySize(S+1);
    for(int i=0;i<M;i++) bySize[sz[i]].push_back(i);
    // local search
    if(!placed.empty()){
        mt19937_64 rng(12345);
        long long iters=(long long)(2e7*16.0/(R+16));
        if(iters<200000) iters=200000;
        for(long long it=0;it<iters;it++){
            if((it&1023)==0){
                double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
                if(el>9.0) break;
            }
            int type=rng()%4;
            int worst=gset.begin()->second;
            if(type==0){ // pool swap
                int a;
                if((rng()&1)&&!members[worst].empty()) a=members[worst][rng()%members[worst].size()];
                else a=rng()%M;
                int b=rng()%M;
                if(spool[a]<0||spool[b]<0||spool[a]==spool[b]) continue;
                int pa=spool[a],pb=spool[b];
                Ch ch[4]={{pa,srow[a],-(ll)cap[a]},{pa,srow[b],(ll)cap[b]},{pb,srow[b],-(ll)cap[b]},{pb,srow[a],(ll)cap[a]}};
                if(tryChanges(ch,4)){
                    remMember(pa,a);remMember(pb,b);
                    spool[a]=pb;spool[b]=pa;addMember(pb,a);addMember(pa,b);
                }
            } else if(type==1){ // move
                int a=rng()%M; if(spool[a]<0) continue;
                int q=(rng()&1)?worst:(int)(rng()%P);
                int pa=spool[a]; if(q==pa) continue;
                Ch ch[2]={{pa,srow[a],-(ll)cap[a]},{q,srow[a],(ll)cap[a]}};
                if(tryChanges(ch,2)){
                    remMember(pa,a);spool[a]=q;addMember(q,a);
                }
            } else if(type==2){ // row swap same size
                int a;
                if((rng()&1)&&!members[worst].empty()) a=members[worst][rng()%members[worst].size()];
                else a=rng()%M;
                if(spool[a]<0) continue;
                auto&v=bySize[sz[a]]; int b=v[rng()%v.size()];
                if(spool[b]<0||srow[a]==srow[b]) continue;
                int pa=spool[a],pb=spool[b];
                Ch ch[4]={{pa,srow[a],-(ll)cap[a]},{pa,srow[b],(ll)cap[a]},{pb,srow[b],-(ll)cap[b]},{pb,srow[a],(ll)cap[b]}};
                if(tryChanges(ch,4)){
                    swap(srow[a],srow[b]);swap(sslot[a],sslot[b]);
                }
            } else { // unplaced swap
                int u=rng()%M; if(spool[u]>=0) continue;
                auto&v=bySize[sz[u]]; int b=v[rng()%v.size()];
                if(spool[b]<0) continue;
                int pb=spool[b];
                Ch ch[2]={{pb,srow[b],-(ll)cap[b]},{pb,srow[b],(ll)cap[u]}};
                if(tryChanges(ch,2)){
                    remMember(pb,b);
                    srow[u]=srow[b];sslot[u]=sslot[b];spool[u]=pb;addMember(pb,u);
                    srow[b]=-1;sslot[b]=-1;spool[b]=-1;
                }
            }
        }
    }
    string out;out.reserve((size_t)M*16);
    char buf[64];
    for(int i=0;i<M;i++){
        if(spool[i]<0||srow[i]<0) out+="x\n";
        else{snprintf(buf,sizeof(buf),"%d %d %d\n",srow[i],sslot[i],spool[i]);out+=buf;}
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
