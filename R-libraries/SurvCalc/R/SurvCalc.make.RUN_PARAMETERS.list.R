`SurvCalc.make.RUN_PARAMETERS.list` <-
function(lines)
{
  result <- list()
  headers <- SurvCalc.get.lines(lines,contains=":")
  if(length(headers) > 0){
      for(i in 1:length(headers)){
          label <- substr(headers[i],1,nchar(headers[i])-1) # de como
          contents <- SurvCalc.get.lines(lines,clip.to=headers[i])
          if(i < length(headers)){
              contents <-  SurvCalc.get.lines(contents,clip.from=headers[i+1])
          }
          result[[label]] <- contents
      }
  }
  result
}

