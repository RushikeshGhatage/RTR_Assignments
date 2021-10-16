//global variables
var canvas = null;
var gl = null;

var canvasOriginalWidth_rvg;
var canvasOriginalHeight_rvg;
var bFullscreen_rvg = false;

var requestAnimationFrame_rvg = window.requestAnimationFrame ||
							window.webkitRequestAnimationFrame ||
							window.mozRequestAnimationFrame ||
							window.oRequestAnimationFrame ||
							window.msRequestAnimationFrame;

var cancelAnimationFrame_rvg = window.cancelAnimationFrame ||
							window.webkitCancelRequestAnimationFrame || window.webkitCancelAnimationFrame ||
							window.mozCancelRequestAnimationFrame || window.mozCancelAnimationFrame ||
							window.oCancelRequestAnimationFrame || window.oCancelAnimationFrame ||
							window.msCancelRequestAnimationFrame || window.msCancelAnimationFrame;

const WebGLMacros = 
{
	RVG_ATTRIBUTE_POSITION:0,	//Key Value Coding
	RVG_ATTRIBUTE_COLOR:1,
	RVG_ATTRIBUTE_NORMAL:2,
	RVG_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObject_rvg;
var fragmentShaderObject_rvg;
var shaderProgramObject_rvg;

var vao_pyramid_rvg;
var vbo_pyramid_position_rvg;
var vbo_pyramid_color_rvg;

var vao_cube_rvg;
var vbo_cube_position_rvg;
var vbo_cube_color_rvg;

var PAngle_rvg = 0.0;
var CAngle_rvg = 0.0;

var perspectiveProjectionMatrix_rvg;

function main()
{
	//Get Canvas from DOM
	canvas = document.getElementById("RVG");
	if(!canvas)
	{
		console.log("Obtaining Canvas Failed!");
	}
	else
	{
		console.log("Canvas obtained Successfully!");
	}

	canvasOriginalWidth_rvg = canvas.width;
	canvasOriginalHeight_rvg = canvas.height;

	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
	window.addEventListener("resize", resize, false);

	init();

	resize();	//WarmUp Call

	draw();		//WarmUp Call
}

function toggleFullscreen()
{
	var fullscreen_element = document.fullscreenElement				||
								document.webkitFullscreenElement	||
								document.mozFullScreenElement 		||
								document.msFullscreenElement 		||
								null;

	if (fullscreen_element == null)
	{
		if (canvas.requestFullscreen)
		{
			canvas.requestFullscreen();
		}
		else if (canvas.webkitRequestFullscreen)
		{
			canvas.webkitRequestFullscreen();
		}
		else if (canvas.mozRequestFullScreen)
		{
			canvas.mozRequestFullScreen();
		}
		else if (canvas.msRequestFullscreen)
		{
			canvas.msRequestFullscreen();
		}

		bFullscreen_rvg = true;
	}
	else
	{
		if (document.exitFullscreen)
		{
			document.exitFullscreen();
		}
		else if (document.webkitExitFullscreen)
		{
			document.webkitExitFullscreen();
		}
		else if (document.mozCancelFullScreen)
		{
			document.mozCancelFullScreen();
		}
		else if (document.msExitFullscreen)
		{
			document.msExitFullscreen();
		}

		bFullscreen_rvg =  false;
	}
}

function keyDown(event)
{
	switch(event.keyCode)
	{
		case 70:	//F or f
			toggleFullscreen();
			break;

		case 27:	//Esc
			uninitialize();
			window.close();
			break;

		default:
			break;
	}
}

function mouseDown()
{
	
}

function init()
{
	//Get Drawing WebGL Context from Canvas
	gl = canvas.getContext("webgl2");
	if(!gl)
	{
		console.log("Obtaining WebGL2 Context Failed!");
	}
	else
	{
		console.log("WebGL2 Context obtained Successfully!");
	}

	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	// ****************** Vertex Shader ******************
	//Create Shader
	vertexShaderObject_rvg = gl.createShader(gl.VERTEX_SHADER);

	//Provide Source Code to Shader
	var vertexShaderSource_rvg =
								"#version 300 es" +
								"\n" +
								"in vec4 vPosition;" +
								"in vec4 vColor;" +
								"out vec4 out_color;" +
								"uniform mat4 u_mvp_matrix;" +
								"void main(void)" +
								"{" +
									"gl_Position = u_mvp_matrix * vPosition;" +
									"out_color = vColor;" +
								"}";

	//Feed to Shader Object
	gl.shaderSource(vertexShaderObject_rvg, vertexShaderSource_rvg);

	//Compile Shader
	gl.compileShader(vertexShaderObject_rvg);

	//Shader Compilation Error Checking
	if (gl.getShaderParameter(vertexShaderObject_rvg, gl.COMPILE_STATUS) == false)
	{
		var error_rvg = gl.getShaderInfoLog(vertexShaderObject_rvg);
		if (error_rvg.length > 0)
		{
			alert(error_rvg);
			uninitialize();
		}
	}
	else
	{
		console.log("Vertex Shader Compilation is done Successfully.");
	}

	// ****************** Fragment Shader ******************
	//Create Shader
	fragmentShaderObject_rvg = gl.createShader(gl.FRAGMENT_SHADER);

	//Provide Source Code to Shader
	var fragmentShaderSource_rvg =
								"#version 300 es" +
								"\n" +
								"precision highp float;" +
								"in vec4 out_color;" +
								"out vec4 fragColor;" +
								"void main(void)" +
								"{" +
									"fragColor = out_color;" +
								"}";

	//Feed to Shader Object
	gl.shaderSource(fragmentShaderObject_rvg, fragmentShaderSource_rvg);

	//Compile Shader
	gl.compileShader(fragmentShaderObject_rvg);

	//Shader Compilation Error Checking
	if (gl.getShaderParameter(fragmentShaderObject_rvg, gl.COMPILE_STATUS) == false)
	{
		var error_rvg = gl.getShaderInfoLog(fragmentShaderObject_rvg);
		if (error_rvg.length > 0)
		{
			alert(error_rvg);
			uninitialize();
		}
	}
	else
	{
		console.log("Fragment Shader Compilation is done Successfully.");
	}

	//Shader Linking Code
	//1)Create the Shader Program which capable of linking Shader.
	shaderProgramObject_rvg = gl.createProgram();

	//2)Attach whichever Shaders you have to this Shader Program Object.
	gl.attachShader(shaderProgramObject_rvg, vertexShaderObject_rvg);
	gl.attachShader(shaderProgramObject_rvg, fragmentShaderObject_rvg);

	//3)(Pre-Linking) Bind with Shader Program Object with Attributes
	gl.bindAttribLocation(shaderProgramObject_rvg, WebGLMacros.RVG_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(shaderProgramObject_rvg, WebGLMacros.RVG_ATTRIBUTE_COLOR, "vColors");

	//4)Then, Link all those attached Shader at once.
	gl.linkProgram(shaderProgramObject_rvg);

	//Shader Linkage Error Checking
	if (!gl.getProgramParameter(shaderProgramObject_rvg, gl.LINK_STATUS))
	{
		var error_rvg = gl.getProgramInfoLog(shaderProgramObject_rvg);
		if (error_rvg.length > 0)
		{
			alert(error_rvg);
			uninitialize();
		}
	}
	else
	{
		console.log("Program Shader Object Compilation is done Successfully.");
	}

	//Get MVP Uniform Location
	mvpUniform_rvg = gl.getUniformLocation(shaderProgramObject_rvg, "u_mvp_matrix");

	//Vertices Array Declaration
	var pyramidVertices_rvg = new Float32Array([
		0.0, 0.75, 0.0, 	//Apex
		-0.5, -0.25, 0.5,	//Front Left Bottom
		0.5, -0.25, 0.5, 	//Front Right Bottom

		0.0, 0.75, 0.0,		//Apex
		0.5, -0.25, 0.5,	//Right Left Bottom
		0.5, -0.25, -0.5,	//Right Right Bottom

		0.0, 0.75, 0.0,		//Apex
		0.5, -0.25, -0.5,	//Back Left Bottom
		-0.5, -0.25, -0.5,	//Back Right Bottom		

		0.0, 0.75, 0.0,		//Apex
		-0.5, -0.25, -0.5,	//Left Left Bottom
		-0.5, -0.25, 0.5	//Left Right Bottom		
												]);

	var pyramidColor_rvg = new Float32Array([
		1.0, 0.0, 0.0,	//Red 		//Front
		0.0, 1.0, 0.0,	//Green
		0.0, 0.0, 1.0,	//Blue

		1.0, 0.0, 0.0,	//Red 		//Right
		0.0, 0.0, 1.0,	//Blue
		0.0, 1.0, 0.0,	//Green

		1.0, 0.0, 0.0,	//Red 		//Back
		0.0, 1.0, 0.0,	//Green
		0.0, 0.0, 1.0,	//Blue

		1.0, 0.0, 0.0,	//Red 		//Left
		0.0, 0.0, 1.0,	//Blue
		0.0, 1.0, 0.0	//Green
											]);

	var cubeVertices_rvg = new Float32Array([
		0.5, 0.5, 0.5,		//Front Top Right
		-0.5, 0.5, 0.5,		//Front Top Left
		-0.5, -0.5, 0.5,		//Front Bottom Left
		0.5, -0.5, 0.5,		//Front Bottom Right
		
		0.5, 0.5, -0.5,		//Right Top Right
		0.5, 0.5, 0.5,		//Right Top Left
		0.5, -0.5, 0.5,		//Right Bottom Left
		0.5, -0.5, -0.5,		//Right Bottom Right

		-0.5, 0.5, -0.5,		//Back Top Right
		0.5, 0.5, -0.5,		//Back Top Left
		0.5, -0.5, -0.5,		//Back Bottom Left
		-0.5, -0.5, -0.5,	//Back Bottom Right

		-0.5, 0.5, 0.5,		//Left Top Right
		-0.5, 0.5, -0.5,		//Left Top Left
		-0.5, -0.5, -0.5,	//Left Bottom Left
		-0.5, -0.5, 0.5,		//Left Bottom Right

		0.5, 0.5, -0.5,		//Top Top Right
		-0.5, 0.5, -0.5,		//Top Top Left
		-0.5, 0.5, 0.5,		//Top Bottom Left
		0.5, 0.5, 0.5,		//Top Bottom Right

		0.5, -0.5, 0.5,		//Bottom Top Right
		-0.5, -0.5, 0.5,		//Bottom Top Left
		-0.5, -0.5, -0.5,	//Bottom Bottom Left
		0.5, -0.5, -0.5		//Bottom Bottom Right		
												]);

	var cubeColor_rvg = new Float32Array([
		1.0, 0.0, 0.0,	//Red 		//FRONT
		0.0, 1.0, 0.0,	//Green
		0.0, 0.0, 1.0,	//Blue
		1.0, 1.0, 0.0,	//Yellow

		0.0, 0.0, 1.0,	//Blue 		//RIGHT
		1.0, 0.0, 0.0,	//Red
		1.0, 1.0, 0.0,	//Yellow
		0.0, 1.0, 0.0,	//Green
		
		1.0, 1.0, 0.0,	//Yellow	//BACK
		0.0, 0.0, 1.0,	//Blue
		0.0, 1.0, 0.0,	//Green
		1.0, 0.0, 0.0,	//Red 

		0.0, 1.0, 0.0,	//Green 	//LEFT
		1.0, 1.0, 0.0,	//Yellow
		1.0, 0.0, 0.0,	//Red
		0.0, 0.0, 1.0,	//Blue

		0.0, 0.0, 1.0,	//Blue 	//TOP
		1.0, 1.0, 0.0,	//Yellow
		0.0, 1.0, 0.0,	//Green
		1.0, 0.0, 0.0,	//Red

		1.0, 1.0, 0.0,	//Yellow 	//BOTTOM
		0.0, 0.0, 1.0,	//Blue
		1.0, 0.0, 0.0,	//Red
		0.0, 1.0, 0.0,	//Green
											]);


	//Create Cassette to use Buffer Array in Display()
	vao_pyramid_rvg = gl.createVertexArray();
	gl.bindVertexArray(vao_pyramid_rvg);
			/////////////////////////////// FOR VERTEX ///////////////////////////////
			//Generate Buffer Array
			vbo_pyramid_position_rvg = gl.createBuffer();

			//Bind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_position_rvg);

			//Fill Data into Buffer Array
			gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices_rvg, gl.STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			gl.vertexAttribPointer(WebGLMacros.RVG_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);

			//Enable Attribute Pointer
			gl.enableVertexAttribArray(WebGLMacros.RVG_ATTRIBUTE_POSITION);

			//Unbind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, null);

			/////////////////////////////// FOR COLOR ///////////////////////////////
			//Generate Buffer Array
			vbo_pyramid_color_rvg = gl.createBuffer();

			//Bind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_color_rvg);

			//Fill Data into Buffer Array
			gl.bufferData(gl.ARRAY_BUFFER, pyramidColor_rvg, gl.STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			gl.vertexAttribPointer(WebGLMacros.RVG_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);

			//Enable Attribute Pointer
			gl.enableVertexAttribArray(WebGLMacros.RVG_ATTRIBUTE_COLOR);

			//Unbind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, null);

	//Stop Creating Cassette
	gl.bindVertexArray(null);

	vao_cube_rvg = gl.createVertexArray();
	gl.bindVertexArray(vao_cube_rvg);
			/////////////////////////////// FOR VERTEX ///////////////////////////////
			//Generate Buffer Array
			vbo_cube_position_rvg = gl.createBuffer();

			//Bind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_position_rvg);

			//Fill Data into Buffer Array
			gl.bufferData(gl.ARRAY_BUFFER, cubeVertices_rvg, gl.STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			gl.vertexAttribPointer(WebGLMacros.RVG_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);

			//Enable Attribute Pointer
			gl.enableVertexAttribArray(WebGLMacros.RVG_ATTRIBUTE_POSITION);

			//Unbind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, null);

			/////////////////////////////// FOR COLOR ///////////////////////////////
			//Generate Buffer Array
			vbo_cube_color_rvg = gl.createBuffer();

			//Bind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_color_rvg);

			//Fill Data into Buffer Array
			gl.bufferData(gl.ARRAY_BUFFER, cubeColor_rvg, gl.STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			gl.vertexAttribPointer(WebGLMacros.RVG_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);

			//Enable Attribute Pointer
			gl.enableVertexAttribArray(WebGLMacros.RVG_ATTRIBUTE_COLOR);

			//Unbind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, null);

	//Stop Creating Cassette
	gl.bindVertexArray(null);

	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);

	///Set Clear Color
	gl.clearColor(0.0, 0.0, 0.0, 1.0);

	//Initialize Projection Matrix
	perspectiveProjectionMatrix_rvg = mat4.create();
}

function resize()
{
	if (bFullscreen_rvg == true)
	{
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}
	else
	{
		canvas.width = canvasOriginalWidth_rvg;
		canvas.height = canvasOriginalHeight_rvg;
	}

	//Set Viewport
	gl.viewport(0, 0 , canvas.width, canvas.height);

	//Set Projection
	mat4.perspective(perspectiveProjectionMatrix_rvg,
					45.0,
					parseFloat(canvas.width) / parseFloat(canvas.height),
					0.1,
					100.0);
}

function draw()
{
	gl.clear(gl.COLOR_BUFFER_BIT);

	//Use Shader Program Object
	gl.useProgram(shaderProgramObject_rvg);

	//Create Matrices
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	
	//Translation fro PYRAMID
	mat4.translate(translateMatrix, translateMatrix, [-1.25, -0.2, -3.5]);
	mat4.rotateY(rotateMatrix, rotateMatrix, degToRad(PAngle_rvg));

	//Multiply Matrices
	mat4.multiply(modelViewMatrix,
					translateMatrix,
					rotateMatrix);

	mat4.multiply(modelViewProjectionMatrix,
					perspectiveProjectionMatrix_rvg,
					modelViewMatrix);

	//Set Uniform
	gl.uniformMatrix4fv(mvpUniform_rvg, false, modelViewProjectionMatrix);

	//Play Cassette
	gl.bindVertexArray(vao_pyramid_rvg);

	//Draw Object
	gl.drawArrays(gl.TRIANGLES, 0, 12);

	//Stop Playing Cassette
	gl.bindVertexArray(null);


	//Create Matrices
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();
	
	//Translation fro CUBE
	mat4.translate(translateMatrix, translateMatrix, [1.25, 0.0, -3.5]);
	
	mat4.rotateX(rotateMatrix, rotateMatrix, degToRad(CAngle_rvg));
	mat4.rotateY(rotateMatrix, rotateMatrix, degToRad(CAngle_rvg));
	mat4.rotateZ(rotateMatrix, rotateMatrix, degToRad(CAngle_rvg));

	//Multiply Matrices
	mat4.multiply(modelViewMatrix,
					translateMatrix,
					rotateMatrix);

	mat4.multiply(modelViewProjectionMatrix,
					perspectiveProjectionMatrix_rvg,
					modelViewMatrix);

	//Set Uniform
	gl.uniformMatrix4fv(mvpUniform_rvg, false, modelViewProjectionMatrix);

	//Play Cassette
	gl.bindVertexArray(vao_cube_rvg);

	//Draw Object
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

	//Stop Playing Cassette
	gl.bindVertexArray(null);

	//Stop using Shader Program Object
	gl.useProgram(null);

	//Animation Loop
	update();
	requestAnimationFrame_rvg(draw, canvas);
}

function update()
{
	//Code
	PAngle_rvg += 1.0;
	if (PAngle_rvg >= 360.0)
	{
		PAngle_rvg = 0.0;
	}

	CAngle_rvg += 1.0;
	if (CAngle_rvg >= 360.0)
	{
		CAngle_rvg = 0.0;
	}
}

function degToRad(degrees)
{
	//Code
	return degrees * (Math.PI / 180.0);
}

function uninitialize()
{
	//Code
	if (vao_pyramid_rvg)
	{
		gl.deleteVertexArray(vao_pyramid_rvg);
		vao_pyramid_rvg = null;
	}

	if (vbo_pyramid_position_rvg)
	{
		gl.deleteBuffer(vbo_pyramid_position_rvg);
		vbo_pyramid_position_rvg = null;
	}

	if (vbo_pyramid_color_rvg)
	{
		gl.deleteBuffer(vbo_pyramid_color_rvg);
		vbo_pyramid_color_rvg = null;
	}

	if (vao_cube_rvg)
	{
		gl.deleteVertexArray(vao_cube_rvg);
		vao_cube_rvg = null;
	}

	if (vbo_cube_position_rvg)
	{
		gl.deleteBuffer(vbo_cube_position_rvg);
		vbo_cube_position_rvg = null;
	}

	if (vbo_cube_color_rvg)
	{
		gl.deleteBuffer(vbo_cube_color_rvg);
		vbo_cube_color_rvg = null;
	}

	if (shaderProgramObject_rvg)
	{
		if (fragmentShaderObject_rvg)
		{
			gl.detachShader(shaderProgramObject_rvg, fragmentShaderObject_rvg);
			gl.deleteShader(fragmentShaderObject_rvg);
			fragmentShaderObject_rvg = null;
		}

		if (vertexShaderObject_rvg)
		{
			gl.detachShader(shaderProgramObject_rvg, vertexShaderObject_rvg);
			gl.deleteShader(vertexShaderObject_rvg);
			vertexShaderObject_rvg = null;
		}

		gl.deleteProgram(shaderProgramObject_rvg);
		shaderProgramObject_rvg = null;
	}
}


