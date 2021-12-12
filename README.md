# bikecomputer
Ce code permet, à l'aide d'un module gps de type Neo 6M et d'un capteur barrométrique bmp280, d'enregistrer les coordonnées GPS et le dénivelé realisé lors d'un parcours en vélo. Ces données sont ensuite traitées pour calculer la puissance dévelopée par la personne en chaque instant, ainsi que l'énergie dépensée sur le parcours.

## Structure :
Le processus `main.py` tourne en boucle, dès que le GPIO no 13 est connecté au GND, l'enregistrement des données commence. Il s'arrête une fois le GPIO deconnecté du GND, en enregistrant les données dans `bikecomputer/logs/date`, format YYYY_MM_DD_HH_minutes. 

Plus de détails sont disponible dans le [wiki](https://github.com/Leowrin/bikecomputer/wiki#principe-g%C3%A9n%C3%A9ral)

## Installation :
pour installet les paquets; `xargs sudo apt install < packages.txt`

pour installer les librairies python3; `sudo pip3 install requirements.txt`

La liste des paquets est dans le fichier `packages.txt`, celle des librairies dans `requirements.txt`,
Le détail de chaque paquet est disponible sur le wiki.


<!----les librairies suivantes sont nécéssaire pour Python3 :
  - bmp280 `pip3`
  - numpy `pip3`
  - matplotlib `pip3`
  - python3-gi-cairo `pip3`---->

Une fois les librairies instalées, le code `compute.c` doit être compilé en un script nommé `gps`.

Pour que le processus tourne à chaque démarrage du RaspberryPi, une méthode possible est d'ajouter `main.py` aux cronjobs, pour se faire,
dans le terminal :
`crontab -e`
puis ajouter en bas de page, en remplacant <path> par le chemin d'accès au dossier (recommandé: `/home/pi/` :
`@reboot python3 <path>/bikecomputer/main.py`
  
  connecter le GPIO 13 à GND pour lancer l'enregistrement et le calcul des forces.
  
  déconnecter le GPIO du GND pour terminer l'enregistrement et créer les graphs sur les résultats




### Ressources :

https://www.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_(GPS.G6-HW-09005).pdf
https://www.openimpulse.com/blog/wp-content/uploads/wpsc/downloadables/GY-NEO6MV2-GPS-Module-Datasheet.pdf

https://gist.github.com/hdoverobinson/42732da4c5b50d031f60c1fae39f0720
https://www.engineersgarage.com/articles-raspberry-pi-neo-6m-gps-module-interfacing/

https://fr.wikipedia.org/wiki/NMEA_0183#cite_note-SommeDeControleNMEA-4


study on cyclist's work :
https://link.springer.com/article/10.1007/s12283-017-0234-1

https://www.raspberrypi.com/documentation/computers/remote-access.html
  
https://docs.python-guide.org/writing/structure/
