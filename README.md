# Helios-2-month-CanSat
*rename so that 2 monthers will not find this code in the future*

Cansat 2 month team 1 code.

# Flow of Data
<img width="1442" height="652" alt="image" src="https://github.com/user-attachments/assets/5a6e48b2-3881-4ba5-a098-47de15fdd09c" />

# Frontend
Found in the `main/ground_frontend` folder.

`main.js` is run with `npm run start` and runs the electron frontend easily.
This was chosen as a way to be easily packaged and transportable between devices.

`index.html` contains the page showing all the graphs and data required for the competition.
*show image*

The `static` folder contains the css and js for the project.

# Backend
- pyserial
- csv
- websocket

# Ground ESP32
- ESPNOW

# Payload ESP32
- loop()
- flight stages
