#include <bits/stdc++.h>
using namespace std;
using Clock=chrono::steady_clock;

int C,P,NS=0;
vector<string> cname,pname;
vector<vector<pair<int,int>>> cskills0; // skill, level
vector<int> PD,PS,PB;
vector<vector<pair<int,int>>> roles; // skill, level
vector<vector<int>> roleOrder;
unordered_map<string,int> skillId;
int getSkill(const string&s){auto it=skillId.find(s);if(it!=skillId.end())return it->second;skillId[s]=NS;return NS++;}

struct Entry{int c;int lvl;};
vector<vector<Entry>> slist;
vector<vector<pair<int,int>>> cs; // skill, idx in slist
vector<long long> avail;
vector<int> stampArr; int stampId=0;
vector<int> byFew;

int lvl(int c,int s){for(auto&pr:cs[c]) if(pr.first==s) return slist[s][pr.second].lvl; return 0;}
void incLvl(int c,int s){
    for(auto&pr:cs[c]) if(pr.first==s){slist[s][pr.second].lvl++;return;}
    slist[s].push_back({c,1}); cs[c].push_back({s,(int)slist[s].size()-1});
}

bool tryAssign(int p,long long t,vector<int>&team){
    int R=roles[p].size();
    team.assign(R,-1);
    stampId++;
    for(int pass=0;pass<2;pass++){
        bool all=true;
        for(int ri:roleOrder[p]){
            if(team[ri]>=0) continue;
            int s=roles[p][ri].first,L=roles[p][ri].second;
            bool hasM=false;
            for(int j=0;j<R&&!hasM;j++) if(team[j]>=0 && lvl(team[j],s)>=L) hasM=true;
            int best=-1,bl=INT_MAX;
            for(auto&e:slist[s]){
                if(e.lvl<L-1) continue;
                if(e.lvl==L-1 && !hasM) continue;
                if(avail[e.c]>t||stampArr[e.c]==stampId) continue;
                if(e.lvl<bl){bl=e.lvl;best=e.c;if(bl==L-1)break;}
            }
            if(best<0 && L==1 && hasM){
                for(int c:byFew){
                    if(avail[c]>t||stampArr[c]==stampId) continue;
                    if(lvl(c,s)==0){best=c;break;}
                }
            }
            if(best>=0){team[ri]=best;stampArr[best]=stampId;}
            else all=false;
        }
        if(all) return true;
    }
    return false;
}

struct Result{long long score=0;vector<pair<int,vector<int>>> out;};

Result run(const vector<int>&order,Clock::time_point deadline){
    Result res;
    slist.assign(NS,{});cs.assign(C,{});
    for(int c=0;c<C;c++) for(auto&pr:cskills0[c]){slist[pr.first].push_back({c,pr.second});cs[c].push_back({pr.first,(int)slist[pr.first].size()-1});}
    avail.assign(C,0);
    vector<int> rem=order;
    priority_queue<long long,vector<long long>,greater<long long>> pq;
    long long t=0; int freeCount=C;
    vector<int> team; long long cnt=0; bool timeout=false;
    while(true){
        vector<int> nr; nr.reserve(rem.size());
        for(size_t i=0;i<rem.size();i++){
            int p=rem[i];
            if(timeout){nr.push_back(p);continue;}
            if(((++cnt)&63)==0 && Clock::now()>deadline){timeout=true;nr.push_back(p);continue;}
            if(t+PD[p]-PB[p]>=PS[p]) continue;
            int R=roles[p].size();
            if(freeCount<R){nr.push_back(p);continue;}
            if(tryAssign(p,t,team)){
                long long sc=PS[p]-max(0LL,t+PD[p]-PB[p]);
                res.score+=sc;
                for(int j=0;j<R;j++){
                    int c=team[j];int s=roles[p][j].first,L=roles[p][j].second;
                    if(lvl(c,s)<=L) incLvl(c,s);
                    avail[c]=t+PD[p];
                }
                pq.push(t+PD[p]);
                freeCount-=R;
                res.out.push_back({p,team});
            } else nr.push_back(p);
        }
        rem.swap(nr);
        if(timeout||rem.empty()) break;
        while(!pq.empty()&&pq.top()<=t) pq.pop();
        if(pq.empty()) break;
        t=pq.top();
        while(!pq.empty()&&pq.top()<=t) pq.pop();
        freeCount=0;
        for(int c=0;c<C;c++) if(avail[c]<=t) freeCount++;
    }
    return res;
}

int main(){
    auto start=Clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>P;
    cname.resize(C);cskills0.resize(C);
    for(int c=0;c<C;c++){
        int n;cin>>cname[c]>>n;
        for(int i=0;i<n;i++){string s;int l;cin>>s>>l;cskills0[c].push_back({getSkill(s),l});}
    }
    pname.resize(P);PD.resize(P);PS.resize(P);PB.resize(P);roles.resize(P);roleOrder.resize(P);
    for(int p=0;p<P;p++){
        int r;cin>>pname[p]>>PD[p]>>PS[p]>>PB[p]>>r;
        for(int i=0;i<r;i++){string s;int l;cin>>s>>l;roles[p].push_back({getSkill(s),l});}
        roleOrder[p].resize(r);iota(roleOrder[p].begin(),roleOrder[p].end(),0);
        stable_sort(roleOrder[p].begin(),roleOrder[p].end(),[&](int a,int b){return roles[p][a].second>roles[p][b].second;});
    }
    stampArr.assign(C,0);
    byFew.resize(C);iota(byFew.begin(),byFew.end(),0);
    {
        vector<long long> tot(C,0);
        for(int c=0;c<C;c++) for(auto&pr:cskills0[c]) tot[c]+=pr.second;
        stable_sort(byFew.begin(),byFew.end(),[&](int a,int b){return tot[a]<tot[b];});
    }
    vector<vector<int>> orders;
    vector<int> base(P);iota(base.begin(),base.end(),0);
    {
        auto o=base;
        stable_sort(o.begin(),o.end(),[&](int a,int b){
            return (double)PS[a]/((double)PD[a]*roles[a].size())>(double)PS[b]/((double)PD[b]*roles[b].size());});
        orders.push_back(o);
    }
    {
        auto o=base;
        stable_sort(o.begin(),o.end(),[&](int a,int b){if(PB[a]!=PB[b])return PB[a]<PB[b];return PS[a]>PS[b];});
        orders.push_back(o);
    }
    {
        auto o=base;
        stable_sort(o.begin(),o.end(),[&](int a,int b){return PS[a]>PS[b];});
        orders.push_back(o);
    }
    {
        auto o=base;
        stable_sort(o.begin(),o.end(),[&](int a,int b){return (long long)PD[a]*roles[a].size()<(long long)PD[b]*roles[b].size();});
        orders.push_back(o);
    }
    Result best; bool have=false;
    double totalBudget=8.6;
    for(size_t i=0;i<orders.size();i++){
        double el=chrono::duration<double>(Clock::now()-start).count();
        double left=totalBudget-el;
        if(left<=0.05) break;
        double per=left/(orders.size()-i);
        auto dl=Clock::now()+chrono::milliseconds((long long)(per*1000));
        Result r=run(orders[i],dl);
        if(!have||r.score>best.score){best=move(r);have=true;}
    }
    string out;
    out+=to_string(best.out.size())+"\n";
    for(auto&pr:best.out){
        out+=pname[pr.first]+"\n";
        for(size_t j=0;j<pr.second.size();j++){if(j)out+=' ';out+=cname[pr.second[j]];}
        out+='\n';
    }
    fwrite(out.data(),1,out.size(),stdout);
}
