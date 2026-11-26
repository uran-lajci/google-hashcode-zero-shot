#include <bits/stdc++.h>
using namespace std;

int L,G,S,B,F,N;
vector<string> sname, fname;
vector<int> sbin, Rb, D, U;
vector<vector<int>> fbins;
vector<int> spanv, workv;

struct Result{ long long score; vector<vector<pair<int,int>>> tasks; };

Result sim(const vector<int>& order){
    Result res; res.score=0;
    res.tasks.assign(G,{});
    vector<int> C(B,0), freeSince(G,0);
    vector<vector<pair<int,int>>> endB(L+1);
    vector<int> idle(G); iota(idle.begin(),idle.end(),0);
    size_t pos=0; int cur=-1; size_t idx=0; int curMax=0;
    bool exhausted=false;
    for(int t=0;t<L && !exhausted;t++){
        vector<int> fr; fr.swap(idle);
        for(auto&pr:endB[t]){ C[pr.second]--; fr.push_back(pr.first);}
        if(fr.empty()) continue;
        sort(fr.begin(),fr.end());
        size_t k=0;
        for(;k<fr.size();k++){
            int e=fr[k];
            int gf=-1,gb=-1,gd=0;
            while(true){
                if(cur>=0 && idx<fbins[cur].size()){
                    int b=fbins[cur][idx];
                    int dur=D[cur]+Rb[b]+C[b];
                    if(t+dur>=L){cur=-1;continue;}
                    idx++;
                    gf=cur;gb=b;gd=dur;
                    curMax=max(curMax,t+dur);
                    if(idx==fbins[cur].size()){
                        res.score+=(long long)U[cur]*(L-curMax);
                        cur=-1;
                    }
                    break;
                }
                if(pos>=order.size()) break;
                int f=order[pos++];
                int mx=0;
                for(int b:fbins[f]) mx=max(mx,D[f]+Rb[b]+C[b]);
                if(t+mx>=L) continue;
                cur=f; idx=0; curMax=0;
            }
            if(gf<0) break;
            if(t>freeSince[e]) res.tasks[e].push_back({-1,t-freeSince[e]});
            res.tasks[e].push_back({gf,gb});
            C[gb]++;
            int en=t+gd;
            freeSince[e]=en;
            endB[en].push_back({e,gb}); // en < L
        }
        if(k<fr.size()){
            for(;k<fr.size();k++) idle.push_back(fr[k]);
            if(pos>=order.size() && cur<0) exhausted=true;
        }
    }
    return res;
}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>L>>G>>S>>B>>F>>N;
    sname.resize(S);sbin.resize(S);Rb.assign(B,0);
    unordered_map<string,int> smap; smap.reserve(S*2);
    for(int i=0;i<S;i++){cin>>sname[i]>>sbin[i];smap[sname[i]]=i;Rb[sbin[i]]++;}
    fname.resize(F);D.resize(F);U.resize(F);fbins.resize(F);
    spanv.resize(F);workv.resize(F);
    for(int i=0;i<F;i++){
        int m;cin>>fname[i]>>m>>D[i]>>U[i];
        vector<int> bs;
        for(int j=0;j<m;j++){string s;cin>>s;bs.push_back(sbin[smap[s]]);}
        sort(bs.begin(),bs.end());bs.erase(unique(bs.begin(),bs.end()),bs.end());
        sort(bs.begin(),bs.end(),[&](int a,int b){if(Rb[a]!=Rb[b])return Rb[a]>Rb[b];return a<b;});
        fbins[i]=bs;
        int sp=0;long long w=0;
        for(int b:bs){sp=max(sp,D[i]+Rb[b]);w+=D[i]+Rb[b];}
        spanv[i]=sp;workv[i]=(int)min<long long>(w,INT_MAX);
    }
    vector<int> cand;
    for(int i=0;i<F;i++) if(spanv[i]<L) cand.push_back(i);
    vector<double> exps={0.0,0.5,1.0,1.5,2.0};
    Result best; best.score=-1;
    for(int mode=0;mode<(int)exps.size()+1;mode++){
        vector<double> pr(F,0);
        for(int f:cand){
            if(mode<(int)exps.size())
                pr[f]=(double)U[f]*(L-spanv[f])/pow((double)workv[f],exps[mode]);
            else
                pr[f]=(double)U[f]/workv[f];
        }
        vector<int> order=cand;
        stable_sort(order.begin(),order.end(),[&](int a,int b){if(pr[a]!=pr[b])return pr[a]>pr[b];return a<b;});
        Result r=sim(order);
        if(r.score>best.score) best=std::move(r);
    }
    string out;
    int E=0;
    for(int e=0;e<G;e++) if(!best.tasks[e].empty()) E++;
    out+=to_string(E)+"\n";
    for(int e=0;e<G;e++){
        auto&tk=best.tasks[e];
        if(tk.empty())continue;
        out+=to_string(tk.size())+"\n";
        for(auto&p:tk){
            if(p.first<0) out+="wait "+to_string(p.second)+"\n";
            else out+="impl "+fname[p.first]+" "+to_string(p.second)+"\n";
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
}
