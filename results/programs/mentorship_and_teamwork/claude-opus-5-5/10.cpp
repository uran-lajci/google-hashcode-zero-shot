#include <bits/stdc++.h>
using namespace std;

struct Proj{ string name; int D,S,B,R; vector<pair<int,int>> roles; vector<int> order; };

int C,P,NS=0;
vector<string> cname;
vector<Proj> projs;
vector<vector<int>> holders0, hlev0;
vector<vector<pair<int,int>>> cs0;
vector<int> byFew;
chrono::steady_clock::time_point T0;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();}

struct Result{ long long score=0; vector<pair<int,vector<int>>> sched; };

Result runGreedy(const vector<int>& prio, double deadline){
    Result res;
    vector<vector<int>> holders=holders0, hlev=hlev0;
    vector<vector<pair<int,int>>> cs=cs0;
    vector<long long> busy(C,0);
    vector<int> inP(C,0), mst(NS,0), mmax(NS,0);
    int stamp=0;
    long long t=0;
    priority_queue<long long, vector<long long>, greater<long long>> heap;
    vector<int> alive=prio;
    auto hasSkill=[&](int c,int s){ for(auto&pr:cs[c]) if(pr.first==s) return true; return false; };
    long long iter=0; bool timeout=false;
    while(!alive.empty() && !timeout){
        while(!heap.empty() && heap.top()<=t) heap.pop();
        int freeCount=C-(int)heap.size();
        vector<int> next; next.reserve(alive.size());
        for(size_t ai=0; ai<alive.size(); ai++){
            int p=alive[ai];
            Proj&pr=projs[p];
            if(t+pr.D-pr.B>=pr.S) continue;
            if(timeout){ next.push_back(p); continue; }
            if((++iter&63)==0 && elapsed()>deadline){ timeout=true; next.push_back(p); continue; }
            if(pr.R>freeCount){ next.push_back(p); continue; }
            stamp++;
            vector<int> asg(pr.R,-1);
            bool ok=true;
            for(int pass=0;pass<2 && ok;pass++){
                for(int idx:pr.order){
                    if(asg[idx]!=-1) continue;
                    int s=pr.roles[idx].first, L=pr.roles[idx].second;
                    bool mentor=(mst[s]==stamp && mmax[s]>=L);
                    int pick=-1,best=-1,bestLvl=INT_MAX;
                    auto&H=holders[s]; auto&HL=hlev[s];
                    for(size_t k=0;k<H.size();k++){
                        int c=H[k];
                        if(busy[c]>t||inP[c]==stamp) continue;
                        int lv=HL[k];
                        if(mentor && lv==L-1){pick=c;break;}
                        if(lv>=L && (lv<bestLvl || (lv==bestLvl && cs[c].size()<cs[best].size()))){best=c;bestLvl=lv;}
                    }
                    if(pick<0 && mentor && L==1){
                        for(int c:byFew){
                            if(busy[c]>t||inP[c]==stamp) continue;
                            if(!hasSkill(c,s)){pick=c;break;}
                        }
                    }
                    if(pick<0) pick=best;
                    if(pick<0){ if(pass==1){ok=false;break;} continue; }
                    asg[idx]=pick; inP[pick]=stamp;
                    for(auto&q:cs[pick]){
                        int lv=hlev[q.first][q.second];
                        if(mst[q.first]!=stamp){mst[q.first]=stamp;mmax[q.first]=lv;}
                        else mmax[q.first]=max(mmax[q.first],lv);
                    }
                }
            }
            if(!ok){ next.push_back(p); continue; }
            // apply
            for(int i=0;i<pr.R;i++){
                int c=asg[i], s=pr.roles[i].first, L=pr.roles[i].second;
                int found=-1;
                for(auto&q:cs[c]) if(q.first==s){found=q.second;break;}
                int lv = found>=0 ? hlev[s][found] : 0;
                if(lv<=L){
                    if(found>=0) hlev[s][found]++;
                    else { holders[s].push_back(c); hlev[s].push_back(1); cs[c].push_back({s,(int)holders[s].size()-1}); }
                }
                busy[c]=t+pr.D;
                heap.push(t+pr.D);
            }
            freeCount-=pr.R;
            res.score+=pr.S-max(0LL,t+pr.D-pr.B);
            res.sched.push_back({p,asg});
        }
        alive.swap(next);
        if(alive.empty()) break;
        while(!heap.empty() && heap.top()<=t) heap.pop();
        if(heap.empty()) break;
        t=heap.top();
    }
    return res;
}

int main(){
    T0=chrono::steady_clock::now();
    ios::sync_with_stdio(false); cin.tie(nullptr);
    if(!(cin>>C>>P)) return 0;
    unordered_map<string,int> sid;
    auto getS=[&](const string&s){ auto it=sid.find(s); if(it!=sid.end()) return it->second; sid[s]=NS; holders0.emplace_back(); hlev0.emplace_back(); return NS++; };
    cname.resize(C); cs0.resize(C);
    for(int i=0;i<C;i++){
        int n; cin>>cname[i]>>n;
        for(int j=0;j<n;j++){
            string s; int l; cin>>s>>l; int id=getS(s);
            if(l<=0) continue;
            bool dup=false;
            for(auto&q:cs0[i]) if(q.first==id){ hlev0[id][q.second]=max(hlev0[id][q.second],l); dup=true; }
            if(dup) continue;
            holders0[id].push_back(i); hlev0[id].push_back(l);
            cs0[i].push_back({id,(int)holders0[id].size()-1});
        }
    }
    projs.resize(P);
    for(int i=0;i<P;i++){
        Proj&p=projs[i];
        cin>>p.name>>p.D>>p.S>>p.B>>p.R;
        p.roles.resize(p.R);
        for(int j=0;j<p.R;j++){ string s; int l; cin>>s>>l; p.roles[j]={getS(s),l}; }
        p.order.resize(p.R); iota(p.order.begin(),p.order.end(),0);
        stable_sort(p.order.begin(),p.order.end(),[&](int a,int b){return p.roles[a].second>p.roles[b].second;});
    }
    byFew.resize(C); iota(byFew.begin(),byFew.end(),0);
    stable_sort(byFew.begin(),byFew.end(),[&](int a,int b){return cs0[a].size()<cs0[b].size();});

    vector<function<double(const Proj&)>> keys={
        [](const Proj&p){return (double)p.S/((double)p.D*p.R);},
        [](const Proj&p){return -(double)p.B;},
        [](const Proj&p){return (double)p.S/p.D;},
        [](const Proj&p){return (double)p.S/p.R;},
    };
    Result best; bool have=false;
    double total=9.0;
    for(size_t k=0;k<keys.size();k++){
        double now=elapsed();
        if(now>total) break;
        double budget=(total-now)/(keys.size()-k);
        vector<double> kv(P); for(int i=0;i<P;i++) kv[i]=keys[k](projs[i]);
        vector<int> prio(P); iota(prio.begin(),prio.end(),0);
        stable_sort(prio.begin(),prio.end(),[&](int a,int b){return kv[a]>kv[b];});
        Result r=runGreedy(prio, now+budget);
        if(!have || r.score>best.score){best=move(r);have=true;}
    }
    string out;
    out+=to_string(best.sched.size())+"\n";
    for(auto&e:best.sched){
        out+=projs[e.first].name+"\n";
        for(size_t i=0;i<e.second.size();i++){ if(i) out+=' '; out+=cname[e.second[i]]; }
        out+="\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
    return 0;
}
