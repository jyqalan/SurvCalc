`input.SurvCalc` <-
function (file, path = "") 
{
  result <- list()
  if ((!missing(path) | path != "") & (substring(path, nchar(path) - 
                                                 1) != "\\" || substring(path, nchar(path) - 1) != "/")) 
    path <- paste(path, "\\", sep = "")
  filename <- SurvCalc.make.filename(path = path, file = file)
  file <- SurvCalc.convert.to.lines(filename)
  if (all(SurvCalc.regexpr("calculations output starts from here", 
                           file) < 0)) 
    stop(paste("No output data found in file '", filename, 
               "'", sep = ""))
  file <- SurvCalc.get.lines(file, clip.to.match = "calculations output starts from here")
  file <- SurvCalc.get.lines(file, clip.from.match = "calculations output finishes from here")
  file <- file[SurvCalc.regexpr(paste(rep("=", 80), sep = "", 
                                      collapse = ""), file) < 0]
  file <- file[SurvCalc.regexpr(paste(rep("=", 40), sep = "", 
                                      collapse = ""), file) < 0]
  trip.headers <- SurvCalc.get.lines(file, contains = "Analysis for trip")
  for (i in 1:length(trip.headers)) {
    this.trip <- SurvCalc.remove.first.words(trip.headers[i], 
                                             3)
    result[[this.trip]] <- list()
    trip.lines <- SurvCalc.get.lines(file, clip.to = paste("Analysis for trip", 
                                             this.trip, sep = " "))
    if (i < length(trip.headers)) {
      next.trip <- SurvCalc.remove.first.words(trip.headers[i + 
                                                            1], 3)
      trip.lines <- SurvCalc.get.lines(trip.lines, clip.from = paste("Analysis for trip", 
                                                     next.trip, sep = " "))
    }
    selection.lines <- SurvCalc.get.lines(trip.lines, clip.to = "* SELECTIONS")
    selection.lines <- SurvCalc.get.lines(selection.lines, 
                                          clip.from = "* RUN PARAMETERS")
    result[[this.trip]][["SELECTIONS"]] <- SurvCalc.make.SELECTIONS.list(selection.lines)
    run.parameters.lines <- SurvCalc.get.lines(trip.lines, 
                                               clip.to = "* RUN PARAMETERS")
    run.parameters.lines <- SurvCalc.get.lines(run.parameters.lines, 
                                               clip.from.match = "Estimates for species")
    result[[this.trip]][["RUN PARAMETERS"]] <- SurvCalc.make.RUN_PARAMETERS.list(run.parameters.lines)
    species.headers <- SurvCalc.get.lines(trip.lines, contains = "Estimates for species")
    for (j in 1:length(species.headers)) {
      sp <- list()
      this.sp <- SurvCalc.remove.last.words(SurvCalc.remove.first.words(species.headers[j], 
                                                                        3), 3)
      sp.lines <- SurvCalc.get.lines(trip.lines, clip.to = paste("Estimates for species", 
                                                   this.sp, "in trip", this.trip, sep = " "))
      if (j < length(species.headers)) {
        next.sp <- SurvCalc.remove.last.words(SurvCalc.remove.first.words(species.headers[j + 
                                                                                          1], 3), 3)
        sp.lines <- SurvCalc.get.lines(sp.lines, clip.from = paste("Estimates for species", 
                                                   next.sp, "in trip", this.trip, sep = " "))
      }
      items.header <- SurvCalc.get.lines(sp.lines, contains = "*")
      for (k in 1:length(items.header)) {
        this.item <- SurvCalc.remove.first.words(items.header[k], 
                                                 1)
        item.lines <- SurvCalc.get.lines(sp.lines, clip.to = paste("*", 
                                                     this.item, sep = " "))
        if (k < length(items.header)) {
          next.item <- SurvCalc.remove.first.words(items.header[k + 
                                                                1], 1)
          item.lines <- SurvCalc.get.lines(item.lines, 
                                           clip.from = paste("*", next.item, sep = " "))
        }
        if (this.item == "DIAGNOSTICS") {
          sp[[this.item]] <- SurvCalc.make.DIAGNOSTICS.list(item.lines)
        }
        else if (this.item == "AREA SWEPT STATISTICS") {
          sp[[this.item]] <- SurvCalc.make.AREA_SWEPT_STATISTICS.table(item.lines)
        }
        else if (this.item == "STRATUM CORRECTION FACTORS") {
          sp[[this.item]] <- SurvCalc.make.STRATUM_CORRECTION_FACTORS.table(item.lines)
        }
        else if (this.item == "STRATUM SUMMARY") {
          sp[[this.item]] <- SurvCalc.make.STRATUM_SUMMARY.table(item.lines)
        }
        else if (this.item == "BIOMASS") {
          sp[[this.item]] <- SurvCalc.make.BIOMASS.table(item.lines)
        }
        else if (this.item == "SUB BIOMASS BY STRATUM") {
          sp[[this.item]] <- SurvCalc.make.SUB_BIOMASS_BY_STRATUM.table(item.lines)
        }
        else if (this.item == "LFS BY STATION") {
          sp[[this.item]] <- SurvCalc.make.LFS_BY_STATION.table(item.lines)
        }
        else if (this.item == "LFS BY STRATUM") {
          sp[[this.item]] <- SurvCalc.make.LFS_BY_STRATUM.table(item.lines)
        }
        else if (this.item == "LF OVERALL") {
          sp[[this.item]] <- SurvCalc.make.LF_OVERALL.table(item.lines)
        }
        else if (this.item == "NUMBER MEASURED") {
          sp[[this.item]] <- SurvCalc.make.NUMBER_MEASURED.table(item.lines)
        }
        else if (this.item == "LF TOTALS") {
          sp[[this.item]] <- SurvCalc.make.LF_TOTALS.table(item.lines)
        }
        else if (this.item == "Phase-2 gains for dummies") {
          sp[[this.item]] <- SurvCalc.make.Phase_2_gains_for_dummies.list(item.lines)
        }
        else if (regexpr("Gains for", this.item) == 1) {
          sp[[this.item]] <- SurvCalc.make.Gains_for.table(item.lines)
        }
        else if (this.item == "BIOMASS BY SPECIES") {
          result[[this.trip]][[this.item]] <- SurvCalc.make.BIOMASS_BY_SPECIES.table(item.lines)
        }
        else if (this.item == "BIOMASS BY SPECIES AND STRATUM") {
          result[[this.trip]][[this.item]] <- SurvCalc.make.BIOMASS_BY_SPECIES_AND_STRATUM.table(item.lines)
        }
        else if (this.item == "PROJECTED CVS") {
          result[[this.trip]][[this.item]] <- SurvCalc.make.PROJECTED_CVS.table(item.lines)
        }
        else if (this.item == "BIOMASS BY SPECIES AND TRIP") {
          result[[this.item]] <- SurvCalc.make.BIOMASS_BY_SPECIES_AND_TRIP.table(item.lines)
        }
        result[[this.trip]][[this.sp]] <- sp
      }
    }
  }
  result
}

