#include <bits/stdc++.h>
using namespace std;
int main(){
    int T,S;
    scanf("%d %d",&T,&S);
    vector<long long> sp(S),sl(S),sv(S),sw(S),sd(S);
    for(int i=0;i<S;i++)scanf("%lld %lld %lld %lld %lld",&sp[i],&sl[i],&sv[i],&sw[i],&sd[i]);
    int C;scanf("%d",&C);
    vector<int> V(C);
    vector<vector<pair<int,int>>> rng(C);
    vector<int> ceStart(C+1,0);
    vector<pair<long long,long long>> ent; // entry coords
    vector<int> entC;
    for(int c=0;c<C;c++){
        int L,R;scanf("%d %d %d",&V[c],&L,&R);
        for(int i=0;i<L;i++){
            long long a,b;scanf("%lld %lld",&a,&b);
            ent.push_back({a,b});entC.push_back(c);
        }
        rng[c].resize(R);
        for(int i=0;i<R;i++)scanf("%d %d",&rng[c][i].first,&rng[c][i].second);
        sort(rng[c].begin(),rng[c].end());
    }
    int E=ent.size();
    // unique locs
    vector<int> ord(E);
    iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){return ent[a]<ent[b];});
    vector<int> locOf(E);
    vector<pair<long long,long long>> locs;
    for(int k=0;k<E;k++){
        int e=ord[k];
        if(locs.empty()||locs.back()!=ent[e])locs.push_back(ent[e]);
        locOf[e]=locs.size()-1;
    }
    int NLoc=locs.size();
    vector<int> leS(NLoc+1,0);
    for(int e=0;e<E;e++)leS[locOf[e]+1]++;
    for(int i=0;i<NLoc;i++)leS[i+1]+=leS[i];
    vector<int> leL(E);
    {
        vector<int> pos(leS.begin(),leS.end()-1);
        for(int e=0;e<E;e++)leL[pos[locOf[e]]++]=e;
    }
    vector<int> rem(C,0);
    for(int e=0;e<E;e++)rem[entC[e]]++;
    vector<char> done(E,0);
    const int CS=16384,NLA=40,NLO=80,LONW=1296000;
    vector<int> cellS(NLA*NLO+1,0);
    vector<int> locCell(NLoc);
    for(int i=0;i<NLoc;i++){
        long long y=locs[i].first+324000,x=locs[i].second+648000;
        locCell[i]=(y/CS)*NLO+(x/CS);
        cellS[locCell[i]+1]++;
    }
    for(int i=0;i<NLA*NLO;i++)cellS[i+1]+=cellS[i];
    vector<int> cellL(NLoc);
    {
        vector<int> pos(cellS.begin(),cellS.end()-1);
        for(int i=0;i<NLoc;i++)cellL[pos[locCell[i]]++]=i;
    }
    auto active=[&](int c,int t){
        auto&r=rng[c];
        int lo=0,hi=r.size()-1,ans=-1;
        while(lo<=hi){int m=(lo+hi)/2;if(r[m].first<=t){ans=m;lo=m+1;}else hi=m-1;}
        return ans>=0&&r[ans].second>=t;
    };
    vector<long long> cp=sp,cl=sl,cv=sv;
    vector<long long> po(S,0),lo_(S,0),lastT(S,0);
    struct P{long long a,b;int t,s;};
    vector<P> out;
    auto st=chrono::steady_clock::now();
    for(int t=0;t<T;t++){
        if((t&255)==0){
            double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
            if(el>8.5)break;
        }
        for(int s=0;s<S;s++){
            if(t>0){
                long long nl=cp[s]+cv[s];
                if(nl>324000){cp[s]=648000-nl;cv[s]=-cv[s];}
                else if(nl<-324000){cp[s]=-648000-nl;cv[s]=-cv[s];}
                else cp[s]=nl;
                long long x=cl[s]-15;
                x=((x+648000)%LONW+LONW)%LONW-648000;
                cl[s]=x;
            }
            if(E==0)continue;
            long long d=sd[s],w=sw[s];
            long long ylo=max(0LL,cp[s]+324000-d),yhi=min(648000LL,cp[s]+324000+d);
            if(ylo>yhi)continue;
            long long x=cl[s]+648000;
            double best=0;int bl=-1;long long bdp=0,bdl=0;
            long long mv=w*(t-lastT[s]);
            for(int sh=-1;sh<=1;sh++){
                long long xl=x-d+(long long)sh*LONW,xh=x+d+(long long)sh*LONW;
                xl=max(xl,0LL);xh=min(xh,(long long)LONW-1);
                if(xl>xh)continue;
                for(long long cy=ylo/CS;cy<=yhi/CS;cy++)
                    for(long long cx=xl/CS;cx<=xh/CS;cx++){
                        int cell=cy*NLO+cx;
                        for(int k=cellS[cell];k<cellS[cell+1];k++){
                            int li=cellL[k];
                            long long dp=locs[li].first-cp[s];
                            if(dp<-d||dp>d)continue;
                            long long dl=locs[li].second-cl[s];
                            dl=((dl+648000)%LONW+LONW)%LONW-648000;
                            if(dl<-d||dl>d)continue;
                            if(llabs(dp-po[s])>mv||llabs(dl-lo_[s])>mv)continue;
                            double wt=0;
                            for(int q=leS[li];q<leS[li+1];q++){
                                int e=leL[q];
                                if(done[e])continue;
                                int c=entC[e];
                                if(!active(c,t))continue;
                                wt+=(double)V[c]/((double)rem[c]*rem[c]);
                            }
                            if(wt>best){best=wt;bl=li;bdp=dp;bdl=dl;}
                        }
                    }
            }
            if(bl>=0){
                for(int q=leS[bl];q<leS[bl+1];q++){
                    int e=leL[q];
                    if(done[e])continue;
                    int c=entC[e];
                    if(!active(c,t))continue;
                    done[e]=1;rem[c]--;
                }
                po[s]=bdp;lo_[s]=bdl;lastT[s]=t;
                out.push_back({locs[bl].first,locs[bl].second,t,s});
            }
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&p:out)printf("%lld %lld %d %d\n",p.a,p.b,p.t,p.s);
}
