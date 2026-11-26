#include <bits/stdc++.h>
using namespace std;

int R,S,U,P,M;
vector<int> zs,cs,prow,pslot,pool_;
vector<int> placedList;
vector<long long> rc; // P*R
vector<long long> tot,mx;
long long units=0;
const long long BUDGET=600000000LL;

void recomputeMx(int p){
    long long m=0; for(int r=0;r<R;r++) m=max(m,rc[(size_t)p*R+r]); mx[p]=m; units+=R;
}
void rebuild(){
    fill(rc.begin(),rc.end(),0); fill(tot.begin(),tot.end(),0);
    for(int s:placedList){int p=pool_[s]; rc[(size_t)p*R+prow[s]]+=cs[s]; tot[p]+=cs[s];}
    for(int p=0;p<P;p++) recomputeMx(p);
    units+=placedList.size();
}
inline long long gcOf(int p){return tot[p]-mx[p];}
long long addGc(int p,int r,long long c){units++; return tot[p]+c-max(mx[p],rc[(size_t)p*R+r]+c);}
long long removeGc(int p,int r,long long c){
    units++;
    long long v=rc[(size_t)p*R+r];
    if(v<mx[p]) return tot[p]-c-mx[p];
    long long m=v-c;
    for(int i=0;i<R;i++) if(i!=r) m=max(m,rc[(size_t)p*R+i]);
    units+=R;
    return tot[p]-c-m;
}
long long swapGc(int p,int rRem,long long cRem,int rAdd,long long cAdd){
    long long nt=tot[p]-cRem+cAdd;
    if(rRem==rAdd){
        long long v=rc[(size_t)p*R+rRem]-cRem+cAdd;
        if(v>=mx[p]) {units++; return nt-v;}
        if(rc[(size_t)p*R+rRem]<mx[p]){units++; return nt-mx[p];}
    }
    long long m=0;
    for(int i=0;i<R;i++){
        long long v=rc[(size_t)p*R+i];
        if(i==rRem) v-=cRem;
        if(i==rAdd) v+=cAdd;
        m=max(m,v);
    }
    units+=R;
    return nt-m;
}
void applyMove(int s,int np){
    int op=pool_[s];
    rc[(size_t)op*R+prow[s]]-=cs[s]; tot[op]-=cs[s];
    rc[(size_t)np*R+prow[s]]+=cs[s]; tot[np]+=cs[s];
    pool_[s]=np;
    recomputeMx(op); recomputeMx(np);
}
long long score(){
    long long m=LLONG_MAX; for(int p=0;p<P;p++) m=min(m,gcOf(p)); return m;
}

int main(){
    if(scanf("%d %d %d %d %d",&R,&S,&U,&P,&M)!=5) return 0;
    vector<char> bad((size_t)R*S,0);
    for(int i=0;i<U;i++){int r,s; scanf("%d %d",&r,&s); bad[(size_t)r*S+s]=1;}
    zs.resize(M);cs.resize(M);
    for(int i=0;i<M;i++) scanf("%d %d",&zs[i],&cs[i]);
    prow.assign(M,-1);pslot.assign(M,-1);pool_.assign(M,-1);
    // segments
    vector<vector<pair<int,int>>> segs(R);
    vector<int> rowMax(R,0);
    multiset<int> maxSegs;
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bad[(size_t)r*S+s]){s++;continue;}
            int t=s; while(t<S && !bad[(size_t)r*S+t]) t++;
            segs[r].push_back({s,t-s});
            rowMax[r]=max(rowMax[r],t-s);
            s=t;
        }
        maxSegs.insert(rowMax[r]);
    }
    vector<int> order(M); iota(order.begin(),order.end(),0);
    sort(order.begin(),order.end(),[&](int a,int b){
        long long l=(long long)cs[a]*zs[b], rr=(long long)cs[b]*zs[a];
        if(l!=rr) return l>rr;
        if(cs[a]!=cs[b]) return cs[a]>cs[b];
        return a<b;
    });
    vector<long long> rowCap(R,0);
    set<pair<long long,int>> rowsByCap;
    for(int r=0;r<R;r++) rowsByCap.insert({0,r});
    for(int s:order){
        int z=zs[s];
        if(maxSegs.empty() || z>*maxSegs.rbegin()) continue;
        int chosen=-1;
        for(auto &pr:rowsByCap){ if(rowMax[pr.second]>=z){chosen=pr.second;break;} }
        if(chosen<0) continue;
        int r=chosen;
        int bi=-1;
        for(int i=0;i<(int)segs[r].size();i++){
            if(segs[r][i].second>=z && (bi<0 || segs[r][i].second<segs[r][bi].second)) bi=i;
        }
        prow[s]=r; pslot[s]=segs[r][bi].first;
        segs[r][bi].first+=z; segs[r][bi].second-=z;
        if(segs[r][bi].second==0) segs[r].erase(segs[r].begin()+bi);
        maxSegs.erase(maxSegs.find(rowMax[r]));
        int nm=0; for(auto&sg:segs[r]) nm=max(nm,sg.second);
        rowMax[r]=nm; maxSegs.insert(nm);
        rowsByCap.erase({rowCap[r],r});
        rowCap[r]+=cs[s];
        rowsByCap.insert({rowCap[r],r});
    }
    for(int i=0;i<M;i++) if(prow[i]>=0) placedList.push_back(i);
    sort(placedList.begin(),placedList.end(),[&](int a,int b){ if(cs[a]!=cs[b]) return cs[a]>cs[b]; return a<b;});
    rc.assign((size_t)P*R,0); tot.assign(P,0); mx.assign(P,0);
    {
        set<pair<long long,int>> pq;
        for(int p=0;p<P;p++) pq.insert({0,p});
        for(int s:placedList){
            auto it=pq.begin(); int p=it->second; pq.erase(it);
            pool_[s]=p;
            rc[(size_t)p*R+prow[s]]+=cs[s]; tot[p]+=cs[s];
            mx[p]=max(mx[p],rc[(size_t)p*R+prow[s]]);
            pq.insert({gcOf(p),p});
        }
    }
    vector<int> bestPool=pool_;
    long long bestScore=score();
    mt19937 rng(12345);
    int n=placedList.size();
    if(n>0 && P>1){
        while(units<BUDGET){
            long long m=score();
            bool improved=false;
            for(int w=0;w<P && !improved && units<BUDGET;w++){
                if(gcOf(w)!=m) continue;
                long long best=m; int type=-1, ma=-1, mb=-1;
                // moves
                for(int s:placedList){
                    int q=pool_[s]; if(q==w) continue;
                    long long nw=addGc(w,prow[s],cs[s]); if(nw<=best) continue;
                    long long nq=removeGc(q,prow[s],cs[s]);
                    long long v=min(nw,nq);
                    if(v>best){best=v;type=0;ma=s;}
                }
                // swaps
                for(int a:placedList){
                    if(pool_[a]!=w) continue;
                    if(units>=BUDGET) break;
                    for(int b:placedList){
                        int q=pool_[b]; if(q==w) continue;
                        if(cs[b]<=cs[a] && prow[a]==prow[b]) continue;
                        long long nw=swapGc(w,prow[a],cs[a],prow[b],cs[b]); if(nw<=best) continue;
                        long long nq=swapGc(q,prow[b],cs[b],prow[a],cs[a]);
                        long long v=min(nw,nq);
                        if(v>best){best=v;type=1;ma=a;mb=b;}
                    }
                }
                if(type==0){applyMove(ma,w);improved=true;}
                else if(type==1){int q=pool_[mb]; applyMove(ma,q); applyMove(mb,w); improved=true;}
            }
            if(!improved){
                long long cur=score();
                if(cur>bestScore){bestScore=cur;bestPool=pool_;}
                else if(cur<bestScore){pool_=bestPool;}
                int k=1+rng()%3;
                for(int i=0;i<k;i++){
                    int s=placedList[rng()%n];
                    pool_[s]=rng()%P;
                }
                rebuild();
            }
        }
        long long cur=score();
        if(cur>bestScore){bestScore=cur;bestPool=pool_;}
        pool_=bestPool;
    }
    string out;
    out.reserve((size_t)M*16);
    char buf[64];
    for(int i=0;i<M;i++){
        if(prow[i]<0) out+="x\n";
        else { snprintf(buf,sizeof(buf),"%d %d %d\n",prow[i],pslot[i],pool_[i]); out+=buf; }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
