// Connect to the socket. Same URL, port 81.
var Socket = new WebSocket('ws://'+window.location.hostname+':81');

// When a new websockets message is recieved, redraw the dial with the updated value
Socket.onmessage = function (evt) {
  var theQRstring = evt.data;
  //drawDial(parseInt(thePercentage), '#ab194f');
  printDial(theQRstring);
};

printDial('wait_qr_code');

function printDial(qrString) {
  
  // First, we get a reference to the div in the HTML which we will draw the dial in
  var dialCanv = document.getElementById('QRcode');
		dialCanv.innerText = qrString;
}  

var qrreader = document.getElementById("read_button");
var qrreader_status = 0;

function qr_state() {
	var request = new XMLHttpRequest();
	request.open('GET','/qr_status',true);
	request.onload = function() {
		if (request.readyState == 4 && request.status == 200) {
			var response = request.responseText;
			qrreader_status = Number.parseInt(response);
			if (qrreader_status == 0)
				qrreader.classList.add('read_off');
			else
				qrreader.classList.add('read_on');
		}
	}
	request.send();
}

function qr_start() {
	var request = new XMLHttpRequest();
	var request_txt = '';
	if (qrreader_status==0) {
		request_txt = '/qr_read_on';
		qrreader_status = 1;
	} else {
		request_txt = '/qr_read_off';
		qrreader_status = 0;
	}
	request.open('GET',request_txt,true);
	request.onload = function() {
		if (request.readyState == 4 && request.status == 200) {
			var response = request.responseText;
			if (response == '0') {
				qrreader.classList.remove('read_on');
				qrreader.classList.add('read_off');
			} else {
				qrreader.classList.remove('read_off');
				qrreader.classList.add('read_on');
			}
		}
	}	
	request.send();
}

document.addEventListener('DOMContentLoaded', qr_state);
qrreader.addEventListener('click', qr_start);

// Draw the dial initially at 0
drawDial(0, '#ab194f');

// This function draws the dial based on a given percventage and color
function drawDial(percentage, color) {
  
  // First, we get a reference to the div in the HTML which we will draw the dial in
  var dialCanv = document.getElementById('dial');
  var ctx = dialCanv.getContext("2d");
  dialCanv.height = dialCanv.offsetHeight * 2;
  dialCanv.width = dialCanv.offsetWidth * 2;

  // Calculate the center of the div
  var centerX = dialCanv.width / 2;
  var centerY = dialCanv.height / 2;

  // Draw the colored arc showing the value
  ctx.beginPath();
  ctx.fillStyle = color;
  ctx.moveTo(centerX, centerY.height / 2);
  ctx.arc(centerX, centerY, centerY*0.8, Math.PI * 1.5, (Math.PI * 2 * (percentage / 100)) + (Math.PI * 1.5), false);
  ctx.lineTo(centerX, centerY);
  ctx.fill();
  ctx.closePath();

  // Draw the white background circle
  ctx.beginPath();
  ctx.fillStyle = "white";
  ctx.arc(centerX, centerY, centerY*0.65, 0, Math.PI * 2, false);
  ctx.fill();
  ctx.closePath();

  // Add label
  ctx.font = "bold 90px sans-serif";
  ctx.fillStyle = color;
  ctx.textBaseline = "center";
  ctx.textAlign = "center";
  ctx.fillText(parseInt(percentage), centerX, centerY * 1.1);
}