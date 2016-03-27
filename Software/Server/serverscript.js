var net = require('net');
var express = require("express");
var app =  express();


app.get('/',function(req,res){
  res.sendfile("index.html");
});
app.listen(3000,function(){
  console.log("Started on PORT 3000");
})
//server.listen(8080, '127.0.0.1');
