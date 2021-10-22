# bikecomputer
Idée de base :
  logger :
    - pts gps (pos & alt) (python sur raspberryPi, fichier csv?)
    - temps (python sur raspberryPi)
  avec ces données :
    - calculer l'effort réalisé sur un parcours. (code C)
      (work pour dénivelée positive + work Ffrottements)
    - envoyer position gps si vol (duo python&C/C++)


    page of interest :
    https://www.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_(GPS.G6-HW-09005).pdf
    https://www.openimpulse.com/blog/wp-content/uploads/wpsc/downloadables/GY-NEO6MV2-GPS-Module-Datasheet.pdf

    https://gist.github.com/hdoverobinson/42732da4c5b50d031f60c1fae39f0720
    https://www.engineersgarage.com/articles-raspberry-pi-neo-6m-gps-module-interfacing/
