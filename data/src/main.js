var ws;
var virtualCar;
var joystick;

// WEBSOCKET
function init_ws(){
	var selector = "#connection-led"
	var element = document.querySelector(selector);
	ws = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
	
	var tm;
	function ping(){
		if(ws.readyState == ws.OPEN){
			ws.send("__ping__")
		}
		
		tm = setTimeout(function(){
			/// connection closed ///
			let element = document.querySelector("#connection-led")
			element.classList.remove("on")
		}, 100)
	}

	function pong(){
		let element = document.querySelector("#connection-led")
		element.classList.add("on")
		clearTimeout(tm)
	}

	ws.addEventListener("open", ()=>{
		ws.send('Connect ' + new Date());
		console.log('WebSocket open');
		let element = document.querySelector("#connection-led")
		element.classList.add("on")

		setInterval(ping, 150)
	});

	ws.addEventListener("error", (error)=>{
		console.error('WebSocket Error ', error.toString());
		let element = document.querySelector("#connection-led")
		element.classList.remove("on")
	});

	ws.addEventListener("message",(e)=> {
		if(e.data=="__pong__"){
			pong()
		}else{
				
			try{
				let data = JSON.parse(e.data)
				if("wall" in data){
					const el = document.getElementById("wall_sensor");
					if(data["wall"]){
						el.classList.add("active")
					}else{
						el.classList.remove("active")
					}	
				}
				if('state' in data){
					const el = document.getElementById("state_label");
					el.innerText = data['state']
				}
			}
			catch(err){
				console.warn("SERVER", "invalid json", e.data)
			}
		}
	});

	ws.addEventListener("close", ()=> {
		console.log('WebSocket closed');
		let element = document.querySelector("#connection-led")
		element.classList.remove("on")
	});
}

function differential_steer(nJoyX, nJoyY){
	// input values are x, and y axis of direction
	// values are in range -1.0 to 1.0 
	// return values are the strength of the left and right wheels
	// values between -1.0 and 1.0
	

	var fPivYLimit = 0.5;

	// TEMP VARIABLES
	var   nMotPremixL;    // Motor (left)  premixed output        (-128..+127)
	var   nMotPremixR;    // Motor (right) premixed output        (-128..+127)
	var   nPivSpeed;      // Pivot Speed                          (-128..+127)
	var   fPivScale;      // Balance scale b/w drive and pivot    (   0..1   )


	// Calculate Drive Turn output due to Joystick X input
	// if (nJoyY >= 0) {
	  // Forward
	  nMotPremixL = (nJoyX>=0)? 1 : (1 + nJoyX);
	  nMotPremixR = (nJoyX>=0)? (1 - nJoyX) : 1;
	// } else {
	//   // Reverse
	//   nMotPremixL = (nJoyX>=0)? (1 - nJoyX) : 1;
	//   nMotPremixR = (nJoyX>=0)? 1 : (1 + nJoyX);
	// }

	// Scale Drive output due to Joystick Y input (throttle)
	nMotPremixL = nMotPremixL * nJoyY;
	nMotPremixR = nMotPremixR * nJoyY;

	// Now calculate pivot amount
	// - Strength of pivot (nPivSpeed) based on Joystick X input
	// - Blending of pivot vs drive (fPivScale) based on Joystick Y input
	nPivSpeed = nJoyX;
	fPivScale = (Math.abs(nJoyY)>fPivYLimit)? 0.0 : (1.0 - Math.abs(nJoyY)/fPivYLimit);

	// Calculate final mix of Drive and Pivot
	return [
	(1.0-fPivScale)*nMotPremixL + fPivScale*( nPivSpeed),
	(1.0-fPivScale)*nMotPremixR + fPivScale*(-nPivSpeed)
	]
}

/* message engines to NodeMCU */
function sendEngines(leftThrottle, rightThrottle){
	let msg = "engines"+" "+parseInt(leftThrottle)+" "+parseInt(rightThrottle);
	// console.log("sendEngines:", msg)

	// gamma correct engine throttle
	const gamma = 3.3
	const leftSign = leftThrottle/Math.abs(leftThrottle)
	const rightSign = rightThrottle/Math.abs(rightThrottle)
	leftThrottle = leftSign*Math.pow(Math.abs(leftThrottle), 1/gamma)
	rightThrottle = rightSign*Math.pow(Math.abs(rightThrottle), 1/gamma)

	// only try to send msg if websocket is open
	if(ws.readyState == ws.OPEN){
		ws.send(msg);
	}
}


init_ws()
joystick = new Joystick("#joystick")
virtualCar = new VirtualCar("#virtual-car")

joystick.element.addEventListener('input', function(){
	let x = joystick.element.value.x;
	let y = joystick.element.value.y;

	var leftThrottle=0.0;
	var rightThrottle=0.0;

	if(x!=0 && y!=0){
		[leftThrottle, rightThrottle] = differential_steer(x, y);
	}

	virtualCar.leftThrottle = leftThrottle
	virtualCar.rightThrottle = rightThrottle
	sendEngines(-rightThrottle*1024, -leftThrottle*1024);// backfacing car
})


window.addEventListener("load", function(){
	// create droidcam
	droidcam = new Droidcam({
		selector: ".droidcam",
		ip: parseURI()['droidcamip'] || "192.168.0.201:4747"
	});
})
