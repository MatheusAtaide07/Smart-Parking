# About the project
The main objective of the project is retrieve and relay useful data around a parking lot, mainly big and sectioned either by area or by floors, and it's spots to users so they can evaluate the parking spot least occupied section and go to it. With this intent in mind, a circuit capable of reading a parking spot and relaying it's status (occupied or vacant) to a database was designed. Further development updates seeks to:
- build a backend capable of filtering the data collected so an app or website may show the user the parking lot occupancy filtred by sections or groups.
- improve circuit code to be able to connect with other databse besides firebase.
# Project worfklow
The project uses an esp32 alongside two HC-SR40 ultrassonic sensor to determine if a parking spot is occupied or vacant and send the collected data to a firebase real-time database. The circuit can be configured to:
- predetermine the parking spot group(common or disabled).
- alter the distance the car is required to be from the circuit so the spot is computed as occupied.
in the file Detec.ino further explanation on the code running those features are included.
