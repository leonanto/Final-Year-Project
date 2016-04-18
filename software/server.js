var net = require('net'),
    express = require("express"),
    app =  express(),
    http = express(),
    bodyParser = require('body-parser'),
    ejs = require('ejs'),
    fs = require('fs'),
    path = require("path"),
    methodOverride = require('method-override');

var data = ({
                  qs_temp1: 0, 
                  qs_moist1: 0,
                  qs_temp2: 0,
                  qs_moist2: 0, 
                  qs_stat: 0
                });
var status = 0;
var override = 0;
// Add Middleware necessarry for REST API's
app.use(bodyParser.urlencoded({
  extended: true
}));
app.use(bodyParser.json());
app.use(methodOverride('X-HTTP-Method-Override'));

//Renders Frontend files statically 
app.use(express.static(__dirname + '/public'));

// CORS Support
app.use(function(req, res, next) {
  res.header('Access-Control-Allow-Origin', '*');
  res.header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE');
  res.header('Access-Control-Allow-Headers', 'Content-Type');
  next();
});


//Handles values passed by the irrigation controller 
app.get('/GetValues/', function(req, res){
    data = ({
                  qs_temp1: req.query.temp1, 
                  qs_moist1: req.query.moist1,
                  qs_temp2: req.query.temp2,
                  qs_moist2: req.query.moist2, 
                  qs_stat: req.query.stat
                });
                
    res.json(data);
});

//Pass values to the javascipt controller
app.get('/PassValues/', function(req, res) {
   res.send(data); 
});

//Gets the status of the irrigation system from the client if they chose to manually control it
app.get('/ClientStatus', function(req, res) {
    status = req.query.stat;
    res.send(status);
})

//Sends the status of the irrigation system to the irrigation controller 
app.get('/IrrigationStatus', function(req, res) {
   if(status == 0){
       res.send("Status: OFF!");
   }
   else{
       res.send("Status: ON!");
   }
});

//Allows the client to override the internal threshold of the irrigation controller
app.get('/ClientOverrideStatus', function(req, res) {
    override = req.query.stat;
    res.send(override);
})

//Send the override status to the irrigation controller so the client can override the internal threshold
app.get('/OverrideStatus', function(req, res) {
   if(override == 0){
       res.send("Status: OFF!");
   }
   else{
       res.send("Status: ON!");
   }
});

app.listen(8080,function(){
  console.log("Started on PORT 8080");

});
