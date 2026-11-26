#include <bits/stdc++.h>
using namespace std;
int main(){
    long long T,D,W,G;
    cin>>T>>D>>W>>G;
    for(int i=0;i<W;i++){long long l,a;cin>>l>>a;}
    vector<string> out;
    for(int i=0;i<G;i++){
        string n;long long s,w,c,r;
        cin>>n>>s>>w>>c>>r;
        if(c*c+r*r<=D*D){
            out.push_back("LoadGift "+n);
            out.push_back("DeliverGift "+n);
        }
    }
    printf("%d\n",(int)out.size());
    for(auto&s:out)printf("%s\n",s.c_str());
}
