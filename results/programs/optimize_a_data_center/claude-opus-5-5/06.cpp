#include <bits/stdc++.h>
using namespace std;

int R,S,U,P,M;
vector<int> Z,C,Rw,Sl,Pl;
vector<long long> rc, tot, gcv;

long long computeGC(int p){
    long long mx=0; long long* a=&rc[(size_t)p*R];
    for(int r=0;r<R;r++) if(a[r]>mx) mx=a[r];
    return tot[p]-mx;
}

int main(){
    if(scanf("%d %d %d %d %d",&R,&S,&U,&P,&M)!=5) return 0;
    vector<char> bad((size_t)R*S,0);
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[(size_t)r*S+s]=1;}
    Z.resize(M);C.resize(M);Rw.assign(M,-1);Sl.assign(M,-1);Pl.assign(M,-1);
    for(int i=0;i<M;i++) scanf("%d %d",&Z[i],&C[i]);
    // free segments
    vector<set<pair<int,int>>> seg(R);
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bad[(size_t)r*S+s]){s++;continue;}
            int t=s; while(t<S && !bad[(size_t)r*S+t]) t++;
            seg[r].insert({t-s,s}); s=t;
        }
    }
    vector<int> ord(M); iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        long long l=(long long)C[a]*Z[b], r=(long long)C[b]*Z[a];
        if(l!=r) return l>r;
        if(C[a]!=C[b]) return C[a]>C[b];
        return a<b;});
    set<pair<long long,int>> rowset; vector<long long> rowcap(R,0);
    for(int r=0;r<R;r++) rowset.insert({0,r});
    int minFail=INT_MAX;
    for(int k:ord){
        int z=Z[k]; if(z>=minFail) continue;
        bool ok=false;
        for(auto it=rowset.begin();it!=rowset.end();++it){
            int r=it->second;
            auto sit=seg[r].lower_bound({z,INT_MIN});
            if(sit==seg[r].end()) continue;
            int len=sit->first, st=sit->second;
            seg[r].erase(sit);
            if(len>z) seg[r].insert({len-z,st+z});
            Rw[k]=r; Sl[k]=st;
            rowset.erase(it);
            rowcap[r]+=C[k]; rowset.insert({rowcap[r],r});
            ok=true; break;
        }
        if(!ok) minFail=z;
    }
    // pool assignment
    rc.assign((size_t)P*R,0); tot.assign(P,0); gcv.assign(P,0);
    vector<int> placed;
    for(int k=0;k<M;k++) if(Rw[k]>=0) placed.push_back(k);
    sort(placed.begin(),placed.end(),[&](int a,int b){ if(C[a]!=C[b]) return C[a]>C[b]; return a<b;});
    set<pair<long long,int>> pset;
    for(int p=0;p<P;p++) pset.insert({0,p});
    vector<long long> mxr(P,0);
    for(int k:placed){
        auto it=pset.begin(); int p=it->second; pset.erase(it);
        Pl[k]=p; size_t id=(size_t)p*R+Rw[k];
        rc[id]+=C[k]; tot[p]+=C[k]; if(rc[id]>mxr[p]) mxr[p]=rc[id];
        gcv[p]=tot[p]-mxr[p]; pset.insert({gcv[p],p});
    }
    // structures for local search
    vector<vector<int>> poolSrv(P); vector<int> pidx(M,-1);
    for(int k:placed){pidx[k]=poolSrv[Pl[k]].size(); poolSrv[Pl[k]].push_back(k);}
    vector<vector<int>> plBy(S+1), unBy(S+1); vector<int> bidx(M,-1);
    vector<int> plist; vector<int> lidx(M,-1);
    for(int k=0;k<M;k++){
        if(Rw[k]>=0){bidx[k]=plBy[Z[k]].size();plBy[Z[k]].push_back(k);lidx[k]=plist.size();plist.push_back(k);}
        else {bidx[k]=unBy[Z[k]].size();unBy[Z[k]].push_back(k);}
    }
    auto remPool=[&](int k){int p=Pl[k];int i=pidx[k];int last=poolSrv[p].back();poolSrv[p][i]=last;pidx[last]=i;poolSrv[p].pop_back();};
    auto addPool=[&](int k,int p){Pl[k]=p;pidx[k]=poolSrv[p].size();poolSrv[p].push_back(k);};
    auto updSet=[&](int p,long long ng){pset.erase({gcv[p],p});gcv[p]=ng;pset.insert({ng,p});};

    if(!plist.empty() && P>=1){
        mt19937_64 rng(12345);
        long long iters=min<long long>(30000000LL, 1200000000LL/(4LL*R+60));
        auto t0=chrono::steady_clock::now();
        for(long long it=0;it<iters;it++){
            if((it&1023)==0){
                double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
                if(el>9.3) break;
            }
            int a=pset.begin()->second;
            int k;
            if((rng()&1) && !poolSrv[a].empty()) k=poolSrv[a][rng()%poolSrv[a].size()];
            else k=plist[rng()%plist.size()];
            int type=rng()%4;
            if(type==0||type==1){
                int l=plist[rng()%plist.size()];
                int pk=Pl[k], pl=Pl[l];
                if(pk==pl) continue;
                long long oldmin=min(gcv[pk],gcv[pl]);
                if(type==0){ // swap pools
                    size_t ik=(size_t)pk*R, il=(size_t)pl*R;
                    rc[ik+Rw[k]]-=C[k]; rc[ik+Rw[l]]+=C[l]; tot[pk]+=C[l]-C[k];
                    rc[il+Rw[l]]-=C[l]; rc[il+Rw[k]]+=C[k]; tot[pl]+=C[k]-C[l];
                    long long nk=computeGC(pk), nl=computeGC(pl);
                    if(min(nk,nl)>=oldmin){
                        remPool(k);remPool(l);addPool(k,pl);addPool(l,pk);
                        updSet(pk,nk);updSet(pl,nl);
                    } else {
                        rc[ik+Rw[k]]+=C[k]; rc[ik+Rw[l]]-=C[l]; tot[pk]-=C[l]-C[k];
                        rc[il+Rw[l]]+=C[l]; rc[il+Rw[k]]-=C[k]; tot[pl]-=C[k]-C[l];
                    }
                } else { // move l into pk
                    size_t ik=(size_t)pk*R, il=(size_t)pl*R;
                    rc[il+Rw[l]]-=C[l]; tot[pl]-=C[l];
                    rc[ik+Rw[l]]+=C[l]; tot[pk]+=C[l];
                    long long nk=computeGC(pk), nl=computeGC(pl);
                    if(min(nk,nl)>=oldmin){
                        remPool(l);addPool(l,pk);
                        updSet(pk,nk);updSet(pl,nl);
                    } else {
                        rc[il+Rw[l]]+=C[l]; tot[pl]+=C[l];
                        rc[ik+Rw[l]]-=C[l]; tot[pk]-=C[l];
                    }
                }
            } else if(type==2){
                auto &b=plBy[Z[k]]; if(b.size()<2) continue;
                int l=b[rng()%b.size()];
                if(Rw[l]==Rw[k]) continue;
                int pk=Pl[k], pl=Pl[l]; int rk=Rw[k], rl=Rw[l];
                size_t ik=(size_t)pk*R, il=(size_t)pl*R;
                rc[ik+rk]-=C[k]; rc[ik+rl]+=C[k];
                rc[il+rl]-=C[l]; rc[il+rk]+=C[l];
                if(pk==pl){
                    long long n=computeGC(pk);
                    if(n>=gcv[pk]){ updSet(pk,n); swap(Rw[k],Rw[l]); swap(Sl[k],Sl[l]); }
                    else { rc[ik+rk]+=C[k]; rc[ik+rl]-=C[k]; rc[il+rl]+=C[l]; rc[il+rk]-=C[l]; }
                } else {
                    long long oldmin=min(gcv[pk],gcv[pl]);
                    long long nk=computeGC(pk), nl=computeGC(pl);
                    if(min(nk,nl)>=oldmin){ updSet(pk,nk);updSet(pl,nl); swap(Rw[k],Rw[l]); swap(Sl[k],Sl[l]); }
                    else { rc[ik+rk]+=C[k]; rc[ik+rl]-=C[k]; rc[il+rl]+=C[l]; rc[il+rk]-=C[l]; }
                }
            } else {
                auto &ub=unBy[Z[k]]; if(ub.empty()) continue;
                int u=ub[rng()%ub.size()];
                if(C[u]<C[k]) continue;
                int p=Pl[k]; size_t ip=(size_t)p*R+Rw[k];
                rc[ip]+=C[u]-C[k]; tot[p]+=C[u]-C[k];
                long long n=computeGC(p);
                if(n>=gcv[p]){
                    updSet(p,n);
                    // swap roles
                    Rw[u]=Rw[k]; Sl[u]=Sl[k]; Rw[k]=-1; Sl[k]=-1;
                    remPool(k); addPool(u,p); Pl[k]=-1;
                    int z=Z[k];
                    int bi=bidx[k], ui=bidx[u];
                    plBy[z][bi]=u; unBy[z][ui]=k; bidx[u]=bi; bidx[k]=ui;
                    int li=lidx[k]; plist[li]=u; lidx[u]=li; lidx[k]=-1;
                } else { rc[ip]-=C[u]-C[k]; tot[p]-=C[u]-C[k]; }
            }
        }
    }
    string out; out.reserve((size_t)M*14);
    char buf[64];
    for(int k=0;k<M;k++){
        if(Rw[k]<0||Pl[k]<0) out+="x\n";
        else { snprintf(buf,sizeof buf,"%d %d %d\n",Rw[k],Sl[k],Pl[k]); out+=buf; }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
