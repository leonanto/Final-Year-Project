var net = require('net'),
    express = require("express"),
    app =  express(),
    http = express(),
    bodyParser = require('body-parser'),
    ejs = require('ejs'),
    fs = require('fs'),
    path = require("path"),
    mongoose = require('mongoose'),
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


/*Connect to MongoDB
mongoose.connect('mongodb://lgrant:leo1993@ds011790.mlab.com:11790/final-year-project');
mongoose.connection.once('open', function(err) {
    if (err) throw err;
    
    // Import Schema
    var testmodel = require('./models/testschema');
    
    testmodel =  ({
        name: "leonardo grant testing mongodb"
      });
      
    testmodel.save();
*/
    // Handles values passed by the irrigation controller 
    app.get('/GetValues', function(req, res){
        data = ({
                  qs_temp1: req.query.temp1, 
                  qs_moist1: req.query.moist1,
                  qs_temp2: req.query.temp2,
                  qs_moist2: req.query.moist2, 
                  qs_stat: req.query.stat
                });
                
        res.send(data);
    });

    // Shortened GetValues
    app.get('/GV', function(req, res){
        data = ({
                  qs_temp1: req.query.t1, 
                  qs_moist1: req.query.m1,
                  qs_temp2: req.query.t2,
                  qs_moist2: req.query.m2, 
                  qs_stat: req.query.s
                });
                
        res.send("Recieved!");
    });


//Pass values to the javascipt controller
app.get('/PassValues', function(req, res) {
   res.send(data); 
});

//Gets the status of the irrigation system from the client if they chose to manually control it
app.get('/ClientStatus', function(req, res) {
    status = req.query.stat;
    res.send(status);
});

//Sends the status of the irrigation system to the irrigation controller 
app.get('/IS', function(req, res) {
   if(status == 0){
       res.send("I Status: OFF!");
   }
  if(status == 1){
       res.send("I Status: ON!");
   }
});

//Allows the client to override the internal threshold of the irrigation controller
app.get('/ClientOverrideStatus', function(req, res) {
    override = req.query.stat;
    res.send(override);

});

//Send the override status to the irrigation controller so the client can override the internal threshold
app.get('/OS', function(req, res ){
   if(override == 0){
       res.send("O Status: OFF!");
   }
   
   if(override == 1){
       res.send("O Status: ON!");
   }
});

console.log("Server running");
app.listen( process.env.PORT, process.env.IP);

