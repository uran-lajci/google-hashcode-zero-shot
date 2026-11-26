#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

int R,S,U,P,M;
vector<int> Z,C,prow,ppos,ppool;
vector<ll> rowsum,total,gcv;

ll evalP(int p,int r1,ll d1,int r2,ll d2){
    ll tot=total[p]+d1+d2; ll mx=LLONG_MIN;
    const ll* rs=&rowsum[(size_t)p*R];
    for(int r=0;r<R;r++){ll v=rs[r]; if(r==r1)v+=d1; if(r==r2)v+=d2; if(v>mx)mx=v;}
    return tot-mx;
}

int main(){
    auto t0=chrono::steady_clock::now();
    if(scanf("%d %d %d %d %d",&R,&S,&U,&P,&M)!=5)return 0;
    vector<char> bad((size_t)R*S,0);
    for(int i=0;i<U;i++){int r,s;scanf("%d %d",&r,&s);bad[(size_t)r*S+s]=1;}
    Z.resize(M);C.resize(M);
    for(int i=0;i<M;i++)scanf("%d %d",&Z[i],&C[i]);
    // segments
    vector<set<pair<int,int>>> seg(R);
    for(int r=0;r<R;r++){
        int s=0;
        while(s<S){
            if(bad[(size_t)r*S+s]){s++;continue;}
            int e=s; while(e<S&&!bad[(size_t)r*S+e])e++;
            seg[r].insert({e-s,s}); s=e;
        }
    }
    vector<int> ord(M); iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){
        ll x=(ll)C[a]*Z[b], y=(ll)C[b]*Z[a];
        if(x!=y)return x>y; if(C[a]!=C[b])return C[a]>C[b]; return a<b;});
    prow.assign(M,-1);ppos.assign(M,-1);ppool.assign(M,-1);
    vector<ll> load(R,0);
    set<pair<ll,int>> rows; for(int r=0;r<R;r++)rows.insert({0,r});
    int minFail=INT_MAX;
    for(int id:ord){
        int z=Z[id]; if(z>=minFail)continue;
        int found=-1;
        for(auto &pr:rows){
            int r=pr.second;
            if(!seg[r].empty() && seg[r].rbegin()->first>=z){found=r;break;}
        }
        if(found<0){minFail=z;continue;}
        int r=found;
        auto it=seg[r].lower_bound({z,-1});
        int len=it->first,st=it->second; seg[r].erase(it);
        if(len>z)seg[r].insert({len-z,st+z});
        prow[id]=r;ppos[id]=st;
        rows.erase({load[r],r}); load[r]+=C[id]; rows.insert({load[r],r});
    }
    vector<int> placed;
    for(int i=0;i<M;i++)if(prow[i]>=0)placed.push_back(i);
    sort(placed.begin(),placed.end(),[&](int a,int b){if(C[a]!=C[b])return C[a]>C[b];return a<b;});
    rowsum.assign((size_t)P*R,0);total.assign(P,0);gcv.assign(P,0);
    vector<ll> mxr(P,0);
    {
        priority_queue<tuple<ll,ll,int>,vector<tuple<ll,ll,int>>,greater<tuple<ll,ll,int>>> pq;
        for(int p=0;p<P;p++)pq.push({0,0,p});
        for(int id:placed){
            auto [g,tt,p]=pq.top();pq.pop();
            ppool[id]=p; int r=prow[id];
            rowsum[(size_t)p*R+r]+=C[id]; total[p]+=C[id];
            mxr[p]=max(mxr[p],rowsum[(size_t)p*R+r]);
            gcv[p]=total[p]-mxr[p];
            pq.push({gcv[p],-total[p],p});
        }
    }
    // pool member lists
    vector<vector<int>> mem(P); vector<int> mpos(M,-1);
    for(int id:placed){mpos[id]=mem[ppool[id]].size();mem[ppool[id]].push_back(id);}
    auto remMem=[&](int id){int p=ppool[id];int i=mpos[id];int l=mem[p].back();mem[p][i]=l;mpos[l]=i;mem[p].pop_back();};
    auto addMem=[&](int id,int p){mpos[id]=mem[p].size();mem[p].push_back(id);};
    mt19937_64 rng(12345);
    if(!placed.empty()&&P>1){
        long long it=0,fails=0;
        while(true){
            if((it&255)==0){
                double el=chrono::duration<double>(chrono::steady_clock::now()-t0).count();
                if(el>8.3)break;
            }
            it++;
            if(fails>3000000)break;
            int w=0; for(int p=1;p<P;p++)if(gcv[p]<gcv[w])w=p;
            ll cur=gcv[w];
            int b=placed[rng()%placed.size()];
            int q=ppool[b]; if(q==w){fails++;continue;}
            int rb=prow[b]; ll cb=C[b];
            bool done=false;
            ll gw=evalP(w,rb,cb,-1,0);
            if(gw>cur){
                ll gq=evalP(q,rb,-cb,-1,0);
                if(gq>cur){
                    rowsum[(size_t)w*R+rb]+=cb;total[w]+=cb;
                    rowsum[(size_t)q*R+rb]-=cb;total[q]-=cb;
                    gcv[w]=gw;gcv[q]=gq;
                    remMem(b);ppool[b]=w;addMem(b,w);done=true;
                }
            }
            if(!done && !mem[w].empty()){
                int a=mem[w][rng()%mem[w].size()];
                int ra=prow[a]; ll ca=C[a];
                if(!(ra==rb&&ca==cb)){
                    ll gw2=evalP(w,ra,-ca,rb,cb);
                    if(gw2>cur){
                        ll gq2=evalP(q,rb,-cb,ra,ca);
                        if(gq2>cur){
                            rowsum[(size_t)w*R+ra]-=ca;rowsum[(size_t)w*R+rb]+=cb;total[w]+=cb-ca;
                            rowsum[(size_t)q*R+rb]-=cb;rowsum[(size_t)q*R+ra]+=ca;total[q]+=ca-cb;
                            gcv[w]=gw2;gcv[q]=gq2;
                            remMem(a);remMem(b);ppool[a]=q;ppool[b]=w;addMem(a,q);addMem(b,w);
                            done=true;
                        }
                    }
                }
            }
            if(done)fails=0; else fails++;
        }
    }
    string out; out.reserve((size_t)M*16);
    char buf[64];
    for(int i=0;i<M;i++){
        if(prow[i]<0){out+="x\n";}
        else{snprintf(buf,sizeof(buf),"%d %d %d\n",prow[i],ppos[i],ppool[i]);out+=buf;}
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
