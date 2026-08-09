`SurvCalc.regexp.in` <-
function(vector, regexp)
{
  if(length(vector) == 0)
    return(F)
  any(SurvCalc.regexpr(regexp, vector) > 0)
}

