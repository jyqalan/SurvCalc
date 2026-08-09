`SurvCalc.pos.match` <-
function(vector, regexp)
{
  min((1:length(vector))[SurvCalc.regexpr(regexp, vector) > 0])
}

