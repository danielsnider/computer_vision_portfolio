// Parameters for bulge algorithm
var x_thin = 1.25;
var bulge_factor = 0.5;
var bulge_boost = 10;
var face_radius_scale = 0.8;
var grid_width = 50;
var grid_height = 50;

// Global OpenGL objects
var camera,
    renderer,
    scene,
    material,
    mesh,
    texture,
    geometry;

function start() {
	
	renderer.render(scene, camera);
	animate(new Date().getTime()); 
  if($) {
    $("p.processing").hide();
    $("h1 a").text("You've just been FaceBulged!");
  }
}

function getVertex(x, y, planeGeom) {
	width = planeGeom.widthSegments + 1;
	
	index = width * y + x;
	return planeGeom.vertices[index];
}

function euclid_distance(x1, y1, x2, y2)
{
	var x = (x2 - x1) * x_thin;
	var y = y2 - y1;
	return Math.sqrt(x * x + y * y);
}

function addBulge(geom, cx, cy, radius)
{
	var divWidth = geom.widthSegments;
	var divHeight = geom.heightSegments;
	var width = geom.width;
	var height = geom.height;
	var divSizeX = width/(divWidth*1.0);
	var divSizeY = height/(divHeight*1.0);
	var divCX = Math.round(((cx * 1.0) / width) * divWidth);
	var divCY = Math.round(((cy * 1.0) / height) * divHeight);
	var divX1 = Math.floor(((cx -radius*1.0) / width) * divWidth);
	var divX2 = Math.ceil(((cx+radius*1.0) / width) * divWidth);
	var divY1 = Math.floor(((cy-radius*1.0) / height) * divHeight);
	var divY2 = Math.ceil(((cy+radius*1.0) / height) * divHeight);

	for (var i=divX1; i<=divX2; i++)
	{
		for (var j=divY1; j<=divY2; j++)
		{
			if (euclid_distance(i*divSizeX, j*divSizeY, cx, cy) > radius)
				continue;

			var x = i * divSizeX;
			var y = j * divSizeY;
			var x_diff = (x - cx) * x_thin;
			var y_diff = y - cy;
			var z = Math.sqrt(radius*radius-x_diff*x_diff-y_diff*y_diff);

			var v = getVertex(i, j, geom);
			if (v != undefined)
			{
				v.z += z * bulge_factor + bulge_boost;
			}

		}
	}
}


function loadScene() {
    var world = document.getElementById('world'),
        WIDTH = 1200,
        HEIGHT = 1000,
        VIEW_ANGLE = 45,
        ASPECT = WIDTH / HEIGHT,
        NEAR = 0.1,
        FAR = 10000;
    
	// bulge grid parameters
	var face = face_info.faces[0]; // only use 1st face for now
	var img_height = face_info.height;
	var img_width = face_info.width;
	
	renderer = new THREE.WebGLRenderer();
	camera = new THREE.PerspectiveCamera(VIEW_ANGLE, ASPECT, NEAR, FAR);
	scene = new THREE.Scene();

	texture = THREE.ImageUtils.loadTexture(photo_path, {}, function() {
			// need to wait until texture is loaded to display stuff
            start();
		});
	material = new THREE.MeshBasicMaterial({map: texture});
	geometry= new THREE.PlaneGeometry(img_width, img_height, grid_width, grid_height);

	if (face_info.numfaces > 0)
	{
		for (var i=0; i<face_info.faces.length; i++) {
			var face = face_info.faces[i];
			var face_cX = face.x + face.width/2;
			var face_cY = face.y + face.height/2;
			var face_radius = face_radius_scale * (face.width+face.height)/2 ;
			addBulge(geometry, face_cX, face_cY, face_radius);
		}
	}
	
    mesh = new THREE.Mesh(geometry, material);
	mesh.geometry.dynamic = true;
    var pointLight = new THREE.PointLight(0xFFFFFF);

	mesh.position.x = 10;
	mesh.position.y = 25;

    camera.position.z = Math.max(img_height, img_width);    
    renderer.setSize(WIDTH, HEIGHT);
    scene.add(mesh);
    world.appendChild(renderer.domElement);
    pointLight.position.x = 50;
    pointLight.position.y = 50;
    pointLight.position.z = 130;
    scene.add(pointLight); 
}

function animate(t) {
	var rotation = 0.2 * Math.PI; // value between 0 - PI, how much the camera rotates in radians
	var period = 4000; // time in ms it takes for the camera to do a full trip (there and back)
	
	var img_height = face_info.height;
	var img_width = face_info.width;
	var multiplier = Math.max(img_height, img_width) * 2;    
	// spin the camera in a circle
	var scale_val = 2 * Math.abs((t % period)/(period) - 0.5);
	camera.position.x = Math.sin(scale_val*rotation-rotation/2)*multiplier;
	camera.position.y = 0;// 150;
	//camera.position.z = Math.sqrt(multiplier*multiplier - camera.position.x * camera.position.x);
	camera.position.z = Math.cos(scale_val*rotation-rotation/2)*multiplier;
	// you need to update lookAt every frame
    //mesh.geometry.verticesNeedUpdate = true;
	camera.lookAt(scene.position);
	renderer.render(scene, camera);	// renderer automatically clears unless autoClear = false
	window.requestAnimationFrame(animate, renderer.domElement);
};
