#include <bits/stdc++.h>
using namespace std;

int L,G,S,B,F,N;
vector<string> fname;
vector<int> fD; vector<long long> fU;
vector<vector<int>> fbins;
vector<int> R;

long long simulate(const vector<int>& order, vector<vector<pair<int,int>>>& asg){
    asg.assign(G, {});
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    for(int i=0;i<G;i++) pq.push({0,i});
    vector<priority_queue<int,vector<int>,greater<int>>> bh(B);
    vector<int> fend(F,0); vector<char> aband(F,0), complete(F,0);
    size_t fi=0; size_t ti=0;
    while(!pq.empty()){
        auto [t,e]=pq.top(); pq.pop();
        if(t>=L) continue;
        int tf=-1,tb=-1;
        while(fi<order.size()){
            int f=order[fi];
            if(aband[f]){fi++;ti=0;continue;}
            if(ti>=fbins[f].size()){complete[f]=1;fi++;ti=0;continue;}
            if(ti==0){
                int mx=fD[f]+R[fbins[f][0]];
                if(t+mx>=L){aband[f]=1;fi++;ti=0;continue;}
            }
            tf=f; tb=fbins[f][ti]; ti++;
            break;
        }
        if(tf<0) continue;
        auto &h=bh[tb];
        while(!h.empty() && h.top()<=t) h.pop();
        int C=h.size();
        int dur=fD[tf]+R[tb]+C;
        int en=t+dur;
        if(en>=L){
            aband[tf]=1; fi++; ti=0;
            pq.push({t,e});
            continue;
        }
        h.push(en);
        asg[e].push_back({tf,tb});
        fend[tf]=max(fend[tf],en);
        pq.push({en,e});
    }
    // mark completion for current feature if all tasks assigned
    if(fi<order.size()){
        int f=order[fi];
        if(!aband[f] && ti>=fbins[f].size()) complete[f]=1;
    }
    long long sc=0;
    for(int f=0;f<F;f++) if(complete[f]&&!aband[f]&&fend[f]<L) sc+=fU[f]*(long long)(L-fend[f]);
    return sc;
}

int main(){
    auto st=chrono::steady_clock::now();
    ios::sync_with_stdio(false);cin.tie(nullptr);
    cin>>L>>G>>S>>B>>F>>N;
    unordered_map<string,int> sb; sb.reserve(S*2+10);
    R.assign(B,0);
    for(int i=0;i<S;i++){string n;int b;cin>>n>>b;sb[n]=b;R[b]++;}
    fname.resize(F);fD.resize(F);fU.resize(F);fbins.resize(F);
    for(int i=0;i<F;i++){
        int m;cin>>fname[i]>>m>>fD[i]>>fU[i];
        vector<int> bs;
        for(int j=0;j<m;j++){string s;cin>>s;bs.push_back(sb[s]);}
        sort(bs.begin(),bs.end());bs.erase(unique(bs.begin(),bs.end()),bs.end());
        sort(bs.begin(),bs.end(),[&](int a,int b){if(R[a]!=R[b])return R[a]>R[b];return a<b;});
        fbins[i]=bs;
    }
    vector<double> cost(F);
    for(int f=0;f<F;f++){double c=0;for(int b:fbins[f])c+=fD[f]+R[b];cost[f]=c;}
    vector<double> exps={1.0,0.5,0.75,1.25,1.5,2.0,0.25,3.0};
    long long best=-1; vector<vector<pair<int,int>>> bestAsg, asg;
    for(double a:exps){
        double el=chrono::duration<double>(chrono::steady_clock::now()-st).count();
        if(el>7.0 && best>=0) break;
        vector<int> order(F); iota(order.begin(),order.end(),0);
        vector<double> key(F);
        for(int f=0;f<F;f++) key[f]=(double)fU[f]/pow(cost[f],a);
        stable_sort(order.begin(),order.end(),[&](int x,int y){return key[x]>key[y];});
        long long sc=simulate(order,asg);
        if(sc>best){best=sc;bestAsg.swap(asg);}
    }
    string out;
    int E=0;
    for(int i=0;i<G;i++) if(!bestAsg[i].empty()) E++;
    out+=to_string(E)+"\n";
    for(int i=0;i<G;i++){
        if(bestAsg[i].empty()) continue;
        out+=to_string(bestAsg[i].size())+"\n";
        for(auto&p:bestAsg[i]){
            out+="impl "; out+=fname[p.first]; out+=" "; out+=to_string(p.second); out+="\n";
        }
    }
    fwrite(out.data(),1,out.size(),stdout);
}
