#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static int wrapLon(ll x){
    ll M=1296000;
    x=(x+648000)%M; if(x<0)x+=M;
    return (int)(x-648000);
}
int T,S,C;
int main(){
    scanf("%d",&T);
    scanf("%d",&S);
    vector<int> sp(S),sl(S),sv(S),sw(S),sd(S);
    for(int i=0;i<S;i++) scanf("%d %d %d %d %d",&sp[i],&sl[i],&sv[i],&sw[i],&sd[i]);
    scanf("%d",&C);
    vector<int> V(C),rem(C),lastEnd(C),rstart(C+1,0);
    vector<int> rs,re;
    unordered_map<ll,int> lm;
    vector<int> lphi,llam;
    vector<int> entLoc,entCol;
    vector<int> colEntStart(C+1,0);
    for(int c=0;c<C;c++){
        int L,R;
        scanf("%d %d %d",&V[c],&L,&R);
        vector<int> ids;
        for(int i=0;i<L;i++){
            int p,l;scanf("%d %d",&p,&l);
            ll key=(ll)(p+400000)*4000000LL+(l+700000);
            auto it=lm.find(key);
            int id;
            if(it==lm.end()){id=lphi.size();lm[key]=id;lphi.push_back(p);llam.push_back(l);}
            else id=it->second;
            ids.push_back(id);
        }
        vector<pair<int,int>> rr;
        for(int i=0;i<R;i++){int a,b;scanf("%d %d",&a,&b);rr.push_back({a,b});}
        sort(rr.begin(),rr.end());
        rstart[c]=rs.size();
        lastEnd[c]=-1;
        for(auto&p:rr){rs.push_back(p.first);re.push_back(p.second);lastEnd[c]=max(lastEnd[c],p.second);}
        rstart[c+1]=rs.size();
        if(V[c]>0){
            for(int id:ids){entLoc.push_back(id);entCol.push_back(c);}
            rem[c]=L;
        } else rem[c]=0;
    }
    // rstart fix: rstart[c+1] set in loop; ok
    int NL=lphi.size();
    int NE=entLoc.size();
    vector<int> cs(NL+1,0);
    for(int e=0;e<NE;e++)cs[entLoc[e]+1]++;
    for(int i=0;i<NL;i++)cs[i+1]+=cs[i];
    vector<int> le(NE);
    {
        vector<int> pos(cs.begin(),cs.end()-1);
        for(int e=0;e<NE;e++)le[pos[entLoc[e]]++]=e;
    }
    vector<char> done(NE,0);
    const int CS=5000,NLAT=130,NLON=260;
    vector<vector<int>> cells(NLAT*NLON);
    for(int i=0;i<NL;i++){
        int a=(lphi[i]+324000)/CS; if(a>=NLAT)a=NLAT-1;
        int b=(llam[i]+648000)/CS; if(b>=NLON)b=NLON-1;
        cells[a*NLON+b].push_back(i);
    }
    vector<int> claim(NL,0);
    ll ops=0; const ll BUDGET=700000000LL;
    auto active=[&](int c,int t)->bool{
        int lo=rstart[c],hi=rstart[c+1]-1,ans=-1;
        while(lo<=hi){int m=(lo+hi)/2; if(rs[m]<=t){ans=m;lo=m+1;}else hi=m-1;}
        return ans>=0 && re[ans]>=t;
    };
    // returns weight; alive set
    auto weight=[&](int L,int t,bool&alive)->double{
        double w=0;alive=false;
        for(int k=cs[L];k<cs[L+1];k++){
            int e=le[k];
            ops+=3;
            if(done[e])continue;
            int c=entCol[e];
            if(rem[c]<=0||lastEnd[c]<t)continue;
            alive=true;
            if(active(c,t))w+=(double)V[c]/rem[c];
        }
        return w;
    };
    vector<int> target(S,-1),cp(S,0),cl(S,0);
    vector<array<int,4>> photos;
    for(int t=0;t<T;t++){
        for(int s=0;s<S;s++){
            int d=sd[s],w=sw[s];
            int sP=sp[s],sL=sl[s],v=sv[s];
            if(target[s]<0 && (t+s)%6==0 && ops<BUDGET){
                int latLo=max(0,(sP-d+324000)/CS), latHi=min(NLAT-1,(sP+d+324000)/CS);
                if(sP-d+324000<0)latLo=0;
                int lo=wrapLon((ll)sL-d)+648000;
                int k=(2*d+CS-1)/CS+2;
                if(k>NLON)k=NLON;
                int c0=lo/CS;
                double best=0;int bl=-1;
                for(int a=latLo;a<=latHi;a++){
                    for(int j=0;j<k;j++){
                        auto&cv=cells[a*NLON+(c0+j)%NLON];
                        for(size_t i=0;i<cv.size();){
                            int L=cv[i];
                            ops+=2;
                            int rp=lphi[L]-sP;
                            if(rp<-d||rp>d){i++;continue;}
                            int rl=wrapLon((ll)llam[L]-sL);
                            if(rl<-d||rl>d){i++;continue;}
                            bool alive;
                            double wt=weight(L,t,alive);
                            if(!alive){cv[i]=cv.back();cv.pop_back();continue;}
                            i++;
                            if(wt<=0)continue;
                            if(claim[L])continue;
                            int tau=-1;
                            for(int u=0;u<=120;u++){
                                ops++;
                                int a1=rp-v*u,b1=rl+15*u;
                                if(a1<-d||a1>d||b1<-d||b1>d){ if(u>0 && (a1<-d||a1>d)&&((v>0&&a1<-d)||(v<0&&a1>d)))break; continue;}
                                if(abs(a1-cp[s])<=(ll)w*(u+1)&&abs(b1-cl[s])<=(ll)w*(u+1)){tau=u;break;}
                            }
                            if(tau<0)continue;
                            double sc=wt/(tau+10.0);
                            if(sc>best){best=sc;bl=L;}
                        }
                    }
                }
                if(bl>=0){target[s]=bl;claim[bl]=1;}
            }
            if(target[s]>=0){
                int L=target[s];
                bool alive;
                double wt=weight(L,t,alive);
                int rp=lphi[L]-sP;
                int rl=wrapLon((ll)llam[L]-sL);
                if(wt<=0||rp<-d||rp>d||rl<-d||rl>d){
                    claim[L]=0;target[s]=-1;
                } else if(abs(rp-cp[s])<=w&&abs(rl-cl[s])<=w){
                    cp[s]=rp;cl[s]=rl;
                    photos.push_back({lphi[L],llam[L],t,s});
                    for(int k=cs[L];k<cs[L+1];k++){
                        int e=le[k];
                        if(done[e])continue;
                        int c=entCol[e];
                        if(active(c,t)){done[e]=1;rem[c]--;}
                    }
                    claim[L]=0;target[s]=-1;
                } else {
                    int np=rp-v,nl=rl+15;
                    np=max(-d,min(d,np));nl=max(-d,min(d,nl));
                    int dp=np-cp[s];dp=max(-w,min(w,dp));
                    int dl=nl-cl[s];dl=max(-w,min(w,dl));
                    cp[s]+=dp;cl[s]+=dl;
                }
            }
            // update satellite
            int np=sP+v;
            int nl=sl[s]-15;
            if(np>324000){np=648000-np;sv[s]=-v;nl+=648000;}
            else if(np<-324000){np=-648000-np;sv[s]=-v;nl+=648000;}
            sp[s]=np;sl[s]=wrapLon(nl);
        }
    }
    printf("%d\n",(int)photos.size());
    for(auto&p:photos)printf("%d %d %d %d\n",p[0],p[1],p[2],p[3]);
}
