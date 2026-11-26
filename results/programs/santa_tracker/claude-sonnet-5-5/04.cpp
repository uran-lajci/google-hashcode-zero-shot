#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int main(){
    ll T,D;int W,G;
    cin>>T>>D>>W>>G;
    vector<ll> l(W),a(W);
    for(int i=0;i<W;i++)cin>>l[i]>>a[i];
    vector<string> nm(G);vector<ll> s(G),w(G),c(G),r(G);
    for(int i=0;i<G;i++)cin>>nm[i]>>s[i]>>w[i]>>c[i]>>r[i];
    auto maxAcc=[&](ll wt)->ll{
        for(int i=0;i<W;i++) if(wt<=l[i]) return a[i];
        return 0;
    };
    vector<string> out;
    vector<int> rest;
    for(int i=0;i<G;i++){
        // c,r up to 1e9: squares up to 2e18 fits in ll (sum 2e18 < 9.2e18)
        if(c[i]*c[i]+r[i]*r[i]<=D*D){
            out.push_back("LoadGift "+nm[i]);
            out.push_back("DeliverGift "+nm[i]);
        } else rest.push_back(i);
    }
    sort(rest.begin(),rest.end(),[&](int x,int y){
        if(s[x]!=s[y])return s[x]>s[y];
        return x<y;
    });
    for(int gi:rest){
        ll A=maxAcc(w[gi]+2);
        if(A<=0)continue;
        ll cc=c[gi],rr=r[gi];
        ll t0=max((llabs(cc)+A-1)/A,(llabs(rr)+A-1)/A+1);
        if(t0<2)t0=2;
        bool found=false;
        for(ll T1=t0;T1<=T&&!found;T1++){
            ll s1=T1-1;
            ll bx=llround((double)cc/T1), by=llround((double)rr/s1);
            for(ll dx=-1;dx<=1&&!found;dx++)for(ll dy=-1;dy<=1&&!found;dy++){
                ll vx=bx+dx,vy=by+dy;
                if(llabs(vx)>A||llabs(vy)>A)continue;
                ll px=vx*T1-cc,py=vy*s1-rr;
                if(px*px+py*py<=D*D){
                    found=true;
                    out.push_back("LoadCarrots 2");
                    out.push_back("LoadGift "+nm[gi]);
                    if(vx>0)out.push_back("AccRight "+to_string(vx));
                    else if(vx<0)out.push_back("AccLeft "+to_string(-vx));
                    out.push_back("Float 1");
                    if(vy>0)out.push_back("AccUp "+to_string(vy));
                    else if(vy<0)out.push_back("AccDown "+to_string(-vy));
                    out.push_back("Float "+to_string(s1));
                    out.push_back("DeliverGift "+nm[gi]);
                }
            }
        }
        if(found)break;
    }
    printf("%d\n",(int)out.size());
    for(auto&x:out)printf("%s\n",x.c_str());
}
