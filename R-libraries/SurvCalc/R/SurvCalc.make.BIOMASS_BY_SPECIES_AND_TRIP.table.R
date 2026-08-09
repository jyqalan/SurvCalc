`SurvCalc.make.BIOMASS_BY_SPECIES_AND_TRIP.table` <-
function(lines)
{
  data <- SurvCalc.make.data.frame(lines)
  if("population" %in% colnames(data)){
      data[[1]] <- as.character(data[[1]])
      data[[2]] <- as.character(data[[2]])
      data[[3]] <- as.character(data[[3]])      
      for(i in 4:ncol(data))
          data[[i]] <- as.numeric(data[[i]])
  } else {
      data[[1]] <- as.character(data[[1]])
      data[[2]] <- as.character(data[[2]])
      for(i in 3:ncol(data))
          data[[i]] <- as.numeric(data[[i]])      
  }
  data
}

