# bikecomputer
Ce code permet, à l'aide d'un module gps de type Neo 6M et d'un capteur barrométrique bmp280, d'enregistrer les coordonnées GPS et le dénivelé realisé lors d'un parcours en vélo. Ces données sont ensuite traitées pour calculer la puissance dévelopée par la personne en chaque instant, ainsi que l'énergie dépensée sur le parcours.

## Structure :
Le processus `logger.py` tourne en boucle, dès que le GPIO 13 (board no°36) est connecté au GND, l'enregistrement des données commence. Il s'arrête une fois le GPIO deconnecté du GND, en enregistrant les données (fichiers .csv et graphs) dans `bikecomputer/logs/<date>/`, format YYYY_MM_DD_HH_minutes. 

Plus de détails sont disponible dans le [wiki](https://github.com/Leowrin/bikecomputer/wiki#principe-g%C3%A9n%C3%A9ral)

## Installation :
Cloner ou télécharger cette page dans `/home/pi/`, renommer le dossier `bikecomputer-main` en `bikecomputer`. Puis installer les paquets et librairies python3 :

-pour installer les paquets; `xargs sudo apt install < packages.txt`

-pour installer les librairies python3; `sudo pip3 install requirements.txt`

(P.S. Les droits administrateurs sont nécéssaire pour installer les paquets/librairies sur tout le système et pas seulement sur l'utilisateur)

La liste des paquets est dans le fichier `packages.txt`, celle des librairies dans `requirements.txt`,



Une fois les librairies instalées, le code `compute.c` doit être compilé dans `/home/pi/bikecomputer/` en un script nommé `gps`.

Pour que le processus tourne à chaque démarrage du RaspberryPi, une méthode possible est d'ajouter `main.py` aux cronjobs, pour se faire,
dans le terminal :
`crontab -e`
puis ajouter en bas de page, en remplacant <path> par le chemin d'accès au dossier (recommandé: `/home/pi/` :
`@reboot python3 <path>/bikecomputer/main.py`
  
  connecter le GPIO 13 à GND pour lancer l'enregistrement et le calcul des forces.
  
  déconnecter le GPIO du GND pour terminer l'enregistrement et créer les graphs sur les résultats




