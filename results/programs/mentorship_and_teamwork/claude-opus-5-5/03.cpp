#include <bits/stdc++.h>
using namespace std;

int C,P;
vector<string> cname;
vector<vector<pair<int,int>>> cskills0;
int NS=0;
unordered_map<string,int> skillId;
struct Proj{string name;int D,S,B;vector<pair<int,int>> roles;vector<int> ord;};
vector<Proj> pr;

int getSkill(const string&s){auto it=skillId.find(s);if(it!=skillId.end())return it->second;skillId[s]=NS;return NS++;}

struct Result{long long score=0;vector<pair<int,vector<int>>> plan;};

Result run(const vector<int>& order,long long budget){
    Result res;
    long long ops=0;
    // per contributor: vector of (skill, posInList)
    vector<vector<pair<int,int>>> cs(C);
    vector<vector<int>> sl(NS), slv(NS);
    for(int c=0;c<C;c++){
        for(auto&pr2:cskills0[c]){
            int s=pr2.first;
            cs[c].push_back({s,(int)sl[s].size()});
            sl[s].push_back(c);slv[s].push_back(pr2.second);
        }
    }
    auto getLvl=[&](int c,int s)->int{
        for(auto&e:cs[c]) if(e.first==s) return slv[s][e.second];
        return 0;
    };
    auto incLvl=[&](int c,int s){
        for(auto&e:cs[c]) if(e.first==s){slv[s][e.second]++;return;}
        cs[c].push_back({s,(int)sl[s].size()});
        sl[s].push_back(c);slv[s].push_back(1);
    };
    vector<char> busy(C,0);
    vector<int> used(C,0);int stamp=0;
    int freeCount=C;
    vector<int> rem=order;
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> ev;
    vector<vector<int>> asg(P);
    long long t=0;
    bool stop=false;
    while(true){
        if(!stop){
            vector<int> nrem;nrem.reserve(rem.size());
            for(size_t idx=0;idx<rem.size();idx++){
                int p=rem[idx];
                Proj&q=pr[p];
                long long sc=q.S-max(0LL,t+q.D-q.B);
                if(sc<=0) continue;
                if(stop||(int)q.roles.size()>freeCount){nrem.push_back(p);continue;}
                stamp++;
                int R=q.roles.size();
                vector<int> a(R,-1);
                bool fail=false;
                for(int r:q.ord){
                    int s=q.roles[r].first,L=q.roles[r].second;
                    int best=-1,bl=INT_MAX;
                    auto&lst=sl[s];auto&lv=slv[s];
                    for(size_t i=0;i<lst.size();i++){
                        ops++;
                        int c=lst[i];
                        if(busy[c]||used[c]==stamp)continue;
                        int l=lv[i];
                        if(l>=L&&l<bl){bl=l;best=c;if(bl==L)break;}
                    }
                    if(best>=0){a[r]=best;used[best]=stamp;}
                }
                bool progress=true;
                while(progress){
                    progress=false;
                    bool anyPending=false;
                    for(int r:q.ord){
                        if(a[r]>=0)continue;
                        int s=q.roles[r].first,L=q.roles[r].second;
                        bool mentor=false;
                        for(int k=0;k<R&&!mentor;k++) if(a[k]>=0&&getLvl(a[k],s)>=L) mentor=true;
                        if(!mentor){anyPending=true;continue;}
                        int best=-1;
                        if(L-1>=1){
                            auto&lst=sl[s];auto&lv=slv[s];
                            int bl=INT_MAX;
                            for(size_t i=0;i<lst.size();i++){
                                ops++;
                                int c=lst[i];
                                if(busy[c]||used[c]==stamp)continue;
                                int l=lv[i];
                                if(l>=L-1&&l<bl){bl=l;best=c;if(bl==L-1)break;}
                            }
                        }else{
                            for(int c=0;c<C;c++){
                                ops++;
                                if(busy[c]||used[c]==stamp)continue;
                                best=c;break; // any level >=0 works
                            }
                        }
                        if(best>=0){a[r]=best;used[best]=stamp;progress=true;}
                        else anyPending=true;
                    }
                    if(!anyPending)break;
                }
                for(int r=0;r<R;r++) if(a[r]<0){fail=true;break;}
                if(fail){nrem.push_back(p);}
                else{
                    for(int c:a){busy[c]=1;}
                    freeCount-=R;
                    ev.push({t+q.D,p});
                    asg[p]=a;
                    res.plan.push_back({p,a});
                    res.score+=sc;
                }
                if(ops>budget)stop=true;
            }
            rem.swap(nrem);
        }
        if(ev.empty())break;
        long long nt=ev.top().first;
        while(!ev.empty()&&ev.top().first==nt){
            int p=ev.top().second;ev.pop();
            Proj&q=pr[p];
            for(size_t r=0;r<q.roles.size();r++){
                int c=asg[p][r];int s=q.roles[r].first,L=q.roles[r].second;
                if(getLvl(c,s)<=L) incLvl(c,s);
                busy[c]=0;freeCount++;
            }
        }
        t=nt;
        if(stop&&ev.empty())break;
    }
    return res;
}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>P;
    cname.resize(C);cskills0.resize(C);
    for(int i=0;i<C;i++){
        int n;cin>>cname[i]>>n;
        for(int j=0;j<n;j++){string s;int l;cin>>s>>l;int id=getSkill(s);
            bool f=false;for(auto&e:cskills0[i]) if(e.first==id){e.second=max(e.second,l);f=true;}
            if(!f)cskills0[i].push_back({id,l});}
    }
    pr.resize(P);
    for(int i=0;i<P;i++){
        int R;cin>>pr[i].name>>pr[i].D>>pr[i].S>>pr[i].B>>R;
        pr[i].roles.resize(R);
        for(int j=0;j<R;j++){string s;int l;cin>>s>>l;pr[i].roles[j]={getSkill(s),l};}
        pr[i].ord.resize(R);iota(pr[i].ord.begin(),pr[i].ord.end(),0);
        auto&rl=pr[i].roles;
        stable_sort(pr[i].ord.begin(),pr[i].ord.end(),[&](int x,int y){return rl[x].second>rl[y].second;});
    }
    vector<vector<int>> orders;
    vector<int> base(P);iota(base.begin(),base.end(),0);
    {
        vector<int> o=base;
        stable_sort(o.begin(),o.end(),[&](int x,int y){
            if(pr[x].B!=pr[y].B)return pr[x].B<pr[y].B;return pr[x].S>pr[y].S;});
        orders.push_back(o);
    }
    {
        vector<int> o=base;
        stable_sort(o.begin(),o.end(),[&](int x,int y){
            double a=(double)pr[x].S/((double)pr[x].D*pr[x].roles.size());
            double b=(double)pr[y].S/((double)pr[y].D*pr[y].roles.size());
            return a>b;});
        orders.push_back(o);
    }
    {
        vector<int> o=base;
        stable_sort(o.begin(),o.end(),[&](int x,int y){
            long long a=(long long)pr[x].B-pr[x].D, b=(long long)pr[y].B-pr[y].D;
            if(a!=b)return a<b;return pr[x].S>pr[y].S;});
        orders.push_back(o);
    }
    Result best;bool has=false;
    for(auto&o:orders){
        Result r=run(o,500000000LL);
        if(!has||r.score>best.score){best=move(r);has=true;}
    }
    string out;
    out+=to_string(best.plan.size())+"\n";
    for(auto&e:best.plan){
        out+=pr[e.first].name+"\n";
        for(size_t i=0;i<e.second.size();i++){
            if(i)out+=' ';
            out+=cname[e.second[i]];
        }
        out+="\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
}
