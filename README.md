# ESP32RGBMatrix

## Projet PlatformIO Arduino pour Matrix Portal S3

Le projet PlatformIO à la racine utilise la bibliothèque HUB75 `Adafruit Protomatter`, recommandée pour le Matrix Portal S3. Le firmware de démonstration est dans [src/main.cpp](src/main.cpp) et utilise le brochage natif de la carte.


https://learn.adafruit.com/adafruit-matrixportal-s3

### Préparer et téléverser

Installer l'extension PlatformIO dans VS Code, puis exécuter depuis le dossier du projet :

```text
pio run
pio run --target upload
pio device monitor
```

Depuis un terminal où la commande `pio` n'est pas dans le PATH, utiliser `python -m platformio` à la place.

La cible est `esp32-s3-devkitc-1`, avec 8 Mo de flash configurés pour le Matrix Portal S3. Le câble USB-C doit être branché sur le port USB de la carte utilisé par le bootloader.

### Matrice utilisée par l'exemple

L'exemple est configuré pour une matrice HUB75 de 64 x 32 pixels. Pour une matrice 32 x 32, modifier `MATRIX_WIDTH` et retirer la cinquième ligne d'adresse (`36`) du tableau `addrPins`, puis passer `4` comme nombre de lignes d'adresse dans le constructeur `Protomatter`.

Le brochage Matrix Portal S3 utilisé est : RGB `42, 41, 40, 38, 39, 37`, adresse `17, 18, 21, 16, 36`, horloge `34`, latch `33`, et output-enable `35`.

## Option préférée
https://www.amazon.fr/Adafruit-aliment%C3%A9-CircuitPython-affichage-5778/dp/B0DXK7D3ML/ref=sr_1_3?sr=8-3

Adafruit Portail Matrix S3 alimenté par CircuitPython, affichage Web, 5778

UTILISATION SIMPLE – Avec le portail Adafruit Matrix S3, vous pouvez vous connecter directement à toutes les matrices LED compatibles HUB-75 sans soudure ni câblage. Branchez et commencez à l'utiliser
PROCESSEUR ESP32-S3 PLUS PUISSANT - Le puissant ESP32-S3 avec 8 Mo de mémoire Flash et 2 Mo de SRAM offre une excellente base pour vos projets et prend en charge CircuitPython et Arduino.
WiFi et Bluetooth : soyez toujours connecté à vos projets Matrix Portal S3 est équipé du WiFi intégré et du Bluetooth LE pour connecter facilement vos écrans LED à Internet.
Divers connecteurs : avec les connecteurs I2C STEMMA QT et JST à 3 broches, vous pouvez facilement connecter des capteurs et des appareils. Idéal pour les projets créatifs et les personnes qui aiment expérimenter.

## Moins facile d'intégration mais ouvert
https://www.amazon.fr/Expansion-ESP32-S3-DevKitC-1-ESP32-DevKitC-Interface-Prototyping/dp/B0G2XDS3YR/ref=sr_1_6?sr=8-6

RGB Matrix Adapter Expansion Board for ESP32-S3-DevKitC-1 & ESP32-DevKitC- USB Type-C Interface, GPIO Expansion Module for IoT Prototyping

Designed for ESP32-S3 and ESP32-DevKitC Series: Compatible with ESP32-S3-DevKitC-1 and ESP32-DevKitC development boards; provides stable USB Type-C power and full pin-out access for prototyping.
Requires Firmware and Module Setup Before Use: This is a developer expansion board. Functionality that depends on wireless communication is provided by the ESP32 module and requires appropriate firmware (ESP-IDF or Arduino IDE) to enable.
Enables Networking & Peripheral Communication via ESP32 Module: When paired with an ESP32 module and firmware, the board supports networking and peripheral communication features. The board itself is an accessory for development and testing.
Comprehensive Documentation & Support: Step-by-step setup, pin definitions and firmware examples are available on the official Wiki: https://seengreat.com/wiki/186/
For Developers and Makers — Not a Consumer Wireless Product: Intended for developers familiar with microcontroller programming. Radio operation, certification and compliance depend on the specific ESP32 module used and local regulations.

## Option 3 (Antenne en plus à prévoir)
https://www.amazon.fr/Adafruit-CircuitPython-6475-Internet-connexion/dp/B0H1NKSGZD/ref=sr_1_2?sr=8-2

Adafruit Matrix Portal S3 CircuitPython 6475 Écran Internet alimenté avec connexion u.FL

Connexion et compatibilité : branchez la carte directement sur les écrans HUB‑75 (de 16 x 32 à 64 x 64), utilisez le connecteur IDC 2 x 8 ou la prise 2 x 10, vissez le câble d'alimentation et chaînez ou panneau les matrices pour les écrans plus grands.
Carte S3 puissante - ESP32-S3 avec Wi-Fi et BLE, 8 Mo Flash + 2 Mo PSRAM, sortie matrice parallèle et puissance double cœur pour une représentation fluide et des tâches réseau.
Développement facile : prise en charge complète Arduino et CircuitPython (uniquement Wi-Fi pour S3), utilisez la bibliothèque Protomatter ; la carte est préprogrammée avec une démonstration pour des tests rapides.
Capteurs et E/S : STEMMA‑QT, JST 3 broches, GPIO‑Breakout, capteur d'accélération LIS3DH, deux boutons utilisateur, réinitialisation, LED d'état NeoPixel et broches de débogage/démarrage pratiques pour les extensions.
Important à savoir : cette version a un port U.FL : vous devez connecter une antenne externe 2,4 GHz (non incluse). La matrice RVB et l'alimentation USB C ne sont pas également incluses ; il existe également une variante avec antenne intégrée.