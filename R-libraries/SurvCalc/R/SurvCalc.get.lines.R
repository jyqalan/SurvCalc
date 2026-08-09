`SurvCalc.get.lines` <-
function(lines, from = -1, to = -1, contains = "", starts.with = "", clip.to = "", clip.from = "", clip.to.match = "", clip.from.match = "", ...)
{
  result <- lines
  if(from > 0) {
    result <- result[(1:length(result)) >= from]
  }
  if(to > 0) {
    result <- result[(1:length(result)) <= to]
  }
  if(clip.to != "") {
    if(any(result==clip.to)){
      result <- result[(SurvCalc.pos(result, clip.to) + 1):length(result)]
    }
  }
  if(clip.from != "") {
    if(any(result==clip.from)){
      result <- result[1:(SurvCalc.pos(result, clip.from) - 1)]
    }
  }
  if(clip.to.match != "") {
    if(SurvCalc.regexp.in(result, clip.to.match)) {
      result <- result[(SurvCalc.pos.match(result, clip.to.match) + 1):length(result)]
    }
  }
  if(clip.from.match != "") {
    if(SurvCalc.regexp.in(result, clip.from.match)) {
      result <- result[1:(SurvCalc.pos.match(result, clip.from.match) - 1)]
    }
  }
  if(contains != "") {
    result <- result[SurvCalc.regexpr(contains, result) > 0]
  }
  if(starts.with != "") {
    result <- result[SurvCalc.regexpr(paste("^", starts.with, sep = ""), result) > 0]
  }
  return(result)
}

