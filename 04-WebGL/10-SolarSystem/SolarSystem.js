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

var year_rvg = 0;
var day_rvg = 0;
var arr_rvg = new Array();

var modelViewProjectionMatrix;

var vertexShaderObject_rvg;
var fragmentShaderObject_rvg;
var shaderProgramObject_rvg;

var sphere = null;

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

		case 89: 	//S or s
			year_rvg = (year_rvg + 3) % 360;
		break;

		case 84: 	//D or d
			year_rvg = (year_rvg - 3) % 360;
		break;

		case 68: 	//E or e
			day_rvg = (day_rvg + 3) % 360;
		break;

		case 83: 	//R or r
			day_rvg = (day_rvg - 3) % 360;
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
									"uniform mat4 u_mvpMatrix;" +
									"void main(void)" +
									"{"	+
										"gl_Position = u_mvpMatrix * vPosition;" +
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
									"{"	+
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
	gl.bindAttribLocation(shaderProgramObject_rvg, WebGLMacros.RVG_ATTRIBUTE_COLOR, "vColor");

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
	mvpUniform_rvg = gl.getUniformLocation(shaderProgramObject_rvg, "u_mvpMatrix");

	//Vertices Array Declaration
	sphere = new Mesh();
	makeSphere(sphere, 2.0, 40, 40);

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
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	//Use Shader Program Object
	gl.useProgram(shaderProgramObject_rvg);

	//Create Matrices
	modelViewProjectionMatrix = mat4.create();
	translateMatrix = mat4.create();
	YrotateMatrix = mat4.create();
	DrotateMatrix = mat4.create();
	scaleMatrix = mat4.create();
	
	mat4.translate(translateMatrix, translateMatrix, [0.0, 0.0, -15.0]);
	modelViewProjectionMatrix = translateMatrix;

	pushMatrix_rvg();						
	 		mat4.multiply(modelViewProjectionMatrix,
						perspectiveProjectionMatrix_rvg,
						modelViewProjectionMatrix);

			//Set Uniform
			gl.uniformMatrix4fv(mvpUniform_rvg, false, modelViewProjectionMatrix);

			gl.vertexAttrib3f(WebGLMacros.RVG_ATTRIBUTE_COLOR, 1.0, 1.0, 0.0);

			//Play Cassette
			sphere.draw();

	popMatrix_rvg();

	mat4.rotateY(modelViewProjectionMatrix, modelViewProjectionMatrix, degToRad(year_rvg));
	mat4.translate(modelViewProjectionMatrix, modelViewProjectionMatrix, [7.0, 0.0, 0.0]);
	
	pushMatrix_rvg();
		mat4.rotateX(modelViewProjectionMatrix, modelViewProjectionMatrix, degToRad(day_rvg));
		mat4.scale(modelViewProjectionMatrix, modelViewProjectionMatrix, [0.5, 0.5, 0.5]);
		
		mat4.multiply(modelViewProjectionMatrix,
						perspectiveProjectionMatrix_rvg,
						modelViewProjectionMatrix);	

		//Set Uniform
		gl.uniformMatrix4fv(mvpUniform_rvg, false, modelViewProjectionMatrix);

		gl.vertexAttrib3f(WebGLMacros.RVG_ATTRIBUTE_COLOR, 0.4, 0.9, 1.0);

		//Play Cassette
		sphere.draw();

	popMatrix_rvg();

	//Stop using Shader Program Object
	gl.useProgram(null);

	//Animation Loop
	update();
	requestAnimationFrame_rvg(draw, canvas);
}

function pushMatrix_rvg()
{
	//Code
	var temp = modelViewProjectionMatrix.slice(0);
	arr_rvg.push(temp);	
}

function popMatrix_rvg()
{
	//Code
	modelViewProjectionMatrix = arr_rvg.pop();
}

function update()
{
	//Code

}

function degToRad(degrees)
{
	//Code
	return degrees * (Math.PI / 180.0);
}

function uninitialize()
{
	//Code
	if (sphere)
	{
		sphere.deallocate();
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


