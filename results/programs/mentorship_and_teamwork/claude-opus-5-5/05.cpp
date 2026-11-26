#include <bits/stdc++.h>
using namespace std;

int C,P,NS=0;
vector<string> cname,pname;
vector<vector<pair<int,int>>> initLv;
vector<int> PD,PS,PB;
vector<vector<pair<int,int>>> roles; // skill, level
vector<vector<int>> roleOrder;
unordered_map<string,int> skillId;
chrono::steady_clock::time_point T0;
double elapsed(){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();}

struct Result{ long long score=-1; vector<pair<int,vector<int>>> out; };

Result runOrder(const vector<int>& order,double deadline){
    Result res; res.score=0;
    vector<vector<pair<int,int>>> lv=initLv;
    vector<vector<vector<int>>> bucket(NS);
    for(int c=0;c<C;c++)for(auto&pr:lv[c]){
        auto&b=bucket[pr.first]; if((int)b.size()<=pr.second)b.resize(pr.second+1); b[pr.second].push_back(c);
    }
    auto getL=[&](int c,int s)->int{for(auto&pr:lv[c])if(pr.first==s)return pr.second;return 0;};
    auto levelUp=[&](int c,int s){
        int old=0;bool f=false;
        for(auto&pr:lv[c])if(pr.first==s){old=pr.second;pr.second++;f=true;break;}
        if(!f)lv[c].push_back({s,1});
        auto&b=bucket[s];
        if(old>=1){auto&v=b[old];for(size_t i=0;i<v.size();i++)if(v[i]==c){v[i]=v.back();v.pop_back();break;}}
        if((int)b.size()<=old+1)b.resize(old+2);
        b[old+1].push_back(c);
    };
    vector<long long> busy(C,0);
    vector<int> stamp(C,0); int st=0;
    vector<vector<pair<int,int>>> learn; // per started project
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> pq;
    vector<int> rem=order;
    long long t=0; int availCount=C;
    long long cnt=0; bool stop=false;
    vector<int> asg; vector<pair<int,int>> lr;
    while(!stop){
        while(!pq.empty()&&pq.top().first<=t){
            int id=pq.top().second;pq.pop();
            for(auto&pr:learn[id])levelUp(pr.first,pr.second);
            availCount+=res.out[id].second.size();
        }
        if(availCount>0){
            vector<int> nrem; nrem.reserve(rem.size());
            for(size_t qi=0;qi<rem.size();qi++){
                int p=rem[qi];
                if(stop){nrem.push_back(p);continue;}
                if(((++cnt)&63)==0&&elapsed()>deadline){stop=true;nrem.push_back(p);continue;}
                long long late=t+PD[p]-PB[p]; if(late<0)late=0;
                long long gain=PS[p]-late;
                if(gain<=0)continue;
                int R=roles[p].size();
                if(R>availCount){nrem.push_back(p);continue;}
                st++;
                asg.assign(R,-1);
                for(int ri:roleOrder[p]){
                    int s=roles[p][ri].first,L=roles[p][ri].second;
                    auto&b=bucket[s]; int found=-1;
                    for(int l=L;l<(int)b.size()&&found<0;l++)for(int c:b[l])if(busy[c]<=t&&stamp[c]!=st){found=c;break;}
                    if(found>=0){asg[ri]=found;stamp[found]=st;}
                }
                bool prog=true;
                while(prog){
                    prog=false;
                    for(int ri:roleOrder[p]){
                        if(asg[ri]>=0)continue;
                        int s=roles[p][ri].first,L=roles[p][ri].second;
                        bool mentor=false;
                        for(int k=0;k<R;k++)if(asg[k]>=0&&getL(asg[k],s)>=L){mentor=true;break;}
                        if(!mentor)continue;
                        int found=-1;
                        if(L-1>=1){
                            auto&b=bucket[s];
                            if(L-1<(int)b.size())for(int c:b[L-1])if(busy[c]<=t&&stamp[c]!=st){found=c;break;}
                        }else{
                            for(int c=0;c<C;c++)if(busy[c]<=t&&stamp[c]!=st&&getL(c,s)==0){found=c;break;}
                        }
                        if(found>=0){asg[ri]=found;stamp[found]=st;prog=true;}
                    }
                }
                bool ok=true;for(int k=0;k<R;k++)if(asg[k]<0){ok=false;break;}
                if(!ok){nrem.push_back(p);continue;}
                lr.clear();
                for(int k=0;k<R;k++){
                    int c=asg[k],s=roles[p][k].first,L=roles[p][k].second;
                    if(L>=getL(c,s))lr.push_back({c,s});
                    busy[c]=t+PD[p];
                }
                availCount-=R;
                res.score+=gain;
                int id=res.out.size();
                res.out.push_back({p,asg});
                learn.push_back(lr);
                pq.push({t+PD[p],id});
            }
            rem.swap(nrem);
        }
        if(pq.empty()||rem.empty())break;
        t=pq.top().first;
    }
    return res;
}

int main(){
    T0=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>C>>P;
    cname.resize(C);initLv.resize(C);
    auto sid=[&](const string&s){auto it=skillId.find(s);if(it!=skillId.end())return it->second;skillId[s]=NS;return NS++;};
    for(int i=0;i<C;i++){
        int n;cin>>cname[i]>>n;
        for(int j=0;j<n;j++){string s;int l;cin>>s>>l;int id=sid(s);
            bool f=false;for(auto&pr:initLv[i])if(pr.first==id){pr.second=max(pr.second,l);f=true;}
            if(!f&&l>0)initLv[i].push_back({id,l});}
    }
    pname.resize(P);PD.resize(P);PS.resize(P);PB.resize(P);roles.resize(P);roleOrder.resize(P);
    for(int i=0;i<P;i++){
        int r;cin>>pname[i]>>PD[i]>>PS[i]>>PB[i]>>r;
        roles[i].resize(r);
        for(int k=0;k<r;k++){string s;int l;cin>>s>>l;roles[i][k]={sid(s),l};}
        roleOrder[i].resize(r);iota(roleOrder[i].begin(),roleOrder[i].end(),0);
        stable_sort(roleOrder[i].begin(),roleOrder[i].end(),[&](int a,int b){return roles[i][a].second>roles[i][b].second;});
    }
    vector<vector<double>> keys;
    {
        vector<double> k(P);
        for(int i=0;i<P;i++)k[i]=-(double)PS[i]/((double)PD[i]*roles[i].size());keys.push_back(k);
        for(int i=0;i<P;i++)k[i]=PB[i];keys.push_back(k);
        for(int i=0;i<P;i++)k[i]=-(double)PS[i]/PD[i];keys.push_back(k);
        for(int i=0;i<P;i++)k[i]=(double)PB[i]-PD[i];keys.push_back(k);
        for(int i=0;i<P;i++)k[i]=PD[i];keys.push_back(k);
        for(int i=0;i<P;i++)k[i]=-(double)PS[i]/roles[i].size();keys.push_back(k);
    }
    Result best;
    double total=9.0;
    int nr=keys.size();
    for(int r=0;r<nr;r++){
        double now=elapsed();
        if(now>total-0.05)break;
        double budget=(total-now)/(nr-r);
        vector<int> ord(P);iota(ord.begin(),ord.end(),0);
        auto&k=keys[r];
        stable_sort(ord.begin(),ord.end(),[&](int a,int b){return k[a]<k[b];});
        Result res=runOrder(ord,now+budget);
        if(res.score>best.score)best=move(res);
    }
    if(best.score<0)best.score=0;
    string o;
    o+=to_string(best.out.size())+"\n";
    for(auto&pr:best.out){
        o+=pname[pr.first];o+="\n";
        for(size_t k=0;k<pr.second.size();k++){if(k)o+=" ";o+=cname[pr.second[k]];}
        o+="\n";
    }
    fwrite(o.data(),1,o.size(),stdout);
}
