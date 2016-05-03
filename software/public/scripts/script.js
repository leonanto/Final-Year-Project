var temp1, temp2, moist1, moist2, avg_temp, avg_moist;
var override = 0;
var stat = 0;
var controller = 0;
$(document).ready(function(){
  setInterval(function(){
      $.get( '/PassValues/', function(data) {
      console.log(data);
      temp1 = parseFloat(data.qs_temp1);
      temp2 = parseFloat(data.qs_temp2);
      moist1 = parseFloat(data.qs_moist1);
      moist2 = parseFloat(data.qs_moist2);
      stat = parseInt(data.qs_stat, 10);
      console.log(stat);
    
      if(stat == 1){
        $('#stat').html("ON");
      }
      if (stat == 0){
        $('#stat').html("OFF");
      }   
      avg_temp = (temp1 + temp2)/2;
      avg_moist = (moist1 + moist2)/2;
       
      //Displays Values gotten from the sensors
      $('#u1_temp').html(temp1);
      $('#u2_temp').html(temp2);
      $('#u1_moist').html(moist1);
      $('#u2_moist').html(moist2);
      $('#avg_temp').html(avg_temp);
      $('#avg_moist').html(avg_moist);
     });
}, 1000);
  console.log("Reached");
  
  $('#switch-1').click(function(){
      
     if (controller == 0){
         controller = 1;
         $('#controller').html("ON");
         if(override == 1){
             $.get("/ClientStatus/?stat=1");
            // $('#stat').html("ON");
         }
     }  
     else{
         controller = 0;
         $('#controller').html("OFF");
         if(override == 1){
             $.get("/ClientStatus/?stat=0");
             //$('#stat').html("ON");
         }
     }
     console.log("controller: "+ controller);
  });
  $('#override').click(function(){
     
     if (override == 0){
         override = 1;
         $.get("/ClientOverrideStatus/?stat=1");
         $('#override').html("ON");
     }  
     else{
         override = 0;
         $.get("/ClientOverrideStatus/?stat=0");
         $('#override').html("OFF");
     }
      console.log("override: "+ override);
  });
  
});

