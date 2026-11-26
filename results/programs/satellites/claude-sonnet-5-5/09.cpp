#include <bits/stdc++.h>
using namespace std;
struct Opp{int sat,t;short dp,dl;};
struct Ph{int dp,dl,loc;};
static int wrapL(long long x){
    x%=1296000; if(x<0)x+=1296000;
    if(x>647999)x-=1296000;
    return (int)x;
}
int main(){
    int T,S;
    scanf("%d %d",&T,&S);
    vector<int> sp(S),sl(S),sv(S),sw(S),sd(S);
    for(int i=0;i<S;i++)scanf("%d %d %d %d %d",&sp[i],&sl[i],&sv[i],&sw[i],&sd[i]);
    int C;scanf("%d",&C);
    vector<int> V(C);
    vector<vector<int>> cl(C);
    vector<vector<pair<int,int>>> cr(C);
    unordered_map<long long,int> idm;
    vector<int> lp,ll;
    for(int c=0;c<C;c++){
        int L,R;scanf("%d %d %d",&V[c],&L,&R);
        for(int i=0;i<L;i++){
            int a,b;scanf("%d %d",&a,&b);
            long long key=(long long)(a+324000)*2000000+(b+648000);
            auto it=idm.find(key);
            int id;
            if(it==idm.end()){id=lp.size();idm[key]=id;lp.push_back(a);ll.push_back(b);}
            else id=it->second;
            cl[c].push_back(id);
        }
        sort(cl[c].begin(),cl[c].end());
        cl[c].erase(unique(cl[c].begin(),cl[c].end()),cl[c].end());
        for(int i=0;i<R;i++){
            int a,b;scanf("%d %d",&a,&b);
            cr[c].push_back({a,b});
        }
        sort(cr[c].begin(),cr[c].end());
    }
    int N=lp.size();
    // grid
    const int CS=2400,NLAT=271,NLON=540;
    vector<int> cstart(NLAT*NLON+1,0),items(N);
    auto cellOf=[&](int i){
        int a=(lp[i]+324000)/CS;
        int b=(ll[i]+648000)/CS;
        return a*NLON+b;
    };
    for(int i=0;i<N;i++)cstart[cellOf(i)+1]++;
    for(size_t i=0;i<cstart.size()-1;i++)cstart[i+1]+=cstart[i];
    {
        vector<int> pos(cstart.begin(),cstart.end()-1);
        for(int i=0;i<N;i++)items[pos[cellOf(i)]++]=i;
    }
    vector<vector<Opp>> opps(N);
    vector<int> lastT(N,-1000000000),lastS(N,-1);
    int CAP=40,GAP=max(5,T/40);
    vector<int> cp(sp),cll(sl),cv(sv);
    long long work=0;
    const long long BUDGET=900000000LL;
    for(int t=0;t<T&&work<BUDGET&&N>0;t++){
        for(int s=0;s<S;s++){
            int d=sd[s];
            int p=cp[s],l=cll[s];
            int a0=max(p-d+324000,0)/CS, a1=min(p+d+324000,647999)/CS;
            if(a1>=NLAT)a1=NLAT-1;
            long long ulo=(long long)l-d+648000, uhi=(long long)l+d+648000;
            long long b0=(ulo>=0?ulo/CS:-((-ulo+CS-1)/CS));
            long long b1=(uhi>=0?uhi/CS:-((-uhi+CS-1)/CS));
            for(int a=a0;a<=a1;a++){
                for(long long b=b0;b<=b1;b++){
                    int bb=(int)(((b%NLON)+NLON)%NLON);
                    int cell=a*NLON+bb;
                    for(int k=cstart[cell];k<cstart[cell+1];k++){
                        int id=items[k];
                        work++;
                        int dp=lp[id]-p;
                        if(dp>d||dp<-d)continue;
                        int dl=wrapL((long long)ll[id]-l);
                        if(dl>d||dl<-d)continue;
                        if((int)opps[id].size()>=CAP)continue;
                        if(lastS[id]==s && t-lastT[id]<GAP)continue;
                        if(lastS[id]!=s && t-lastT[id]<GAP/2)continue;
                        lastT[id]=t;lastS[id]=s;
                        opps[id].push_back({s,t,(short)dp,(short)dl});
                    }
                }
            }
            // step
            int v=cv[s];
            int np=p+v;
            int nl=l-15;
            if(np>324000){np=648000-np;nl+=648000;cv[s]=-v;}
            else if(np<-324000){np=-648000-np;nl+=648000;cv[s]=-v;}
            cp[s]=np;cll[s]=wrapL(nl);
        }
    }
    // order collections
    vector<int> ord;
    for(int c=0;c<C;c++)if(V[c]>0&&!cl[c].empty())ord.push_back(c);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){
        return (long long)V[a]*cl[b].size()>(long long)V[b]*cl[a].size();
    });
    vector<map<int,Ph>> sm(S);
    vector<vector<int>> photos(N);
    auto inRange=[&](int c,int t){
        auto &r=cr[c];
        auto it=upper_bound(r.begin(),r.end(),make_pair(t,INT_MAX));
        if(it==r.begin())return false;
        --it;
        return t<=it->second;
    };
    auto feasible=[&](const Opp&o){
        auto &m=sm[o.sat];
        if(m.count(o.t))return false;
        int w=sw[o.sat];
        auto it=m.lower_bound(o.t);
        int pdp=0,pdl=0,pt=0;
        if(it!=m.begin()){auto q=prev(it);pdp=q->second.dp;pdl=q->second.dl;pt=q->first;}
        long long dt=o.t-pt;
        if(abs(o.dp-pdp)>w*dt||abs(o.dl-pdl)>w*dt)return false;
        if(it!=m.end()){
            long long dt2=it->first-o.t;
            if(abs(o.dp-it->second.dp)>w*dt2||abs(o.dl-it->second.dl)>w*dt2)return false;
        }
        return true;
    };
    for(int c:ord){
        vector<pair<int,int>> ins; // sat,t,loc
        vector<array<int,3>> rec;
        bool ok=true;
        for(int id:cl[c]){
            bool have=false;
            for(int t:photos[id])if(inRange(c,t)){have=true;break;}
            if(have)continue;
            bool done=false;
            for(auto &o:opps[id]){
                if(!inRange(c,o.t))continue;
                if(!feasible(o))continue;
                sm[o.sat][o.t]={o.dp,o.dl,id};
                photos[id].push_back(o.t);
                rec.push_back({o.sat,o.t,id});
                done=true;break;
            }
            if(!done){ok=false;break;}
        }
        if(!ok){
            for(int i=(int)rec.size()-1;i>=0;i--){
                sm[rec[i][0]].erase(rec[i][1]);
                photos[rec[i][2]].pop_back();
            }
        }
    }
    long long P=0;
    for(int s=0;s<S;s++)P+=sm[s].size();
    string out=to_string(P)+"\n";
    for(int s=0;s<S;s++)for(auto &kv:sm[s]){
        out+=to_string(lp[kv.second.loc])+" "+to_string(ll[kv.second.loc])+" "+to_string(kv.first)+" "+to_string(s)+"\n";
    }
    fputs(out.c_str(),stdout);
}
