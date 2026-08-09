`extract.header` <-
function(file,path)
{
  header <- list()
  if(missing(path)) path<-""
  filename<-SurvCalc.make.filename(path=path,file=file)
  file <- SurvCalc.convert.to.lines(filename)
  header$call <- SurvCalc.remove.first.words(SurvCalc.get.lines(file,starts.with = "Call:"), 1)
  header$date <- SurvCalc.remove.first.words(SurvCalc.get.lines(file,starts.with = "Date:"), 1)
  header$version <- SurvCalc.get.lines(file, starts.with = "v")
  header$user <- SurvCalc.remove.first.words(SurvCalc.get.lines(file,starts.with = "User name:"), 2)
  header$machine <- SurvCalc.remove.first.words(SurvCalc.get.lines(file,starts.with = "Machine name:"), 2)
  return(header)
}

