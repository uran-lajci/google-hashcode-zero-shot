#include <bits/stdc++.h>
using namespace std;
int main(){
    int T,S;
    scanf("%d %d",&T,&S);
    vector<long long> sp(S),sl(S),sv(S),sw(S),sd(S);
    for(int i=0;i<S;i++)scanf("%lld %lld %lld %lld %lld",&sp[i],&sl[i],&sv[i],&sw[i],&sd[i]);
    int C;scanf("%d",&C);
    vector<int> val(C),rem(C);
    vector<vector<pair<int,int>>> rng(C);
    map<pair<int,int>,int> lid;
    vector<pair<int,int>> locs;
    vector<vector<int>> locColl;
    for(int c=0;c<C;c++){
        int V,L,R;scanf("%d %d %d",&V,&L,&R);
        val[c]=V;
        set<int> mine;
        for(int i=0;i<L;i++){
            int a,b;scanf("%d %d",&a,&b);
            auto k=make_pair(a,b);
            auto it=lid.find(k);
            int id;
            if(it==lid.end()){id=locs.size();lid[k]=id;locs.push_back(k);locColl.push_back({});}
            else id=it->second;
            if(mine.insert(id).second){locColl[id].push_back(c);}
        }
        rem[c]=mine.size();
        for(int i=0;i<R;i++){int a,b;scanf("%d %d",&a,&b);rng[c].push_back({a,b});}
        sort(rng[c].begin(),rng[c].end());
    }
    int N=locs.size();
    vector<vector<char>> done(N);
    for(int i=0;i<N;i++)done[i].assign(locColl[i].size(),0);
    const int CS=20000,NLAT=33,NLON=65;
    vector<vector<int>> cell(NLAT*NLON);
    for(int i=0;i<N;i++){
        int a=(locs[i].first+324000)/CS;
        int b=(int)(((long long)locs[i].second+648000)%1296000)/CS;
        cell[a*NLON+b].push_back(i);
    }
    auto inR=[&](int c,int t){
        auto&r=rng[c];
        int lo=0,hi=r.size();
        while(lo<hi){int m=(lo+hi)/2;if(r[m].first<=t)lo=m+1;else hi=m;}
        if(lo==0)return false;
        return r[lo-1].second>=t;
    };
    vector<long long> lastT(S,0),lastP(S,0),lastL(S,0);
    vector<char> had(S,0);
    struct Out{int p,l,t,s;};
    vector<Out> out;
    for(int t=0;t<T;t++){
        for(int s=0;s<S;s++){
            long long P=sp[s],Lg=sl[s],d=sd[s];
            long long reach=sw[s]*(t-lastT[s]);
            if(!(had[s]&&t<=lastT[s])){
                int la0=(int)max(0LL,(P-d+324000)/CS),la1=(int)min((long long)NLAT-1,max(0LL,(P+d+324000)/CS));
                long long lb=Lg+648000-d;
                long long lo=((lb%1296000)+1296000)%1296000;
                int c0=lo/CS;
                int cnt=(int)((lo+2*d)/CS)-c0+1;
                double best=0;int bi=-1;long long bp=0,bl=0;
                for(int a=la0;a<=la1;a++)for(int k=0;k<cnt;k++){
                    int b=(c0+k)%NLON;
                    auto&cv=cell[a*NLON+b];
                    for(int id:cv){
                        long long dp=locs[id].first-P;
                        if(dp>d||dp<-d)continue;
                        long long dl=locs[id].second-Lg;
                        dl=((dl+648000)%1296000+1296000)%1296000-648000;
                        if(dl>d||dl<-d)continue;
                        if(llabs(dp-lastP[s])>reach||llabs(dl-lastL[s])>reach)continue;
                        double w=0;
                        for(size_t j=0;j<locColl[id].size();j++){
                            if(done[id][j])continue;
                            int c=locColl[id][j];
                            if(inR(c,t))w+=(double)val[c]/rem[c];
                        }
                        if(w>best){best=w;bi=id;bp=dp;bl=dl;}
                    }
                }
                if(bi>=0){
                    for(size_t j=0;j<locColl[bi].size();j++){
                        if(done[bi][j])continue;
                        int c=locColl[bi][j];
                        if(inR(c,t)){done[bi][j]=1;rem[c]--;}
                    }
                    out.push_back({locs[bi].first,locs[bi].second,t,s});
                    had[s]=1;lastT[s]=t;lastP[s]=bp;lastL[s]=bl;
                }
            }
            // advance
            long long np=sp[s]+sv[s];
            if(np>324000){np=648000-np;sv[s]=-sv[s];sl[s]+=648000;}
            else if(np<-324000){np=-648000-np;sv[s]=-sv[s];sl[s]+=648000;}
            sp[s]=np;
            long long nl=sl[s]-15;
            nl=((nl+648000)%1296000+1296000)%1296000-648000;
            sl[s]=nl;
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out)printf("%d %d %d %d\n",o.p,o.l,o.t,o.s);
}
