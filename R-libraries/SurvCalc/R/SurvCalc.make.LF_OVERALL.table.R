`SurvCalc.make.LF_OVERALL.table` <-
function(lines)
{
  data <- SurvCalc.make.data.frame(lines)
  for(i in 1:ncol(data))
      data[[i]] <- as.numeric(data[[i]])
  data    
 
}

