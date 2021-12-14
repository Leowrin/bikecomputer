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


//constant
double Froll(float cr, float m, float g){
  return cr * m * g;
}

//r = air density, cwA drag coef* area, Ws = wind speed (suposed = cyclist's speed)
double Fwind(float r, float cwA, float ws){
  return r * cwA * ws * ws / 2;
}


double potentialEnergy(double m, double g, double dh){
  return m * g * dh;
}









int main(int argc, char *argv[]) {

  int mass = 80;
  double cr = 0.003;
  float g = 9.81;
  float n = 0.94;
  float cwA = 0.39;
  float r = 1.2;


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

  double * deltaHGPS = calloc(csvLen, sizeof (double));
  double * deltaHPres = calloc(csvLen, sizeof (double));
  double * altimetry = malloc(csvLen * sizeof (double));

  double * deltaX = calloc(csvLen, sizeof (double));
  double * deltaY = calloc(csvLen, sizeof (double));

  double * deltaPos = calloc(csvLen, sizeof (double));

  double * distance = calloc(csvLen, sizeof (double));

  double * workgGPS = calloc(csvLen, sizeof (double));
  double * powergGPS = malloc(csvLen * sizeof (double));

  double * workgPres = calloc(csvLen, sizeof (double));
  double * powergPres = malloc(csvLen * sizeof (double));


  double * airResistance = calloc(csvLen, sizeof (double));

  //double * slopeForce = calloc(csvLen, sizeof (double));

  double * totalForce = calloc(csvLen, sizeof (double));

  double * power = calloc(csvLen, sizeof (double));
  double * sumEnergy = calloc(csvLen, sizeof (double));


  //lecture du fichier et copie dans memoire.
  readCsv(filename, pythonFile, csvWid, csvLen);


  //copie des points GPS, utile uniquement pour debug et probleme export python
  for (size_t i = 0; i < csvLen; i++) {
    for (size_t j = 0; j < 2; j++) {
      wgs84[i * 2 + j] = pythonFile[i * csvWid + j];
    }
  }


  for (size_t i = 0; i < csvLen; i++){
    //int tmpA = i;
    double tmpX;
    double tmpY;
    double tmpH;
    if (wgs84[i * 2] == 0){
      int j = i;
      while (wgs84[j * 2] == 0) {
        j++;
        if (j == csvLen){
          break;
        }
        if (wgs84[j * 2] != 0) {
          tmpX = wgs84[j * 2];
          tmpY = wgs84[j * 2 + 1];
          tmpH = pythonFile[j * csvWid + 2];
          for (size_t k = i; k < j; k++) {
            wgs84[i * 2] = tmpX;
            wgs84[i * 2 + 1] = tmpY;
            pythonFile[i * csvWid + 2] = tmpH;
          }
        }
      }
    }
  }
  if ( wgs84[(csvLen - 1) * 2] == 0 ){
    if ( wgs84[0] == 0 ) {
    }
    else {
      for (size_t i = csvLen - 1; i > 0; i--) {
        if (wgs84[i * 2] != 0){
          double tmpX = wgs84[i * 2];
          double tmpY = wgs84[i * 2 + 1];
          double tmpH = pythonFile[i * csvWid + 2];
          for (size_t k = i + 1; k < csvLen; k++) {
            wgs84[2 * k] = tmpX;
            wgs84[2 * k + 1] = tmpY;
            pythonFile[k * csvWid + 2] = tmpH;
          }
          break;
        }
      }
    }
  }




  //creation des coo lv95 N (y) et E (x)
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






  //calcul de deltaHGPS entre chaque mesure, pas utile, juste pour data
  for (size_t i = 1; i < csvLen; i++) {
    int j = i - 1;
    deltaHGPS[i] = pythonFile[i * csvWid + 2] - pythonFile[j * csvWid + 2];
  }


  //calcul de deltaHPres
  for (size_t i = 1; i < csvLen; i++) {
    int j = i - 1;
    double deltaPres = pythonFile[j * csvWid + 3] - pythonFile[i * csvWid + 3];
    deltaHPres[i] = deltaPres * 100 / 12;
  }

  altimetry[0] = 0;
  for (size_t i = 1; i < csvLen; i++) {
    double deltaPres = pythonFile[0 * csvWid + 3] - pythonFile[i * csvWid + 3];
    altimetry[i] = deltaPres * 100 / 12;
  }

  //calcul de powergGPS positif, pas utile, data
  for (size_t i = 0; i < csvLen; i++) {
    if (deltaHGPS[i] > 0) {
      powergGPS[i] = mass * 9.81 * deltaHGPS[i];
    }
  }


  //calcul de workgGPS possitif, a remplacer par F -------------------------------------------------------------------------------
  for (size_t i = 1; i < csvLen; i++) {
    workgGPS[i] = workgGPS[i - 1];
    if (deltaHGPS[i] > 0) {
      // /3600 pour wattheure au lieu de joules
      workgGPS[i] += (mass * 9.81 * deltaHGPS[i]) / 3600;
    }
  }






  //calcul de powergPres positif, a remplacer par F ------------------------------------------------------------------------------
  for (size_t i = 0; i < csvLen; i++) {
    if (deltaHPres[i] > 0) {
      powergPres[i] = mass * 9.81 * deltaHPres[i];
    }
  }




  //calcul de workgPres possitif, a remplacer par Sigma F/n*v --------------------------------------------------------------------
  for (size_t i = 1; i < csvLen; i++) {
    workgPres[i] = workgPres[i - 1];
    if (deltaHPres[i] > 0) {
      // /3600 pour wattheure au lieu de joules
      workgPres[i] += (mass * 9.81 * deltaHPres[i]) / 3600;
    }
    workgPres[i] -= 0.004 / 90 * mass;
  }








  // creation delta X et debug pour enlever les "teleport"
  for (size_t i = 1; i < csvLen; i++) {
    double tmp = lv95X[i] - lv95X[i - 1];

    //check if distance is over 20 meter / 1s (70 km/h) to cancel any impossible data, a retirer ? -------------------------------????
    if (fabs(tmp) > 20 || fabs(tmp) < 0.4) {
      tmp = 0;
    }

    deltaX[i] = tmp;
  }





  // creation delta Y et debug pour enlever les "teleport"
  for (size_t i = 1; i < csvLen; i++) {
    double tmp = lv95Y[i] - lv95Y[i + 1];

    //check if distance is over 20 meter / 1s (70 km/h) to cancel any impossible data, a retirer ? -------------------------------????
    if (fabs(tmp) > 20 || fabs(tmp) < 0.4) {
      tmp = 0;
    }

    deltaY[i] = tmp;
  }




  //creation de deltaPos
  for (size_t i = 1; i < csvLen; i++) {
    double tmp = deltaX[i] * deltaX[i] + deltaY[i] * deltaY[i];
    tmp = sqrt(tmp);

    deltaPos[i] = tmp;
  }

  //creation de distance.
  for (size_t i = 1; i < csvLen; i++) {
    distance[i] = distance[i-1] + deltaPos[i];
  }



  //force de l'airResistance
  for (size_t i = 1; i < csvLen; i++) {
    airResistance[i] = Fwind(r, cwA, deltaPos[i]);
  }

/*
  //force slope
  for (size_t i = 1; i < csvLen; i++) {
    if (deltaPos[i] != 0){
      slopeForce[i] = slopeForce(m, g, deltaHPres[i], deltaPos[i]);
    }
  }
*/

  //totalForce
  for (size_t i = 1; i < csvLen; i++) {
    totalForce[i] = (airResistance[i] + Froll(cr, mass, g)) / n;
  }


  for (size_t i = 1; i < csvLen; i++) {
    power[i] = totalForce[i] * deltaPos[i];
    power[i] += potentialEnergy(mass, g, deltaHPres[i]);

  }


  for (size_t i = 1; i < csvLen; i++) {
    sumEnergy[i] += sumEnergy[i - 1];
    if (0 < power[i]) {
      sumEnergy[i] += power[i] / 3600;
    }
    if (sumEnergy[i] > 0){
      sumEnergy[i] -= 0.004 / 90 * mass;
    }
  }

  // à remplacer par un proper fprintf ?-----------------------------------------------------------------------------------------
  for (size_t i = 0; i < csvLen; i++) {
    printf("%f, %f, %f, %f, %f, %f, %f, ", lv95X[i], deltaX[i], lv95Y[i], deltaY[i], deltaPos[i], distance[i], pythonFile[i * csvWid + 2]);
    printf("%f, %f, %f, %f, %f, %f, %f, %f, %f\n", deltaHGPS[i], powergGPS[i], workgGPS[i], deltaHPres[i], powergPres[i], workgPres[i], power[i], sumEnergy[i], altimetry[i]);
  }





  //printf("%f\n", workgGPS);


  free(pythonFile);
  free(wgs84);
  free(lv95X);
  free(lv95Y);
  free(deltaHGPS);
  free(deltaHPres);
  free(altimetry);
  free(deltaX);
  free(deltaY);
  free(deltaPos);
  free(distance);
  free(powergGPS);
  free(workgGPS);
  free(workgPres);
  free(powergPres);
  free(airResistance);
  free(totalForce);
  free(power);
  free(sumEnergy);

  return 0;
}
