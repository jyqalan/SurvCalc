# Get SPM Version
VERSION<-system("..//SurvCalc -h",intern=TRUE)[4]
VERSION<-substring(VERSION,1,regexpr(" ",VERSION)-1)
version.number<-substring(VERSION,2,regexpr("-",VERSION)-1)
version.date<-substring(VERSION,regexpr("-",VERSION)+1)

# Build DESCRIPTION file
filename<-"SurvCalc/DESCRIPTION"
cat("Package: SurvCalc\nTitle: Read SurvCalc output file into R\nVersion: ",file=filename)
cat(version.number,file=filename,append=TRUE)
cat("\nDate: ",file=filename,append=TRUE)
cat(version.date,file=filename,append=TRUE)
cat("\n",file=filename,append=TRUE)
cat("Author: Dan Fu\n",file=filename,append=TRUE)
cat("Description: A set of R functions for extracting an SurvCalc output file into R.\n",file=filename,append=TRUE)
cat("Maintainer: Dan Fu <d.fu@niwa.co.nz>\n",file=filename,append=TRUE)
cat("License: NIWA. See the SurvCalc User Manual for license details.\n",file=filename,append=TRUE)
cat("URL: http://www.niwa.co.nz\n",file=filename,append=TRUE)
cat("Copyright: National Institute of Water & Atmospheric Research (NIWA).\n",file=filename,append=TRUE)
cat("LazyLoad: yes\n",file=filename,append=TRUE)

# Create R function to return version number
filename<-"SurvCalc/R/SurvCalc.binary.version.R"
cat("\"SurvCalc.binary.version\"<-\n",file=filename)
cat("function() {\n",file=filename,append=T)
cat(paste("return(\"",VERSION,"\")\n",sep=""),file=filename,append=T)
cat("}\n",file=filename,append=T)

# Write a polar.html file to report version number for the Wiki
cat(paste("Version",VERSION),file="SurvCalc.html")

# Exit
q()

