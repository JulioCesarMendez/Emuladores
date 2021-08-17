# TANK OF GREEN HOUSE
developed in ANSI C, this emulathor of green house tank, have the isues as real tank, with several sensors:
By default:
limit switch hh (positioned at 95% of the tank height) - Can be desabled -
limit switch h  (positioned at 90% of the tank height) - Can be desabled -
limit switch l  (positioned at 10% of the tank height) - Can be desabled -
limit switch ll (positioned at 05% of the tank height) - Can be desabled -
level transmitter LT (calculated fron mix imput liquid minus mix output liquid) - Can be desabled -
Temp transmitter TT x 3 (Emulated with a 60 days histogram of texas city) - Can be desabled, each -
pH transmitter pH x 3 (Prom calculated from type and volume of mix imput liquid) - Can be desabled, each -
Press transmitter TP x 3 (Atmospheric press prom, plus press of mix liquid on board) - Can be desabled, each -
conductivity transmitter EC x 3 (Conductivity prom of mix liquid on board) - Can be desabled, each -
Vibration transmitter VT x 3 (Vibration prom, estimated from imput/output activity plus geoactivity vibration) - Can be desabled, each -
dissolved oxygen transmitter OT x 3 ( Dissolved oxygen prom calculated from level of mix liquid on board) - Can be desabled, each -
turbidity transmitter TUT x 3 (turbidity prom of mix liquid on board) - Can be desabled, each -
input stream FT x 3 (Each associated with a filling valve -inlet-) - Can be desabled, each -
outlet valve (tank drain valve)

So, can be configurated until 10 tanks. Each have these configuration zone in configurations files.
2 tank Example of configuration file: (see ./data/SimTnkGH_cfg.help)
In this file, you can see an example of emulator configuration for two (02) tanks. 
The configuration file used by the program is named < SimTnkGH1.cfg >. 
If you want to use a file with another name, it can be changed in the < ../data/emulacion.txt > file. 
This other file contains the initial parameters that are passed to each instance of the emulator. 
By default, only one instance of the tank emulator is run, but you can add as many as you can, 
depending on the number of modbus registers used by each instance, which must total a maximum of 2014 of each type.

In addition to the tank emulator, a modbus tcp server (port 502, slave 1) is raised, 
which can be used to modify some parameters of each tank (see file ../data/SimTnkGH_cfg.help ) 
and to read the states of each sensor/transmitter associated to each tank.

To run the service, download the container, unzip it in a working directory, 
enter the bin directory and run: ./thoriumcorp &

The emulator uses a character-like interface on the console.

To move between tanks use the Up, Down, Left, Right keys. To exit use the Escape key

There is no limit to its use.

If you have any comments or suggestions, or if you observe any problems, you can write me at: mendezjcx@tutanota.com

I hope you find it useful.

We will be adding features as time permits.
Best regards.
Julio César Méndez
