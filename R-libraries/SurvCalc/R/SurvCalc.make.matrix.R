`SurvCalc.make.matrix` <-
function(lines)
{
  columns <- SurvCalc.string.to.vector.of.words(lines[1])
  if(length(lines) < 2) return(NA)
  data <- matrix(0, length(lines) - 1, length(columns))
  for(i in 2:length(lines)) {
    data[i - 1,  ] <- SurvCalc.string.to.vector.of.numbers(lines[i])
  }
  colnames(data) <- columns
  data
}

