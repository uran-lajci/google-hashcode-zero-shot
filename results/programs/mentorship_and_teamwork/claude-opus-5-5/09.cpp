#include <bits/stdc++.h>
using namespace std;

struct Proj{string name;int D,S,B;vector<pair<int,int>> roles;vector<int> ord;};
int C,P,K;
vector<string> cname;
vector<vector<pair<int,int>>> cskills;
vector<Proj> pr;
bool dense;
size_t Ksz;

struct Lv{
    vector<uint8_t> d; unordered_map<long long,uint8_t> m;
    inline int get(int c,int s){
        if(dense) return d[(size_t)c*Ksz+s];
        auto it=m.find((long long)c*Ksz+s); return it==m.end()?0:it->second;
    }
    inline void set(int c,int s,int v){
        if(dense) d[(size_t)c*Ksz+s]=(uint8_t)v; else m[(long long)c*Ksz+s]=(uint8_t)v;
    }
};

long long simulate(const vector<int>& order,long long budget,vector<pair<int,vector<int>>>& out){
    out.clear();
    Lv lv;
    if(dense) lv.d.assign((size_t)C*Ksz,0);
    vector<vector<int>> holders(K);
    for(int c=0;c<C;c++) for(auto&sk:cskills[c]){ if(lv.get(c,sk.first)==0) holders[sk.first].push_back(c); lv.set(c,sk.first,sk.second);}
    vector<int> freeAt(C,0);
    priority_queue<int,vector<int>,greater<int>> times;
    vector<int> live=order;
    vector<char> availF(C,0),used(C,0);
    vector<int> avail;
    long long ops=0,score=0;
    int t=0;
    vector<int> asg;
    while(true){
        avail.clear();
        for(int c=0;c<C;c++){ availF[c]=freeAt[c]<=t; if(availF[c]) avail.push_back(c);}
        ops+=C;
        int availCnt=avail.size();
        vector<int> nl; nl.reserve(live.size());
        for(size_t li=0;li<live.size();li++){
            int p=live[li];
            if(ops>budget){ nl.push_back(p); continue; }
            Proj&q=pr[p];
            ops++;
            long long late=max(0LL,(long long)t+q.D-q.B);
            if(q.S-late<=0) continue;
            int R=q.roles.size();
            if(availCnt<R){ nl.push_back(p); continue; }
            asg.assign(R,-1);
            int filled=0;
            bool progress=true;
            while(progress&&filled<R){
                progress=false;
                for(int r:q.ord){
                    if(asg[r]!=-1) continue;
                    int s=q.roles[r].first,L=q.roles[r].second;
                    bool ment=false;
                    for(int a=0;a<R;a++) if(asg[a]!=-1&&lv.get(asg[a],s)>=L){ment=true;break;}
                    ops+=R;
                    int lo=ment?L-1:L;
                    int best=-1,bl=INT_MAX;
                    if(lo<=0){
                        for(int c:avail){ ops++; if(!used[c]&&lv.get(c,s)==0){best=c;bl=0;break;} }
                    }
                    if(best==-1){
                        for(int c:holders[s]){
                            ops++;
                            if(!availF[c]||used[c]) continue;
                            int l=lv.get(c,s);
                            if(l>=lo&&l<bl){bl=l;best=c; if(l==lo)break;}
                        }
                    }
                    if(best!=-1){asg[r]=best;used[best]=1;filled++;progress=true;}
                }
            }
            if(filled<R){
                for(int a=0;a<R;a++) if(asg[a]!=-1) used[asg[a]]=0;
                nl.push_back(p); continue;
            }
            // commit
            for(int a=0;a<R;a++){
                int c=asg[a]; used[c]=0; availF[c]=0; freeAt[c]=t+q.D;
                int s=q.roles[a].first,L=q.roles[a].second;
                int l=lv.get(c,s);
                if(L>=l){ if(l==0) holders[s].push_back(c); lv.set(c,s,l+1);}
            }
            availCnt-=R;
            times.push(t+q.D);
            score+=q.S-late;
            out.push_back({p,asg});
        }
        live.swap(nl);
        if(live.empty()||ops>budget) break;
        while(!times.empty()&&times.top()<=t) times.pop();
        if(times.empty()) break;
        t=times.top();
    }
    return score;
}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>P;
    unordered_map<string,int> sid;
    auto getS=[&](const string&s){auto it=sid.find(s); if(it!=sid.end()) return it->second; int id=sid.size(); sid[s]=id; return id;};
    cname.resize(C);cskills.resize(C);
    for(int c=0;c<C;c++){
        int n;cin>>cname[c]>>n;
        for(int i=0;i<n;i++){string s;int l;cin>>s>>l;cskills[c].push_back({getS(s),l});}
    }
    pr.resize(P);
    for(int p=0;p<P;p++){
        int R;cin>>pr[p].name>>pr[p].D>>pr[p].S>>pr[p].B>>R;
        for(int i=0;i<R;i++){string s;int l;cin>>s>>l;pr[p].roles.push_back({getS(s),l});}
        auto&o=pr[p].ord; o.resize(R); iota(o.begin(),o.end(),0);
        auto&rl=pr[p].roles;
        stable_sort(o.begin(),o.end(),[&](int a,int b){return rl[a].second>rl[b].second;});
    }
    K=max<int>(1,sid.size()); Ksz=K;
    dense=(double)C*K<=1.5e8;

    vector<vector<int>> orders;
    vector<int> base(P); iota(base.begin(),base.end(),0);
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){ if(pr[a].B!=pr[b].B) return pr[a].B<pr[b].B; return pr[a].S>pr[b].S;}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){ double x=(double)pr[a].S/((double)pr[a].D*pr[a].roles.size()); double y=(double)pr[b].S/((double)pr[b].D*pr[b].roles.size()); return x>y;}); orders.push_back(o);}
    {auto o=base; stable_sort(o.begin(),o.end(),[&](int a,int b){ double x=(double)pr[a].S/pr[a].D; double y=(double)pr[b].S/pr[b].D; return x>y;}); orders.push_back(o);}

    long long bestScore=-1; vector<pair<int,vector<int>>> best,cur;
    long long budget=(long long)8e7;
    for(auto&o:orders){
        long long s=simulate(o,budget,cur);
        if(s>bestScore){bestScore=s;best=cur;}
    }
    string outS;
    outS+=to_string(best.size())+"\n";
    for(auto&e:best){
        outS+=pr[e.first].name+"\n";
        for(size_t i=0;i<e.second.size();i++){ if(i) outS+=' '; outS+=cname[e.second[i]]; }
        outS+="\n";
    }
    fwrite(outS.data(),1,outS.size(),stdout);
}
