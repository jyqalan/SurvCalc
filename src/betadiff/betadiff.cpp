// const char* time_stamp = "$Date: 2008-08-14 16:56:06 +1200 (Thu, 14 Aug 2008) $\n";
// const char* betadiff_id = "$Id: betadiff.cpp 234 2008-08-14 04:56:06Z fud $\n";

#include "betadiff.h"
#include "limits.h"

static double betadiff_seed=-1; // Ugh! A global variable. This is used by the random number generators
                                // to remember the current seed.

void error(const std::string& problem){
  if(problem.size() > 1) std::cerr << "Betadiff error: " << problem << '\n';
  std::cerr << "Please notify the software maintainer about this error.\n\n";
  exit(BETADIFF_FATAL_EXIT);
}



dvector::dvector(int i1, int i2){
  first = i1;
  int i = i2-i1+1;
#if !defined(_UNCHECKED_)
  if (i<=0){
    error("Attempt to create dvector of length <=0: this is illegal");}
#endif
  sz = i;
  ptr = new double[i];
}
dvector::dvector(int i){
#if !defined(_UNCHECKED_)
  if (i<=0){
    error("Attempt to create dvector of length <=0: this is illegal");}
#endif
  first = 1;
  sz = i;
  ptr = new double[i];
}
dvector::dvector(const dvector& d){
  first = d.first;
  sz = d.sz;
  ptr = new double[sz];
  for (int i=0; i<sz; i++){
    ptr[i] = d.ptr[i];}
}
dvector::dvector(char* c){
  first = 1;
  // is it a file or a set of values?
  if (c[0]=='{'){
    // it's a set of values
    c++;
    char **u=(char**)malloc(sizeof(char*));
    double **buffer;
    buffer = (double**)malloc(10*sizeof(double*));
    buffer[0] = (double*)malloc(50*sizeof(double));
    int bufcount=0; // the sub-buffer we are about to write to
    int subcount=0; // the position in sub-buffer we are about to write to
    while (c[0]!=0){
      if (subcount == 50){
        subcount=0;
        bufcount++;
        if (bufcount%10 == 0){
          buffer=(double**)realloc(buffer,10*bufcount*sizeof(double*));}
        buffer[bufcount]=(double*)malloc(50*sizeof(double));
      }
      buffer[bufcount][subcount]=strtod(c,u);
      c=(*u)+1;
      subcount++;
    }
    sz = bufcount*50+subcount;
    if (sz){
      ptr = new double[sz];}

    int count1,count2;
    for (count1=0;count1<=bufcount;count1++){
      for (count2=0;count2<50;count2++){
        if (count1==bufcount & count2==subcount) break;
        ptr[count1*50+count2]=buffer[count1][count2];
      }
      free(buffer[count1]);
    }
    free(buffer);
    free(u);
  }
  else{
    // it's a file
    std::ifstream istr(c);
    if (!istr) {error("bad file in dvector::dvector(" + std::string(c) + ")");}
    double **buffer;
    buffer = (double**)malloc(10*sizeof(double*));
    buffer[0] = (double*)malloc(50*sizeof(double));
    int bufcount=0; // the sub-buffer we are about to write to
    int subcount=0; // the position in sub-buffer we are about to write to
    while (1){
      if (subcount == 50){
        subcount=0;
        bufcount++;
        if (bufcount%10 == 0){
          buffer=(double**)realloc(buffer,10*bufcount*sizeof(double*));
        }
        buffer[bufcount]=(double*)malloc(50*sizeof(double));
      }
      if (!GOOD(istr >> buffer[bufcount][subcount])){
        break;
      } else {
        subcount++;
      }
    }
    sz = bufcount*50+subcount;
    if (sz){
      ptr = new double[sz];}

    int count1,count2;
    for (count1=0;count1<=bufcount;count1++){
      for (count2=0;count2<50;count2++){
        if (count1==bufcount & count2==subcount) break;
        ptr[count1*50+count2]=buffer[count1][count2];
      }
      free(buffer[count1]);
    }
    free(buffer);
  }
}
dvector::dvector(){
  first = 1;
  sz = 1;
  ptr = new double[1];
}
dvector& dvector::operator=(const dvector& d){
#if !defined(_UNCHECKED_)
  if (sz != d.sz){
    error("bad size in dvector::operator=(dvector)");}
#endif
  for (int i=0; i<sz; i++){
    ptr[i] = d.ptr[i];}
  return *this;
}
dvector& dvector::operator=(double t){
  for (int i=0; i<sz; i++){
    ptr[i] = t;}
  return *this;
}
dvector& dvector::operator=(int j){
  for (int i=0; i<sz; i++){
    ptr[i] = j;}
  return *this;
}

void dvector::initialize(){
  (*this) = 0.0;
}

int dvector::indexmin() const {
  return first;
}
int dvector::indexmax() const {
  return first+sz-1;
}
int dvector::size() const{
  return sz;
}
void dvector::shift(int m){
  first=m;
}

double& dvector::operator[](int i) const{
#if !defined(_UNCHECKED_)
  if (i<first || (i >= first + sz)){
    error("bad index in dvector::operator[](int)");}
#endif
  return ptr[i-first];
}
double& dvector::operator[](double t) const{
  int i = (int) t;
#if !defined(_UNCHECKED_)
  if (i<first || (i >= first + sz)){
    error("bad index in dvector::operator[](double)");}
#endif
  return ptr[i-first];
}

dvector dvector::operator[](const dvector& d) const{
  dvector n(d.indexmin(),d.indexmax());
  int count;
  for (count=0;count<d.sz;count++){
    n.ptr[count] = ptr[(int)d.ptr[count]-first];}
  return n;
}

void dvector::fill_randu(const long int& i){
  static Uniform u;
  double newseed=double(abs(i+1))/RNG_LONGMAX;
  newseed=newseed-floor(newseed);
  if (betadiff_seed!=newseed){
    Random::Set(newseed);
    betadiff_seed=newseed;}
  int count;
  for (count=0;count<sz;count++){
    ptr[count]=u.Next();}
}
void dvector::fill_randn(const long int& i){
  static Normal n;
  double newseed;
  newseed=double(abs(i+1))/RNG_LONGMAX;
  newseed=newseed-floor(newseed);
  if (betadiff_seed!=newseed){
    Random::Set(newseed);
    betadiff_seed=newseed;}
  int count;
  for (count=0;count<sz;count++){
    ptr[count]=n.Next();}
}
void dvector::fill_randbi(const long int& i, double p){
  static Uniform u;
  double newseed;
  newseed=double(abs(i+1))/RNG_LONGMAX;
  newseed=newseed-floor(newseed);
  if (betadiff_seed!=newseed){
    Random::Set(newseed);
    betadiff_seed=newseed;}
  int count;
  for (count=0;count<sz;count++){
    ptr[count]=((u.Next()>p)?0:1);}
}

int rbinomial(const long int& i, int N, double p){
  // slow - accurate but inefficient
  if (N<1) return 0;
  dvector bern(1,N);
  bern.fill_randbi(i,p);
  return (int)sum(bern);
}

void dvector::fill_multinomial(const long int& i, const dvector& p){
  if (fabs(sum(p)-1) >= 1e-10){
    error("Bad p in dvector::fill_multinomial");}
  static Uniform u;
  double newseed=double(abs(i+1))/RNG_LONGMAX;
  newseed=newseed-floor(newseed);
  if (betadiff_seed!=newseed){
    Random::Set(newseed);
    betadiff_seed=newseed;}
  for (int count=0;count<size();count++){
    ptr[count]=u.Next();
    double sum=0;
    for (int count2=p.indexmin(); count2<=p.indexmax(); count2++){
      sum += p[count2];
      if (sum >= ptr[count]){
        ptr[count] = count2;
        break;
      }
    }
  }
}
void dvector::fill_multinomial_counts(const long int& i, const dvector& p, int N){
  // number of multinomial random numbers in each category
  dvector temp(1,N);
  temp.fill_multinomial(i,p);
  initialize();
  for (int j=1; j<=N; j++){
        (*this)[(int)(temp[j])]++;}
}
void dvector::fill_chisq(const long int& i, int df){
  static ChiSq c(df);
  double newseed=double(abs(i+1))/RNG_LONGMAX;
  newseed=newseed-floor(newseed);
  if (betadiff_seed!=newseed){
    Random::Set(newseed);
    betadiff_seed=newseed;}
  int count;
  for (count=0;count<sz;count++){
    ptr[count]=c.Next();}
}
void dvector::fill_mvnorm(const long int& s, const dvector& mean, const dmatrix& _covar){
  // Found a problem here for the case where some variances (and hence the corresponding
  // rows and columns of covar) are 0. The cholesky decomposition does not work in these
  // cases. I thought of this kludge: Set the 0 variances to 1, leaving the covariances at 0.
  // When finished, set the corresponding deviations from the mean to 0.
  dmatrix covar(_covar);
  int n = covar.rowsize();
  if (!(n == covar.colsize() && n == size() && n == mean.size())){
    error("Bad argument to fill_mvnorm");}
  // set zero variances to 1 as above
  dvector zero_variances(covar.rowmin(),covar.rowmax());
  zero_variances = 0;
  for (int i=covar.rowmin(); i<=covar.rowmax(); i++){
    if (covar[i][i]==0){
      covar[i][i] = 1;
      zero_variances[i] = 1;
    }
  }
  // generate the standard normal random numbers
  fill_randn(s);
  // get lower triangular part of choleski decomposition of covar
  dmatrix L(1,n,1,n);
  if (!chol(covar,L)){
    error("Failed choleski decomposition in fill_mvnorm");}
  (*this) = L*(*this) + mean;
  // find the elements with 0 variances and set them to their means
  for (int i=covar.rowmin(); i<=covar.rowmax(); i++){
    if (zero_variances[i]){
      (*this)[i] = mean[i];
    }
  }
}
void dvector::fill_mvt(const long int& s, const dvector& mean, const dmatrix& _covar, int df){
  // Result is mean + 1/sqrt(y/df) * x,
  // where x ~ multivariate normal(0,_covar)
  // and y ~ chi-square(df).
  // See fill_mvnorm for treatment of zero variances.
  dvector zeros(mean * 0);
  fill_mvnorm(s,zeros,_covar);
  dvector y(1,1);
  y.fill_chisq(s,df);
  (*this) *= (1/sqrt(y[1]/df));
  (*this) += mean;
}
void dvector::fill(char *s){
  dvector dv(s);
  (*this) = dv;
}
void dvector::fill_seqadd(double base, double offset){
  int count;
  for (count=0;count<sz;count++){
    ptr[count]=base+count*offset;}
}

void dvector::operator*=(const dvector& dv){
#if !defined(_UNCHECKED_)
  if (sz != dv.sz){
    error("bad size in dvector::operator*=");}
#endif
  for (int i=0; i<sz; i++){
    ptr[i] *= dv.ptr[i];}
}
void dvector::operator+=(const dvector& dv){
#if !defined(_UNCHECKED_)
  if (sz != dv.sz){
    error("bad size in dvector::operator+=");}
#endif
  for (int i=0; i<sz; i++){
    ptr[i] += dv.ptr[i];}
}
void dvector::operator-=(const dvector& dv){
#if !defined(_UNCHECKED_)
  if (sz != dv.sz){
    error("bad size in dvector::operator-=");}
#endif
  for (int i=0; i<sz; i++){
    ptr[i] -= dv.ptr[i];}
}
void dvector::operator/=(double t){
  for (int i=0; i<sz; i++){
    ptr[i] /= t;}
}
void dvector::operator*=(double t){
  for (int i=0; i<sz; i++){
    ptr[i] *= t;}
}
void dvector::operator+=(double t){
  for (int i=0; i<sz; i++){
    ptr[i] += t;}
}
void dvector::operator-=(double t){
  for (int i=0; i<sz; i++){
    ptr[i] -= t;}
}

dvector::~dvector(){
  delete[] ptr;
}

double operator*(const dvector& dv1, const dvector& dv2){
#if !defined(_UNCHECKED_)
  if (dv1.sz != dv2.sz){
    error("bad size in operator*(dvector,dvector)");}
#endif
  double total = 0;
  for (int i=0; i<dv1.sz; i++){
    total += dv1.ptr[i] * dv2.ptr[i];}
  return total;
}
dvector operator*(double t, const dvector& dv){
  dvector n=dv;
  for (int i=0; i<dv.sz; i++){
    n.ptr[i] *= t;}
  return n;
}
dvector operator*(const dvector& dv, double t){
  dvector n=dv;
  for (int i=0; i<dv.sz; i++){
    n.ptr[i] *= t;}
  return n;
}
dvector operator/(double t, const dvector& dv){
  dvector n(1,dv.sz);
  for (int i=0; i<dv.sz; i++){
    n.ptr[i] = t / dv.ptr[i];}
  return n;
}
dvector operator/(const dvector& dv, double t){
  dvector n(1,dv.sz);
  for (int i=0; i<dv.sz; i++){
    n.ptr[i] = dv.ptr[i] / t;}
  return n;
}
dvector operator+(const dvector& dv1, const dvector& dv2){
#if !defined(_UNCHECKED_)
  if (dv1.sz != dv2.sz){
    error("bad size in operator+(dvector,dvector)");}
#endif
  dvector n(1,dv1.sz);
  for (int i=0; i<dv1.sz; i++){
    n.ptr[i] = dv1.ptr[i] + dv2.ptr[i];}
  return n;
}
dvector operator+(const dvector& dv, double t){
  dvector n(1,dv.sz);
  for (int i=0; i<dv.sz; i++){
    n.ptr[i] = dv.ptr[i] + t;}
  return n;
}
dvector operator+(double t, const dvector& dv){
  dvector n(1,dv.sz);
  for (int i=0; i<dv.sz; i++){
    n.ptr[i] = dv.ptr[i] + t;}
  return n;
}
dvector operator-(const dvector& dv1, const dvector& dv2){
#if !defined(_UNCHECKED_)
  if (dv1.sz != dv2.sz){
    error("bad size in operator-(dvector,dvector)");}
#endif
  dvector n(1,dv1.sz);
  for (int i=0; i<dv1.sz; i++){
    n.ptr[i] = dv1.ptr[i] - dv2.ptr[i];}
  return n;
}
dvector operator-(const dvector& dv, double t){
  dvector n(1,dv.sz);
  for (int i=0; i<dv.sz; i++){
    n.ptr[i] = dv.ptr[i] - t;}
  return n;
}
dvector operator-(double t, const dvector& dv){
  dvector n(1,dv.sz);
  for (int i=0; i<dv.sz; i++){
    n.ptr[i] = t - dv.ptr[i];}
  return n;
}
dvector operator-(const dvector& dv){
  dvector n(1,dv.sz);
  for (int i=0; i<dv.sz; i++){
    n.ptr[i] = -dv.ptr[i];}
  return n;
}
dvector operator&(const dvector& dv1, const dvector& dv2){
  int s1=dv1.size();
  int s2=dv2.size();
  dvector n(s1+s2);
  int count;
  for (count=0;count<s1;count++){
    n[count+1]=dv1.ptr[count];}
  for (count=0;count<s2;count++){
    n[count+s1+1]=dv2.ptr[count];}
  return n;
}

dvector value(const dvector& dv){
  return dv;
}

double sum(const dvector& d){
  dvector ones(1,d.size());
  ones=1;
  return d*ones;
}
double norm(const dvector& d){
  return sqrt(d*d);
}
double norm2(const dvector& d){
  return d*d;
}
dvector elem_prod(const dvector& d1, const dvector& d2){
  int count;
  int l1=d1.size(),l2=d2.size();
#if !defined(_UNCHECKED_)
  if (l1!=l2) {error("bad size in elem_prod(dvector,dvector)");}
#endif
  dvector d(l1);
  for (count=0;count<l1;count++)
    d.ptr[count]=d1.ptr[count]*d2.ptr[count];
  return d;
}
dvector elem_prod(const dvector& d1, const dvector& d2, const dvector& d3){
  int count;
  int l1=d1.size(),l2=d2.size(),l3=d3.size();
#if !defined(_UNCHECKED_)
  if (l1!=l2 || l2!=l3) {error("bad size in elem_prod(dvector,dvector,dvector)");}
#endif
  dvector d(l1);
  for (count=0;count<l1;count++)
    d.ptr[count]=d1.ptr[count]*d2.ptr[count]*d3.ptr[count];
  return d;
}
dvector elem_div(const dvector& d1, const dvector& d2){
  int count;
  int l1=d1.size(),l2=d2.size();
#if !defined(_UNCHECKED_)
  if (l1!=l2) {error("bad size in elem_div(dvector,dvector)");}
#endif
  dvector d(l1);
  for (count=0;count<l1;count++)
    d.ptr[count]=d1.ptr[count]/d2.ptr[count];
  return d;
}
double max(const dvector& d){
  int count;
  double m=d[d.indexmin()];
  if (d.size()>1)
    for (count=d.indexmin()+1;count<=d.indexmax();count++)
      m=fmax(m,d[count]);
  return m;
}
double min(const dvector& d){
  int count;
  double m=d[d.indexmin()];
  if (d.size()>1)
    for (count=d.indexmin()+1;count<=d.indexmax();count++)
      m=fmin(m,d[count]);
  return m;
}
double mean(const dvector& d){
  double m;
  m=sum(d)/d.size();
  return m;
}
double std_dev(const dvector& d){
  double m;
  int n=d.size();
  m=sqrt((norm2(d)-sum(d)*sum(d)/n)/(n-1));
  return m;
}
dvector pow(const dvector& d1, const dvector& d2){
#if !defined(_UNCHECKED_)
  if (d1.sz != d2.sz) error("bad size in pow(dvector,dvector)");
#endif
  dvector n(d1.sz);
  for (int i=0; i<d1.sz; i++){
    n.ptr[i] = pow(d1.ptr[i],d2.ptr[i]);}
  return n;
}
dvector pow(double t, const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = pow(t,d.ptr[i]);}
  return n;
}
dvector pow(const dvector& d, double t){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = pow(d.ptr[i],t);}
  return n;
}
dvector sin(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = sin(d.ptr[i]);
  }
  return n;
}
dvector cos(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = cos(d.ptr[i]);
  }
  return n;
}
dvector tan(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = tan(d.ptr[i]);
  }
  return n;
}
dvector asin(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = asin(d.ptr[i]);
  }
  return n;
}
dvector acos(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = acos(d.ptr[i]);
  }
  return n;
}
dvector atan(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = atan(d.ptr[i]);
  }
  return n;
}
dvector sqrt(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = sqrt(d.ptr[i]);
  }
  return n;
}
dvector exp(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = exp(d.ptr[i]);
  }
  return n;
}
dvector log(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = log(d.ptr[i]);
  }
  return n;
}
dvector log10(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = log10(d.ptr[i]);
  }
  return n;
}
dvector fabs(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = fabs(d.ptr[i]);
  }
  return n;
}
dvector sfabs(const dvector& d){
  return fabs(d);
}
dvector ceil(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = ceil(d.ptr[i]);
  }
  return n;
}
dvector floor(const dvector& d){
  dvector n(d.sz);
  for (int i=0; i<d.sz; i++){
    n.ptr[i] = floor(d.ptr[i]);
  }
  return n;
}

std::istream& operator>>(std::istream& istr, dvector& dv){
  int count;
  double t;
  for (count=dv.indexmin();count<=dv.indexmax();count++){
    istr >> t;
    if (istr) dv[count]=t;
    else {
      error("Error in input file - invalid number of values found.");
    }
  }
  return istr;
}
std::ostream& operator<<(std::ostream& ostr, const dvector& dv){
  int count;
  for (count=dv.indexmin();count<=dv.indexmax();count++){
    ostr << dv[count] << " ";}
  return ostr;
}

// sorts
int qsortcmp(const void* t1, const void* t2){
  return ((*(double*)t1==*(double*)t2) ? 0 : (*(double*)t1>*(double*)t2 ? 1 : -1));
}
dvector sort(const dvector& d){
  int length=d.size();
  dvector res(length);
  double* t = new double[length];
  int count;
  for (count=0;count<length;count++){
    t[count]=d.ptr[count];}
  qsort(t,length,sizeof(double),&qsortcmp);
  for (count=0;count<length;count++){
    res.ptr[count]=t[count];}
  delete[] t;
  return res;
}
class double_and_int{
  public:
  double t;
  int i;
};
int qsortcmp_di(const void* ti1, const void* ti2){
  double t1=((double_and_int*)ti1)->t;
  double t2=((double_and_int*)ti2)->t;
  return ((t1==t2) ? 0 : (t1>t2 ? 1 : -1));
}
dvector sort(const dvector& d, dvector& indices){
  int length=d.size();
  int first=d.indexmin();
  int count;
  dvector res(length);
  double_and_int* ti = new double_and_int[length];
  for (count=0;count<length;count++){
    ti[count].t=d.ptr[count];
    ti[count].i=count+first;}
  qsort(ti,length,sizeof(double_and_int),&qsortcmp_di);
  for (count=0;count<length;count++){
    res.ptr[count]=ti[count].t;
    indices.ptr[count]=ti[count].i;}
  delete[] ti;
  return res;
}

int chol(const dmatrix& M,dmatrix& L){
  // calculates the choleski decomposition of M and puts the lower triangular part into L
  // returns 1 if successful, 0 else
  // used by fill_mvnorm
  int n = M.rowsize();
  int i, j, k;
#if !defined(_UNCHECKED_)
  if (!(n==M.colsize() && n==L.rowsize() && n==L.colsize())){
    error("Bad size in chol()");}
#endif
  L.initialize();
  for (i=1;i<=n;i++){
    L[i][i]=1;}
  double sum;
  if (M[1][1]<0){
    return(0);
  } else {
    L[1][1] = sqrt(M[1][1]);
    for (i=2;i<=n;i++){
      L[i][1]=M[i][1]/L[1][1];}
    for (i=2;i<n;i++){
      sum = 0;
      for (k=1;k<i;k++){
        sum+=L[i][k]*L[i][k];}
      if (M[i][i] <= sum){
        return(0);
      } else {
        L[i][i] = sqrt(M[i][i]-sum);
        for (j=i+1;j<=n;j++){
          sum = 0;
          for (k=1;k<i;k++){
            sum += L[j][k]*L[i][k];}
          L[j][i] = (M[j][i]-sum)/L[i][i];
        }
      }
    }
    sum = 0;
    for (int k=1;k<n;k++){
      sum+=L[n][k]*L[n][k];}
    if (M[n][n] <= sum)
      return(0);
    L[n][n] = sqrt(M[n][n] - sum);
  }
  return(1);
}

dmatrix::dmatrix(int lbr,int lur,int lbc,int luc){
  rows = lur - lbr + 1;
  cols = luc - lbc + 1;
  firstrow = lbr;
  firstcol = lbc;
  //index = new dvector[rows](firstcol,firstcol+cols-1);
  index.resize(rows,dvector(firstcol,firstcol+cols-1));
}
dmatrix::dmatrix(int lur,int luc){
  rows = lur;
  cols = luc;
  firstrow = 1;
  firstcol = 1;
  //index = new dvector[rows](1,cols);
  index.resize(rows,dvector(1,cols));
}
dmatrix::dmatrix(const dmatrix &dm){
  rows = dm.rows;
  cols = dm.cols;
  firstrow = dm.firstrow;
  firstcol = dm.firstcol;
  //index = new dvector[rows](firstcol,firstcol+cols-1);
  index.resize(rows,dvector(firstcol,firstcol+cols-1));
  for (int i=0; i<rows; i++){
    index[i] = dm.index[i];}
}
dmatrix::dmatrix(char* c, int skip_lines){
// a bit different from the Betadiff version: takes only a single string argument.
// Example usage: dmatrix W("{1 2 3}{4 5 6}{7 8 9}");
// but not dmatrix W("{ 1 2 3}... or dmatrix W("{1 2 3 }...
// ie. watch out for excess spaces.
// note, stray text words in a file come out as 0's
// note again, if skip_lines=x then skip the first few x lines in the file  (header?)
  firstrow = 1;
  firstcol = 1;
  if (c[0]=='{'){ // it's a string
    c++;
    int width=-1,height=1;
    int thisrow=0;
    int count;
    char **u=(char**)malloc(sizeof(char*));
    double **buffer;
    buffer = (double**)malloc(10*sizeof(double*));
    buffer[0] = (double*)malloc(50*sizeof(double));
    int bufcount=0; // the sub-buffer we are about to write to
    int subcount=0; // the position in sub-buffer we are about to write to
    while (c[0]!=0){
      if (c[0]=='{'){
        if (width==-1){
          width=thisrow;
        } else {
          if (thisrow!=width){
            std::cerr << "bad data in mat::mat\n";
            break;
          }
        }
        if (c[1]==0){  // last row
          break;
        }
        c+=1; // pass over the braces
        height++;
        thisrow=0;  // start next row
      }
      else {
        thisrow++;
        if (subcount == 50){
          subcount=0;
          bufcount++;
          if (bufcount%10 == 0){
            buffer=(double**)realloc(buffer,10*bufcount*sizeof(double*));}
          buffer[bufcount]=(double*)malloc(50*sizeof(double));
        }
        buffer[bufcount][subcount]=strtod(c,u);
        c=(*u)+1;
        subcount++;
      }
    }

    rows = height;
    cols = width;
    //index = new dvector[rows](firstcol,firstcol+cols-1);
    index.resize(rows,dvector(firstcol,firstcol+cols-1));


    int count1,count2;
    int ele,row,col;
    ele=0;
    for (count1=0;count1<=bufcount;count1++){
      for (count2=0;count2<50;count2++){
        if (count1==bufcount && count2==subcount) {break;}
        col=ele%cols;
        row=ele/cols;
        ele++;
        (*this)[row+1][col+1]=buffer[count1][count2];
      }
      free(buffer[count1]);
    }
    free(buffer);
  } else { // it's a file
    std::ifstream istr(c);
    if (!istr) {error("bad file in dmatrix::dmatrix(" + std::string(c) + ")");}
    for (int skip=1; skip<=skip_lines; skip++){
      std::string junk;
      getline(istr,junk);
    }
    //istr.unsetf(ios::skipws);//richard
    int width=-1,height=0;
    int thisrow=0;
    char ch;
    int count;
    double **buffer;
    buffer = (double**)malloc(10*sizeof(double*));
    buffer[0] = (double*)malloc(50*sizeof(double));
    int bufcount=0; // the sub-buffer we are about to write to
    int subcount=0; // the position in sub-buffer we are about to write to
    while (1){
      if (subcount == 50){
        subcount=0;
        bufcount++;
        if (bufcount%10 == 0){
          buffer=(double**)realloc(buffer,10*bufcount*sizeof(double*));}
        buffer[bufcount]=(double*)malloc(50*sizeof(double));
      }
      if (!GOOD(istr >> buffer[bufcount][subcount])){break;}
      thisrow++;
      subcount++;
      ch=' ';
      while (ch==' '){
        istr >> ch;
      }
      if (ch=='0' || ch=='1' || ch=='2' || ch=='3' || ch=='4' || ch=='5' || ch=='6' || ch=='7' || ch=='8' || ch=='9' || ch=='-' || ch=='.') istr.putback(ch);
      if (ch=='\n'){
        if (width==-1){
          width=thisrow;}
        else {
          if (thisrow!=width){
            std::cerr << "bad data in mat::mat\n";
            break; }
        }
        height++;
        thisrow=0;  // start next row
      }
    }

    rows = height;
    cols = width;
    //index = new dvector[rows](firstcol,firstcol+cols-1);
    index.resize(rows,dvector(firstcol,firstcol+cols-1));

    int count1,count2;
    int ele,row,col;
    ele=0;
    for (count1=0;count1<=bufcount;count1++){
      for (count2=0;count2<50;count2++){
        if (count1==bufcount && count2==subcount) {break;}
        col=ele%cols;
        row=ele/cols;
        ele++;
        (*this)[row+1][col+1]=buffer[count1][count2];
      }
      free(buffer[count1]);
    }
    free(buffer);
  }
}
dmatrix::dmatrix(){
  rows = 1;
  cols = 1;
  firstrow = 1;
  firstcol = 1;
  //index = new dvector[rows](firstcol,firstcol+cols-1);
  index.resize(rows,dvector(firstcol,firstcol+cols-1));
}
dmatrix& dmatrix::operator=(const dmatrix &dm){
#if !defined(_UNCHECKED_)
  if (dm.rows!=rows || dm.cols!=cols){
    error("bad size in dmatrix::operator=(dmatrix)");}
#endif
  int count;
  for (count=0;count<rows;count++){
    index[count]=dm.index[count];}
  return *this;
}
dmatrix& dmatrix::operator=(double t){
  int count;
  for (count=0;count<rows;count++){
    (*this)[count+firstrow]=t;}
  return *this;
}

void dmatrix::initialize(){
  int count;
  for (count=firstrow;count<=firstrow+rows-1;count++){
    (*this)[count]=0.0;}
}

int dmatrix::rowmin() const{
  return firstrow;
}
int dmatrix::rowmax() const{
  return firstrow+rows-1;
}
int dmatrix::colmin() const{
  return firstcol;
}
int dmatrix::colmax() const{
  return firstcol+cols-1;
}
int dmatrix::rowsize() const{
  return rows;
}
int dmatrix::colsize() const{
  return cols;
}
void dmatrix::colshift(int min){
  firstcol=min;
}
void dmatrix::rowshift(int min){
  firstrow=min;
}

dvector& dmatrix::operator[](int i) {
#if !defined(_UNCHECKED_)
  if (i<firstrow || i>(firstrow+rows-1)){
    error("Subscript out of range: dmatrix[int]");}
#endif
  return index[i-firstrow];
}

const dvector& dmatrix::operator[](int i) const{
//return const_cast<const dvector&>(operator[](i));
#if !defined(_UNCHECKED_)
  if (i<firstrow || i>(firstrow+rows-1)){
    error("Subscript out of range: dmatrix[int]");}
#endif
  return index[i-firstrow];
}

dvector& dmatrix::operator[](double t) {
  int i = (int)t;
#if !defined(_UNCHECKED_)
  if (i<firstrow || i>(firstrow+rows-1)){
    error("Subscript out of range: dmatrix[double]");}
#endif
  return index[i-firstrow];
}

const dvector& dmatrix::operator[](double t) const{
  int i = (int)t;
//return const_cast<const dvector&>(operator[](i));
#if !defined(_UNCHECKED_)
  if (i<firstrow || i>(firstrow+rows-1)){
    error("Subscript out of range: dmatrix[double]");}
#endif
  return index[i-firstrow];
}

void dmatrix::rowfill(int i, const dvector& dv){
#if !defined(_UNCHECKED_)
  if (dv.size() != cols){
    error("bad size in dmatrix::rowfill");}
#endif
  (*this)[i]=dv;
}
void dmatrix::colfill(int j, const dvector& dv){
#if !defined(_UNCHECKED_)
  if (dv.size() != rows){
    error("bad size in dmatrix::colfill");}
#endif
  int count;
  for (count=0;count<dv.size();count++){
    (*this)[count+rowmin()][j]=dv.ptr[count];}
}
void dmatrix::rowfill_seqadd(int i, const double base, const double offset){
  dvector dv(cols);
  dv.fill_seqadd(base,offset);
  (*this).rowfill(i,dv);
}
void dmatrix::colfill_seqadd(int j, const double base, const double offset){
  dvector dv(rows);
  dv.fill_seqadd(base,offset);
  (*this).colfill(j,dv);
}
void dmatrix::rowfill_randu(int i, const long int& s){
  dvector dv(cols);
  dv.fill_randu(s);
  (*this).rowfill(i,dv);
}
void dmatrix::colfill_randu(int j, const long int& s){
  dvector dv(rows);
  dv.fill_randu(s);
  (*this).colfill(j,dv);
}
void dmatrix::rowfill_randn(int i, const long int& s){
  dvector dv(cols);
  dv.fill_randn(s);
  (*this).rowfill(i,dv);
}
void dmatrix::colfill_randn(int j, const long int& s){
  dvector dv(rows);
  dv.fill_randn(s);
  (*this).colfill(j,dv);
}
void dmatrix::rowfill_randbi(int i, const long int& s, double t){
  dvector dv(cols);
  dv.fill_randbi(s,t);
  (*this).rowfill(i,dv);
}
void dmatrix::colfill_randbi(int j, const long int& s, double t){
  dvector dv(rows);
  dv.fill_randbi(s,t);
  (*this).colfill(j,dv);
}
void dmatrix::fill_mvnorm(const long int& s, const dvector& mean, const dmatrix& _covar){
  // Found a problem here for the case where some variances (and hence the corresponding
  // rows and columns of covar) are 0. The cholesky decomposition does not work in these
  // cases. I thought of this kludge: Set the 0 variances to 1, leaving the covariances at 0.
  // When finished, set the corresponding deviations from the mean to 0.
  dmatrix covar(_covar);
  int n = covar.rowsize();
#if !defined(_UNCHECKED_)
  if (!(n == covar.colsize() && n == rowsize() && n == mean.size())){
    error("Bad argument to dmatrix::fill_mvnorm");}
#endif
  // set zero variances to 1 as above
  dvector zero_variances(covar.rowmin(),covar.rowmax());
  zero_variances = 0;
  for (int i=covar.rowmin(); i<=covar.rowmax(); i++){
    if (covar[i][i]==0){
      covar[i][i] = 1;
      zero_variances[i] = 1;
    }
  }
  // generate all the standard normal random numbers at once
  dvector all_rands(1,n*colsize());
  all_rands.fill_randn(s);
  // get lower triangular part of choleski decomposition of covar
  dmatrix L(1,n,1,n);
  if (!chol(covar,L)){
    error("Failed choleski decomposition in fill_mvnorm");}
  dvector dv(1,n);
  for (int j=colmin();j<=colmax();j++){
    for (int i=1;i<=n;i++){
      dv[i]=all_rands[(j-colmin())*n+i];}
    dv = L*dv + mean;
    colfill(j,dv);
  }
  // find the elements with 0 variances and set them to their means
  for (int i=covar.rowmin(); i<=covar.rowmax(); i++){
    if (zero_variances[i]){
      (*this)[i] = mean[i];
    }
  }
}
void dmatrix::fill_mvt(const long int& s, const dvector& mean, const dmatrix& _covar, int df){
  // Result is mean + 1/sqrt(y/df) * x,
  // where x ~ multivariate normal(0,_covar)
  // and y ~ chi-square(df).
  // See fill_mvnorm for treatment of zero variances.
  dvector zeros(mean * 0);
  fill_mvnorm(s,zeros,_covar);
  dvector y(colmin(),colmax());
  y.fill_chisq(s,df);
  for (int j=colmin(); j<=colmax(); j++){
    colfill(j,mean + (1/sqrt(y[j]/df)) * extract_column(*this,j));
  }
}

void dmatrix::operator+=(const dmatrix& dm){
#if !defined(_UNCHECKED_)
  if(rows!=dm.rows || cols!=dm.cols){
    error("bad size in dmatrix::operator+=(dmatrix)");}
#endif
  int count;
  for (count=firstrow;count<=firstrow+rows-1;count++){
    (*this)[count]+=dm[count+dm.firstrow-firstrow];}
}
void dmatrix::operator-=(const dmatrix& dm){
#if !defined(_UNCHECKED_)
  if(rows!=dm.rows || cols!=dm.cols){
    error("bad size in dmatrix::operator-=(dmatrix)");}
#endif
  int count;
  for (count=firstrow;count<=firstrow+rows-1;count++){
    (*this)[count]-=dm[count+dm.firstrow-firstrow];}
}
void dmatrix::operator+=(double t){
  dvector dv(cols);
  dv=t;
  int count;
  for (count=rowmin();count<=rowmax();count++){
    (*this)[count]+=dv;}
}
void dmatrix::operator-=(double t){
  dvector dv(cols);
  dv=t;
  int count;
  for (count=rowmin();count<=rowmax();count++){
    (*this)[count]-=dv;}
}
void dmatrix::operator*=(double t){
  int count;
  for (count=rowmin();count<=rowmax();count++){
    (*this)[count]*=t;}
}
void dmatrix::operator/=(double t){
  int count;
  for (count=rowmin();count<=rowmax();count++){
    (*this)[count]/=t;}
}

dmatrix::~dmatrix(){
//  delete[] index;
}

dmatrix operator+(const dmatrix& m1,const dmatrix& m2){
#if !defined(_UNCHECKED_)
  if (m1.rows != m2.rows || m1.cols != m2.cols){
    error("bad size in operator+(dmatrix,dmatrix)");}
#endif
  dmatrix m(m1);
  m+=m2;
  return m;
}
dmatrix operator-(const dmatrix& m1,const dmatrix& m2){
#if !defined(_UNCHECKED_)
  if (m1.rows != m2.rows || m1.cols != m2.cols){
    error("bad size in operator-(dmatrix,dmatrix)");}
#endif
  dmatrix m(m1);
  m-=m2;
  return m;
}
dmatrix operator+(const dmatrix& dm,double t){
  dmatrix m(dm);
  m+=t;
  return m;
}
dmatrix operator+(double t,const dmatrix& dm){
  dmatrix m(dm);
  m+=t;
  return m;
}
dmatrix operator-(const dmatrix& dm,double t){
  dmatrix m(dm);
  m-=t;
  return m;
}
dmatrix operator-(double t,const dmatrix& dm){
  dmatrix m(dm);
  m=-m;
  m+=t;
  return m;
}
dmatrix operator*(const dmatrix& dm,double t){
  dmatrix m(dm);
  m*=t;
  return m;
}
dmatrix operator*(double t,const dmatrix& dm){
  dmatrix m(dm);
  m*=t;
  return m;
}
dmatrix operator-(const dmatrix& dm){
  dmatrix m(dm);
  m*=-1;
  return m;
}
dmatrix elem_div(const dmatrix& m1,const dmatrix& m2){
#if !defined(_UNCHECKED_)
  if (m1.rows != m2.rows || m1.cols != m2.cols){
    error("bad size in elem_div(dmatrix,dmatrix)");}
#endif
  dmatrix m=m1;
  int count;
  int rm=m.rowmin(),rm1=m1.rowmin(),rm2=m2.rowmin();
  for (count=0;count<m.rows;count++){
    m[count+rm]=elem_div(m1[count+rm1],m2[count+rm2]);}
  return m;
}
dmatrix elem_prod(const dmatrix& m1,const dmatrix& m2){
#if !defined(_UNCHECKED_)
  if (m1.rows != m2.rows || m1.cols != m2.cols){
    error("bad size in elem_prod(dmatrix,dmatrix)");}
#endif
  dmatrix m=m1;
  int count;
  int rm=m.rowmin(),rm1=m1.rowmin(),rm2=m2.rowmin();
  for (count=0;count<m.rows;count++){
    m[count+rm]=elem_prod(m1[count+rm1],m2[count+rm2]);}
  return m;
}
dmatrix operator*(const dmatrix& m1,const dmatrix& m2){
  int count,count2;
  int m1r=m1.rows,m2c=m2.cols,m1c=m1.cols,m2r=m2.rows;
  int m1rm=m1.rowmin(),m2cm=m2.colmin();
#if !defined(_UNCHECKED_)
  if (m1c != m2r){
    error("bad size in operator*(dmatrix,dmatrix)");}
#endif
  dmatrix m3(1,m1r,1,m2c);
  dvector this_col(m2r);
  for (count2=0;count2<m2c;count2++){
    this_col = extract_column(m2,count2+m2cm);
    for (count=0;count<m1r;count++){
      m3[count+1][count2+1] = m1[count+m1rm]*this_col;}}  // easier to extract row than col
  return m3;
}
dmatrix operator*(const dvector& v1,const dmatrix& m2){
  int count;
  int v1s=v1.size(),m2c=m2.cols,m2r=m2.rows;
  int v1m=v1.first,m2cm=m2.colmin();
#if !defined(_UNCHECKED_)
  if (v1s != m2r){
    error("bad size in operator*(dvector,dmatrix)");}
#endif
  dmatrix m3(1,1,1,m2c);
  dvector this_col(m2r);
  for (count=0;count<m2c;count++){
    this_col = extract_column(m2,count+m2cm);
    m3[1][count+1] = v1*this_col;}
  return m3;
}
dvector operator*(const dmatrix& m1,const dvector& v2){
  int count;
  int m1r=m1.rows,m1c=m1.cols,v2s=v2.size();
  int m1rm=m1.rowmin(),v2m=v2.first;
#if !defined(_UNCHECKED_)
  if (m1c != v2s){
    error("bad size in operator*(dmatrix,dvector)");}
#endif
  dvector v3(1,m1r);
  for (count=0;count<m1r;count++){
    v3[count+1] = m1[count+m1rm]*v2;}  // easier to extract row than col
  return v3;
}

dmatrix column_vector(dvector& dv){
  dmatrix m(dv.size(),1);
  int count;
  for (count=dv.indexmin();count<=dv.indexmax();count++){
    m[count-dv.indexmin()+1][1]=dv[count];}
  return m;
}
dmatrix row_vector(dvector& dv){
  dmatrix m(1,dv.size());
  int count;
  for (count=dv.indexmin();count<=dv.indexmax();count++){
    m[1][count-dv.indexmin()+1]=dv[count];}
  return m;
}
dvector extract_column(const dmatrix& dm, const int j){
  dvector d(dm.rowmin(),dm.rowmax());
  int count;
  for (count=dm.rowmin();count<=dm.rowmax();count++){
    d[count]=dm[count][j];}
  return d;
}
dvector extract_diagonal(const dmatrix& dm){
#if !defined(_UNCHECKED_)
  if(dm.rows!=dm.cols){
    error("extract_diagonal(dmatrix): matrix not square!");}
#endif
  dvector d(dm.rows);
  int count;
  for (count=0;count<dm.rows;count++){
    d[count+1]=dm[count+dm.rowmin()][count+dm.colmin()];}
  return d;
}
dmatrix sqrt(const dmatrix& dm){
  dmatrix r(dm);
  for (int i=r.rowmin(); i<=r.rowmax(); i++){
    r[i] = sqrt(dm[i]);
  }
  return r;
}
dmatrix log(const dmatrix& dm){
  dmatrix r(dm);
  for (int i=r.rowmin(); i<=r.rowmax(); i++){
    r[i] = log(dm[i]);
  }
  return r;
}
double norm(dmatrix dm){
  double sum=0;
  int count;
  for (count=dm.rowmin();count<=dm.rowmax();count++){
    sum += norm2(dm[count]);}
  return sqrt(sum);
}
double norm2(dmatrix dm){
  double sum=0;
  int count;
  for (count=dm.rowmin();count<=dm.rowmax();count++){
    sum += norm2(dm[count]);}
  return sum;
}
dvector rowsum(dmatrix dm){
  dvector rsum(dm.rowmin(),dm.rowmax());
  int count;
  for (count=dm.rowmin();count<=dm.rowmax();count++){
    rsum[count] = sum(dm[count]);}
  return rsum;
}
dvector colsum(dmatrix dm){
  dvector csum(dm.colmin(),dm.colmax());
  int count;
  for (count=dm.colmin();count<=dm.colmax();count++){
    csum[count] = sum(extract_column(dm,count));}
  return csum;
}
dmatrix identity_matrix(int i,int j){
  dmatrix d(i,j,i,j);
  d.initialize();
  int count;
  for (count=i;count<=j;count++){
    d[count][count]++;}
  return d;
}
dmatrix inverse(const dmatrix& dm){
  // Invert the square matrix argument, which should have rows and columns starting from 1.
  // See also the differentiable version.
  //
  // This was written for inverting a Hessian to get an approximate covariance matrix.
  // Sometimes the Hessian can have a zero row and corresponding zero column, when a
  // parameter has no effect on the objective function (perhaps because it is not allowed
  // to vary). In this case the matrix is less than full rank and not invertible.
  // We kludge this by temporarily setting the relevant diagonal element to 1, going ahead
  // and doing the inverse, and setting the corresponding row and column of the result to 0.
  // The result is not strictly an inverse - the product A*inv(A) will have zeros
  // in the corresponding diagonal elements rather than ones - but will do for the purpose at hand.
  //
  // The algorithm used for the inverse is Gauss-Jordan, from Numerical Recipes in C, section 2.1.
  //  We could do better, see other algorithms in the same reference.
  dmatrix a(dm);
  int n = a.rowsize();
  dmatrix b(identity_matrix(1,n));
#if !defined(_UNCHECKED_)
  if (a.rowmin() != 1 || a.colmin() != 1 || a.rowsize() != a.colsize()){
    std::cerr << a << '\n'; error("Bad matrix in inverse");
  }
#endif
  // go through, find zero rows (with corresponding zero columns)
  // set the corresponding diagonal elements equal to 1 -
  // we will zero the corresponding rows and columns of the inverse at the end.
  dvector zero_rows(1,n);
  zero_rows = 0;
  for (int i=1; i<=n; i++){
    if (max(fabs(a[i]))==0.0 && max(fabs(extract_column(a,i)))==0.0){
      zero_rows[i] = 1;
      a[i][i] = 1;
    }
  }
  // invert a (store the result in a)
  // this is equivalent to solving a * a_inverse = b.
  dvector indxc(1,n);
  dvector indxr(1,n);
  dvector ipiv(1,n);
  ipiv = 0;
  double big, dum, pivinv, temp;
  int icol=0, irow=0;
  for (int i=1; i<=n; i++){
    big = 0;
    for (int j=1; j<=n; j++){
      if (ipiv[j] != 1){
        for (int k=1; k<=n; k++){
          if (ipiv[k] == 0){
            if (fabs(a[j][k]) >= big){
              big = fabs(a[j][k]);
              irow = j;
              icol = k;
            }
          } else {
            if (ipiv[k] > 1){
              error("Singular matrix in inverse");
            }
          }
        }
      }
    }
    ipiv[icol]++;
    if (irow != icol){
      for (int l=1; l<=n; l++){
        temp = a[irow][l];
        a[irow][l] = a[icol][l];
        a[icol][l] = temp;
        temp = b[irow][l];
        a[irow][l] = b[icol][l];
        b[icol][l] = temp;
      }
    }
    indxr[i] = irow;
    indxc[i] = icol;
    if (a[icol][icol]==0){
      error("Singular matrix in inverse");
    }
    pivinv = 1.0/a[icol][icol];
    a[icol][icol] = 1;
    for (int l=1; l<=n; l++){
      a[icol][l] *= pivinv;
      b[icol][l] *= pivinv;
    }
    for (int ll=1; ll<=n; ll++){
      if (ll != icol){
        dum = a[ll][icol];
        a[ll][icol] = 0;
        for (int l=1; l<=n; l++){
          a[ll][l] -= a[icol][l] * dum;
          b[ll][l] -= b[icol][l] * dum;
        }
      }
    }
  }
  for (int l=n; l>=1; l--){
    if (indxr[l] != indxc[l]){
      for (int k=1; k<=n; k++){
        temp = a[k][indxr[l]];
        a[k][indxr[l]] = a[k][indxc[l]];
        a[k][indxc[l]] = temp;
      }
    }
  }
  // zero the rows and columns corresponding to zero rows and columns in the original matrix.
  dvector zeros(1,n);
  zeros = 0;
  for (int i=1; i<=n; i++){
    if (zero_rows[i]){
      a[i] = zeros;
      a.colfill(i,zeros);
    }
  }
  return a;
}

std::ostream& operator<<(std::ostream& ostr, const dmatrix& dm){
  int count;
  for (count=dm.rowmin();count<=dm.rowmax();count++){
    ostr << dm[count];
    if (count<dm.rowmax()){
      ostr << "\n";}}
  return ostr;
}
std::istream& operator>>(std::istream& istr, dmatrix& dm){
  int count,count2;
  double t;
  for (count=dm.rowmin();count<=dm.rowmax();count++){
    for (count2=dm.colmin();count2<=dm.colmax();count2++){
      istr >> t;
      if (istr) dm[count][count2]=t;
      else {
        error("Error in input file - invalid number of values found.");
      }
    }
  }
  return istr;
}

 dmatrix sort(const dmatrix& d,int sorton){
  dvector sortcol=extract_column(d,sorton);
  dvector indices(1,d.rowsize());
  sort(sortcol,indices);  // returns nothing but changes indices
  dmatrix res(d.rowsize(),d.colsize());
  int count;
  for (count=1;count<=d.rowsize();count++){
    res[count]=d[(int)indices[count]];}
  return res;
}

void RNG_reset(long seed){
  // go back to the start of the random number sequence generated from 'seed'
  // You should only need to use this when you want to generate the same
  //  sequence of random numbers multiple times within one program.
  double newseed=double(abs(seed+1))/RNG_LONGMAX;
  newseed=newseed-floor(newseed);
  Random::Set(newseed);
  betadiff_seed = newseed;
}


