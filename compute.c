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


//fonction force de frottements
double Froll(float cr, float m, float g){
  return cr * m * g;
}

//r = air density, cwA drag coef* area, Ws = wind speed (suposed = cyclist's speed)
double Fwind(float r, float cwA, float ws){
  return r * cwA * ws * ws / 2;
}

//fonction energie potentielle
double potentialEnergy(double m, double g, double dh){
  return m * g * dh;
}




int main(int argc, char *argv[]) {

  //lecture des arguments lors de l'appel du script
  if (argc != 5) {
    printf("errno1");
    return 1;
  }

  char * filename = argv[1];
  int csvLen = atoi(argv[2]);
  int csvWid = atoi(argv[3]);
  double mass = atof(argv[4]);

  double cr = 0.003;
  float g = 9.81;
  float n = 0.94;
  float cwA = 0.39;
  float r = 1.2;



  //creation des tableaux
  //coordonnees
  double * pythonFile = malloc(csvWid * csvLen * sizeof (double));
  double * wgs84 = malloc(2 * csvLen * sizeof (double));
  double * lv95X = malloc(csvLen * sizeof (double));
  double * lv95Y = malloc(csvLen * sizeof (double));

  //profile
  double * deltaHGPS = calloc(csvLen, sizeof (double));
  double * deltaHPres = calloc(csvLen, sizeof (double));
  double * altimetry = malloc(csvLen * sizeof (double));

  //mouvement
  double * deltaX = calloc(csvLen, sizeof (double));
  double * deltaY = calloc(csvLen, sizeof (double));

  double * deltaPos = calloc(csvLen, sizeof (double));

  double * distance = calloc(csvLen, sizeof (double));

  //forces et energies
  double * workgGPS = calloc(csvLen, sizeof (double));
  double * powergGPS = malloc(csvLen * sizeof (double));

  double * workgPres = calloc(csvLen, sizeof (double));
  double * powergPres = malloc(csvLen * sizeof (double));


  double * airResistance = calloc(csvLen, sizeof (double));

  double * totalForce = calloc(csvLen, sizeof (double));

  double * power = calloc(csvLen, sizeof (double));
  double * sumEnergy = calloc(csvLen, sizeof (double));


  //lecture du fichier et copie dans memoire.
  readCsv(filename, pythonFile, csvWid, csvLen);



  //copie des points GPS
  for (size_t i = 0; i < csvLen; i++) {
    for (size_t j = 0; j < 2; j++) {
      wgs84[i * 2 + j] = pythonFile[i * csvWid + j];
    }
  }


  //creation des coordonnees lv95 N (y) et E (x)
  for (size_t i = 0; i < csvLen; i++) {
    if (wgs84[i * 2] == 0 || wgs84[i * 2 + 1] == 0){
      lv95X[i] = 0;
      lv95Y[i] = 0;
    }
    else{
      double phi = wgs84[i * 2]*3600;
      double phiP = (phi - 169028.66) / 10000;
      double lambda = wgs84[i * 2 + 1/* condition */]*3600;
      double lambdaP = (lambda - 26782.5) / 10000;

      //axe X selon LV95
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
  }


  // correction des valeurs GPS, debut d'acquisition de donnees sans signal
  if (lv95X[0] == 0) {
    int i = 0;
    while (lv95X[i] == 0) {
      i++;
      if (lv95X[i] != 0) {
        for (size_t j = 0; j < i; j++) {
          lv95X[j] = lv95X[i];
          lv95Y[j] = lv95Y[i];
        }
        break;
      if (i >= csvLen) {
        break;
      }
      }
    }
  }

  // correction des pertes de signal en cours de parcours
  for (size_t i = 1; i < csvLen - 1; i++){
    if (lv95X[i] == 0){
      int j = i;
      double tmpXA = lv95X[i - 1];
      double tmpYA = lv95Y[i - 1];
      double tmpHA = pythonFile[(i - 1) * csvWid + 2];

      while (lv95X[j] == 0) {
        j++;
        if (j == csvLen){
          break;
        }
        if (lv95X[j] != 0) {
          double tmpXB = lv95X[j];
          double tmpYB = lv95Y[j];
          double tmpHB = pythonFile[j * csvWid + 2];

          int deltaLoss = j - i + 1;
          double tmpDeltaX = (tmpXB - tmpXA) / deltaLoss;
          double tmpDeltaY = (tmpYB - tmpYA) / deltaLoss;
          double tmpDeltaH = (tmpHB - tmpHA) / deltaLoss;

          for (size_t k = i; k < j; k++) {
            lv95X[k] = lv95X[k - 1] + tmpDeltaX;
            lv95Y[k] = lv95Y[k - 1] + tmpDeltaY;
            pythonFile[k * csvWid + 2] = pythonFile[(k - 1) * csvWid + 2] + tmpDeltaH;
          }
          i = j;
          break;
        }
      }
    }
  }

  // correction des dernieres valeurs en cas de non-signal en fin de parcours
  if ( lv95X[(csvLen - 1)] == 0 ){
    if ( lv95X[0] == 0 ) {
    }
    else {
      for (size_t i = csvLen - 1; i > 0; i--) {
        if (lv95X[i] != 0){
          double tmpX = lv95X[i];
          double tmpY = lv95Y[i];
          double tmpH = pythonFile[i * csvWid + 2];
          for (size_t k = i + 1; k < csvLen; k++) {
            lv95X[k] = tmpX;
            lv95Y[k] = tmpY;
            pythonFile[k * csvWid + 2] = tmpH;
          }
          break;
        }
      }
    }
  }



  //calcul de deltaHGPS entre chaque mesure, pas utile pour les calcus, mais juste pour les analyses initiales
  for (size_t i = 1; i < csvLen; i++) {
    int j = i - 1;
    deltaHGPS[i] = pythonFile[i * csvWid + 2] - pythonFile[j * csvWid + 2];
  }


  //calcul du denivele entre chaque point, en fonction de la difference de pression atmospherique
  for (size_t i = 1; i < csvLen; i++) {
    int j = i - 1;
    double deltaPres = pythonFile[j * csvWid + 3] - pythonFile[i * csvWid + 3];
    deltaHPres[i] = deltaPres * 100 / 12;
  }


  //calcul du profil altimetrique
  altimetry[0] = 0;
  for (size_t i = 1; i < csvLen; i++) {
    double deltaPres = pythonFile[0 * csvWid + 3] - pythonFile[i * csvWid + 3];
    altimetry[i] = deltaPres * 100 / 12;
  }


  //calcul de powergGPS positif, pas utilisé dans les graphs, mais utile pour debug ou analyse de donees
  for (size_t i = 0; i < csvLen; i++) {
    if (deltaHGPS[i] > 0) {
      powergGPS[i] = mass * g * deltaHGPS[i];
    }
  }


  //calcul du travail a partir du denivelee GPS
  for (size_t i = 1; i < csvLen; i++) {
    workgGPS[i] = workgGPS[i - 1];
    if (deltaHGPS[i] > 0) {
      // /3600 pour avoir des wattheure au lieu de joules
      workgGPS[i] += (mass * g * deltaHGPS[i]) / 3600;
    }
  }


  //calcul de la puissance derivee du denivele mesure par le capteur de pression
  for (size_t i = 0; i < csvLen; i++) {
    powergPres[i] = mass * g * deltaHPres[i];
  }


  //calcul du travail derive du denivele mesure par le capteur de pression
  for (size_t i = 1; i < csvLen; i++) {
    workgPres[i] = workgPres[i - 1];
    if (deltaHPres[i] > 0) {
      // /3600 pour wattheure au lieu de joules
      workgPres[i] += (mass * g * deltaHPres[i]) / 3600;
    }
    if (workgPres[i] > 0){
      workgPres[i] -= 0.004 / 90 * mass;
    }

  }


  // creation delta X et debug pour enlever les "teleport"
  for (size_t i = 1; i < csvLen; i++) {
    double tmp = lv95X[i] - lv95X[i - 1];

    //check if distance is over 20 meter (70 km/h) or below 0.4m (1.4 km/h) to cancel any impossible data
    if (fabs(tmp) > 20 || fabs(tmp) < 0.4) {
      tmp = 0;
    }

    deltaX[i] = tmp;
  }

  // creation delta Y et debug pour enlever les "teleport"
  for (size_t i = 1; i < csvLen; i++) {
    double tmp = lv95Y[i] - lv95Y[i + 1];

    //check if distance is over 20 meter (70 km/h) or below 0.4m (1.4 km/h) to cancel any impossible data
    if (fabs(tmp) > 20 || fabs(tmp) < 0.4) {
      tmp = 0;
    }

    deltaY[i] = tmp;
  }


  //creation de delta position
  for (size_t i = 1; i < csvLen; i++) {
    double tmp = deltaX[i] * deltaX[i] + deltaY[i] * deltaY[i];
    tmp = sqrt(tmp);

    deltaPos[i] = tmp;
  }


  //creation de la distance cumulee
  for (size_t i = 1; i < csvLen; i++) {
    distance[i] = distance[i-1] + deltaPos[i];
  }


  //force de  resistance de l'air
  for (size_t i = 1; i < csvLen; i++) {
    airResistance[i] = Fwind(r, cwA, deltaPos[i]);
  }


  //toutes les forces de frottements
  for (size_t i = 1; i < csvLen; i++) {
    totalForce[i] = (airResistance[i] + Froll(cr, mass, g)) / n;
  }


  // calcul de la puissance totale en chaque instant
  for (size_t i = 1; i < csvLen; i++) {
    power[i] = totalForce[i] * deltaPos[i];
    power[i] += potentialEnergy(mass, g, deltaHPres[i]);
  }


  //calcul de l'energie cumulee
  for (size_t i = 1; i < csvLen; i++) {
    sumEnergy[i] += sumEnergy[i - 1];
    if (0 < power[i]) {
      sumEnergy[i] += power[i] / 3600;
    }
    if (sumEnergy[i] > 0){
      sumEnergy[i] -= 0.004 / 90 * mass;
    }
  }



  // print des donnees
  for (size_t i = 0; i < csvLen; i++) {
    printf("%f, %f, %f, %f, %f, %f, %f, ", lv95X[i], deltaX[i], lv95Y[i], deltaY[i], deltaPos[i], distance[i], pythonFile[i * csvWid + 2]);
    printf("%f, %f, %f, %f, %f, %f, %f, %f, %f\n", deltaHGPS[i], powergGPS[i], workgGPS[i], deltaHPres[i], powergPres[i], workgPres[i], power[i], sumEnergy[i], altimetry[i]);
  }



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
