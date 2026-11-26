#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int L,G,S,B,F,N;
    if(!(cin>>L>>G>>S>>B>>F>>N))return 0;
    unordered_map<string,int> sid; sid.reserve(S*2+10);
    vector<int> binOf(S);
    vector<long long> R(B,0);
    for(int i=0;i<S;i++){string nm;int b;cin>>nm>>b;sid[nm]=i;binOf[i]=b;R[b]++;}
    vector<string> fname(F);
    vector<long long> D(F),U(F),W(F),MX(F);
    vector<vector<int>> bins(F);
    for(int i=0;i<F;i++){
        int m;cin>>fname[i]>>m>>D[i]>>U[i];
        vector<int> bs;bs.reserve(m);
        for(int j=0;j<m;j++){string s;cin>>s;bs.push_back(binOf[sid[s]]);}
        sort(bs.begin(),bs.end());bs.erase(unique(bs.begin(),bs.end()),bs.end());
        bins[i]=bs;
        long long w=0,mx=0;
        for(int b:bs){long long d=D[i]+R[b];w+=d;mx=max(mx,d);}
        W[i]=w;MX[i]=mx;
    }
    vector<long long> busy(B,0);
    vector<vector<pair<int,int>>> ends(L+1);
    set<int> freeE; for(int e=0;e<G;e++)freeE.insert(e);
    vector<int> freeAt(G,0);
    vector<vector<array<int,2>>> tasks(G);
    vector<int> state(F,0); // 0 unstarted,1 active,2 done,3 abandoned
    vector<int> nextIdx(F,0);
    vector<int> active;
    auto assign=[&](int e,int f,int b,int t,int dur){
        if(freeAt[e]<t) tasks[e].push_back({-1,t-freeAt[e]});
        tasks[e].push_back({f,b});
        busy[b]++;
        ends[t+dur].push_back({e,b});
        freeAt[e]=t+dur;
    };
    int unstartedCount=F;
    for(int t=0;t<L;t++){
        for(auto&pr:ends[t]){busy[pr.second]--;freeE.insert(pr.first);}
        if(freeE.empty())continue;
        // active features
        vector<int> na;
        for(int f:active){
            if(state[f]!=1)continue;
            while(!freeE.empty()&&nextIdx[f]<(int)bins[f].size()){
                int b=bins[f][nextIdx[f]];
                long long dur=D[f]+R[b]+busy[b];
                if(t+dur>=L){state[f]=3;break;}
                int e=*freeE.begin();freeE.erase(freeE.begin());
                assign(e,f,b,t,(int)dur);
                nextIdx[f]++;
            }
            if(state[f]==1){
                if(nextIdx[f]>=(int)bins[f].size())state[f]=2;
                else na.push_back(f);
            }
        }
        active.swap(na);
        if(freeE.empty()||unstartedCount==0)continue;
        long long m0=freeE.size();
        vector<pair<double,int>> cand;
        for(int f=0;f<F;f++){
            if(state[f]!=0)continue;
            long long k=bins[f].size();
            long long m=min(k,m0);
            long long est=t+max(MX[f],(W[f]+m-1)/m);
            if(est>=L)continue;
            double v=(double)U[f]*(double)(L-est)/(double)W[f];
            cand.push_back({-v,f});
        }
        sort(cand.begin(),cand.end());
        for(auto&c:cand){
            if(freeE.empty())break;
            int f=c.second;
            long long k=bins[f].size();
            long long m=min(k,(long long)freeE.size());
            long long est=t+max(MX[f],(W[f]+m-1)/m);
            if(est>=L)continue;
            // precise check for immediately assignable tasks
            long long cnt=min(k,(long long)freeE.size());
            bool ok=true;
            {
                // simulate busy increments (bins distinct so each busy +0 from this feature)
                for(long long i=0;i<cnt;i++){
                    int b=bins[f][i];
                    if(t+D[f]+R[b]+busy[b]>=L){ok=false;break;}
                }
            }
            if(!ok)continue;
            state[f]=1;unstartedCount--;
            for(long long i=0;i<cnt;i++){
                int b=bins[f][i];
                long long dur=D[f]+R[b]+busy[b];
                int e=*freeE.begin();freeE.erase(freeE.begin());
                assign(e,f,b,t,(int)dur);
            }
            nextIdx[f]=(int)cnt;
            if(nextIdx[f]>=(int)k)state[f]=2;
            else active.push_back(f);
        }
    }
    // output
    string out;
    int E=0;
    for(int e=0;e<G;e++)if(!tasks[e].empty())E++;
    out+=to_string(E)+"\n";
    for(int e=0;e<G;e++){
        if(tasks[e].empty())continue;
        out+=to_string(tasks[e].size())+"\n";
        for(auto&tk:tasks[e]){
            if(tk[0]<0){out+="wait "+to_string(tk[1])+"\n";}
            else{out+="impl "+fname[tk[0]]+" "+to_string(tk[1])+"\n";}
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
