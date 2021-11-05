#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


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




int main(int argc, char *argv[]) {

  double dPos = 0;
  //lecture des arguments
  if (argc != 3) {
    printf("err1");
    return 1;
  }
  char * filename = argv[1];
  int csvLen = atoi(argv[2]);


  //creation du bloc memoire pour les points gps
  double * gpsPoints = malloc(3 * csvLen * sizeof (double));
  //lecture du fichier et copie dans memoire ^.
  readCsv(filename, gpsPoints, 3, csvLen);



  //creation d'une variable deltaH
  double * deltaH = malloc((csvLen-1) * 1 * sizeof (double));

  //calcul de deltaH entre chaque mesure
  for (size_t i = 0; i < csvLen-1; i++) {
    int j = i+1;
    deltaH[i] = gpsPoints[j * 3 + 2] - gpsPoints[i * 3 + 2];
  }

  //calcul du denivele possitif
  for (size_t i = 0; i < csvLen-1; i++) {
    if (deltaH[i] > 0) {
      dPos += deltaH[i];
    }
  }

  //calcul du travail en joules, masse * g * H
  double workg = 0;
  workg = 90 * 9.81 * dPos;

  //conversion en wattheure.
  workg /= 3600;
  printf("%f\n", workg);


  return 0;
}
