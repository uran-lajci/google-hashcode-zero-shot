#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static inline ll cdist(int r1,int c1,int r2,int c2){
    ll dr=r1-r2,dc=c1-c2; ll d2=dr*dr+dc*dc;
    ll s=(ll)sqrt((double)d2);
    while(s*s<d2) s++;
    while(s>0&&(s-1)*(s-1)>=d2) s--;
    return s;
}
struct Drone{int r,c; ll fr;};

int main(){
    int R,Cc,D; ll T; int cap;
    if(scanf("%d %d %d %lld %d",&R,&Cc,&D,&T,&cap)!=5) return 0;
    int P; scanf("%d",&P);
    vector<int> wt(P); for(auto&x:wt) scanf("%d",&x);
    int W; scanf("%d",&W);
    vector<int> wr(W),wc(W); vector<vector<int>> stock(W,vector<int>(P));
    for(int i=0;i<W;i++){scanf("%d %d",&wr[i],&wc[i]); for(int j=0;j<P;j++) scanf("%d",&stock[i][j]);}
    int C; scanf("%d",&C);
    vector<int> orr(C),oc(C); vector<vector<pair<int,int>>> items(C);
    vector<ll> oweight(C,0);
    for(int i=0;i<C;i++){
        scanf("%d %d",&orr[i],&oc[i]); int L; scanf("%d",&L);
        map<int,int> m; for(int k=0;k<L;k++){int t; scanf("%d",&t); m[t]++; oweight[i]+=wt[t];}
        for(auto&p:m) items[i].push_back(p);
        sort(items[i].begin(),items[i].end(),[&](const pair<int,int>&a,const pair<int,int>&b){return wt[a.first]>wt[b.first];});
    }
    // estimate cost
    vector<double> est(C);
    for(int i=0;i<C;i++){
        ll best=LLONG_MAX;
        for(int w=0;w<W;w++) best=min(best,cdist(wr[w],wc[w],orr[i],oc[i]));
        ll trips=(oweight[i]+cap-1)/cap;
        est[i]=(double)trips*(2.0*best+2)+items[i].size();
    }
    vector<int> ord(C); iota(ord.begin(),ord.end(),0);
    stable_sort(ord.begin(),ord.end(),[&](int a,int b){return est[a]<est[b];});

    vector<Drone> dr(D); for(auto&d:dr){d.r=wr[0];d.c=wc[0];d.fr=0;}
    vector<string> out;
    char buf[100];
    vector<int> widx(W);
    for(int oi:ord){
        vector<pair<int,int>> rem=items[oi];
        ll remW=oweight[oi];
        vector<Drone> saved=dr;
        vector<array<int,3>> sch; // w,t,n
        vector<string> cmds;
        bool ok=true;
        vector<ll> wd(W);
        for(int w=0;w<W;w++) wd[w]=cdist(wr[w],wc[w],orr[oi],oc[oi]);
        iota(widx.begin(),widx.end(),0);
        sort(widx.begin(),widx.end(),[&](int a,int b){return wd[a]<wd[b]||(wd[a]==wd[b]&&a<b);});
        while(remW>0){
            // candidate drones
            vector<int> cd(D); iota(cd.begin(),cd.end(),0);
            if(D>64){
                nth_element(cd.begin(),cd.begin()+64,cd.end(),[&](int a,int b){return dr[a].fr<dr[b].fr||(dr[a].fr==dr[b].fr&&a<b);});
                cd.resize(64); sort(cd.begin(),cd.end());
            }
            double bestCost=1e300; int bd=-1,bw=-1; vector<pair<int,int>> bpack; ll bpw=0;
            int cnt=0;
            for(int w:widx){
                if(cnt>=40) break;
                // pack
                vector<pair<int,int>> pack; int left=cap; ll pw=0;
                for(auto&p:rem){
                    if(p.second<=0) continue;
                    int n=min(p.second,stock[w][p.first]);
                    n=min(n,left/wt[p.first]);
                    if(n>0){pack.push_back({p.first,n}); left-=n*wt[p.first]; pw+=(ll)n*wt[p.first];}
                }
                bool any=false;
                for(auto&p:rem) if(p.second>0&&stock[w][p.first]>0){any=true;break;}
                if(!any) continue;
                cnt++;
                if(pw==0) continue;
                ll k=pack.size();
                double remCost=(double)(remW-pw)/cap*(2.0*wd[w]+2);
                for(int d:cd){
                    ll fin=dr[d].fr+cdist(dr[d].r,dr[d].c,wr[w],wc[w])+k+wd[w]+k;
                    if(fin>T) continue;
                    double cost=fin+remCost;
                    if(cost<bestCost){bestCost=cost;bd=d;bw=w;bpack=pack;bpw=pw;}
                }
            }
            if(bd<0){ok=false;break;}
            Drone&d=dr[bd];
            ll k=bpack.size();
            d.fr=d.fr+cdist(d.r,d.c,wr[bw],wc[bw])+k+wd[bw]+k;
            d.r=orr[oi]; d.c=oc[oi];
            for(auto&p:bpack){
                stock[bw][p.first]-=p.second; sch.push_back({bw,p.first,p.second});
                snprintf(buf,sizeof buf,"%d L %d %d %d",bd,bw,p.first,p.second); cmds.push_back(buf);
                for(auto&q:rem) if(q.first==p.first) q.second-=p.second;
            }
            for(auto&p:bpack){
                snprintf(buf,sizeof buf,"%d D %d %d %d",bd,oi,p.first,p.second); cmds.push_back(buf);
            }
            remW-=bpw;
        }
        if(ok){
            for(auto&s:cmds) out.push_back(s);
        } else {
            dr=saved;
            for(auto&s:sch) stock[s[0]][s[1]]+=s[2];
        }
    }
    string res=to_string(out.size())+"\n";
    for(auto&s:out){res+=s;res+='\n';}
    fwrite(res.data(),1,res.size(),stdout);
    return 0;
}
