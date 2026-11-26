#include <bits/stdc++.h>
using namespace std;

int C,P;
vector<string> cname, pname;
vector<vector<pair<int,int>>> cs; // per contributor (skill, level)
vector<vector<int>> holders;
vector<int> PD,PS,PB;
vector<vector<pair<int,int>>> roles;
vector<long long> avail;
unordered_map<string,int> skillId;

int getSkill(const string&s){
    auto it=skillId.find(s);
    if(it!=skillId.end())return it->second;
    int id=skillId.size(); skillId[s]=id; holders.push_back({}); return id;
}
inline int lvl(int c,int s){
    for(auto&pr:cs[c]) if(pr.first==s) return pr.second;
    return 0;
}
void inc(int c,int s){
    for(auto&pr:cs[c]) if(pr.first==s){pr.second++;return;}
    cs[c].push_back({s,1}); holders[s].push_back(c);
}

vector<int> usedStamp; int stampV=0;
vector<int> freeList;

bool tryAssign(int p,long long t,vector<int>&asg){
    int R=roles[p].size();
    asg.assign(R,-1);
    stampV++;
    vector<int> ord(R);
    iota(ord.begin(),ord.end(),0);
    sort(ord.begin(),ord.end(),[&](int a,int b){return roles[p][a].second>roles[p][b].second;});
    vector<int> members;
    auto mentorAvail=[&](int s,int L)->bool{
        for(int m:members) if(lvl(m,s)>=L) return true;
        return false;
    };
    auto fill=[&](int r)->bool{
        int s=roles[p][r].first,L=roles[p][r].second;
        bool men=mentorAvail(s,L);
        int best=-1; double bc=1e18;
        for(int c:holders[s]){
            if(avail[c]>t||usedStamp[c]==stampV)continue;
            int l=lvl(c,s);
            double cost;
            if(l>=L) cost=l-L;
            else if(l==L-1&&men) cost=-0.5;
            else continue;
            if(cost<bc){bc=cost;best=c; if(cost<0)break;}
        }
        if(L==1&&men&&(best<0||bc>0.5)){
            for(int c:freeList){
                if(usedStamp[c]==stampV)continue;
                if(lvl(c,s)==0){best=c;break;}
            }
        }
        if(best<0)return false;
        asg[r]=best; usedStamp[best]=stampV; members.push_back(best);
        return true;
    };
    for(int r:ord) fill(r);
    bool prog=true;
    while(prog){
        prog=false; bool all=true;
        for(int r:ord) if(asg[r]<0){
            if(fill(r))prog=true; else all=false;
        }
        if(all)break;
    }
    for(int r=0;r<R;r++) if(asg[r]<0) return false;
    return true;
}

int main(){
    auto T0=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>P;
    cname.resize(C);cs.resize(C);
    for(int i=0;i<C;i++){
        int n;cin>>cname[i]>>n;
        for(int j=0;j<n;j++){
            string s;int l;cin>>s>>l;
            int id=getSkill(s);
            if(l>0){cs[i].push_back({id,l});holders[id].push_back(i);}
        }
    }
    pname.resize(P);PD.resize(P);PS.resize(P);PB.resize(P);roles.resize(P);
    for(int i=0;i<P;i++){
        int r;cin>>pname[i]>>PD[i]>>PS[i]>>PB[i]>>r;
        roles[i].resize(r);
        for(int j=0;j<r;j++){string s;int l;cin>>s>>l;roles[i][j]={getSkill(s),l};}
    }
    avail.assign(C,0);usedStamp.assign(C,0);
    vector<int> rem(P);iota(rem.begin(),rem.end(),0);
    vector<pair<int,vector<int>>> out;
    long long t=0;
    vector<int> asg;
    while(true){
        double el=chrono::duration<double>(chrono::steady_clock::now()-T0).count();
        if(el>9.0)break;
        // filter and sort
        vector<int> nr; vector<pair<double,int>> keyed;
        for(int p:rem){
            long long late=max(0LL,t+PD[p]-PB[p]);
            long long eff=PS[p]-late;
            if(eff<=0)continue;
            nr.push_back(p);
            double k=(double)eff/((double)PD[p]*roles[p].size());
            keyed.push_back({k,p});
        }
        rem=nr;
        if(rem.empty())break;
        sort(keyed.begin(),keyed.end(),[&](const pair<double,int>&a,const pair<double,int>&b){
            if(a.first!=b.first)return a.first>b.first;
            if(PB[a.second]!=PB[b.second])return PB[a.second]<PB[b.second];
            return a.second<b.second;});
        freeList.clear();
        for(int c=0;c<C;c++) if(avail[c]<=t) freeList.push_back(c);
        int freeCnt=freeList.size();
        vector<char> done(P,0);
        bool any=false;
        for(auto&kp:keyed){
            if(freeCnt==0)break;
            int p=kp.second;
            if((int)roles[p].size()>freeCnt)continue;
            if(((C>2000)||(P>20000)) && (chrono::duration<double>(chrono::steady_clock::now()-T0).count()>9.3))break;
            if(tryAssign(p,t,asg)){
                // level up based on pre-project levels
                int R=roles[p].size();
                vector<char> up(R,0);
                for(int r=0;r<R;r++) if(lvl(asg[r],roles[p][r].first)<=roles[p][r].second) up[r]=1;
                for(int r=0;r<R;r++){
                    if(up[r]) inc(asg[r],roles[p][r].first);
                    avail[asg[r]]=t+PD[p];
                }
                freeCnt-=R;
                out.push_back({p,asg});
                done[p]=1; any=true;
                if(freeCnt>0){
                    vector<int> nf; for(int c:freeList) if(avail[c]<=t) nf.push_back(c);
                    freeList.swap(nf);
                }
            }
        }
        if(any){
            vector<int> nr2; for(int p:rem) if(!done[p]) nr2.push_back(p); rem.swap(nr2);
        }
        long long nt=LLONG_MAX;
        for(int c=0;c<C;c++) if(avail[c]>t) nt=min(nt,avail[c]);
        if(nt==LLONG_MAX)break;
        t=nt;
    }
    string o;
    o+=to_string(out.size())+"\n";
    for(auto&pr:out){
        o+=pname[pr.first]+"\n";
        for(size_t i=0;i<pr.second.size();i++){
            if(i)o+=' ';
            o+=cname[pr.second[i]];
        }
        o+="\n";
    }
    fwrite(o.data(),1,o.size(),stdout);
}
