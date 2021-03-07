const shiftLeft = (collection, values) => {
	const step = values.length
  for (let i = 0; i < collection.length - step; i++) {
    collection[i] = collection[i + step]; // Shift left
  }
  for(let j=0; j<step;j++){
  	collection[collection.length - (step-j)] = values[j];// Place new value at tail
  }
   
  return collection;
}

class VirtualCar{
	constructor(selector){
		// create renderer
		console.assert(THREE)
		this.element = document.querySelector(selector)
		console.assert(this.element)
		const renderer = new THREE.WebGLRenderer({antialias: true, alpha: true})
		renderer.setClearColor(new THREE.Color(255,255,255), 0)
		renderer.setPixelRatio( window.devicePixelRatio );
		renderer.setSize(200, 200)

		this.element.appendChild(renderer.domElement)
		window.renderer = renderer;

		// create scene
		const scene = new THREE.Scene()

		// create scene helpers
		let axis = new THREE.AxisHelper()
		scene.add(axis)

		const gridHelper = new THREE.GridHelper( 20, 20 );
		scene.add( gridHelper );
		window.grid = gridHelper


		// create car mesh
		const geometry = new THREE.BoxGeometry(0.75, 0.2, 1.);
		const material = new THREE.MeshBasicMaterial( { color: 'red' } );
		const body = new THREE.Mesh( geometry, material );
		const leftWheel = new THREE.Mesh(
			new THREE.CylinderGeometry(0.2, 0.2, 0.2),
			new THREE.MeshBasicMaterial( { color: 'grey' } )
		)
		leftWheel.rotateZ(Math.PI/2)
		leftWheel.position.x=0.5
		body.add(leftWheel)
		const rightWheel = new THREE.Mesh(
			new THREE.CylinderGeometry(0.2, 0.2, 0.2),
			new THREE.MeshBasicMaterial( { color: 'grey' } )
		)
		rightWheel.rotateZ(Math.PI/2)
		rightWheel.position.x=-0.5
		body.add(rightWheel)
		body.position.set(0,1,0)
		scene.add( body );

		// create camera
		let camera = new THREE.PerspectiveCamera(75, 1/1, 0.1, 1000)
		camera.position.set(0,3, 0.0);
		camera.lookAt(new THREE.Vector3(0,0,0), new THREE.Vector3(0,1,0))

		body.add(camera)

		// Create Trajectory
		let trajectory;
		const trajectory_max_points = 200;
		let trajectory_current_points = 0;
		(function create_trajectory(){
			var trajectory_geo = new THREE.BufferGeometry();
			trajectory_geo.addAttribute( 'position', new THREE.BufferAttribute( new Float32Array( trajectory_max_points * 3 ), 3 ) );
			trajectory_geo.setDrawRange( 0, trajectory_current_points );
			trajectory = new THREE.Line(trajectory_geo, new THREE.LineBasicMaterial({color: "blue"}));
			trajectory.frustumCulled = false
		})()

		scene.add(trajectory);

		function update_trajectory(point){
			if(trajectory_current_points<trajectory_max_points){
				trajectory_current_points++
				trajectory.geometry.attributes['position'].array[ trajectory_current_points*3+0 ] = point.x;
				trajectory.geometry.attributes['position'].array[ trajectory_current_points*3+1 ] = point.y;
				trajectory.geometry.attributes['position'].array[ trajectory_current_points*3+2 ] = point.z;
			}else{
				shiftLeft(trajectory.geometry.attributes['position'].array, [point.x, point.y, point.z])
			}

			trajectory.geometry.attributes.position.needsUpdate = true
			trajectory.geometry.setDrawRange( 0, trajectory_current_points );
		}

		//
		this.speed = 0.05
		this.pivotWidth = 0.8
		this.leftThrottle = 0.0;
		this.rightThrottle = 0.0;
		this.m = new THREE.Matrix4().identity()
		this.m.setPosition(new THREE.Vector3(0,0,0))
		this.leftPivot = new THREE.Matrix4().makeTranslation(-this.pivotWidth/2,0,0)
		this.rightPivot = new THREE.Matrix4().makeTranslation(this.pivotWidth/2,0,0)

		window.car = this;

		const draw = ()=>{
			// Animate car body
			let right = new THREE.Matrix4().makeRotationY(this.rightThrottle*this.speed)

			let r = new THREE.Matrix4().identity()
			r=r.multiply(this.leftPivot)
			r=r.multiply(right)
			r=r.multiply(this.rightPivot)

			let left = new THREE.Matrix4().makeRotationY(-this.leftThrottle*this.speed)
			let l = new THREE.Matrix4().identity()
			l=l.multiply(this.rightPivot)
			l=l.multiply(left)
			l=l.multiply(this.leftPivot)

			this.m = this.m.multiply(r).multiply(l)
			
			body.position.setFromMatrixPosition(this.m)
			body.setRotationFromMatrix(this.m)
			body.scale.setFromMatrixScale(this.m)

			// Update car trajectory
			if(this.leftThrottle!=0 || this.rightThrottle!=0){
				update_trajectory(body.position);
			}

			// Draw
			renderer.render( scene, camera );

			// loop
			requestAnimationFrame( draw );
		}
		draw();

		window.addEventListener('resize', ()=>{
			const rect = this.element.getBoundingClientRect()
			camera.aspect = rect.width/rect.height
			camera.updateProjectionMatrix()
			renderer.setSize(rect.width, rect.height)
		})
	}
}
