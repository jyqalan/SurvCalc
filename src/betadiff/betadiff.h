// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";

#if !defined(BETADIFF)
#define BETADIFF
// static const char* betadiff_header_id = "$Id: betadiff.h 2237 2008-09-08 00:33:48Z fud $\n";
// extern const char* betadiff_id;

/* Betadiff: Brian Bull 15/5/02.

This program is an attempt to emulate early 90's versions of AUTODIF (pre arrays of dimension >2)
but with greater ease of use - in particular without having to do 'gradient_structure' and
'return_arrays' calls, and with a more convenient interface to the minimiser.

The automatic differentiation part of Betadiff is based on a program called ADOL-C v1.8.4,
subtitled "A package for automatic differentiation of algorithms written in C/C++",
which is made by a team including Andreas Griewank (Technical University of Dresden,
griewank@math.tu-dresden.de).
However I have had to heavily modify ADOL-C as well as adding an AUTODIF-like interface to it.

The other externally sourced component is the 'newran' random number generation package,
which is freeware written by a guy called Robert Davies (see http://webnz.co.nz/robert).

The minimiser is closely based on the main algorithm of Dennis & Schnabel, Numerical Methods for
Unconstrained Optimisation and Nonlinear Equations (1996). I have implemented a new, object-oriented
interface, which makes it somewhat simpler to use - see optimise(), also optimise_finite_differences()
which uses finite difference gradients instead of automatic generation.

User information follows, and should be used as an addition to
an early AUTODIF manual. Note that there is not enough information in
this documentation to diagnose or correct an ADOL-C error. Contact Brian Bull if you find one.

Make betadiff into a library file using the makefile provided, which should compile all the components.
Use either 'make libbd.a' or preferably 'make test' which makes libbd.a, compiles a small test program
and diff's the results against the 'correct' output.
Use betadiff by linking -lbd -lad -lm,
and #include "betadiff.h" (instead of the AUTODIF #include <fvar.hpp>


Differences from AUTODIF:

-There is a new handler to the minimiser - see optimise() at the bottom of the file.
  Now you only need to supply the model, objective function, start values, bounds,
  and control values, and the handler does all the rest. You no longer need
  to extract the gradient and pass it to the fmc minimiser. If you do want to extract
  the gradient vector for some other purpose, have a look at the contents of the
  optimise() class and copy the calls there.

-The minimisation algorithm is different, though still of the same general type.

-gradient_structure and RETURN_ARRAYS calls are unnecessary and ignored.

-My boundp and boundpin are as documented for AUTODIF, except with the error corrected
 (the lower quadratic term now starts at -.9999 instead of 0.00001), a slightly more
 accurate value of pi, and a bigger penalty on values outside [-1, 1].
 If you find moderate numerical differences between results using AUTODIF and results
 using the replacement, this could well be a reason.

-The RNGs use different algorithms. The only difference in usage is that if you make
 two successive seed calls to the same seed, the second is ignored. So if you set the
 seed to a value v, get a sequence S, and then set the seed to v again, the sequence S
 _will not_ come out again; it will go on as if the second seed call had not been made.
 Whereas if you set it to v1, get S1, set it to v2, get S2, and then set it
 back to v1, you _will_ get S1 again. (Or if you get S1, then call RNG_reset(v1),
 you will get S1 again)

-We now have a multivariate normal RNG, dmatrix::fill_mvnorm and dvector::fill_mvnorm,
 and a multivariate t RNG, dmatrix::fill_mvt and dvector::fill_mvt,
 and a chi-square RNG, dvector::fill_chisq,
 and multinomial RNGs, dvector::fill_multinomial and dvv:fill_multinomial.

-The product of two matrices is now a matrix whose row and column indices start at 1,
 not 0 as in AUTODIF.
 The product of a matrix and a vector is now a vector whose indices start at 1,
 not a matrix as in AUTODIF.
 The product of a vector and a matrix is now a matrix with one row whose indices start at 1,
 not 0 as in AUTODIF.

-The ivector and imatrix classes are now based on doubles, not ints. They use double
 arithmetic! I think this is probably not a problem. Occasionally you may need to
 add an (int) cast to prevent a compile-time error.

-dvector[dvector], dvv[dvector] can't be the left side of an expression.

-The index matrix(int) is no longer supported: use matrix[int]

-A copy constructor is not a shallow copy, for user classes.
 In AUTODIF, dvector v=w; v=v*2; has the effect of doubling w. Not any more:
 they have different memory locations.

-There is some vector/matrix range checking provided. Like AUTODIF, I also implement a
 non-range-checked version - see libbd_unchecked.a. Only use this for code which has
 been tested with libbd.a!!

-Ragged arrays have not been implemented.

-Avoid excess spaces when inputting character strings (ie. not dvv a("{ 1 , 2 }"))

-When inputting character strings into vectors, I have added an optional integer argument
 which is the first index of the result (default 1).

-The constructor dvar_matrix(char*) now works slightly differently. If the argument is
 some data (not a filename), it now takes only a single string argument of the form:
 dvm W("{1 2 3}{4 5 6}{7 8 9}");
 Alternatively the argument can be the name of a datafile. Do not put any excess spaces or words
 inside the file. testfile.mat is a valid example (comes with betadiff).

-On Kupe, cout does not flush until cin is called. So use cout.flush() or use cerr instead
 or set the cout stream to flush straightaway, if that is what you want.


Known bugs:

-Betadiff can run out of memory. If this happens, it writes out the contents of memory to several
 large files prefixed _adol_ in the current working directory. This is a bad thing because it
 is very slow: it also will cause problems if multiple processes are running in the same directory
 because they will each try and use the same filenames.
 However, this problem is avoidable. In my experience, it only happens when you create many active
 variables using 'new' and fail to destroy them using 'delete': a stack within ADOL-C then just keeps
 getting larger and larger. So, make sure to delete anything you create.
 If you want to check whether the ADOL-C stack is getting out of hand, look at the standard error
 for messages like "resizing from 10000 to 11000". These indicate the current size of the stack of
 active variables. The stack should grow a lot at the start of a minimisation, but should stop
 growing after the first 1 or 2 differentiations. If it keeps growing, then you are probably
 failing to delete something.
 See also the ADOLC_storage() function, which prints out a message about the total amount of 'tape'
 storage used, which increases with the number of operations which have been carried out on active
 variables in the current objective function evaluation. When this number gets very high, you are
 in danger of paging to disk, and you need to reduce the number of active-variable operations in
 your objective function.

-NaNs in the gradient vector can indicate a non-differentiable function. This can come about
 either because your function is genuinely, mathematically non-differentiable, or simply as
 the result of a (seemingly unrelated) programming error which confuses ADOL-C, e.g. one time I got
 this problem because I was taking the log of a non-positive number.
 If you really can't get the differentiation to work, you can resort to the finite differences
 version of the minimiser, but this can be a lot slower.

- 22/9/04 Updated memory allocation for betadiff in usrparms.h from 200K to 1000K
*/

// standard libraries
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../newran_02C/newran.h"

#define NOTKUPE
#define GOOD(x) (x).good()

/* With gcc v 3.2 these should be read out of math.h but aren't, not sure why */

#ifdef WIN32
inline double fmax(double a, double b){
        return (a > b) ? a : b;}
inline double fmin(double a, double b){
        return (a < b) ? a : b;}
#endif

// This 'very large number' may need to change on some machines?
#define RNG_LONGMAX 2147483647

// Error handling
void error(const std::string& problem);
#define BETADIFF_FATAL_EXIT 12

// This is a complete list of the classes in Betadiff.
class dmatrix;

class dvector{
  // This is a non-differentiable vector class, like dvector in AUTODIF.

 public:
  // The guts of a dvector is an array of doubles.
  // Users should not manipulate this object except through Betadiff functions:
  // I've left it public to avoid a long list of friends.
  double *ptr; // contains the data

  int first;  // the index of the first element, often 1
  int sz;     // the length of the vector, so indices are first...first+sz-1

  // These are the constructors and assignments:
  dvector(int,int);
  dvector(int);  // lower bound = 1
  dvector(const dvector&);
  dvector(char*);
  dvector(char*,int);
  dvector& operator=(const dvector&);
  dvector& operator=(double);
  dvector& operator=(int i); // set all elements to i
  dvector(); // don't use this explicitly

  void initialize(); // assign a value of 0 to all elements

  // size and indices
  int indexmin() const;
  int indexmax() const;
  int size() const;
  void shift(int); // changes the index range: this is the new first index

  // access an element (to read or change it);
  double& operator[](int) const;
  double& operator[](double) const;

  // access a bunch of elements (read-only)
  dvector operator[](const dvector&) const;

  // put contents into the dvector
  void fill(char*); // same format as the char* constructor
  void fill_seqadd(double base, double offset); // arithmetic sequence
  void fill_randu(const long int&);                       // uniform random numbers
  void fill_randn(const long int&);                       // normal random numbers
  void fill_randbi(const long int&, const double);        // binomial (n=1) random numbers
  void fill_multinomial(const long int&, const dvector&); // multinomial random numbers
  void fill_multinomial_counts(const long int&, const dvector&, int N); // number of multinomial random numbers in each category
  void fill_chisq(const long int&, int);                  // chi-square random numbers
  void fill_mvnorm(const long int&, const dvector&, const dmatrix&);
  void fill_mvt(const long int&, const dvector&, const dmatrix&, int df);

  // Member function operators are listed here.
  void operator+=(const dvector&);
  void operator-=(const dvector&);
  void operator+=(double);
  void operator-=(double);
  void operator/=(double);
  void operator*=(double);
  void operator*=(const dvector&);

  ~dvector();
};

// Non-member operators on dvectors are listed here.
double operator*(const dvector&, const dvector&);  // dot product - as opposed to elem_prod()
dvector operator*(const dvector&, double);
dvector operator*(double, const dvector&);
dvector operator*(const dvector&, double);
dvector operator*(double, const dvector&);
dvector operator/(const dvector&, double);
dvector operator/(const dvector&, double);
dvector operator/(double, const dvector&);
dvector operator/(double, const dvector&);
dvector operator+(const dvector&, const dvector&);
dvector operator+(const dvector&, double);
dvector operator+(double, const dvector&);
dvector operator-(const dvector&, const dvector&);
dvector operator-(const dvector&, double);
dvector operator-(double, const dvector&);
dvector operator-(const dvector&);             // unary -
dvector operator&(const dvector&, const dvector&); // concatenation

// The value() function is just the identity function for dvectors but we include it anyway for
// compatibility with dvv.
dvector value(const dvector&);

// Math functions on dvectors.
dvector pow(const dvector&, const dvector&);
dvector pow(const dvector&, double);  // not double*
dvector pow(double, const dvector&);
dvector sin(const dvector&);
dvector cos(const dvector&);
dvector tan(const dvector&);
dvector asin(const dvector&);
dvector acos(const dvector&);
dvector atan(const dvector&);
dvector sqrt(const dvector&);
dvector exp(const dvector&);
dvector log(const dvector&);
dvector log10(const dvector&);
dvector fabs(const dvector&);
dvector sfabs(const dvector&); // same as fabs
dvector ceil(const dvector&);
dvector floor(const dvector&);
double mean(const dvector&);
double std_dev(const dvector&);
double sum(const dvector&);
double norm(const dvector&);
double norm2(const dvector&);
dvector elem_prod(const dvector&, const dvector&);
dvector elem_prod(const dvector&, const dvector&, const dvector&);
dvector elem_div(const dvector&, const dvector&);
double min(const dvector&);
double max(const dvector&);

// Stream-based I/O of dvectors.
std::ostream& operator<<(std::ostream&, const dvector&);
std::istream& operator>>(std::istream&, dvector&);

// Sorting - only implemented for dvector, not dvv
dvector sort(const dvector&);
dvector sort(const dvector&,dvector&);  // second argument becomes the index table (see AD4-12)


int chol(const dmatrix&,dmatrix&); // used by fill_mvnorm

class dmatrix{
  // This is a non-differentiable matrix class, like dmatrix in AUTODIF.
  // The key component of a dmatrix is an vector of pointers to dvector,
  // like dvm and dvv.
  // Row and column indices can start at any integer.

 public:
  // The guts of a dmatrix is a vector of dvector's.
  // Users should not manipulate this object except through Betadiff functions:
  // I've left it public to avoid a huge list of 'friend's.
  // Ditto 'firstrow', 'firstcol', 'rows', 'cols'.
  std::vector<dvector> index;  // was:  dvector *index;
  int firstrow, firstcol;
  int rows, cols;

  // These are the constructors and assignments:
  dmatrix(int lbr,int lur,int lbc,int luc);
  dmatrix(int lur,int luc); // lower bounds=1;
  dmatrix(const dmatrix&);
  dmatrix(char*, int skip_lines=0);  // slightly different to AUTODIF, see definition
  dmatrix(); // don't use this explicitly
  dmatrix& operator=(const dmatrix&);
  dmatrix& operator=(double);

  void initialize(); // assign a value of 0 to all elements

  // size and indices
  int rowmin() const;
  int rowmax() const;
  int colmin() const;
  int colmax() const;
  int rowsize() const;
  int colsize() const;
  void colshift(int); // changes the index range: this is the new first column
  void rowshift(int); // changes the index range: this is the new first row

  // access an element (to read or change it);
  dvector& operator[](int);
  dvector& operator[](double);
  const dvector& operator[](int) const;
  const dvector& operator[](double) const;

  // put contents into the dmatrix
  // in each case the first argument is the row or column into which to insert
  void rowfill(int, const dvector&);  // a specified vector
  void colfill(int, const dvector&);
  void rowfill_seqadd(int, const double, const double);    // an arithmetic sequence
  void colfill_seqadd(int, const double, const double);
  void rowfill_randu(int, const long int&);                // uniform random numbers
  void colfill_randu(int, const long int&);
  void rowfill_randn(int, const long int&);                // normal random numbers
  void colfill_randn(int, const long int&);
  void rowfill_randbi(int, const long int&, const double); // binomial (n=1) random numbers
  void colfill_randbi(int, const long int&, const double);
  void fill_mvnorm(const long int&, const dvector&, const dmatrix&);
  void fill_mvt(const long int&, const dvector&, const dmatrix&, int df);

  // Member function operators are listed here.
  void operator+=(const dmatrix&);
  void operator-=(const dmatrix&);
  void operator+=(double);
  void operator-=(double);
  void operator*=(double);
  void operator/=(double);

  ~dmatrix();
};

// Non-member operators on dmatrix's, and dmatrix's with dvectors, are listed here.
dmatrix operator*(const dmatrix&,const dmatrix&); // matrix products - as distinct from elem_prod()
dvector operator*(const dmatrix&,const dvector&); // note result is dvector, not dmatrix
dmatrix operator*(const dvector&,const dmatrix&);
dmatrix operator+(const dmatrix&,const dmatrix&);
dmatrix operator-(const dmatrix&,const dmatrix&);
dmatrix operator+(double,const dmatrix&);
dmatrix operator+(const dmatrix&,double);
dmatrix operator-(double,const dmatrix&);
dmatrix operator-(const dmatrix&,double);
dmatrix operator*(double,const dmatrix&);
dmatrix operator*(const dmatrix&,double);
dmatrix operator*(double,const dmatrix&);
dmatrix operator*(const dmatrix&,double);
dmatrix operator-(const dmatrix&); // unary -

// Math functions on dmatrix's.
dmatrix sqrt(const dmatrix&);
dmatrix log(const dmatrix&);
dmatrix elem_div(const dmatrix&, const dmatrix&);
dmatrix elem_prod(const dmatrix&, const dmatrix&);
dvector rowsum(dmatrix);
dvector colsum(dmatrix);
dmatrix column_vector(dvector&); // make a dmatrix with 1 column
dmatrix row_vector(dvector&);    // make a dmatrix with 1 row
dvector extract_column(const dmatrix&, const int);
dvector extract_diagonal(const dmatrix&);
dmatrix identity_matrix(int,int);
dmatrix inverse(const dmatrix&);

// Stream-based I/O of dmatrix's.
std::ostream& operator<<(std::ostream&, const dmatrix&);
std::istream& operator>>(std::istream&, dmatrix&);

// Sorting - only implemented for dmatrix, not dvm
dmatrix sort(const dmatrix&, int); // sort the rows based on the (int)th comment


#endif

