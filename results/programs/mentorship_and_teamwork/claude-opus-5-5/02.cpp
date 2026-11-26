#include <bits/stdc++.h>
using namespace std;

int C,P;
vector<string> cname, pname;
vector<vector<pair<int,int>>> initLvl;
vector<int> PD,PS,PB;
vector<vector<pair<int,int>>> roles; // (skill, level)
vector<vector<int>> roleOrder;
int NS=0;
chrono::steady_clock::time_point T0;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();}

struct Run{int p; vector<int> who; vector<char> learn; long long start,end;};

struct Solver{
    vector<vector<pair<int,int>>> lvl;
    vector<map<int,set<int>>> buckets;
    set<int> freeSet;
    vector<char> inTeam;
    vector<Run> runs;
    long long score=0;

    int getLevel(int c,int s){
        auto &v=lvl[c];
        auto it=lower_bound(v.begin(),v.end(),make_pair(s,INT_MIN));
        if(it!=v.end()&&it->first==s) return it->second;
        return 0;
    }
    void incLevel(int c,int s){
        auto &v=lvl[c];
        auto it=lower_bound(v.begin(),v.end(),make_pair(s,INT_MIN));
        if(it!=v.end()&&it->first==s) it->second++;
        else v.insert(it,make_pair(s,1));
    }
    void addFree(int c){
        freeSet.insert(c);
        for(auto &pr:lvl[c]) buckets[pr.first][pr.second].insert(c);
    }
    void removeFree(int c){
        freeSet.erase(c);
        for(auto &pr:lvl[c]){
            auto &m=buckets[pr.first];
            auto it=m.find(pr.second);
            if(it!=m.end()){it->second.erase(c); if(it->second.empty()) m.erase(it);}
        }
    }
    int findCand(int s,int L,const vector<int>&team){
        int tm=0;
        for(int c:team){int l=getLevel(c,s); if(l>tm)tm=l;}
        bool mentor = tm>=L;
        auto &m=buckets[s];
        if(mentor){
            if(L-1>=1){
                auto it=m.find(L-1);
                if(it!=m.end()) for(int c:it->second) if(!inTeam[c]) return c;
            } else {
                int cnt=0;
                for(int c:freeSet){
                    if(++cnt>300)break;
                    if(!inTeam[c]&&getLevel(c,s)==0) return c;
                }
            }
        }
        for(auto it=m.lower_bound(L);it!=m.end();++it)
            for(int c:it->second) if(!inTeam[c]) return c;
        return -1;
    }
    bool tryAssign(int p,long long t){
        auto &R=roles[p];
        int nr=R.size();
        if((int)freeSet.size()<nr) return false;
        for(auto &r:R){
            auto &m=buckets[r.first];
            if(m.empty()) return false;
            if(m.rbegin()->first<r.second) return false;
        }
        vector<int> chosen(nr,-1), team;
        vector<int> pending=roleOrder[p];
        bool progress=true;
        while(!pending.empty()&&progress){
            progress=false;
            vector<int> np;
            for(int k:pending){
                int c=findCand(R[k].first,R[k].second,team);
                if(c<0){np.push_back(k);continue;}
                chosen[k]=c; inTeam[c]=1; team.push_back(c); progress=true;
            }
            pending=np;
        }
        if(!pending.empty()){
            for(int c:team) inTeam[c]=0;
            return false;
        }
        Run r; r.p=p; r.who=chosen; r.learn.resize(nr); r.start=t; r.end=t+PD[p];
        for(int k=0;k<nr;k++){
            r.learn[k]= getLevel(chosen[k],R[k].first)<=R[k].second;
        }
        for(int c:team){inTeam[c]=0; removeFree(c);}
        long long late=max(0LL,r.end-PB[p]);
        score+=max(0LL,(long long)PS[p]-late);
        runs.push_back(r);
        return true;
    }
    void solve(vector<int> order,double deadline){
        lvl=initLvl;
        buckets.assign(NS,{});
        freeSet.clear();
        inTeam.assign(C,0);
        for(int c=0;c<C;c++) addFree(c);
        priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
        long long t=0;
        while(true){
            while(!pq.empty()&&pq.top().first<=t){
                int ri=pq.top().second; pq.pop();
                Run &r=runs[ri];
                auto &R=roles[r.p];
                for(size_t k=0;k<r.who.size();k++){
                    if(r.learn[k]) incLevel(r.who[k],R[k].first);
                    addFree(r.who[k]);
                }
            }
            vector<int> rem; rem.reserve(order.size());
            bool timeout=false;
            for(size_t i=0;i<order.size();i++){
                int p=order[i];
                if(timeout){rem.push_back(p);continue;}
                if((i&255)==0&&elapsed()>deadline){timeout=true;rem.push_back(p);continue;}
                long long eff=(long long)PS[p]-max(0LL,t+PD[p]-PB[p]);
                if(eff<=0) continue;
                if(tryAssign(p,t)){
                    pq.push({runs.back().end,(int)runs.size()-1});
                } else rem.push_back(p);
            }
            order.swap(rem);
            if(timeout||elapsed()>deadline) break;
            if(pq.empty()||order.empty()) break;
            t=pq.top().first;
        }
    }
};

int main(){
    T0=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>P;
    unordered_map<string,int> sid;
    auto getS=[&](const string&s){auto it=sid.find(s); if(it!=sid.end())return it->second; sid[s]=NS; return NS++;};
    cname.resize(C); initLvl.resize(C);
    for(int i=0;i<C;i++){
        int n;cin>>cname[i]>>n;
        for(int j=0;j<n;j++){string s;int l;cin>>s>>l;int id=getS(s); if(l>0) initLvl[i].push_back({id,l});}
        sort(initLvl[i].begin(),initLvl[i].end());
        // merge duplicates if any
        vector<pair<int,int>> v;
        for(auto &pr:initLvl[i]){ if(!v.empty()&&v.back().first==pr.first) v.back().second=max(v.back().second,pr.second); else v.push_back(pr);}
        initLvl[i]=v;
    }
    pname.resize(P);PD.resize(P);PS.resize(P);PB.resize(P);roles.resize(P);roleOrder.resize(P);
    for(int i=0;i<P;i++){
        int r;cin>>pname[i]>>PD[i]>>PS[i]>>PB[i]>>r;
        for(int j=0;j<r;j++){string s;int l;cin>>s>>l;roles[i].push_back({getS(s),l});}
        vector<int> o(r);iota(o.begin(),o.end(),0);
        stable_sort(o.begin(),o.end(),[&](int a,int b){return roles[i][a].second>roles[i][b].second;});
        roleOrder[i]=o;
    }
    vector<vector<int>> orders;
    {
        vector<int> o(P);iota(o.begin(),o.end(),0);
        auto o1=o; stable_sort(o1.begin(),o1.end(),[&](int a,int b){
            double ka=(double)PS[a]/((double)PD[a]*roles[a].size()), kb=(double)PS[b]/((double)PD[b]*roles[b].size());
            if(ka!=kb) return ka>kb; return PB[a]<PB[b];});
        orders.push_back(o1);
        auto o2=o; stable_sort(o2.begin(),o2.end(),[&](int a,int b){
            if(PB[a]!=PB[b]) return PB[a]<PB[b]; return PS[a]>PS[b];});
        orders.push_back(o2);
        auto o3=o; stable_sort(o3.begin(),o3.end(),[&](int a,int b){
            double ka=(double)PS[a]/PD[a], kb=(double)PS[b]/PD[b];
            if(ka!=kb) return ka>kb; return PB[a]<PB[b];});
        orders.push_back(o3);
        auto o4=o; stable_sort(o4.begin(),o4.end(),[&](int a,int b){
            double ka=(double)PS[a]/roles[a].size(), kb=(double)PS[b]/roles[b].size();
            if(ka!=kb) return ka>kb; return PB[a]<PB[b];});
        orders.push_back(o4);
    }
    const double TOTAL=9.0;
    long long best=-1; vector<Run> bestRuns;
    for(size_t i=0;i<orders.size();i++){
        double now=elapsed();
        if(now>TOTAL-0.2&&best>=0) break;
        double dl=now+(TOTAL-now)/(orders.size()-i);
        Solver s; s.solve(orders[i],dl);
        if(s.score>best){best=s.score;bestRuns=move(s.runs);}
    }
    string out;
    out+=to_string(bestRuns.size())+"\n";
    for(auto &r:bestRuns){
        out+=pname[r.p]; out+="\n";
        for(size_t k=0;k<r.who.size();k++){ if(k) out+=" "; out+=cname[r.who[k]]; }
        out+="\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
}
