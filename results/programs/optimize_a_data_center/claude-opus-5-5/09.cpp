#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,S,U,P,M;
vector<int> Z,C,srow,sslot,spool;
vector<ll> rowcap,total,maxrow,gcv;
vector<vector<int>> members;
vector<int> mpos;
set<pair<ll,int>> poolSet;

inline void recompute(int p){
    ll mx=0; ll* rc=&rowcap[(size_t)p*R];
    for(int r=0;r<R;r++) if(rc[r]>mx) mx=rc[r];
    maxrow[p]=mx;
}
inline ll gcOf(int p){return total[p]-maxrow[p];}

void removeMember(int s){
    int p=spool[s]; int i=mpos[s]; int last=members[p].back();
    members[p][i]=last; mpos[last]=i; members[p].pop_back();
}
void addMember(int s,int p){
    spool[s]=p; mpos[s]=members[p].size(); members[p].push_back(s);
}

int main(){
    auto T0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %d",&R,&S,&U,&P,&M)!=5) return 0;
    vector<vector<char>> blocked(R, vector<char>(S,0));
    for(int i=0;i<U;i++){int a,b;scanf("%d %d",&a,&b); if(a>=0&&a<R&&b>=0&&b<S) blocked[a][b]=1;}
    Z.resize(M);C.resize(M);
    for(int i=0;i<M;i++) scanf("%d %d",&Z[i],&C[i]);
    srow.assign(M,-1);sslot.assign(M,-1);spool.assign(M,-1);
    // segments
    vector<vector<pair<int,int>>> segs(R);
    vector<int> rowMax(R,0);
    multiset<int> maxes;
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(blocked[r][s]){s++;continue;}
            int t=s; while(t<S&&!blocked[r][t]) t++;
            segs[r].push_back({s,t-s}); rowMax[r]=max(rowMax[r],t-s); s=t;
        }
        maxes.insert(rowMax[r]);
    }
    vector<ll> rowTot(R,0);
    set<pair<ll,int>> rowsByCap;
    for(int r=0;r<R;r++) rowsByCap.insert({0,r});
    vector<int> ord(M); iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        ll l=(ll)C[a]*Z[b], rr=(ll)C[b]*Z[a];
        if(l!=rr) return l>rr;
        if(C[a]!=C[b]) return C[a]>C[b];
        return a<b;});
    vector<int> placedList;
    for(int s:ord){
        int z=Z[s];
        if(maxes.empty()||z>*maxes.rbegin()) continue;
        int row=-1;
        for(auto &pr:rowsByCap){ if(rowMax[pr.second]>=z){row=pr.second;break;} }
        if(row<0) continue;
        int bi=-1,bl=INT_MAX;
        for(int i=0;i<(int)segs[row].size();i++){
            int l=segs[row][i].second;
            if(l>=z&&l<bl){bl=l;bi=i;}
        }
        if(bi<0) continue;
        srow[s]=row; sslot[s]=segs[row][bi].first;
        segs[row][bi].first+=z; segs[row][bi].second-=z;
        if(segs[row][bi].second==0) segs[row].erase(segs[row].begin()+bi);
        maxes.erase(maxes.find(rowMax[row]));
        int mx=0; for(auto &sg:segs[row]) mx=max(mx,sg.second);
        rowMax[row]=mx; maxes.insert(mx);
        rowsByCap.erase({rowTot[row],row}); rowTot[row]+=C[s]; rowsByCap.insert({rowTot[row],row});
        placedList.push_back(s);
    }
    // pool assignment
    rowcap.assign((size_t)P*R,0); total.assign(P,0); maxrow.assign(P,0); gcv.assign(P,0);
    members.assign(P,{}); mpos.assign(M,-1);
    vector<int> pl=placedList;
    sort(pl.begin(),pl.end(),[&](int a,int b){ if(C[a]!=C[b]) return C[a]>C[b]; return a<b;});
    for(int p=0;p<P;p++) poolSet.insert({0,p});
    for(int s:pl){
        int p=poolSet.begin()->second;
        poolSet.erase(poolSet.begin());
        rowcap[(size_t)p*R+srow[s]]+=C[s]; total[p]+=C[s];
        maxrow[p]=max(maxrow[p],rowcap[(size_t)p*R+srow[s]]);
        gcv[p]=gcOf(p);
        addMember(s,p);
        poolSet.insert({gcv[p],p});
    }
    // size buckets
    map<int,vector<int>> bucketMap;
    for(int s:placedList) bucketMap[Z[s]].push_back(s);
    vector<vector<int>*> bucketOf(M,nullptr);
    for(auto &kv:bucketMap) for(int s:kv.second) bucketOf[s]=&kv.second;

    mt19937_64 rng(12345);
    long long iters = 400000000LL/(2LL*R+50);
    if(placedList.empty()||P<1) iters=0;
    auto better=[&](ll oa,ll ob,ll na,ll nb)->bool{ // lexicographic sorted pair >=
        if(oa>ob) swap(oa,ob); if(na>nb) swap(na,nb);
        if(na!=oa) return na>oa; return nb>=ob;
    };
    for(long long it=0;it<iters;it++){
        if((it&4095)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-T0).count();
            if(el>9.0) break;
        }
        int k=rng()%min(P,4);
        auto itp=poolSet.begin(); for(int j=0;j<k;j++) ++itp;
        int w=itp->second;
        int type=rng()%3;
        if(type==0||type==1){
            if(P<2) continue;
            int q=rng()%P; if(q==w) continue;
            if(members[q].empty()) continue;
            int s=members[q][rng()%members[q].size()];
            int t=-1;
            if(type==1){ if(members[w].empty()) continue; t=members[w][rng()%members[w].size()]; }
            ll og1=gcv[q],og2=gcv[w];
            size_t qb=(size_t)q*R, wb=(size_t)w*R;
            rowcap[qb+srow[s]]-=C[s]; total[q]-=C[s];
            rowcap[wb+srow[s]]+=C[s]; total[w]+=C[s];
            if(t>=0){
                rowcap[wb+srow[t]]-=C[t]; total[w]-=C[t];
                rowcap[qb+srow[t]]+=C[t]; total[q]+=C[t];
            }
            ll omq=maxrow[q],omw=maxrow[w];
            recompute(q);recompute(w);
            ll n1=gcOf(q),n2=gcOf(w);
            if(better(og1,og2,n1,n2)){
                poolSet.erase({gcv[q],q}); poolSet.erase({gcv[w],w});
                gcv[q]=n1; gcv[w]=n2;
                poolSet.insert({n1,q}); poolSet.insert({n2,w});
                removeMember(s); addMember(s,w);
                if(t>=0){removeMember(t); addMember(t,q);}
            } else {
                rowcap[qb+srow[s]]+=C[s]; total[q]+=C[s];
                rowcap[wb+srow[s]]-=C[s]; total[w]-=C[s];
                if(t>=0){
                    rowcap[wb+srow[t]]+=C[t]; total[w]+=C[t];
                    rowcap[qb+srow[t]]-=C[t]; total[q]-=C[t];
                }
                maxrow[q]=omq; maxrow[w]=omw;
            }
        } else {
            if(members[w].empty()) continue;
            int s=members[w][rng()%members[w].size()];
            auto &bk=*bucketOf[s];
            int t=bk[rng()%bk.size()];
            if(srow[s]==srow[t]) continue;
            int p1=spool[s],p2=spool[t];
            int rs=srow[s],rt=srow[t];
            size_t b1=(size_t)p1*R,b2=(size_t)p2*R;
            ll og1=gcv[p1],og2=gcv[p2];
            ll om1=maxrow[p1],om2=maxrow[p2];
            rowcap[b1+rs]-=C[s]; rowcap[b1+rt]+=C[s];
            rowcap[b2+rt]-=C[t]; rowcap[b2+rs]+=C[t];
            recompute(p1); if(p2!=p1) recompute(p2);
            ll n1=gcOf(p1),n2=gcOf(p2);
            bool acc;
            if(p1==p2) acc = n1>=og1;
            else acc=better(og1,og2,n1,n2);
            if(acc){
                poolSet.erase({gcv[p1],p1}); if(p2!=p1) poolSet.erase({gcv[p2],p2});
                gcv[p1]=n1; gcv[p2]=n2;
                poolSet.insert({n1,p1}); if(p2!=p1) poolSet.insert({n2,p2});
                swap(srow[s],srow[t]); swap(sslot[s],sslot[t]);
            } else {
                rowcap[b1+rs]+=C[s]; rowcap[b1+rt]-=C[s];
                rowcap[b2+rt]+=C[t]; rowcap[b2+rs]-=C[t];
                maxrow[p1]=om1; maxrow[p2]=om2;
            }
        }
    }
    string out; out.reserve((size_t)M*14);
    char buf[64];
    for(int i=0;i<M;i++){
        if(srow[i]<0||spool[i]<0) out+="x\n";
        else { snprintf(buf,sizeof buf,"%d %d %d\n",srow[i],sslot[i],spool[i]); out+=buf; }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
