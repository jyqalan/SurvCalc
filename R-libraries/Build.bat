R --vanilla < make_version.R
R CMD INSTALL SurvCalc --build
copy SurvCalc_* \\niwa.local\groups\Wellington\NIWAFisheries\R\ /Y
copy SurvCalc.html \\niwa.local\groups\Wellington\NIWAFisheries\R\ /Y
del SurvCalc.html
