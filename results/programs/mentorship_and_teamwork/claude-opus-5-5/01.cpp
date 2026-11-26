#include <bits/stdc++.h>
using namespace std;

int C,P,S=0;
unordered_map<string,int> skillId;
vector<string> cname;
bool dense;
vector<uint8_t> lvD;
vector<unordered_map<int,int>> lvS;
inline int getL(int c,int s){
    if(dense) return lvD[(size_t)c*S+s];
    auto it=lvS[c].find(s); return it==lvS[c].end()?0:it->second;
}
inline void setL(int c,int s,int v){
    if(dense) lvD[(size_t)c*S+s]=(uint8_t)v; else lvS[c][s]=v;
}
struct Proj{string name;int D,Sc,B,R;vector<pair<int,int>> roles;vector<int> order;};

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    auto T0=chrono::steady_clock::now();
    auto elapsed=[&](){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();};
    cin>>C>>P;
    cname.resize(C);
    vector<vector<pair<int,int>>> initSk(C);
    auto gid=[&](const string&s){auto it=skillId.find(s);if(it!=skillId.end())return it->second;int id=S++;skillId[s]=id;return id;};
    for(int i=0;i<C;i++){int n;cin>>cname[i]>>n;for(int j=0;j<n;j++){string s;int l;cin>>s>>l;initSk[i].push_back({gid(s),l});}}
    vector<Proj> pr(P);
    for(int i=0;i<P;i++){
        cin>>pr[i].name>>pr[i].D>>pr[i].Sc>>pr[i].B>>pr[i].R;
        for(int j=0;j<pr[i].R;j++){string s;int l;cin>>s>>l;pr[i].roles.push_back({gid(s),l});}
        pr[i].order.resize(pr[i].R);iota(pr[i].order.begin(),pr[i].order.end(),0);
        auto&rl=pr[i].roles;
        stable_sort(pr[i].order.begin(),pr[i].order.end(),[&](int a,int b){return rl[a].second>rl[b].second;});
    }
    dense=(long long)C*max(S,1)<=300000000LL;
    if(dense) lvD.assign((size_t)C*max(S,1),0); else lvS.resize(C);
    vector<vector<int>> holders(S);
    vector<int> nskills(C,0);
    for(int i=0;i<C;i++)for(auto&pp:initSk[i]){
        if(getL(i,pp.first)==0&&pp.second>0){holders[pp.first].push_back(i);nskills[i]++;}
        setL(i,pp.first,pp.second);
    }
    vector<long long> avail(C,0);
    vector<int> stamp(C,0);int curStamp=0;
    vector<pair<int,vector<int>>> out;
    vector<int> rem(P);iota(rem.begin(),rem.end(),0);
    set<long long> events;
    long long t=0;bool timeout=false;
    vector<int> team;
    while(true){
        vector<pair<double,int>> keyed;
        for(int p:rem){
            long long eff=pr[p].Sc-max(0LL,t+pr[p].D-pr[p].B);
            if(eff<=0)continue;
            keyed.push_back({(double)eff/((double)pr[p].D*pr[p].R),p});
        }
        sort(keyed.begin(),keyed.end(),[](const pair<double,int>&a,const pair<double,int>&b){if(a.first!=b.first)return a.first>b.first;return a.second<b.second;});
        int freeCnt=0;for(int c=0;c<C;c++)if(avail[c]<=t)freeCnt++;
        vector<int> nrem;
        int cnt=0;
        for(size_t ki=0;ki<keyed.size();ki++){
            int p=keyed[ki].second;
            if(timeout){nrem.push_back(p);continue;}
            if((++cnt&15)==0&&elapsed()>8.5){timeout=true;nrem.push_back(p);continue;}
            Proj&q=pr[p];
            if(q.R>freeCnt){nrem.push_back(p);continue;}
            curStamp++;
            vector<int> asg(q.R,-1);
            team.clear();
            auto hasMentor=[&](int s,int L){for(int c:team)if(getL(c,s)>=L)return true;return false;};
            bool ok=true;
            for(int pass=0;pass<2&&ok;pass++){
                for(int ri:q.order){
                    if(asg[ri]!=-1)continue;
                    int s=q.roles[ri].first,L=q.roles[ri].second;
                    bool ment=hasMentor(s,L);
                    int best=-1,bc=INT_MAX;
                    for(int c:holders[s]){
                        if(avail[c]>t||stamp[c]==curStamp)continue;
                        int l=getL(c,s);
                        int cost;
                        if(l>=L)cost=(l-L)*1000+nskills[c];
                        else if(l==L-1&&ment)cost=-100000+nskills[c];
                        else continue;
                        if(cost<bc){bc=cost;best=c;}
                    }
                    if(best==-1&&pass==1&&L==1&&ment){
                        int bn=INT_MAX;
                        for(int c=0;c<C;c++){
                            if(avail[c]>t||stamp[c]==curStamp)continue;
                            if(getL(c,s)!=0)continue;
                            if(nskills[c]<bn){bn=nskills[c];best=c;}
                        }
                    }
                    if(best!=-1){asg[ri]=best;stamp[best]=curStamp;team.push_back(best);}
                }
            }
            for(int ri=0;ri<q.R;ri++)if(asg[ri]==-1){ok=false;break;}
            if(!ok){nrem.push_back(p);continue;}
            for(int ri=0;ri<q.R;ri++){
                int c=asg[ri];int s=q.roles[ri].first,L=q.roles[ri].second;
                avail[c]=t+q.D;
                int l=getL(c,s);
                if(l<=L){
                    if(l==0){holders[s].push_back(c);nskills[c]++;}
                    setL(c,s,l+1);
                }
            }
            freeCnt-=q.R;
            events.insert(t+q.D);
            out.push_back({p,asg});
        }
        rem=nrem;
        if(rem.empty()||timeout)break;
        auto it=events.upper_bound(t);
        if(it==events.end())break;
        t=*it;
        events.erase(events.begin(),it);
    }
    string o;
    o+=to_string(out.size());o+='\n';
    for(auto&e:out){
        o+=pr[e.first].name;o+='\n';
        for(size_t i=0;i<e.second.size();i++){if(i)o+=' ';o+=cname[e.second[i]];}
        o+='\n';
    }
    fwrite(o.data(),1,o.size(),stdout);
    return 0;
}
