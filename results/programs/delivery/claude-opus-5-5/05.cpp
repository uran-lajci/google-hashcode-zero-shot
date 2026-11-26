#include <bits/stdc++.h>
using namespace std;

static long long idist(long long r1,long long c1,long long r2,long long c2){
    long long dr=r1-r2,dc=c1-c2; long long d2=dr*dr+dc*dc;
    long long s=(long long)sqrt((double)d2);
    while(s*s<d2) s++;
    while(s>0 && (s-1)*(s-1)>=d2) s--;
    return s;
}

int main(){
    long long R,Cc,D,T,CAP;
    if(!(cin>>R>>Cc>>D>>T>>CAP)) return 0;
    int P; cin>>P; vector<long long> wt(P); for(auto&x:wt)cin>>x;
    int W; cin>>W; vector<long long> wr(W),wc(W); vector<vector<int>> stock(W,vector<int>(P));
    for(int i=0;i<W;i++){cin>>wr[i]>>wc[i]; for(int j=0;j<P;j++)cin>>stock[i][j];}
    int C; cin>>C; vector<long long> orr(C),oc(C); vector<vector<int>> items(C);
    for(int i=0;i<C;i++){cin>>orr[i]>>oc[i]; int L;cin>>L; items[i].resize(L); for(auto&x:items[i])cin>>x;}

    // order priority
    vector<double> key(C);
    for(int i=0;i<C;i++){
        long long tw=0; for(int x:items[i]) tw+=wt[x];
        long long trips=(tw+CAP-1)/CAP;
        long long dn=LLONG_MAX; for(int w=0;w<W;w++) dn=min(dn,idist(wr[w],wc[w],orr[i],oc[i]));
        key[i]=(double)trips*(2.0*dn+2)+items[i].size()*0.5;
    }
    vector<int> ord(C); iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return key[a]<key[b];});

    vector<long long> dt(D,0),dr(D,wr[0]),dc(D,wc[0]);
    vector<string> out; out.reserve(1<<20);

    struct Trip{int w; vector<pair<int,int>> tc;};
    for(int o:ord){
        map<int,int> needm; for(int x:items[o]) needm[x]++;
        vector<pair<int,int>> need(needm.begin(),needm.end());
        vector<tuple<int,int,int>> undo; // w,type,cnt
        vector<Trip> trips;
        bool fail=false;
        vector<long long> wd(W);
        for(int w=0;w<W;w++) wd[w]=idist(wr[w],wc[w],orr[o],oc[o]);
        while(true){
            long long remW=0; for(auto&p:need) remW+=(long long)p.second*wt[p.first];
            if(remW==0) break;
            int best=-1; long long bestSup=-1, bestD=LLONG_MAX; int fullBest=-1; long long fullD=LLONG_MAX;
            for(int w=0;w<W;w++){
                long long sup=0;
                for(auto&p:need) sup+=(long long)min(p.second,stock[w][p.first])*wt[p.first];
                if(sup==remW){ if(wd[w]<fullD){fullD=wd[w];fullBest=w;} }
                if(sup>bestSup||(sup==bestSup&&wd[w]<bestD)){bestSup=sup;bestD=wd[w];best=w;}
            }
            int w = fullBest>=0?fullBest:best;
            if(w<0||bestSup<=0){fail=true;break;}
            vector<int> its;
            for(auto&p:need){
                int t=min(p.second,stock[w][p.first]);
                if(t>0){stock[w][p.first]-=t; p.second-=t; undo.emplace_back(w,p.first,t); for(int k=0;k<t;k++) its.push_back(p.first);}
            }
            sort(its.begin(),its.end(),[&](int a,int b){return wt[a]>wt[b];});
            vector<long long> load; vector<map<int,int>> bins;
            for(int x:its){
                bool placed=false;
                for(size_t b=0;b<bins.size();b++) if(load[b]+wt[x]<=CAP){load[b]+=wt[x];bins[b][x]++;placed=true;break;}
                if(!placed){load.push_back(wt[x]);bins.push_back({});bins.back()[x]++;}
            }
            for(auto&b:bins){Trip tr; tr.w=w; tr.tc.assign(b.begin(),b.end()); trips.push_back(tr);}
        }
        vector<long long> ndt, ndr, ndc;
        vector<pair<int,int>> assign; // trip -> drone
        if(!fail){
            ndt=dt; ndr=dr; ndc=dc;
            for(size_t i=0;i<trips.size();i++){
                auto&tr=trips[i]; long long k=tr.tc.size();
                long long bestF=LLONG_MAX; int bd=-1;
                for(int d=0;d<D;d++){
                    long long f=ndt[d]+idist(ndr[d],ndc[d],wr[tr.w],wc[tr.w])+k+wd[tr.w]+k;
                    if(f<bestF){bestF=f;bd=d;}
                }
                if(bd<0||bestF>T){fail=true;break;}
                ndt[bd]=bestF; ndr[bd]=orr[o]; ndc[bd]=oc[o];
                assign.push_back({(int)i,bd});
            }
        }
        if(fail){
            for(auto&u:undo) stock[get<0>(u)][get<1>(u)]+=get<2>(u);
            continue;
        }
        dt=ndt; dr=ndr; dc=ndc;
        for(auto&a:assign){
            auto&tr=trips[a.first]; int d=a.second;
            for(auto&p:tr.tc) out.push_back(to_string(d)+" L "+to_string(tr.w)+" "+to_string(p.first)+" "+to_string(p.second));
            for(auto&p:tr.tc) out.push_back(to_string(d)+" D "+to_string(o)+" "+to_string(p.first)+" "+to_string(p.second));
        }
    }
    string s; s.reserve(out.size()*16+32);
    s+=to_string(out.size()); s+='\n';
    for(auto&x:out){s+=x;s+='\n';}
    fwrite(s.data(),1,s.size(),stdout);
    return 0;
}
