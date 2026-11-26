#include <bits/stdc++.h>
using namespace std;

int R,S,U,P,M;
vector<int> zs, cs, srow, spos, spool;
vector<vector<long long>> capPR;
vector<long long> tot, gcv;
mt19937_64 rng(12345);

long long computeGC(int p){
    long long mx=0;
    for(int r=0;r<R;r++) mx=max(mx,capPR[p][r]);
    return tot[p]-mx;
}
void addS(int s,int sign){
    if(srow[s]<0||spool[s]<0) return;
    capPR[spool[s]][srow[s]]+=sign*cs[s];
    tot[spool[s]]+=sign*cs[s];
}

int main(){
    if(scanf("%d %d %d %d %d",&R,&S,&U,&P,&M)!=5) return 0;
    vector<vector<char>> bad(R, vector<char>(S,0));
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);if(r>=0&&r<R&&s>=0&&s<S)bad[r][s]=1;}
    zs.resize(M);cs.resize(M);
    for(int i=0;i<M;i++) scanf("%d %d",&zs[i],&cs[i]);
    srow.assign(M,-1);spos.assign(M,-1);spool.assign(M,-1);
    // free segments per row
    vector<vector<pair<int,int>>> seg(R); // start,len
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bad[r][s]){s++;continue;}
            int t=s;while(t<S&&!bad[r][t])t++;
            seg[r].push_back({s,t-s});s=t;
        }
    }
    vector<int> maxLen(R,0);
    for(int r=0;r<R;r++) for(auto&x:seg[r]) maxLen[r]=max(maxLen[r],x.second);
    vector<long long> rowCap(R,0);
    vector<int> ord(M);iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        long long l=(long long)cs[a]*zs[b], rr=(long long)cs[b]*zs[a];
        if(l!=rr) return l>rr;
        if(cs[a]!=cs[b]) return cs[a]>cs[b];
        return a<b;
    });
    vector<int> rows(R);iota(rows.begin(),rows.end(),0);
    for(int s:ord){
        int z=zs[s];
        sort(rows.begin(),rows.end(),[&](int a,int b){if(rowCap[a]!=rowCap[b])return rowCap[a]<rowCap[b];return a<b;});
        for(int r:rows){
            if(maxLen[r]<z) continue;
            int best=-1;
            for(int i=0;i<(int)seg[r].size();i++){
                if(seg[r][i].second>=z && (best<0||seg[r][i].second<seg[r][best].second)) best=i;
            }
            if(best<0) continue;
            srow[s]=r;spos[s]=seg[r][best].first;
            seg[r][best].first+=z;seg[r][best].second-=z;
            if(seg[r][best].second==0) seg[r].erase(seg[r].begin()+best);
            maxLen[r]=0;for(auto&x:seg[r])maxLen[r]=max(maxLen[r],x.second);
            rowCap[r]+=cs[s];
            break;
        }
    }
    capPR.assign(P, vector<long long>(R,0));
    tot.assign(P,0);gcv.assign(P,0);
    // greedy pool assignment
    vector<int> placed;
    for(int i=0;i<M;i++) if(srow[i]>=0) placed.push_back(i);
    sort(placed.begin(),placed.end(),[&](int a,int b){if(cs[a]!=cs[b])return cs[a]>cs[b];return a<b;});
    for(int s:placed){
        int bp=0;
        for(int p=1;p<P;p++){
            if(gcv[p]<gcv[bp] || (gcv[p]==gcv[bp] && capPR[p][srow[s]]<capPR[bp][srow[s]])) bp=p;
        }
        spool[s]=bp;addS(s,1);gcv[bp]=computeGC(bp);
    }
    // local search
    if(!placed.empty() && P>=1 && R>=2){
        vector<vector<int>> bySize(S+1);
        for(int i=0;i<M;i++) bySize[zs[i]].push_back(i);
        auto evalMin=[&](long long &mn,int &cnt){
            mn=LLONG_MAX;cnt=0;
            for(int p=0;p<P;p++){
                if(gcv[p]<mn){mn=gcv[p];cnt=1;}
                else if(gcv[p]==mn)cnt++;
            }
        };
        long long curMin;int curCnt;evalMin(curMin,curCnt);
        long long iters=(long long)(3e8/(double)(R+P+10));
        iters=min(iters,(long long)6000000);
        // list of placed servers (changes with replacement moves; use all servers and filter)
        for(long long it=0;it<iters;it++){
            // pick server
            int s=-1;
            bool wantMin = (rng()%2==0);
            for(int tr=0;tr<60;tr++){
                int c=rng()%M;
                if(srow[c]<0) continue;
                s=c;
                if(!wantMin || gcv[spool[c]]==curMin) break;
            }
            if(s<0) continue;
            int type=rng()%3;
            int a=spool[s];
            int affected[3];int na=0;
            if(type==0){
                if(P<2) continue;
                int b=rng()%P; if(b==a) continue;
                addS(s,-1);spool[s]=b;addS(s,1);
                affected[na++]=a;affected[na++]=b;
                long long sv[2]={gcv[a],gcv[b]};
                gcv[a]=computeGC(a);gcv[b]=computeGC(b);
                long long nm;int nc;evalMin(nm,nc);
                if(nm>curMin||(nm==curMin&&nc<=curCnt)){curMin=nm;curCnt=nc;}
                else{addS(s,-1);spool[s]=a;addS(s,1);gcv[a]=sv[0];gcv[b]=sv[1];}
            } else if(type==1){
                int t=rng()%M;
                if(srow[t]<0||spool[t]==a) continue;
                int b=spool[t];
                addS(s,-1);addS(t,-1);swap(spool[s],spool[t]);addS(s,1);addS(t,1);
                long long sv[2]={gcv[a],gcv[b]};
                gcv[a]=computeGC(a);gcv[b]=computeGC(b);
                long long nm;int nc;evalMin(nm,nc);
                if(nm>curMin||(nm==curMin&&nc<=curCnt)){curMin=nm;curCnt=nc;}
                else{addS(s,-1);addS(t,-1);swap(spool[s],spool[t]);addS(s,1);addS(t,1);gcv[a]=sv[0];gcv[b]=sv[1];}
            } else {
                auto &lst=bySize[zs[s]];
                if(lst.size()<2) continue;
                int t=lst[rng()%lst.size()];
                if(t==s) continue;
                if(srow[t]>=0 && srow[t]==srow[s]) continue;
                int b=spool[t];
                auto doSwap=[&](){
                    addS(s,-1);addS(t,-1);
                    swap(srow[s],srow[t]);swap(spos[s],spos[t]);swap(spool[s],spool[t]);
                    addS(s,1);addS(t,1);
                };
                doSwap();
                na=0;affected[na++]=a;if(b>=0&&b!=a)affected[na++]=b;
                long long sv[2];
                for(int i=0;i<na;i++){sv[i]=gcv[affected[i]];gcv[affected[i]]=computeGC(affected[i]);}
                long long nm;int nc;evalMin(nm,nc);
                if(nm>curMin||(nm==curMin&&nc<=curCnt)){curMin=nm;curCnt=nc;}
                else{doSwap();for(int i=0;i<na;i++)gcv[affected[i]]=sv[i];}
            }
        }
    }
    string out;
    out.reserve(M*16);
    char buf[64];
    for(int i=0;i<M;i++){
        if(srow[i]<0||spool[i]<0) out+="x\n";
        else{snprintf(buf,sizeof(buf),"%d %d %d\n",srow[i],spos[i],spool[i]);out+=buf;}
    }
    fputs(out.c_str(),stdout);
    return 0;
}
