#include<bits/stdc++.h>
#include<unistd.h>
#define k double
#define O int
#define Z return
#define AR auto
#define BL vector
#define AZ continue
#define BP struct
#define BY emplace_back
#define BH else
#define BT unsigned
#define AN bool
#define BK while
#define Bj static
#define CP pop_back
#define CD string
#define CE size_t
#define CF assign
#define Bk using
#define Bl clear
#pragma GCC optimize("O3,unroll-loops")
#define f =Bf;
#define N ;Y
#define n =AS
#define AC .size()
#define t }void
#define AD .begin()
#define BM BH Y i==
#define r {AR&
#define x ;BL<
#define U );}
#define BC )Z 0;
#define Y if(
#define d =0;
#define BN .end()
#define Ba BS::Br
#define BD (O C
#define AV ;AR&
#define AM O B
#define BF f E.
#define s ;k
#define Bm .Bh 0);
#define AO (k)
#define m ;O
#define AS max(
#define o ].F
#define CI max<T>(1,
#define AW .resize(
#define w =L[
#define q =-1
#define CY __builtin_ctzll(
#define BV )AZ s
#define BI min(
#define BE ;for(
#define AA ].K
#define Co ;g(D,C.F,C.L)r E w D];
#define BG .push_back(
#define Bb )AZ N
#define BJ .M&&
#define Bf io._()
#define CQ long long
#define BW pair<
#define AT ();
#define BX ;};BP
#define AG >=0
#define CJ .second
#define Bp io.F+=
#define CK .F*AE[y[
#define Bh fill(
#define CR .back AT
#define CZ =LLONG_MAX;
#define CS .empty()
#define AP [B]
#define AQ ;}}
#define Cp ]=D.top AT D.pop AT
#define AU array<
#define CL 1ULL<<
#define CM d for(
#define BR (Bd&
#define Cj .F>BI I,Do[
#define Cq (BS&C,BS&A,BL<O>&
#define Cr P.e*R[G.K o+P.v-P.e
#define AX ){O
#define Br size[
#define AY C.F
#define CT =v._(
#define CU BN,[&](
#define CV G.L-G.F
#define g(P,C,B)for(O P=C;P<B;++P)
#define i(P,D)for(O P:D)
#define p(P,D)for(AR&P:D)
#define v(_,C,B,A)_.C!=B){T A CT _.C,B)N A<0
#define y(_,C,B)uint64_t _=C.B;_;_&=_-1)
#define u(_,C,B)_ w C]AV B=u[_.K]N
#define l(_,C)_.C n _.C,
#define BZ uint64_t
Bk T=CQ;Bk namespace std;BP Db{O V=0 x O>J x T>M x BL<BW O,O>>>R x BT char>F;AU AU T,30>,30>e x T>L x BW T,O>>K x BT char>b;void A(AM,O I,O D){V=B/I;J AW D);R AW B);F.CF(B,1);L AW B);b.CF(B,0);C AT t B BD,O A,T I AX D=M AC;M BG I);R[C].BY(A,D);R[A].BY(C,D);t C(){p(B,e)B.Bh-2);t D(O A,AN B AX I=A*V N F[I]!=B){Bh F AD+I,F AD+I+V,B);C(U t E(AM,AN A){Y(M AP AG)!=A){M AP=~M AP;C(U}T _(AM,O C){T&A=e AP[C];Z A==-2?A=I(B,C):A;}T I BD,AM AX A=J[C],D=J[B]N!F[A]||!F[D])Z-1 N A==D BC Bh b AD,b BN,0)m E d i(H,J)Y!b[H]){b[H]=1;E+=F[H];}Bh L AD,L BN,-1);L[A]d K.Bl AT K.BY(0,A);BK(!K CS){pop_heap(K AD,K BN);AR[H,S]=K CR H=-H;K.CP()N H!=L[S]Bb b[S]&&!--E)break BE AR[W,X]:R[S])Y F[W]&&M[X]AG&&H<=numeric_limits<T>::AS)-M[X]){T a=H+M[X]N L[W]<0||a<L[W]){L[W]=a;K.BY(-a,W);push_heap(K AD,K BN U}}for(CE H d H<J AC;++H)e[H][C]=e[C][H]w J[H]];Z e[C]AP AQ;BP BS{Bj const O l=400;inline Bj CQ Br l]{};CQ J=0,L d AU short,l+1>F{},K{};BS(){C(U AN _ BD){Z F[C]AG;t C(){F.Bh-1);F[l]=K[l]=l;L d t A BD){K[F[C]]=K[C];F[K[C]]=F[C];F[C]q;t B(O I){Y _(I)){Y K[I]==l)Z;A(I U BH{Y Br I]>J){Y K[l]!=l)C AT Z;}BK(L>J-Br I]AX D=K[l];L-=Br D];A(D U L+=Br I];}F[I]=F[l];K[I]=l;K[F[l]]=I;F[l]=I AQ;BP Be{CQ F,K;};inline Be Bx Cq I){Be D{};i(E,I){Y!C._(E)){(A._(E)?D.K:D.F)+=Ba E];A.B(E U C.B(E U Z D;}inline Be By Cq B){i(I,B)Y!C._(I)){BS D=C,E=A;Z Bx(D,E,B U Z{};}BP Da{BP DE{AU O,8>F{};T J d T K d}x DE>F{1};AU AU BZ,256>,17>b{};AU AU O,2>,17>L{}m M=0 x T>K x BL<T>>J;void A BD,AM,AU T,9>&I){K.CF(C,0);J.CF(B,BL<T>(C));Bk z=BW T,O>;priority_queue<z,BL<z>,greater<z>>D;g(E,0,9)D.emplace(I[E],E);g(E,9,17){AR[H,S Cp AR[W,X Cp L[E]={S,X};D.emplace(H+W,E U g(E,9,17)g(H,0,256)r W=b[E][H]m S=E,X CM O a=7;a AG;--a){S w S][H>>a&1]N S<9){W|=BZ(S+1)<<(4*X++);S=16 AQ W|=BZ(S)<<32;}t I(CD&B,T C AX A CM char D:B AX H=D-'a',E=F[A o[H]N!E){E=F AC;F[A o[H]=E;F.BY(U A=E;}F[A].J+=C;F[A AA n F[A AA,C);t B(T C){BL<O>I(F AC),A;A.reserve(F AC);i(D,F[0].F)Y D)A BG D)BE CE D d D<A AC;++D AX E=A[D],H=I[E];F[E].J+=F[H].J;F[E AA=F[E AA>=C||F[H AA;g(S,0,8){O&W=F[E o[S]N W){I[W]=F[H o[S];A BG W U BH W=F[H o[S]AQ t D BD){M=C%(O)J AC AV B=J[M]BE CE A d A<B AC;++A){K[A]-=B[A];B[A]d}}Bj O _(char C){Z C<='9'?C-'0':(C|32)-'a'+10;}T C(O I,AM,CD&A AX D=16,E=0,H d T S d AN W d AR X=[&](O a){Y a==8)E d BH{E=F[E o[a];S+=F[E].J;W|=F[E AA AQ BE;B>=8;B-=8,H+=2){AR a=b[D][_(A[H])*16+_(A[H+1])];D=a>>32 BE BT AB=a;AB;AB>>=4)X((AB&15)-1 U g(a,0,B){D w D][_(A[H+a/4])>>(3-a%4)&1]N D<9){X(D);D=16 AQ J[M][I]+=S;K[I]+=S;Z W AQ;Bk Bc=__int128_t;const O CH=20000;Bj T Bo(T C,T B){Z C<B?(C!=0):C/B+(C%B!=0 U BP Dc{char J[1<<16]m K,L;CD F;AM(){Y K==L){L=::read(0,J,sizeof J);K d}Z J[K++];}O I(AX C;do C=B AT BK(C<=32);Z C;t C(CD&A){A.Bl()m D=I()BE;D>32;D=B())A+=D;}T _(AX C=I AT AN D=C=='-' N D)C=B AT BZ A CM;C>32;C=B())A=A*10+C-'0';Z D?-A:A;}template<class AI>void A(AI C){char I[48];AM=0 N C<0){F+='-';C=-C;}do{I[B++]='0'+C%10;C/=10;}BK(C);BK(B)F+=I[--B];t D(){F+="END\n";fwrite(F.data(),1,F AC,stdout);fflush(stdout);F.Bl(U}io;BP DC{O b q,R;T K,V;Bc L,e s M,F=1.2 x O>J BX Bg{O F,V=-100,i=-100;T K,L;T e,R;AN M=1;BS b x O>J BX DH{O F;BS K BX DG{BL<O>F;T K;AU O,3>J{}BX Bi{T K,J m F BX Bt{O AI,K,L,V q m i,y,Ds,z,Dt m R q,AE q,BB m Du q;BT M:2,Dp:30;T F,J,b,AH,l;T e,v,AF,BA x Bi>p s u q,Dq;BZ Do,BO BX Bd{T e m K,F,L,p,z,v,y m BA q,Do;T i,AI,l s AE,u,AH,b,AF,Dq;AN M m BB q m J q,Ds=-100,Dp q s R,V m Du q,Dt q BX WarmHint{O J q m F;Be K BX DF{T K q;Bc F;};Bk Bq=BW k,O>;BP DD{O Bu,Cs,Ah,AH,Dq,z,Ag,BO,Ai,Aj,Af m M,Dp,Am m Aa m l;AN Cv;AN Cu;T Ae,Ad,CN,Bs,Bv,Cw;T Cx s Al x T>AE x DH>y x Bg>V x DC>R x DG>Dt x BW O,T>>Ab x Bt>L x Bd>u x O>AI,CC,Du,CA,Cg x T>Do,BA x Bq>p s An[2][40],Ct[2][40]m BB d Bk Snapshot=AU BW k,k>,40>x BL<O>>Ds x k>CO;AU T,30>Cd{},Ce{};AU k,30>AF{},Ak;AU T,30>CB{},Cb{},Cc{};AU O,30>Bz;unordered_map<O,O>Ca{65536}x WarmHint>Ac;Db v;Da Bw;CD i,Cf;Bj AN H(Bg&C,Bt&B){Z C.e>=B.F&&C.R>=B.J;t j(){Bu f Cs f Ah f AH f Dq f z f Ag f BO f Ai f Aj f Af f Ae f Ad f CN f Bs f Bv f AE AW Cs);p(E,AE)E f Cx=*min_element(AE AD,AE BN);v.A(Ah,Cs,AH);y AW AH);V AW Dq);R AW z);Dt AW BO);g(E,0,AH){y[E o f v.J[E]f y[E AA.J f}p(E,V){E.F BF K=E.e BF L=E.R BF b.J f}g(E,0,Af){O H=Bf,S f T W f v.B(H,S,W U p(E,R){E.b BF K f}g(E,0,z)for(O H=E;H AG;H=R[H].b)R[E].J BG H);g(E,0,Ag)Ba E]f p(E,Dt){E.F AW Bf)m H d g(W,0,(O)E.F.size()){O X=E.F[W]BF K+=Ba X]N Ba X]>Ba E.F[H]])H=W;}O S=E.F AC N S)E.J={E.F[0],E.F[H==0?S/2:H],E.F[S-1]};}AU T,9>D;p(E,D)E f Bw.A(Dq,Aj,D);g(E,0,Ai){io.C(Cf);T H f Bw.I(Cf,H);Cw n Cw,H U Bw.B(max<T>(8,Cw/6));L.reserve(60000);Ac AW AH*BO);Do AW z);BA AW z);CO AW z);Ds AW AH);g(E,0,Dq)Ds[V[E o]BG E);t W BD){L[C].R=Du AC;Du BG C);t Dj BD){AM w C].R;Du AP=Du CR L[Du AP].R=B;Du.CP AT L[C].R q;t P(){Bf;Bd C{};C.K f C.i f C.p f O A f C.z n 5,(C.p-M)/3);C.v=min<T>(Bu,(T)C.p+C.z-1);AY=L AC;C.L=AY+A m I=u AC;L AW C.L)Co E.AI f Ca[E.AI]=D;E.K=I;E.F BF J BF b BF L BF V BF AH BF l BF Dt BF i=AI AC;g(S,0,E.Dt){O X=Ca[Bf];AI BG X);++L[X].z;}E.y=AI AC;C.AE+=AO E.F*E.b;C.l+=E.F;l(C,e)E.F)N!E.Dt)W(D)N none_of(V AD,V CU Bg&S){Z H(S,E U))C.v d}AM=CC AC Co B+=E.z;E.Ds=E.z=B;}CC AW B)BE O D=C.L;D-->AY;)r E w D];E.e+=E.b;E.v+=E.b+1;g(S,E.i,E.y)r X w AI[S]]m a=--X.Ds;CC[a]=D;l(X,e)E.e);l(X,v)E.v U}Y A<=40){BZ D[40]{};g(E,C.F,C.L)r X w E];BZ S d g(a,X.i,X.y){O AB=AI[a]-AY;S|=CL AB;D[E-AY]|=D[AB]|(CL AB U BK(S AX a=63-__builtin_clzll(S);X.Do|=CL a;S&=~(D[a]|(CL a)U}}u BG C U T B BD){T A CZ i(I,R[C].J)A=BI A,R[I AA-R[I].V);Z A;t a(Bt&C,O A,AM,Bc I=0){Bc D=(Bc)C CK V[A o o]*B;i(E,R[u[C.K].K].J)r H=R[E];H.V+=AY*B;H.e+=D;H.L+=I-D*M;}t Dr BD){g(B,L[C].Ds,L[C].z)r A w CC AP]N A.y-A.i<=40)A.BO^=CL(find(AI AD+A.i,AI AD+A.y,C)-AI AD-A.i U t X BD,AN B){Dr(C)AV A w C]AV I=V[A.AE]AV D=u[A.K];a(A,A.AE,-1);I.K+=A.F;I.L+=A.J;I.J[A.R]=I.J CR L[I.J[A.R]].R=A.R;I.J.CP AT A.R=A.AE q N!B){A.M d W(C);Z;}A.M=2;++D.y;D.AE-=AO A.F*A.b;D.l-=A.F N A.F==D.e)D.e q N A.b){k H=clamp((M-A.BB-A.BA)/AO A.b,.25,4.)AV S=R[D.K];S.M+=H;++S.R;Al+=H;++Am;}T E=Bo(A.l,CN);g(H,A.Ds,A.z){O S=CC[H]AV AB w S];r AJ=AB.p;AR AK=find_if(AJ AD,AJ CU Bi&Dl){Z Dl.F==I.F;})N AK==AJ BN)AJ BG{A.l,E,I.F});BH{AK->K+=A.l;AK->J n AK->J,E U}Y!--AB.Dt)W(S U BL<Bi>().swap(A.p U T b BD)r A w C]N A.M==2 BC AR&B=u[A.K]N B.BA!=B.y){B.BA=B.y;B.AI d g(I,B.F,B.L)r D w I]N D.M==2)AZ;D.AF d g(E,D.i,D.y)r H w AI[E]]N H.M!=2)l(D,AF)H.AF+H.b U l(B,AI)D.e U}Z B.AI-A.AF-A.e;t c(){io.C(i)N i!="LOG")++l N i=="WF"){P AT Z;}AM f Y i=="DONE")X(Ca AP,1);BM"FAIL"||i=="UP")r A=V AP;A.M=i=="UP";A.b.C()N!A.M){Cu=1;A.i=M;A.V=-100;BK(!A.J CS)X(A.J.back(),0 U}BM"DCDOWN"||i=="DCUP"){v.D(B,i=="DCUP" U BM"LDOWN"||i=="LUP"){v.E(B,i=="LUP" U BM"QUOTA"){Ab.BY(B,R[B AA);R[B AA f}BM"UNDO"){BK(B--){AR[A,I]=Ab CR Ab.CP AT R[A AA=I AQ BH{O A f io.C(Cf)N Bw.C(B,A,Cf))V AP.V=M;}t G(O I){Bp"ANS";BK(I--){io.C(i)m D f Bc E=0 N i=="BILL")E=R[D].L+R[D].e*M;BM"HEAD")E=B(D);BM"ROUTE")E CT D,Bf);BM"ALARM")E=Bw.K[D];BM"SLACK")E=b(Ca[D]);BM"CACHE")E=V[D].b.L;BH E=y[D AA.L;Bp' ';io.A(E U Bp'\n';t AJ(const char*B,O I,O C q){Bp B;io.A(I)N C AG){Bp' ';io.A(C U Bp'\n';++Dp;}k I BR C,k B){Y B>Bu BC Z AO C.i*clamp((C.p+k(C.z)-B)/C.z,0.,1. U k _ BR C,k B){Z.72*I(C,M+B)+.28*I(C,M+AS 1.,.77*B)U Bj k AB(Bt&C,k B){Z C.u AG?C.u:(C.e-C.b)*B+(C.v-C.e-1 U k D(Bt&C){k B n 1.,C.b*R[u[C.K AA o)N C.M!=1)Z B s A=M-C.BB-C.BA;Z A<0?B-A:AS{1.,B-A,.25*B}U k C BR B){Z B.AE*R[B.K o+B.l;}T A(Be C){Z Bo(AY,Ae)+Bo(C.K,Ad);t AK BR C,Snapshot&B){g(A,C.F,C.L){swap(L[A].u,B[A-AY].first);swap(L[A].Dq,B[A-AY]CJ U t Dk BR C){g(B,C.F,C.L)L AP.u CM AM=C.L;B-->AY;)r A w B];A.Dq=Ct[BB][B-AY]BE y(I,A,Do)r D w AY+CY I)];l(D,u)An[BB][B-AY]+A.u U}}AN S BR I,AM,AN W=1,O E q){Y E<0)E=Bz[B]N E<0||!v.F[v.J AP]BC BS AB=V[E].b,Dl=y[B AA s a[40]s X=0,AJ=0,AK=0,Dj=AE[y[B o]m Dr q;g(Q,I.F,I.L){O c=Q-I.F AV G w Q]s&Dm=An[BB][c];a[c]=Dm=Ct[BB][c]=0 N G.M==2 BV Dn=D(G),P=0,j=0,e=0 N G.M==1){a[c]=Dm=Dn;j=Dn*G CK V[G.AE o o];}BH{Y!(G.Dp>>B&1)){Y none_of(Ds AP AD,Ds AP CU O Ar){Z V[Ar]BJ H(V[Ar],G U)BC G.Dp|=1u<<B;}k Ap d AR Aq=[&](O Ar,T As){Y Ar==B)Z 1;T At CT Ar,B)N At<0 BC P n P,AO(At+Bo(As,CN)));Ap+=AO As*At*Bv;Z 1;}N G.V AG&&!Aq(G.V,G.AH)BC for(Bi&Ar:G.p)Y v(Ar,F,B,As)BC P n P,AO(As+Ar.J));Ap+=AO Ar.K*As*Bv;}for(y(Ar,G,Do)e n e,a[CY Ar)])BE y(Ar,G,BO)r As w AI[G.i+CY Ar)]]N!Aq(V[As.AE o,As.l)BC}Be Ao{}N G.L!=Dr)Ao=Bx(AB,Dl,Dt[G.L o);Dr=Dt[G.L AA<=AB.J?G.L:-1 N Ao.F||Ao.K)P n P,AO A(Ao));j=Ap+AO Ao.F*Bs+(P+Dn)*G.F*Dj;Dm=P+Dn;a[c]=e+Dm;}Ct[BB][c]=j;AJ+=j;AK+=Dm*G.F;X n X,a[c]U X n X,AK/CI BI BA[I.K],CB AP)))s h=I BJ B==I.J?C(I):0;X+=.25*AS 0.,AF AP-h)/CI CB AP);I.R=AJ;I.V=X N W)Dk(I);Z 1;t Dm BR B){Y B.Ds>M-7&&B.Dp==B.y&&B.J AG)Z;Bc a[30]{};BT W d g(h,B.F,B.L)r Q w h]N Q.M==2 Bb Q.V AG){W|=1u<<Q.V;a[Q.V]+=Q.AH;}for(Bi&G:Q.p){a[G.F]+=G.K;W|=1u<<G.F AQ k AJ[30]BE BT h=W;h;h&=h-1 AX Q=__builtin_ctz(h);AJ[Q]=AO a[Q];}O D q,AB=B.J s Dl=-INFINITY,AK=0,Dr d k Dj=.25*B.i/AS 5,B.v-M);AU BW k,O>,30>H m X d O I=B.F;BK(I<B.L&&L[I].M!=0)++I;g(h,0,AH){Y I<B.L&&(Cb[h]<L[I o||Cc[h]<L[I].J)BV Q=AE[y[h o]*B.AE*R[B.K o N Bz[h]<0||!v.F[v.J[h]]BV G CM BT P=W&~(1u<<h);P;P&=P-1 AX c=__builtin_ctz(P);T j CT c,h)N j<0){G q;break;}G+=AJ[c]*j*Bv;}Y G<0)Q=1e50;BH Q+=G N h==B.J)Q*=.92 N I<B.L)r c w I]AV P=Ac[h*BO+c.L]N P.J!=M/7){P.J=M/7;P.F q s j=INFINITY,Dn=AO c CK h o]AV e=y[h AA;i(Ao,Ds[h])r Ap=V[Ao]N!Ap.M||Ap.K<c.F||Ap.L<c.J)AZ;Be Aq{};i(As,Dt[c.L].J)Y!Ap.b._(As))(e._(As)?Aq.K:Aq.F)+=Ba As]s Ar=AO Aq.F*Bs+A(Aq)*Dn N Ar<j){j=Ar;P.F=Ao N!Ar)break AQ P.K=E(P.F<0?Bz[h]:P.F,c U Q+=AO P.K.F*Bs+AO A(P.K)*c CK h o];}Y!B.M)Q+=BI.1*Q,Dj*AF[h]*Ak[h]);H[X++]={Q,h};}partial_sort(H AD,H AD+BI 4,X),H AD+X);g(h,0,min(4,X)){O Q=H[h]CJ m P q N!B BJ I<B.L)r j w I]m c=Ac[Q*BO+j.L o N c AG&&V[c]BJ V[c AA>=j.F&&V[c].L>=j.J)P=c;}Y!S(B,Q,0,P)BV G=_(B,B.V)-B.R N Q==B.J)G+=.015*B.i N G>Dl){Dl=G;D=Q;AK=B.R;Dr=B.V;BB^=1 AQ Y B BJ D!=AB){k h=C(B)N AB AG)AF[AB]-=h N D AG)AF[D]+=h;}B.J=D;B.Ds=M;B.Dp=B.y N D AG){B.R=AK;B.V=Dr;BB^=1;Dk(B);B.Du=M;B.Dt=l;}BH{B.V=1e9;B.R=1e30;}t h BR C,AM){Y C.Do==M)Z;C.Do=M;C.u=C.AH d C.Dq d Cg BG B U Be E BD,Bt&B)r A=V[C];Z By(A.b,y[A.F AA,Dt[B.L o U AN J BD)r A w C]AV I=Dt[A.L];g(D,0,min(64,(O)p.size()))r E w p[D]CJ]N E.K==A.K||E.M!=0||B(u[E.K AA)<E.F)AZ AV H=Dt[E.L]N!H.K)AZ;i(S,I.J)i(W,H.J)Y S==W&&Ba S]>=CI BI I.K,H.K)/4))Z 1;}Z 0;}k e(Bt&C,AM){k A=1e9;i(I,Ds[B])r E=V[I]N!E.M||!H(E,C)Bb E.K>=AY&&E.L>=C.J)Z 0 x BW k,O>>S;S.reserve(E.J AC);i(a,E.J)S.BY(D(L[a]),a);sort(S AD,S BN);T W=E.K,X=E.L BE AR[AB,a]:S){W+=L[a o;X+=L[a].J N W>=AY&&X>=C.J){A=BI A,AB);break AQ}Z A;}AN Dn(AM)r u(D,B,A)!A.M||A.J<0||A.BB==M||Aa>=8 BC A.BB=M;AR W=[&](O AJ){Y!v.F[v.J[AJ]]BC i(Dr,Ds[AJ])Y V[Dr]BJ V[Dr AA>=D.F&&V[Dr].L>=D.J)Z 1;Z 0;}N W(A.J)BC++Aa;Bd AB=A;Snapshot X{};AK(A,X)m E=A.J,I q s a n 0.,_(A,A.V+.5*e(D,E))-A.R)+.005*A.i;g(AJ,0,AH)Y AJ!=E&&W(AJ)&&S(A,AJ)){k Dr=_(A,A.V)-A.R N Dr>a){a=Dr;I=AJ AQ k H=C(A)N I AG){AF[E]-=H;AF[I]+=H;A.J=I;A.Du=A.Dt q N Dl(B)){A.Ds=M;A.Dp=A.y;Z 1;}AF[E]+=H;AF[I]-=H;}A=AB;AK(A,X);Z 0;}AN Dl(AM)r u(H,B,D)D.J AG&&((D.Du==M&&D.Dt==l)||S(D,D.J))){D.Du=M;D.Dt=l;D.b=D.V;D.AF=D.R;}k P=R[D.K o,AK n 1.,H.b*P)s Dm=AB(H,P),c n 0.,D.AF-H.Dq)AV Ap=Dt[H.L];DF Dk[30];BT h d O Aq=D.J<0?AH:D.J+1;g(Av,max(0,D.J),Aq){Y Cd[Av]<H.F||Ce[Av]<H.J)AZ;T Aw d Bc Ax d p(Ay,H.p)Y v(Ay,F,Av,Az)){Aw q;break;}Aw n Aw,Az+Ay.J);Ax+=(Bc)Ay.K*Az*Bv;}Y Aw AG&&H.V AG&&v(H,V,Av,Ay))Aw q;BH{Aw n Aw,Ay+Bo(H.AH,CN));Ax+=(Bc)H.AH*Ay*Bv AQ Y Aw<0)AZ;h|=1u<<Av;Dk[Av]={Aw,Ax};}Y!h BC k j=D.v-M,Ar=AO D.i/AS 5,D.z)s As=.09*D.i/AS 4.,j)+.48*Ar*clamp(1.+(M+D.b-D.p)/6.,0.,1.);AR At=[&](Bg&Av){Z AS 0,7-(M-Av.V))*(AK+3.)*H.F*(.22+.07*Cx)+AS 0,5-(M-Av.i))*.15*H.F;}m W q,Q q;T Dn d Bc e d k G=INFINITY;AN Ao=0 x O>&Au=D.J AG?Ds[D.J]:CA;i(Av,Au)r Ax=V[Av]m Ch=Ax.F N!Ax.M||!(h>>Ch&1)||Ax.K<H.F||Ax.L<H.J)AZ;Be Cy=E(Av,H);T Ay n Dk[Ch AA,A(Cy))N Ay>=j)AZ;Bc Ck=Dk[Ch o+(Bc)Cy.F*Bs s Cz=AO H CK Ch o]*(Ay+AK),Az=AO Ck+Cz s Ci=Az+As*Ay+At(Ax)+.15*(Ax.K>H.F)+.07*(Ax.L>H.J)N Ci>=G BV DA n D.b,Ay+AK+Dm)s DB n.77*D.b,Ay+AS 1.,.72*AK)+.77*Dm)s Aw=.72*I(D,M+DA)+.28*I(D,M+DB);AN Cl=0 N(Az>Aw||Az+c>Aw*1.03)&&!D BJ!Cv&&Cy.F){Y Q<0)Q=J(B)N Q){k Cm=.45*_(D,D.u);Cl=Cm>Aw;Aw n Aw,Cm U}Y Az>Aw||Az+c>Aw*(D.M?1.12:1.03))AZ;Ci+=(D.i-Aw)*.76 N Ci<G){G=Ci;W=Av;Dn=Ay;e=Ck;Ao=Cl AQ Y W<0 BC AR&X=V[W];Bx(X.b,y[X.F AA,Ap.F);++l;Cv|=Ao N!D BJ D.J AG)AF[D.J]+=C(D);Dj(B);H.R=X.J AC;X.J BG B);H.M=1;H.AE=W;H.BB=M;H.BA=Dn;Dr(B);X.K-=H.F;X.L-=H.J;D.M=1;a(H,W,1,e);AJ("RUN ",H.AI,W);Z 1;t K(){CA.Bl AT Cg.Bl AT p.Bl AT Cd Bm Ce Bm Dp d Cv d T W=0,I d Aa d CO.CF(z,0);AF Bm CB Bm Cb Bm Cc Bm Bz.Bh-1)s a[30]s Dr=(Al+9.6)/(Am+8.);g(Q,0,z)r G=R[Q];G.F=(G.M+4*Dr)/(G.R+4.);BA[Q]CZ i(P,G.J)BA[Q]=BI BA[Q],R[P AA U for BR Q:u)Y Q BJ Q.AE>0&&M<Q.v){k G=C(Q);i(P,R[Q.K].J)CO[P]+=G N Q.J AG)AF[Q.J]+=G;}g(Q,0,Dq)r G=V[Q]N!G.M)AZ m P=G.F;CB[P]+=G.e;Cb[P]n Cb[P],G.e);Cc[P]n Cc[P],G.R)s c=G.b.J+3*G.e+G.R N Bz[P]<0||c>a[P]){a[P]=c;Bz[P]=Q;}for(CE j d j<G.J AC;AX Aq=G.J[j]AV u(Ao,Aq,Ap)M>=Ap.v&&Dp<CH){AJ("KILL ",Ao.AI);X(Aq,0);++l;AZ;}++j;W+=Ao.F s Ar=R[Ap.K o,At n 1.,Ao.b*Ar)s As=D(Ao);h(Ap,Ao.K);l(Ap,u)As+AB(Ao,Ar));Ap.AH+=Ao.F*AS 0.,At+1-As U W+=G.K;I+=G.K;CA BG Q);Cd[G.F]n Cd[G.F],G.K);Ce[G.F]n Ce[G.F],G.L U Y M==Bu||CA CS||Dp==CH)Z;g(Q,0,AH)Ak[Q]=1./CI CB[Q]);g(Q,0,z)Do[Q]=B(Q)x T>Dk(BO,-1)BE CE Q d Q<Du AC;AX c=Du[Q]AV u(P,c,G)M>=G.v){Dj(c)x Bi>().swap(P.p);AZ;}++Q N!G BJ P.Du==l Bb P Cj G.K])||G.e>BA[G.K]Bb!G BJ CV==1){T&j=Dk[P.L]N j<0){j CZ i(Ao,CA){Be Ap=E(Ao,P);j=BI j,A(Ap)U}Y j>=G.v-M){P.Du=l;AZ AQ h(G,P.K);l(G,u)Cr U Y Cg CS)Z;i(Q,Cg)r G=u[Q]s c=R[G.K o N G.e<0){G.e d g(Ar,G.F,G.L)Y L[Ar].M!=2)l(G,e)L[Ar o U Y G.e>BA[G.K]||CV>40 BV j n 0.,G.AE*c+G.l-G.AH);Dm(G);G.b n 1.,G.V-(M-G.Ds));G.AF=G.R N!G.M){k Ar d i(As,R[G.K].J)Ar n Ar,CO[As]/CI BI R[As AA,W)));G.b+=.45*Ar*AS 0.,1.-Do[G.K]*G.b/AS 1.,j)U k P=_(G,G.b)-G.AF N!G.M)P n P,.15*(_(G,G.u)-AS 0.,G.AE*c-G.AH)*Cx))N P<=0 Bb!G BJ CV>12&&(G.AF>.7*G.i||M+1.25*G.b>BI Bu,G.p))BV Ao n 0.,G.p-M-G.b),Ap=1.+2./(3.+Ao)s Aq=G.M?1.8+1.2*G.y/AS 1,CV):1.;G.Dq=P*Ap*Aq/(18.+.8*j+3.*G.b U i(Q,Du)r u(P,Q,G)G.Do!=M||G.Dq<=0||(!G BJ P.Du==l)||P Cj G.K])BV c=(Cr)/AS 1.,G.u);p BG{-G.Dq*(.78+.22*c),Q}U O S n 144,Dq),AK n 400,4*Dq),H=p AC BE O Q d Q<H&&S>0&&Dp<CH;Q+=AK AX G=BI H,Q+AK)N G<H)nth_element(p AD+Q,p AD+G,p BN);sort(p AD+Q,p AD+G);g(P,Q,G&&S>0&&Dp<CH){O j=p[P]CJ AV c w j]m Ao=u[c.K AA N c.F>I||B(Ao)<c.F Bb Dl(j)||Dn(j))I-=c.F;BH{--S N!u[c.K].M)L[j].Du=l AQ Y G<H&&S>0&&Dp<CH){g(P,0,z)Do[P]=B(P);p.erase(remove_if(p AD+G,p CU Bq&P)r c w P CJ];Z c Cj u[c.K AA]U),p BN);H=p AC AQ t F(){j()BE AM=1;B<=Bu;++B){io.C(i);M f O A=Bf,I f Bw.D(M);BK(A--)c()N Cu){p(E,L)E.Dp d Cu d}G(I);K AT io.D(U}}m main(){
#ifdef _WIN32
_setmode(0,32768);
#endif
DD C{};AY(U