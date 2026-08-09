`SurvCalc.remove.first.words` <-
function(string, words = 1)
{
  paste(SurvCalc.unpaste(string, sep = " ")[ - (1:words)], collapse = " ")
}

