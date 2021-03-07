class Joystick{
	element;

	update(){
		const inner = this.element.querySelector(".inner")

		let x = this.element.value.x;
		let y = this.element.value.y;
		// deal with thumb radius
		const outerRect = this.element.querySelector(".outer").getBoundingClientRect()
		const innerRect = this.element.querySelector(".inner").getBoundingClientRect()
		x/=outerRect.width/(outerRect.width-innerRect.width)
		y/=outerRect.height/(outerRect.height-innerRect.height)
		y=-1*y;
		inner.style.left = x*50 + 50 + "%"
		inner.style.top = y*50 + 50 + "%"
	}

	constructor(selector){
		console.log("creating joystick")
		// get canvas
		this.element = document.querySelector(selector);
		this.element.value = {x: 0, y:0}

		let dragging = false;

		let onDown = ()=>{
			dragging = true;
		}

		let onMove = (clientX, clientY)=>{
			if(dragging){
				const outerRect = this.element.querySelector(".outer").getBoundingClientRect()
				const innerRect = this.element.querySelector(".inner").getBoundingClientRect()
				let x = (clientX - outerRect.x)/outerRect.width*2-1;
				let y = (clientY - outerRect.y)/outerRect.height*2-1;
				y=y*-1;
				// deal with thumb radius
				x*=outerRect.width/(outerRect.width-innerRect.width)
				y*=outerRect.height/(outerRect.height-innerRect.height)
				
				// limit length
				const length = Math.sqrt(x*x+y*y);
				if(length>1.0){
					x=x/length;
					y=y/length
				}

				this.element.value = {x, y};
				this.update()

				const event = new Event('input');
				this.element.dispatchEvent(event)
			}
		}

		let onUp = ()=>{
			dragging=false;
			this.element.value = {x: 0, y: 0};
			this.update();

			const event = new Event('input');
			this.element.dispatchEvent(event)
		}

		this.element.addEventListener("mousedown", ()=>onDown(), {passive: true});
		window.addEventListener("mousemove", (e)=>onMove(e.clientX, e.clientY), {passive: true});
		window.addEventListener("mouseup", onUp, {passive: true});

		this.element.addEventListener("touchstart", ()=>onDown(), {passive: true});
		this.element.addEventListener("touchmove", (e)=>onMove(e.targetTouches[0].clientX, e.targetTouches[0].clientY), {passive: true});
		this.element.addEventListener("touchend", onUp);
	}
};
