// const char* time_stamp = "$Date: 2008/08/14 04:53:06 $\n";
// const char* sample_id = "$Id: sample.h,v 1.1 2008/08/14 04:53:06 fud Exp $\n";

#if !defined(SAMPLE)
#define SAMPLE

//############################## LengthTable ##############################

//////////////////////////////////////////////////////////////////////////////////
class Sample{
public:
  static void initialise(long int RNG_seed);
  static vector<double> get_sample(vector<double> numbers);
  static void get_sample(vector<vector<double> >& n);
  static vector<unsigned> get_sample(unsigned number);

  Sample();
  ~Sample();
private:
  static Uniform U;
};

//############################## END OF Sample.h ##############################
#endif
