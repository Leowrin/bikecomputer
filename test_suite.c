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

/*
double wgsTolv95X {

}
*/




int main(int argc, char *argv[]) {
  //lecture des arguments lors de l'appel du script
  if (argc != 3) {
    printf("err1");
    return 1;
  }
  char * filename = argv[1];
  int csvLen = atoi(argv[2]);


  //creation des tableaux
    //creation du bloc memoire pour les points gps
  double * pythonFile = malloc(3 * csvLen * sizeof (double));

  double * wgs84 = malloc(2 * csvLen * sizeof (double));
  double * lv95X = malloc(csvLen * sizeof (double));
  double * lv95Y = malloc(csvLen * sizeof (double));

  double * deltaH = malloc((csvLen-1) * 1 * sizeof (double));

  double * deltaX = calloc((csvLen - 1), sizeof (double));
  double * deltaY = calloc((csvLen - 1), sizeof (double));

  //lecture du fichier et copie dans memoire ^.
  readCsv(filename, pythonFile, 3, csvLen);



  double dPos = 0;


  //copie des points GPS, utile uniquement pour debug et probleme export python
  for (size_t i = 0; i < csvLen; i++) {
    for (size_t j = 0; j < 2; j++) {
      wgs84[i * 2 + j] = pythonFile[i * 3 + j] / 100;  //a supprimer le /100 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

  //creation d'une variable deltaH


  //calcul de deltaH entre chaque mesure
  for (size_t i = 0; i < csvLen-1; i++) {
    int j = i+1;
    deltaH[i] = pythonFile[j * 3 + 2] - pythonFile[i * 3 + 2];
  }

  //calcul du denivele possitif
  for (size_t i = 0; i < csvLen-1; i++) {
    if (deltaH[i] > 0) {
      dPos += deltaH[i];
    }
  }





  for (size_t i = 0; i < csvLen - 1; i++) {
    double tmp = fabs(lv95X[i] - lv95X[i + 1]);

    //check if distance is over 20 meter / 1s (70 km/h) to cancel any impossible data
    if (tmp > 20 || tmp < 0.4) {
      tmp = 0;
    }

    deltaX[i] = tmp;
  }


  for (size_t i = 0; i < csvLen - 1; i++) {
    double tmp = fabs(lv95Y[i] - lv95Y[i + 1]);

    //check if distance is over 20 meter / 1s (70 km/h) to cancel any impossible data
    if (tmp > 20 || tmp < 0.4) {
      tmp = 0;
    }

    deltaY[i] = tmp;
  }







/*
  for (size_t i = 0; i < csvLen; i++) {
    printf("E: %f, N: %f\n", lv95X[i], lv95Y[i]);
  }
*/
/*
  for (size_t i = 0; i < csvLen - 1; i++) {
    printf("%f, %f, %f, %f\n", lv95X[i], deltaX[i], lv95Y[i], deltaY[i]);
  }
*/
  for (size_t i = 0; i < csvLen - 1; i++) {
    printf("%f, %f\n", wgs84[i * 2], wgs84[i * 2 + 1]);
    printf("%f, %f\n", lv95X[i], lv95Y[i]);
  }

  //calcul du travail en joules, masse * g * H
  double workg = 0;
  workg = 90 * 9.81 * dPos;

  //conversion en wattheure.
  workg /= 3600;
  //printf("%f\n", workg);


  free(pythonFile);
  free(wgs84);
  free(lv95X);
  free(lv95Y);
  free(deltaH);
  free(deltaX);
  free(deltaY);

  return 0;
}
