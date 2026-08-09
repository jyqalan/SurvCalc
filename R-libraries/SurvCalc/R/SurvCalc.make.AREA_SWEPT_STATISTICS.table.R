`SurvCalc.make.AREA_SWEPT_STATISTICS.table` <-
function(lines)
{
  columns <- SurvCalc.string.to.vector.of.words(lines[1])
  if(length(lines) < 2) return(NA)
  data <- data.frame(stringsAsFactors=F)
  for(i in 2:length(lines)) {
    data <- rbind(data,data.frame(rbind(SurvCalc.string.to.vector.of.words(lines[i])[-1]),stringsAsFactors=F))
    rownames(data)[i-1] <- SurvCalc.string.to.vector.of.words(lines[i])[1]
  }
  data <- data.frame(data)
  colnames(data) <- columns
  data    
}

