# bikecomputer
Ce code permet, à l'aide d'un module GPS de type Neo 6M et d'un capteur barrométrique BMP280, d'enregistrer les coordonnées GPS et le dénivelé realisé lors d'un parcours en vélo. Ces données sont ensuite traitées pour calculer la puissance dévelopée par la personne en chaque instant, ainsi que l'énergie dépensée sur le parcours. <br />
Ces données sont disponibles sous forme de graphe (ou de fichier `.csv`), enregistrés dans les dossiers locaux, accessible en se connectant au RaspBerryPi par VNC par exemple.

(Les données pourraient également être accessible directement sur la carte SD, en configurant un serveur web sur le Raspberry ou en connectant un écran, méthode libre à l'utilisateur)

## Structure :
Le processus `logger.py` tourne en boucle, dès que le GPIO 13 (board no°36) est connecté au GND, l'enregistrement des données commence. L'enregistrement s'arrête une fois le GPIO deconnecté du GND, en enregistrant les données (fichiers .csv et graphiques) dans `/home/pi/bikecomputer/logs/<date>/`, format YYYY_MM_DD_HH_mm. 

Il n'y a pas de fin au processus `logger.py`, alors des enregistrements peuvent être lancé à la suite et à tout moment.
Pour arrêter tous les processus python3: `killall python3`.

Plus de détails sont disponible sur la page [wiki](https://github.com/Leowrin/bikecomputer/wiki#principe-g%C3%A9n%C3%A9ral) du projet.

## Installation :
Cloner ou télécharger ce repository dans `/home/pi/`, renommer le dossier `bikecomputer-main` en `bikecomputer`. Puis installer les paquets et librairies python3 :

- pour installer les paquets; `xargs sudo apt install < packages.txt`

- pour installer les librairies python3; `sudo pip3 install -r requirements.txt`

(P.S. Les droits administrateurs sont nécéssaire pour installer les paquets/librairies sur tout le système et pas seulement sur l'utilisateur)

La liste des paquets est dans le fichier `packages.txt`, celle des librairies dans `requirements.txt`,



Une fois les librairies instalées, le code `compute.c` doit être compilé dans `/home/pi/bikecomputer/` en un script nommé `gps`.
<br />(Une méthode possible est `gcc -Wall compute.c -o gps -lm`)

Pour que le processus tourne à chaque démarrage du RaspberryPi, une méthode possible est d'ajouter `logger.py` aux cronjobs, pour se faire,
dans le terminal :
`crontab -e`
puis ajouter en bas de page :
`@reboot python3 home/pi/bikecomputer/logger.py`

Certains paramètres peuvent être modifiés dans le fichier `config.py` :
|mass             |recordSwitch_gpio                                   |recordLED_gpio       |i2c_bus |
|-----------------|----------------------------------------------------|---------------------|--------|
|masse du cycliste [kg]| pin à connecter au GND pour lancer l'enregistrement| pin de la LED témoin| bus I2C|

  
- Connecter le recordSwitch_gpio au GND pour lancer l'enregistrement et le calcul des forces. Si une led a été branchée sur le pin recordLED_gpio, elle s'allume pour confirmer l'enregistrement des données.
  
- Déconnecter le recordSwitch_gpio du GND pour terminer l'enregistrement et créer les graphiques des résultats. La LED s'éteint.




