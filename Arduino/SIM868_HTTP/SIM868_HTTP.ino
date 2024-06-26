#include "SIM800L.h"

#define SIM800_RST_PIN 6
#define SIM808_POWER_STAT		A5
#define POWER_GSM	8

const char APN[] = "internet.yettel.bg";
const char URL[] = "https://eesystems.net/tracker/api.php";

SIM800L* sim800l;

char response[512];
char frame[200];

char latitude[15];
char longitude[15];
char altitude[6];
char date[16];
char time[7];
char satellites[3];
char speedOTG[10];
char course[10];
char dataToServer[512];
char battPerc[10];
char battMv[10];

uint16_t iBattPerc = 0;
uint16_t iBattMv = 0;

uint16_t dataInt[10];

void setup() {
    // Initialize Serial Monitor for debugging
    SerialUSB.begin(115200);
    delay(1000);

    // Initialize the hardware Serial1
    Serial1.begin(9600);
    delay(1000);

    pinMode(POWER_GSM, OUTPUT);
    pinMode(SIM800_RST_PIN, OUTPUT);
    digitalWrite(POWER_GSM, HIGH);
    delay(10000);
    digitalWrite(POWER_GSM, LOW);
    delay(1000);

    //Initialize SIM800L driver with an internal buffer of 200 bytes and a reception buffer of 512 bytes, debug disabled
    sim800l = new SIM800L((Stream*)&Serial1, SIM800_RST_PIN, 200, 512);
    //Equivalent line with the debug enabled on the Serial
    //sim800l = new SIM800L((Stream *)&Serial1, SIM800_RST_PIN, 200, 512, (Stream *)&Serial);
    while (start_GPS() == 0);
    setupModule();
    //start_GPS();
}

void loop() {
    readBatt();
    get_GPS();
    httpSEND();
    // Establish GPRS connectivity (5 trials)
    delay(40000);
}

void httpSEND() {
    bool connected = false;
    for (uint8_t i = 0; i < 5 && !connected; i++) {
        delay(1000);
        connected = sim800l->connectGPRS();
    }

    // Check if connected, if not reset the module and setup the config again
    if (connected) {
        SerialUSB.print(F("GPRS connected with IP "));
        SerialUSB.println(sim800l->getIP());
    }
    else {
        SerialUSB.println(F("GPRS not connected !"));
        SerialUSB.println(F("Reset the module."));
        sim800l->reset();
        setupModule();
        start_GPS();
        return;
    }

    SerialUSB.println(F("Start HTTP GET..."));

    String sval = "";
    int i;

    sval += "https://eesystems.net/tracker/t.php";

    sval += "?w=1&id=12&type=10&s=1&sats=" + String(satellites) + "&lat=" + String(latitude, 8) + "&lon=" + String(longitude, 8) + "&alt=" + String(altitude,8) + "&spd=" + String(speedOTG,6);

    for (i = 0; i < sval.length(); i++)
    {
        dataToServer[i] = sval[i];
    }
    dataToServer[i++] = '\0';

    SerialUSB.print("URL - ..."); SerialUSB.println(dataToServer);

    // Do HTTP GET communication with 10s for the timeout (read)
    uint16_t rc = sim800l->doGet(dataToServer, 10000);
    if (rc == 200) {
        // Success, output the data received on the serial
        SerialUSB.print(F("HTTP GET successful ("));
        SerialUSB.print(sim800l->getDataSizeReceived());
        SerialUSB.println(F(" bytes)"));
        SerialUSB.print(F("Received : "));
        SerialUSB.println(sim800l->getDataReceived());
    }
    else {
        // Failed...
        SerialUSB.print(F("HTTP GET error "));
        SerialUSB.println(rc);
        sim800l->reset();
        setupModule();
        start_GPS();

    }

    // Close GPRS connectivity (5 trials)
    bool disconnected = sim800l->disconnectGPRS();
    for (uint8_t i = 0; i < 5 && !connected; i++) {
        delay(1000);
        disconnected = sim800l->disconnectGPRS();
    }

    if (disconnected) {
        SerialUSB.println(F("GPRS disconnected !"));
    }
    else {
        SerialUSB.println(F("GPRS still connected !"));
    }
    

    // Go into low power mode
    /*
    bool lowPowerMode = sim800l->setPowerMode(MINIMUM);
    if (lowPowerMode) {
        SerialUSB.println(F("Module in low power mode"));
    }
    else {
        SerialUSB.println(F("Failed to switch module to low power mode"));
    }
    */
    //End of program... wait...

}

void setupModule() {
    // Wait until the module is ready to accept AT commands
    while (!sim800l->isReady()) {
        SerialUSB.println(F("Problem to initialize AT command, retry in 1 sec"));
        delay(1000);
    }
    SerialUSB.println(F("Setup Complete!"));

    // Wait for the GSM signal
    uint8_t signal = sim800l->getSignal();
    while (signal <= 0) {
        delay(1000);
        signal = sim800l->getSignal();
        SerialUSB.println(signal);
    }
    SerialUSB.print(F("Signal OK (strenght: "));
    SerialUSB.print(signal);
    SerialUSB.println(F(")"));
    delay(1000);

    // Wait for operator network registration (national or roaming network)
    NetworkRegistration network = sim800l->getRegistrationStatus();
    while (network != REGISTERED_HOME && network != REGISTERED_ROAMING) {
        delay(1000);
        network = sim800l->getRegistrationStatus();
    }
    SerialUSB.println(F("Network registration OK"));
    delay(1000);

    // Setup APN for GPRS configuration
    bool success = sim800l->setupGPRS(APN);
    while (!success) {
        success = sim800l->setupGPRS(APN);
        delay(5000);
    }
    SerialUSB.println(F("GPRS config OK"));
}

int8_t start_GPS() {

    unsigned long previous;

    previous = millis();
    // starts the GPS
    sendATcommand("AT+CGPSPWR=1", "OK", 2000);
    sendATcommand("AT+CGPSRST=0", "OK", 2000);

    // waits for fix GPS
    while (((
        sendATcommand("AT+CGPSSTATUS?", "ERROR", 5000) ||
        sendATcommand("AT+CGPSSTATUS?", "Location 2D Fix", 5000) ||
        sendATcommand("AT+CGPSSTATUS?", "Location 3D Fix", 5000) ||
        sendATcommand("AT+CGPSSTATUS?", "2D Fix", 5000) ||
        sendATcommand("AT+CGPSSTATUS?", "3D Fix", 5000)) == 0)
        &&
        ((millis() - previous) < 90000));

    if ((millis() - previous) < 90000)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int8_t sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout) {

    uint8_t x = 0, answer = 0;
    unsigned long previous;

    memset(response, '\0', 512);    // Initialize the string

    SerialUSB.println(ATcommand);

    delay(100);

    while (Serial1.available() > 0) Serial1.read();    // Clean the input buffer

    Serial1.println(ATcommand);    // Send the AT command 

//    String newline = "\r\n";
//    String sendr = expected_answer1 + newline;
//    Serial1.print(sendr);

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do {
        if (Serial1.available() != 0) {
            response[x] = Serial1.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer1) != NULL)
            {
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    } while ((answer == 0) && ((millis() - previous) < timeout));

    SerialUSB.println(response);

    return answer;
}

uint8_t readBatt() {
    int8_t counter, answer;
    long previous;

    while (Serial1.available() > 0) Serial1.read();
    // request Basic string
    sendATcommand("AT+CBC", "AT+CBC", 2000);
    // First get the NMEA string
    // Clean the input buffer

    counter = 0;
    answer = 0;
    memset(frame, '\0', 200);    // Initialize the string
    previous = millis();
    // this loop waits for the NMEA string
    do {

        if (Serial1.available() != 0) {
            frame[counter] = Serial1.read();
            counter++;
            // check if the desired answer is in the response of the module
            if (strstr(frame, "OK") != NULL)
            {
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    } while ((answer == 0) && ((millis() - previous) < 2000));

    SerialUSB.print("ANSVER = "); SerialUSB.println(answer);

    frame[counter - 3] = '\0';

    SerialUSB.println("Frame.....");

    SerialUSB.println(frame);
    SerialUSB.println();



    // Parses the string 
    strtok(frame, ",");
    strcpy(battPerc, strtok(NULL, ",")); // Gets longitude
    strcpy(battMv, strtok(NULL, ",")); // Gets latitude

    SerialUSB.print(F("BattPercent - ")); SerialUSB.println(battPerc);
    SerialUSB.print(F("BattMv - ")); SerialUSB.println(battMv);

    return answer;

}


int getData(char* _char)
{
    int	count = 0;
    int16_t val = 0;
    bool neg = false;
    for (int i = 0; i < sizeof(_char); i++)
    {
        if (_char[i] == '/' || _char[i] == '.' || _char[i] == ' ' || _char[i] == ':' || _char[i] == ',')
        {
            if (neg)
            {
                neg = false;
                val *= -1;
            }
            dataInt[count] = val;
            val = 0;
            count++;
            if (_char[i] == '/') break;
        }
        else
        {
            if (int(_char[i]) >= 48 && int(_char[i]) <= 57)
            {
                val = (val * 10) + (int(_char[i]) - 48);
            }
            if (int(_char[i]) == 45)
            {
                neg = true;
            }
        }
    }
    return count;
}


int8_t get_GPS() {

    int8_t counter, answer;
    long previous;

    while (Serial1.available() > 0) Serial1.read();
    // request Basic string
    sendATcommand("AT+CBC", "AT+CBC", 2000);

    // First get the NMEA string
    // Clean the input buffer
    while (Serial1.available() > 0) Serial1.read();
    // request Basic string
    sendATcommand("AT+CGPSINF=0", "AT+CGPSINF=0", 2000);

    counter = 0;
    answer = 0;
    memset(frame, '\0', 200);    // Initialize the string
    previous = millis();
    // this loop waits for the NMEA string
    do {

        if (Serial1.available() != 0) {
            frame[counter] = Serial1.read();
            counter++;
            // check if the desired answer is in the response of the module
            if (strstr(frame, "OK") != NULL)
            {
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    } while ((answer == 0) && ((millis() - previous) < 2000));


    SerialUSB.println(frame);
    SerialUSB.println();


    frame[counter - 3] = '\0';

    SerialUSB.println(frame);
    SerialUSB.println();


    // Parses the string 
    strtok(frame, ",");
    strcpy(longitude, strtok(NULL, ",")); // Gets longitude
    strcpy(latitude, strtok(NULL, ",")); // Gets latitude
    strcpy(altitude, strtok(NULL, ".")); // Gets altitude 
    strtok(NULL, ",");
    strcpy(date, strtok(NULL, ".")); // Gets date
    strtok(NULL, ",");
    strtok(NULL, ",");
    strcpy(satellites, strtok(NULL, ",")); // Gets satellites
    strcpy(speedOTG, strtok(NULL, ",")); // Gets speed over ground. Unit is knots.
    strcpy(course, strtok(NULL, "\r")); // Gets course

    convert2Degrees(latitude);
    convert2Degrees(longitude);

    SerialUSB.print(F("Latitude - ")); SerialUSB.println(latitude);
    SerialUSB.print(F("Longtitude - ")); SerialUSB.println(longitude);
    SerialUSB.print(F("Altitude - ")); SerialUSB.println(altitude);
    SerialUSB.print(F("DateTime - ")); SerialUSB.println(date);
    SerialUSB.print(F("Satelites - ")); SerialUSB.println(satellites);
    SerialUSB.print(F("Speed - ")); SerialUSB.println(speedOTG);
    SerialUSB.print(F("Course - ")); SerialUSB.println(course);

    return answer;
}

/* convert2Degrees ( input ) - performs the conversion from input
 * parameters in  DD°MM.mmm’ notation to DD.dddddd° notation.
 *
 * Sign '+' is set for positive latitudes/longitudes (North, East)
 * Sign '-' is set for negative latitudes/longitudes (South, West)
 *
 */
int8_t convert2Degrees(char* input) {

    float deg;
    float minutes;
    boolean neg = false;

    //auxiliar variable
    char aux[10];

    if (input[0] == '-')
    {
        neg = true;
        strcpy(aux, strtok(input + 1, "."));

    }
    else
    {
        strcpy(aux, strtok(input, "."));
    }

    // convert string to integer and add it to final float variable
    deg = atof(aux);

    strcpy(aux, strtok(NULL, "\0"));
    minutes = atof(aux);
    minutes /= 1000000;
    if (deg < 100)
    {
        minutes += deg;
        deg = 0;
    }
    else
    {
        minutes += int(deg) % 100;
        deg = int(deg) / 100;
    }

    // add minutes to degrees 
    deg = deg + minutes / 60;


    if (neg == true)
    {
        deg *= -1.0;
    }

    neg = false;

    if (deg < 0) {
        neg = true;
        deg *= -1;
    }

    float numeroFloat = deg;
    int parteEntera[10];
    int cifra;
    long numero = (long)numeroFloat;
    int size = 0;

    while (1) {
        size = size + 1;
        cifra = numero % 10;
        numero = numero / 10;
        parteEntera[size - 1] = cifra;
        if (numero == 0) {
            break;
        }
    }

    int indice = 0;
    if (neg) {
        indice++;
        input[0] = '-';
    }
    for (int i = size - 1; i >= 0; i--)
    {
        input[indice] = parteEntera[i] + '0';
        indice++;
    }

    input[indice] = '.';
    indice++;

    numeroFloat = (numeroFloat - (int)numeroFloat);
    for (int i = 1; i <= 6; i++)
    {
        numeroFloat = numeroFloat * 10;
        cifra = (long)numeroFloat;
        numeroFloat = numeroFloat - cifra;
        input[indice] = char(cifra) + 48;
        indice++;
    }
    input[indice] = '\0';


}
