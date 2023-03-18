// import modules
const serialport = require('serialport');
const nmea       = require('nmea-0183');
const mqtt       = require('mqtt');

// define parameters
var PORT = '/dev/ttyACM0';
var BAUDRATE = 9600;


// define serialport
const port = new serialport(PORT, {
    baudRate: parseInt(BAUDRATE),
    //parser: new serialport.parsers.Readline(readline)
    parser: new serialport.parsers.Readline("\r\n")
});

// open serial port
port.on('open', function(err){
    console.log('port open');
});


// mqtt modules
const mqtthost = 'localhost'
const mqttPub = mqtt.connect('mqtt://'+mqtthost);

mqttPub.on('connect', function(){
    console.log('mqtt ' + mqtthost + ' connected.')
});

mqttPub.on('error', function(err){
    console.log(err);
});

port.on('data', readData);

function readData(data)
{
    try
    {
        var nmeaData = nmea.parse(data.toString());
        if(nmeaData['id'] == 'GPRMC')
        {
            var gpsData = {"GPS_OUTPUT":{"key":"", "latitude":0.0, "longitude":0.0}};
            gpsData["GPS_OUTPUT"]["key"]="takehara"
            gpsData["GPS_OUTPUT"]["latitude"]  = Number(nmeaData.latitude);
            gpsData["GPS_OUTPUT"]["longitude"] = Number(nmeaData.longitude);
            if(isNaN(gpsData.GPS_OUTPUT.latitude) == false && isNaN(gpsData.GPS_OUTPUT.longitude) == false)
            {
                console.log(gpsData);
                mqttPub.publish('gps/ucsk', JSON.stringify(gpsData));
            }
        }
        else
        {
            // mqttPub.publish('gps', null);
        }
    }catch(e)
    {
        // mqttPub.publish('gps', null);
    }
}
