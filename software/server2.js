// var net = require('net'),
//     express = require("express"),
//     app =  express(),
//     http = express(),
//     bodyParser = require('body-parser'),
//     ejs = require('ejs'),
//     fs = require('fs'),
//     path = require("path"),
//     methodOverride = require('method-override');


// // Add Middleware necessarry for REST API's
// app.use(bodyParser.urlencoded({
//   extended: true
// }));
// app.use(bodyParser.json());
// app.use(methodOverride('X-HTTP-Method-Override'));

// //Renders Frontend files statically 
// app.use(express.static(__dirname + '/public'));

// // CORS Support
// app.use(function(req, res, next) {
//   res.header('Access-Control-Allow-Origin', '*');
//   res.header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE');
//   res.header('Access-Control-Allow-Headers', 'Content-Type');
//   next();
// });

// app.listen(8080,function(){
//   console.log("Started on PORT 8080");

// });
