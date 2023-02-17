var serialport = require("serialport");
var nmea = require('nmea-0183');
var port = '/dev/ttyUSB0';
var baudrate = 4800;

var receiver = new serialport(port, {
    baudRate: parseInt(baudrate),
    //parser: new serialport.parsers.Readline(readline)
    parser: new serialport.parsers.Readline("\r\n")
});

receiver.on('open', function(err){
    console.log('serialPort:Port open');
});

receiver.on('data', function(data){
    try{
        // for (key in data)
        // {
        //     console.log(key);
        // }
        // return 0;
        console.log(data.toString());
        var nmeaData = nmea.parse(data.toString());
        var nmeaData = nmea.encode("GNRMC", data);
        if(nmeaData['id'] == 'GNRMC'){
            var gps = {};
            var coords = {};
            gps.coords = {};
            gps.coords.latitude = Number(nmeaData.latitude);
            gps.coords.longitude = Number(nmeaData.longiture);
            console.log(gps);
            console.log(nmeaData);
        }
        else
        {
            console.log("do not exits GPRMC");
        }
    }catch(e)
    {
        // console.log(e);
    }
});



// const SerialPort = require('serialport');
// const nmea       = require('nmea-0183')
// const Readline = SerialPort.parsers.Readline;
// const port       = new SerialPort('/dev/ttyUSB0');
// const parser     = new Readline("\r\n");
// port.pipe(parser);

// port.on('data', function(data){
//     try{
//         var gps = nmea.parse(data);
//         if (gps['id'] == 'GPRMC'){
//         console.log('GPRMC');
//             console.log(gps);
//         }
//     }catch(e){
//         console.log(e);
//     }
// });
