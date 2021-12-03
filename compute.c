#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


//read csv
bool readCsv(char * filename, double * values, int sizeX, int sizeY) {
    FILE * file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "File %s not found.", filename);
        return false;
    }

    int y = 0;
    char buffer[10000];
    while (fgets(buffer, 10000, file) != NULL) {
        int x = 0;
        char * start = buffer;
        while (true) {
            values[y * sizeX + x] = atof(start);
            start = strchr(start, ',');
            if (start == NULL) break;
            start += 1;

            x += 1;
            if (x >= sizeX) break;
        }

        y += 1;
        if (y >= sizeY) break;
    }

    fclose(file);
    return true;
}



double Froll(float cr, float m, float g){
  return cr * m * g;
}

//r = air density, cwA drag coef* area, Ws = wind speed (suposed = cyclist's speed)
double Fwind(float r, float cwA, float ws){
  return r * cwA * ws * ws / 2;
}


double Fslope(double m, double g, double h){
  return m * g * h;
}










int main(int argc, char *argv[]) {

  int mass = 80;
  double cr = 0.003
  float g = 9.81
  float n = 0.94


  //lecture des arguments lors de l'appel du script
  if (argc != 4) {
    printf("errno1");
    return 1;
  }
  char * filename = argv[1];
  int csvLen = atoi(argv[2]);
  int csvWid = atoi(argv[3]);

  //creation des tableaux
    //creation du bloc memoire pour les poidouble Froll(float cr, float m, float g)nts gps
  double * pythonFile = malloc(csvWid * csvLen * sizeof (double));

  double * wgs84 = malloc(2 * csvLen * sizeof (double));
  double * lv95X = malloc(csvLen * sizeof (double));
  double * lv95Y = malloc(csvLen * sizeof (double));

  double * deltaH = calloc(csvLen, sizeof (double));
  double * deltaHPres = calloc(csvLen, sizeof (double));

  double * deltaX = calloc(csvLen, sizeof (double));
  double * deltaY = calloc(csvLen, sizeof (double));

  double * deltaM = calloc(csvLen, sizeof (double));

  double * sumDeltaM = calloc(csvLen, sizeof (double));

  double * workg = calloc(csvLen, sizeof (double));
  double * powerg = malloc(csvLen * sizeof (double));

  double * workgPres = calloc(csvLen, sizeof (double));
  double * powergPres = malloc(csvLen * sizeof (double));



  //lecture du fichier et copie dans memoire.
  readCsv(filename, pythonFile, csvWid, csvLen);


  //variable denivelee positif cumulee



  //copie des points GPS, utile uniquement pour debug et probleme export python
  for (size_t i = 0; i < csvLen; i++) {
    for (size_t j = 0; j < 2; j++) {
      wgs84[i * 2 + j] = pythonFile[i * csvWid + j];
    }
  }


  //creation des coo lv95 N et E
  for (size_t i = 0; i < csvLen; i++) {
    //axe X selon LV95
    double phi = wgs84[i * 2]*3600;
    double phiP = (phi - 169028.66) / 10000;
    double lambda = wgs84[i * 2 + 1]*3600;
    double lambdaP = (lambda - 26782.5) / 10000;

    lv95X[i] = 2600072.37;
    lv95X[i] += 211455.93 * lambdaP;
    lv95X[i] -= 10938.51 * lambdaP * phiP;
    lv95X[i] -= 0.36 * lambdaP * phiP * phiP;
    lv95X[i] -= 44.54 * phiP * phiP * phiP;

    //axe Y
    lv95Y[i] = 1200147.07;
    lv95Y[i] += 308807.95 * phiP;
    lv95Y[i] += 3745.25 * lambdaP * lambdaP;
    lv95Y[i] += 76.63 * phiP * phiP;
    lv95Y[i] -= 194.56 * lambdaP * lambdaP * phiP;
    lv95Y[i] += 119.79 * phiP * phiP * phiP;
  }



  //calcul de deltaHGPS entre chaque mesure
  for (size_t i = 1; i < csvLen; i++) {
    int j = i - 1;
    deltaH[i] = pythonFile[i * csvWid + 2] - pythonFile[j * csvWid + 2];
  }

  //calcul de deltaHPres
  for (size_t i = 1; i < csvLen; i++) {
    int j = i - 1;
    double deltaPres = pythonFile[i * csvWid + 3] - pythonFile[j * csvWid + 3];
    deltaHPres[i] = deltaPres * 100 / 12;
  }

  //calcul de powergGPS positif
  for (size_t i = 0; i < csvLen; i++) {
    if (deltaH[i] > 0) {
      powerg[i] = mass * 9.81 * deltaH[i];
    }
  }

  //calcul de workgGPS possitif
  for (size_t i = 1; i < csvLen; i++) {
    workg[i] = workg[i - 1];
    if (deltaH[i] > 0) {
      // /3600 pour wattheure au lieu de joules
      workg[i] += (mass * 9.81 * deltaH[i]) / 3600;
    }
  }

  //calcul de powergPres positif
  for (size_t i = 0; i < csvLen; i++) {
    if (deltaHPres[i] > 0) {
      powergPres[i] = mass * 9.81 * deltaHPres[i];
    }
  }

  //calcul de workgPres possitif
  for (size_t i = 1; i < csvLen; i++) {
    workgPres[i] = workgPres[i - 1];
    if (deltaHPres[i] > 0) {
      // /3600 pour wattheure au lieu de joules
      workgPres[i] += (mass * 9.81 * deltaHPres[i]) / 3600;
    }
  }




  // creation delta X et debug pour enlever les "teleport"
  for (size_t i = 1; i < csvLen; i++) {
    double tmp = lv95X[i] - lv95X[i - 1];

    //check if distance is over 20 meter / 1s (70 km/h) to cancel any impossible data
    if (fabs(tmp) > 20 || fabs(tmp) < 0.4) {
      tmp = 0;
    }

    deltaX[i] = tmp;
  }

  // creation delta Y et debug pour enlever les "teleport"
  for (size_t i = 1; i < csvLen; i++) {
    double tmp = lv95Y[i] - lv95Y[i + 1];

    //check if distance is over 20 meter / 1s (70 km/h) to cancel any impossible data
    if (fabs(tmp) > 20 || fabs(tmp) < 0.4) {
      tmp = 0;
    }

    deltaY[i] = tmp;
  }

  //creation de deltaM
  for (size_t i = 1; i < csvLen; i++) {
    double tmp = deltaX[i] * deltaX[i] + deltaY[i] * deltaY[i];
    tmp = sqrt(tmp);

    deltaM[i] = tmp;
  }

  //creation de sumDeltaM.
  for (size_t i = 1; i < csvLen; i++) {
    sumDeltaM[i] = sumDeltaM[i-1] + deltaM[i];
  }





  for (size_t i = 0; i < csvLen; i++) {
    printf("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", lv95X[i], deltaX[i], lv95Y[i], deltaY[i], deltaM[i], sumDeltaM[i], pythonFile[i * csvWid + 2], deltaH[i], powerg[i], workg[i], deltaHPres[i], powergPres[i], workgPres[i]);
  }





  //printf("%f\n", workg);


  free(pythonFile);
  free(wgs84);
  free(lv95X);
  free(lv95Y);
  free(deltaH);
  free(deltaHPres);
  free(deltaX);
  free(deltaY);
  free(deltaM);
  free(sumDeltaM);
  free(powerg);
  free(workg);
  free(workgPres);
  free(powergPres);

  return 0;
}
