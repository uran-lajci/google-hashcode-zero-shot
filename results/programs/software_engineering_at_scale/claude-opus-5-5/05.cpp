#include <bits/stdc++.h>
using namespace std;

int L,G,S,B,F,N;
vector<string> sname, fname;
vector<int> binOf, Rb;
vector<int> FD; vector<long long> FU;
vector<vector<int>> fb; // distinct binaries, sorted ascending by base
vector<int> maxBase; vector<long long> sumBase;

struct Task{int type; int a; int b;}; // type 0 wait a days; 1 impl feature a bin b

long long simulate(double pa,double pb, vector<vector<Task>>& tasks){
    tasks.assign(G, {});
    vector<int> freeAt(G,0);
    vector<int> cnt(B,0);
    vector<vector<pair<int,int>>> bucket(L+1);
    set<int> freeSet;
    for(int i=0;i<G;i++) freeSet.insert(i);
    vector<char> dead(F,0), started(F,0);
    vector<int> finishF(F,0);
    vector<vector<int>> pending(F);
    deque<int> dq;
    vector<int> cand(F); iota(cand.begin(),cand.end(),0);
    for(int t=0;t<L;t++){
        for(auto&pr:bucket[t]){ freeSet.insert(pr.first); cnt[pr.second]--; }
        while(!freeSet.empty()){
            int e=*freeSet.begin();
            int tf=-1,tb=-1,dur=0;
            while(true){
                bool got=false;
                while(!dq.empty()){
                    int f=dq.front();
                    if(dead[f]||pending[f].empty()){dq.pop_front();continue;}
                    int b=pending[f].back();
                    int d=FD[f]+Rb[b]+cnt[b];
                    if(t+d>=L){dead[f]=1;dq.pop_front();continue;}
                    pending[f].pop_back();
                    tf=f;tb=b;dur=d;got=true;
                    finishF[f]=max(finishF[f],t+d);
                    break;
                }
                if(got) break;
                // pick new feature
                int best=-1; double bp=-1;
                for(size_t i=0;i<cand.size();){
                    int f=cand[i];
                    if(t+maxBase[f]>=L){cand[i]=cand.back();cand.pop_back();continue;}
                    double rem=L-t-maxBase[f];
                    double p=(double)FU[f]*pow(rem,pa)/pow((double)sumBase[f],pb);
                    if(p>bp||(p==bp&&f<best)){bp=p;best=f;}
                    i++;
                }
                if(best<0) break;
                for(size_t i=0;i<cand.size();i++) if(cand[i]==best){cand[i]=cand.back();cand.pop_back();break;}
                started[best]=1;
                pending[best]=fb[best];
                dq.push_back(best);
            }
            if(tf<0) break;
            if(freeAt[e]<t) tasks[e].push_back({0,t-freeAt[e],0});
            tasks[e].push_back({1,tf,tb});
            cnt[tb]++;
            int end=t+dur;
            freeAt[e]=end;
            freeSet.erase(freeSet.begin());
            if(end<=L) bucket[end].push_back({e,tb});
        }
    }
    long long score=0;
    for(int f=0;f<F;f++){
        if(started[f]&&!dead[f]&&pending[f].empty()&&finishF[f]<L)
            score+=FU[f]*(long long)(L-finishF[f]);
    }
    return score;
}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>L>>G>>S>>B>>F>>N;
    unordered_map<string,int> sid; sid.reserve(S*2);
    sname.resize(S); binOf.resize(S); Rb.assign(B,0);
    for(int i=0;i<S;i++){cin>>sname[i]>>binOf[i];sid[sname[i]]=i;Rb[binOf[i]]++;}
    fname.resize(F);FD.resize(F);FU.resize(F);fb.resize(F);maxBase.resize(F);sumBase.resize(F);
    for(int i=0;i<F;i++){
        int m;cin>>fname[i]>>m>>FD[i]>>FU[i];
        vector<int> bs;
        for(int j=0;j<m;j++){string s;cin>>s;bs.push_back(binOf[sid[s]]);}
        sort(bs.begin(),bs.end());bs.erase(unique(bs.begin(),bs.end()),bs.end());
        sort(bs.begin(),bs.end(),[&](int x,int y){ if(Rb[x]!=Rb[y]) return Rb[x]<Rb[y]; return x<y;});
        fb[i]=bs;
        int mx=0; long long sm=0;
        for(int b:bs){mx=max(mx,FD[i]+Rb[b]); sm+=FD[i]+Rb[b];}
        maxBase[i]=mx; sumBase[i]=sm;
    }
    vector<pair<double,double>> vars={{1,1},{1,0.5},{0,1},{1,1.5},{2,1},{0.5,1},{1,0},{2,1.5}};
    auto st=chrono::steady_clock::now();
    long long bestS=-1; vector<vector<Task>> bestT, cur;
    for(auto&v:vars){
        double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
        if(bestS>=0 && el>6.0) break;
        long long sc=simulate(v.first,v.second,cur);
        if(sc>bestS){bestS=sc;bestT.swap(cur);}
    }
    string out;
    int E=0; for(int e=0;e<G;e++) if(!bestT[e].empty()) E++;
    out+=to_string(E)+"\n";
    for(int e=0;e<G;e++){
        if(bestT[e].empty()) continue;
        out+=to_string(bestT[e].size())+"\n";
        for(auto&tk:bestT[e]){
            if(tk.type==0) out+="wait "+to_string(tk.a)+"\n";
            else out+="impl "+fname[tk.a]+" "+to_string(tk.b)+"\n";
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
