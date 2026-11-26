#include <bits/stdc++.h>
using namespace std;
int main(){
    int T,S;
    scanf("%d %d",&T,&S);
    struct Sat{long long phi,lam,v,w,d;};
    vector<Sat> sats(S);
    for(auto&s:sats)scanf("%lld %lld %lld %lld %lld",&s.phi,&s.lam,&s.v,&s.w,&s.d);
    int C;scanf("%d",&C);
    vector<int> val(C),rem(C);
    vector<vector<pair<int,int>>> rng(C);
    map<pair<int,int>,int> lid;
    vector<pair<int,int>> lc;
    vector<vector<int>> lent; // entries per loc
    vector<int> entC;
    for(int c=0;c<C;c++){
        int V,L,R;scanf("%d %d %d",&V,&L,&R);
        val[c]=V;rem[c]=L;
        for(int i=0;i<L;i++){
            int a,b;scanf("%d %d",&a,&b);
            auto k=make_pair(a,b);
            auto it=lid.find(k);
            int id;
            if(it==lid.end()){id=lc.size();lid[k]=id;lc.push_back(k);lent.emplace_back();}
            else id=it->second;
            lent[id].push_back(entC.size());
            entC.push_back(c);
        }
        for(int i=0;i<R;i++){
            int a,b;scanf("%d %d",&a,&b);
            rng[c].push_back({a,b});
        }
        sort(rng[c].begin(),rng[c].end());
    }
    vector<char> done(entC.size(),0);
    const int G=2000;
    const int NL=307,NW=648;
    int n=lc.size();
    auto cellOf=[&](int phi,int lam,int&i,int&j){
        i=(phi+306000)/G;j=(lam+648000)/G;
    };
    vector<int> cnt(NL*NW+1,0);
    for(int k=0;k<n;k++){int i,j;cellOf(lc[k].first,lc[k].second,i,j);cnt[i*NW+j+1]++;}
    for(int i=0;i<NL*NW;i++)cnt[i+1]+=cnt[i];
    vector<int> items(n);
    {
        vector<int> pos(cnt.begin(),cnt.end()-1);
        for(int k=0;k<n;k++){int i,j;cellOf(lc[k].first,lc[k].second,i,j);items[pos[i*NW+j]++]=k;}
    }
    vector<int> P((NL+1)*(NW+1),0);
    for(int i=0;i<NL;i++)for(int j=0;j<NW;j++){
        int c=cnt[i*NW+j+1]-cnt[i*NW+j];
        P[(i+1)*(NW+1)+j+1]=P[i*(NW+1)+j+1]+P[(i+1)*(NW+1)+j]-P[i*(NW+1)+j]+(c>0);
    }
    auto rect=[&](int i0,int i1,int j0,int j1){
        return P[(i1+1)*(NW+1)+j1+1]-P[i0*(NW+1)+j1+1]-P[(i1+1)*(NW+1)+j0]+P[i0*(NW+1)+j0];
    };
    auto active=[&](int c,int t){
        auto&r=rng[c];
        int lo=0,hi=r.size();
        while(lo<hi){int m=(lo+hi)/2;if(r[m].first<=t)lo=m+1;else hi=m;}
        if(lo==0)return false;
        return t<=r[lo-1].second;
    };
    vector<long long> lastPhi(S,0),lastLam(S,0),lastT(S,0);
    struct Ph{int a,b,t,s;};
    vector<Ph> out;
    long long work=0;
    const long long BUDGET=400000000LL;
    for(int t=0;t<T;t++){
        for(int s=0;s<S;s++){
            Sat&st=sats[s];
            if(t>0){
                long long np=st.phi+st.v;
                long long nl=st.lam-15;
                if(np>324000){np=648000-np;st.v=-st.v;nl+=648000;}
                else if(np<-324000){np=-648000-np;st.v=-st.v;nl+=648000;}
                nl=((nl+648000)%1296000+1296000)%1296000-648000;
                st.phi=np;st.lam=nl;
            }
            if(work>BUDGET||n==0)continue;
            long long d=st.d;
            long long plo=max<long long>(st.phi-d,-306000),phi2=min<long long>(st.phi+d,306000);
            if(plo>phi2)continue;
            int i0=(plo+306000)/G,i1=(phi2+306000)/G;
            long long llo=st.lam-d,lhi=st.lam+d;
            long long seg[2][2];int ns=0;
            if(llo<-648000){seg[ns][0]=llo+1296000;seg[ns][1]=647999;ns++;seg[ns][0]=-648000;seg[ns][1]=lhi;ns++;}
            else if(lhi>647999){seg[ns][0]=llo;seg[ns][1]=647999;ns++;seg[ns][0]=-648000;seg[ns][1]=lhi-1296000;ns++;}
            else{seg[ns][0]=llo;seg[ns][1]=lhi;ns++;}
            long long dt=t-lastT[s];
            long long reach=st.w*dt;
            double best=0;int bk=-1;
            for(int q=0;q<ns;q++){
                int j0=(seg[q][0]+648000)/G,j1=(seg[q][1]+648000)/G;
                if(rect(i0,i1,j0,j1)==0)continue;
                for(int i=i0;i<=i1;i++)for(int j=j0;j<=j1;j++){
                    int cid=i*NW+j;
                    for(int p=cnt[cid];p<cnt[cid+1];p++){
                        int k=items[p];
                        work++;
                        long long dp=lc[k].first-st.phi;
                        long long dl=lc[k].second-st.lam;
                        if(dl>648000)dl-=1296000;
                        if(dl<-648000)dl+=1296000;
                        if(dp<-d||dp>d||dl<-d||dl>d)continue;
                        if(llabs(dp-lastPhi[s])>reach||llabs(dl-lastLam[s])>reach)continue;
                        double w=0;
                        for(int e:lent[k]){
                            if(done[e])continue;
                            int c=entC[e];
                            if(!active(c,t))continue;
                            w+=(double)val[c]/rem[c];
                        }
                        if(w>best){best=w;bk=k;}
                    }
                }
            }
            if(bk>=0){
                long long dp=lc[bk].first-st.phi;
                long long dl=lc[bk].second-st.lam;
                if(dl>648000)dl-=1296000;
                if(dl<-648000)dl+=1296000;
                lastPhi[s]=dp;lastLam[s]=dl;lastT[s]=t;
                out.push_back({lc[bk].first,lc[bk].second,t,s});
                for(int e:lent[bk]){
                    if(done[e])continue;
                    int c=entC[e];
                    if(!active(c,t))continue;
                    done[e]=1;rem[c]--;
                    if(rem[c]<=0)rem[c]=1;
                }
            }
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&o:out)printf("%d %d %d %d\n",o.a,o.b,o.t,o.s);
}
