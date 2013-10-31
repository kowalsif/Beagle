    var socket;
    var firstconnect = true,
        i2cNum  = "0x70",
	disp = [];
	dispSave = [];

// Create a matrix of LEDs inside the <table> tags.
var matrixData;
for(var j=7; j>=0; j--) { 
	matrixData += '<tr>';
	for(var i=0; i<8; i++) {
	    matrixData += '<td><div class="LED" id="id'+i+'_'+j+
		'" onclick="LEDclick('+i+','+j+')">'+
		i+','+j+'</div></td>';
	    }
	matrixData += '</tr>';
}
$('#matrixLED').append(matrixData);

// Send one column when LED is clicked.
function LEDclick(i, j) { //i is column, j is row. 
//	alert(i2cNum+","+i+","+j+","+disp[i].toString(16)+ " clicked");
//    if(disp[i]>>j&0x1 === 1) { //LED is green
//    	//change to orange
//    	disp[i] ^= 0x1<<j+8; //turn on red led
//    } else if(disp[i]>>j&0x1 === 1 && disp[i]>>j+8&0x1 === 1) { //LED is orange
    	//change to red
//    	disp[i] ^= 0x1<<j; //turn off green
//    } else if(disp[i]>>j+8&0x1 === 1) { //LED is red
    	//turn off
//    	disp[i] ^= 0x1<<j+8; //turn off red
//    } else { //LED is off, turn on
    	//change to green. 
//    	disp[i] ^= 0x1<<j; //turn on green
//    }
	i2 = i*2;
	if (disp[i2]>>j&0x1===1 & disp[i2+1]>>j&0x1===1){
		disp[i2] ^= 0x1<<j;
	} else if (disp[i2]>>j&0x1===1){
		disp[i2+1] ^= 0x1<<j;
	} else if (disp[i2+1]>>j&0x1===1){
		disp[i2+1] ^= 0x1<<j;
	} else {
		disp[i2] ^= 0x1<<j;
	}
    socket.emit('i2cset', {i2cNum: i2cNum, i: i2, 
			     disp: disp[i2].toString()});
	socket.emit('i2cset', {i2cNum: i2cNum, i: i2+1, 
			     disp: disp[i2+1].toString()});

//	socket.emit('i2c', i2cNum);
    // Toggle bit on display
    if(disp[i2]>>j&0x1 === 1 & disp[i2+1]>>j&0x1 === 1) {
    	$('#id'+i+'_'+j).removeClass('on');
        $('#id'+i+'_'+j).addClass('ono');
    }  else if(disp[i2]>>j&0x1 === 1) {
    	$('#id'+i+'_'+j).addClass('on');
    } else if(disp[i2+1]>>j&0x1 === 1) { //LED is red
    	$('#id'+i+'_'+j).removeClass('ono');
    	$('#id'+i+'_'+j).addClass('onr');
    }
     else {
        $('#id'+i+'_'+j).removeClass('onr');
    }
}

    function connect() {
      if(firstconnect) {
        socket = io.connect(null);

        // See https://github.com/LearnBoost/socket.io/wiki/Exposed-events
        // for Exposed events
        socket.on('message', function(data)
            { status_update("Received: message " + data);});
        socket.on('connect', function()
            { status_update("Connected to Server"); });
        socket.on('disconnect', function()
            { status_update("Disconnected from Server"); });
        socket.on('reconnect', function()
            { status_update("Reconnected to Server"); });
        socket.on('reconnecting', function( nextRetry )
            { status_update("Reconnecting in " + nextRetry/1000 + " s"); });
        socket.on('reconnect_failed', function()
            { message("Reconnect Failed"); });

        socket.on('matrix',  matrix);
        // Read display for initial image.  Store in disp[]
        socket.emit("matrix", i2cNum);

        firstconnect = false;
      }
      else {
        socket.socket.reconnect();
      }
    }

    function disconnect() {
      socket.disconnect();
    }

    // When new data arrives, convert it and display it.
    // data is a string of 16 values, each a pair of hex digits.
    function matrix(data) {
        var i, j;
        disp = [];
        dispSave = [];
        //        status_update("i2c: " + data);
        // Make data an array, each entry is a pair of digits
        data = data.split(" ");
        //        status_update("data: " + data);
        // Every other pair of digits are Green. The others are red.
        // Ignore the red.
        // Convert from hex.
        for (i = 0; i < data.length; i += 1) {
            disp[i] = parseInt(data[i], 16);
        }
        //        status_update("disp: " + disp);
        // i cycles through each column
        for (i = 0; i < disp.length; i+=2) {
            // j cycles through each bit
            for (j = 0; j < 8; j+=1) {
                if (((disp[i] >> j) & 0x1 === 1) & ((disp[i+1] >> j) & 0x1 === 1)) {
                    $('#id' + i/2 + '_' + j).addClass('ono');
                    $('#id' + i/2 + '_' + j).removeClass('on');
                    $('#id' + i/2 + '_' + j).removeClass('onr');
                } else if ((disp[i] >> j) & 0x1 === 1){
                    $('#id' + i/2 + '_' + j).removeClass('onr');
                    $('#id' + i/2 + '_' + j).removeClass('ono');
                    $('#id' + i/2 + '_' + j).addClass('on');
                } else if ((disp[i+1] >> j) & 0x1 === 1){
                    $('#id' + i/2 + '_' + j).removeClass('on');
                    $('#id' + i/2 + '_' + j).removeClass('ono');
                    $('#id' + i/2 + '_' + j).addClass('onr');
                } else {
                    $('#id' + i/2 + '_' + j).removeClass('on');
                    $('#id' + i/2 + '_' + j).removeClass('ono');
                    $('#id' + i/2 + '_' + j).removeClass('onr');
                }
            }
        }
    }

    function status_update(txt){
	$('#status').html(txt);
    }

    function updateFromLED(){
      socket.emit("matrix", i2cNum);    
    }
    
    function save(){
      //alert("saving "+dispSave[0]);
      var i, j;
	  dispSave = [];
		//data = data.split(" ");
	  for (i = 0; i < 16; i++) {
          dispSave[i] = disp[i];//parseInt(data[i], 16);
      }
    }
    
    function load(){
      //alert("loading");
      var i, j;
	  for (i = 0; i < 16; i++){
			socket.emit('i2cset', {i2cNum: i2cNum, i: i, 
			     disp: dispSave[i].toString()});
	  }
		
	  for (i = 0; i < dispSave.length; i+=2) {
            // j cycles through each bit
            for (j = 0; j < 8; j+=1) {
            	disp[i] = dispSave[i];
            	disp[i+1] = dispSave[i+1];
                if (((dispSave[i] >> j) & 0x1 === 1) & ((dispSave[i+1] >> j) & 0x1 === 1)) {
                    $('#id' + i/2 + '_' + j).addClass('ono');
                    $('#id' + i/2 + '_' + j).removeClass('on');
                    $('#id' + i/2 + '_' + j).removeClass('onr');
                } else if ((dispSave[i] >> j) & 0x1 === 1){
                    $('#id' + i/2 + '_' + j).removeClass('onr');
                    $('#id' + i/2 + '_' + j).removeClass('ono');
                    $('#id' + i/2 + '_' + j).addClass('on');
                } else if ((dispSave[i+1] >> j) & 0x1 === 1){
                    $('#id' + i/2 + '_' + j).removeClass('on');
                    $('#id' + i/2 + '_' + j).removeClass('ono');
                    $('#id' + i/2 + '_' + j).addClass('onr');
                } else {
                    $('#id' + i/2 + '_' + j).removeClass('on');
                    $('#id' + i/2 + '_' + j).removeClass('ono');
                    $('#id' + i/2 + '_' + j).removeClass('onr');
                }
            }
      }

    }
    
    function clears(){
    	//alert("clearing");
    	var i, j;
    	
    	for (i = 0; i < 16; i++){
    		disp[i] = 0000000;
			socket.emit('i2cset', {i2cNum: i2cNum, i: i, 
			     disp: disp[i].toString()});
	   }
    	
    	
    	for (j = 0; j < 8; j+=1) {
    		for (i = 0; i < 16; i++){
    			$('#id' + i/2 + '_' + j).removeClass('on');
                $('#id' + i/2 + '_' + j).removeClass('onr');
                $('#id' + i/2 + '_' + j).removeClass('ono');
    		}
    	}
    
    }
    
	
connect();

$(function () {
    // setup control widget
    $("#i2cNum").val(i2cNum).change(function () {
        i2cNum = $(this).val();
    });
});
