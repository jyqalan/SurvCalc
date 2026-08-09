`SurvCalc.string.to.vector.of.words` <-
function(string)
{
  temp <- SurvCalc.unpaste(string, sep = " ")
  return(temp[temp != ""])
}

