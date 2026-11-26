#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    auto T0=chrono::steady_clock::now();
    auto elapsed=[&](){return chrono::duration<double>(chrono::steady_clock::now()-T0).count();};
    int C,P; if(!(cin>>C>>P)) return 0;
    unordered_map<string,int> sid;
    auto getS=[&](const string&s){auto it=sid.find(s);if(it!=sid.end())return it->second;int id=sid.size();sid[s]=id;return id;};
    vector<string> cname(C);
    vector<unordered_map<int,int>> lv(C);
    vector<vector<int>> skillList;
    auto ensure=[&](int s){ if((int)skillList.size()<=s) skillList.resize(s+1); };
    for(int i=0;i<C;i++){
        int n;cin>>cname[i]>>n;
        for(int j=0;j<n;j++){string s;int l;cin>>s>>l;int id=getS(s);ensure(id);
            if(l>0){ if(!lv[i].count(id)) skillList[id].push_back(i); lv[i][id]=max(lv[i][id],l);} }
    }
    vector<string> pname(P);
    vector<long long> D(P),S(P),B(P);
    vector<vector<pair<int,int>>> roles(P), lmax(P);
    for(int i=0;i<P;i++){
        int r;cin>>pname[i]>>D[i]>>S[i]>>B[i]>>r;
        roles[i].resize(r);
        map<int,int> mx;
        for(int j=0;j<r;j++){string s;int l;cin>>s>>l;int id=getS(s);ensure(id);roles[i][j]={id,l};
            mx[id]=max(mx[id],l);}
        for(auto&p:mx) lmax[i].push_back(p);
    }
    int NS=sid.size(); ensure(max(NS-1,0));
    auto getL=[&](int c,int s)->int{auto it=lv[c].find(s);return it==lv[c].end()?0:it->second;};
    vector<long long> busy(C,0);
    vector<vector<int>> asg(P);
    priority_queue<pair<long long,int>,vector<pair<long long,int>>,greater<>> ev;
    vector<int> order;
    vector<int> rem(P); iota(rem.begin(),rem.end(),0);
    vector<int> maxFree(NS+1,0);
    vector<int> usedMark(C,-1); int stamp=0;
    long long t=0;
    bool timeout=false;
    auto scoreAt=[&](int p,long long tt)->long long{long long late=max(0LL,tt+D[p]-B[p]);return max(0LL,S[p]-late);};
    while(true){
        while(!ev.empty()&&ev.top().first<=t){
            int p=ev.top().second;ev.pop();
            for(size_t r=0;r<roles[p].size();r++){
                int c=asg[p][r],s=roles[p][r].first,L=roles[p][r].second;
                int cur=getL(c,s);
                if(cur<=L){ if(cur==0) skillList[s].push_back(c); lv[c][s]=cur+1; }
            }
        }
        vector<int> freeL;
        for(int c=0;c<C;c++) if(busy[c]<=t) freeL.push_back(c);
        fill(maxFree.begin(),maxFree.end(),0);
        for(int c:freeL) for(auto&kv:lv[c]) maxFree[kv.first]=max(maxFree[kv.first],kv.second);
        int freeCount=freeL.size();
        // filter
        vector<int> nr; nr.reserve(rem.size());
        for(int p:rem) if(scoreAt(p,t)>0) nr.push_back(p);
        rem.swap(nr);
        vector<pair<double,int>> cand;
        for(int p:rem) cand.push_back({-(double)scoreAt(p,t)/((double)D[p]*roles[p].size()),p});
        sort(cand.begin(),cand.end());
        vector<char> done(P,0); bool anyDone=false;
        for(auto&cp:cand){
            if(elapsed()>8.5){timeout=true;break;}
            int p=cp.second;
            int R=roles[p].size();
            if(R>freeCount) continue;
            bool ok=true;
            for(auto&q:lmax[p]) if(maxFree[q.first]<q.second){ok=false;break;}
            if(!ok) continue;
            stamp++;
            vector<int> as(R,-1);
            vector<int> idx(R); iota(idx.begin(),idx.end(),0);
            sort(idx.begin(),idx.end(),[&](int a,int b){return roles[p][a].second>roles[p][b].second;});
            vector<int> members;
            int filled=0;
            bool progress=true;
            while(progress&&filled<R){
                progress=false;
                for(int r:idx){
                    if(as[r]>=0) continue;
                    int s=roles[p][r].first,L=roles[p][r].second;
                    bool mentor=false;
                    for(int m:members) if(getL(m,s)>=L){mentor=true;break;}
                    int best=-1,bestL=INT_MAX;
                    for(int c:skillList[s]){
                        if(busy[c]>t||usedMark[c]==stamp) continue;
                        int l=getL(c,s);
                        if(mentor&&l==L-1){best=c;bestL=l;break;}
                        if(l>=L&&l<bestL){best=c;bestL=l;if(l==L&&!mentor)break;}
                    }
                    if(best<0&&mentor&&L==1){
                        for(int c:freeL){ if(busy[c]>t||usedMark[c]==stamp) continue; if(getL(c,s)==0){best=c;break;} }
                    }
                    if(best>=0){as[r]=best;usedMark[best]=stamp;members.push_back(best);filled++;progress=true;}
                }
            }
            if(filled<R) continue;
            asg[p]=as;
            for(int c:as) busy[c]=t+D[p];
            freeCount-=R;
            ev.push({t+D[p],p});
            order.push_back(p);
            done[p]=1; anyDone=true;
        }
        if(anyDone){ vector<int> nr2; for(int p:rem) if(!done[p]) nr2.push_back(p); rem.swap(nr2);}
        if(timeout) break;
        if(ev.empty()) break;
        t=ev.top().first;
    }
    string out;
    out+=to_string(order.size())+"\n";
    for(int p:order){
        out+=pname[p]+"\n";
        for(size_t r=0;r<asg[p].size();r++){ if(r) out+=' '; out+=cname[asg[p][r]]; }
        out+="\n";
    }
    fwrite(out.data(),1,out.size(),stdout);
}
