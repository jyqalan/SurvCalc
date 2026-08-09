`SurvCalc.make.SELECTIONS.list` <-
function(lines)
{
  debracket <- function(s){
      substr(s,2,nchar(s)-1)
  }
  result <- list()
  TEMP <- SurvCalc.get.lines(lines,clip.to="SELECTED STRATA (EFFECTIVE AREAS IN PARENTHESES):")
  TEMP <- SurvCalc.get.lines(TEMP,clip.from="SELECTED STATIONS (STRATUM CODES IN PARENTHESES):")
  data <- data.frame()
  for(i in 1:length(TEMP)){
      words <- SurvCalc.string.to.vector.of.words(TEMP[i])
      for(j in 1:(length(words)/2)){
          data <- rbind(data,data.frame(rbind(c(words[2*j-1],debracket(words[2*j]))),stringsAsFactors=F))
      }
  }
  colnames(data) <- c("Stratum","Area")
  data[[2]] <- as.numeric(data[[2]])
  result[["SELECTED STRATA"]] <- data

  TEMP <- SurvCalc.get.lines(lines,clip.to="SELECTED STATIONS (STRATUM CODES IN PARENTHESES):")
  data <- data.frame()
  for(i in 1:length(TEMP)){
      words <- SurvCalc.string.to.vector.of.words(TEMP[i])
      for(j in 1:(length(words)/2)){
          data <- rbind(data,data.frame(rbind(c(words[2*j-1],debracket(words[2*j]))),stringsAsFactors=F))
      }
  }
  colnames(data) <- c("Station","Stratum")
  result[["SELECTED STATIONS"]] <- data

  result

}

