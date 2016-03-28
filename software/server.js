var net = require('net'),
    express = require("express"),
    app =  express(),
    http = express(),
    bodyParser = require('body-parser'),
    methodOverride = require('method-override');

// Add Middleware necessarry for REST API's
app.use(bodyParser.urlencoded({
  extended: true
}));
app.use(bodyParser.json());
app.use(methodOverride('X-HTTP-Method-Override'));


// CORS Support
app.use(function(req, res, next) {
  res.header('Access-Control-Allow-Origin', '*');
  res.header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE');
  res.header('Access-Control-Allow-Headers', 'Content-Type');
  next();
});


// Renders front end files
app.use(express.static(__dirname + '/public'));


app.get('/options', function(res){
    var data = '';

    res.on('data', function (chunk){
        data += chunk;
    });

    res.on('end',function(){
        var obj = JSON.parse(data);
        console.log( obj.buck.email );
    })

});

app.listen(3000,function(){
  console.log("Started on PORT 3000");
});
