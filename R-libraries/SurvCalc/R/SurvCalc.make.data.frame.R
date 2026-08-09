`SurvCalc.make.data.frame` <-
function(lines)
{
  columns <- SurvCalc.string.to.vector.of.words(lines[1])
  data <- data.frame(matrix(nrow=length(lines)-1,ncol=length(columns)))
  for(i in 2:length(lines)) {
    data[i-1,] <- SurvCalc.string.to.vector.of.words(lines[i])
  }
  colnames(data) <- columns
  data
}

