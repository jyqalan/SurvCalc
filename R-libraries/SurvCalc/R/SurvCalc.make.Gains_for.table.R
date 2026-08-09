`SurvCalc.make.Gains_for.table` <-
function(lines)
{
  data <- SurvCalc.make.data.frame(lines)
  data[,1] <- as.character(data[,1])
  for(i in 2:ncol(data))
      data[[i]] <- as.numeric(data[[i]])
  data
}

