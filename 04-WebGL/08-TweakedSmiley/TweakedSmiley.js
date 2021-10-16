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

var vao_plane_rvg;
var vbo_plane_position_rvg;
var vbo_plane_texcoord_rvg;

var Smiley_Texture;

var mvpUniform_rvg;
var textureSamplerUniform_rvg;
var defaultColorUniform_rvg;

var PressedDigit_rvg = 0;

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
		case 49:
		case 97:
			PressedDigit_rvg = 1;
			break;

		case 50:
		case 98:
			PressedDigit_rvg = 2;
			break;			

		case 51:
		case 99:
			PressedDigit_rvg = 3;
			break;			

		case 52:
		case 100:
			PressedDigit_rvg = 4;
			break;			

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
									"in vec2 vTexcoord;" +
									"out vec2 out_Texcoord;" +
									"uniform mat4 u_mvpMatrix;" +
									"void main(void)" +
									"{"	+
										"gl_Position = u_mvpMatrix * vPosition;" +
										"out_Texcoord = vTexcoord;" +
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
									"in vec2 out_Texcoord;" +
									"uniform int u_whiteColor;" +
									"uniform highp sampler2D u_texture_sampler;" +
									"out vec4 fragColor;" +
									"void main(void)" +
									"{"	+
										"if(u_whiteColor == 1)" +
										"{" +
											"fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" +
										"}"+
										"else" +
										"{" +
											"fragColor = texture(u_texture_sampler, out_Texcoord);" +
										"}" +
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
	gl.bindAttribLocation(shaderProgramObject_rvg, WebGLMacros.RVG_ATTRIBUTE_TEXTURE0, "vTexcoord");

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
	textureSamplerUniform_rvg = gl.getUniformLocation(shaderProgramObject_rvg, "u_texture_sampler");
	defaultColorUniform_rvg = gl.getUniformLocation(shaderProgramObject_rvg, "u_whiteColor");	

	//Vertices Array Declaration
	var planeVertices_rvg = new Float32Array([
		1.0, 1.0, 0.0,		//Front
		-1.0, 1.0, 0.0,
		-1.0, -1.0, 0.0,
		1.0, -1.0, 0.0		
						]);

	var planeTexcoords_rvg = new Float32Array([
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
		1.0, 0.0
				]);


	//Create Cassette to use Buffer Array in Display()
	vao_plane_rvg = gl.createVertexArray();
	gl.bindVertexArray(vao_plane_rvg);
			/////////////////////////////// FOR VERTEX ///////////////////////////////
			//Generate Buffer Array
			vbo_plane_position_rvg = gl.createBuffer();

			//Bind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo_plane_position_rvg);

			//Fill Data into Buffer Array
			gl.bufferData(gl.ARRAY_BUFFER, planeVertices_rvg, gl.STATIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			gl.vertexAttribPointer(WebGLMacros.RVG_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);

			//Enable Attribute Pointer
			gl.enableVertexAttribArray(WebGLMacros.RVG_ATTRIBUTE_POSITION);

			//Unbind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, null);

			/////////////////////////////// FOR TEXCOORD ///////////////////////////////
			//Generate Buffer Array
			vbo_plane_texcoord_rvg = gl.createBuffer();

			//Bind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo_plane_texcoord_rvg);

			//Fill Data into Buffer Array
			gl.bufferData(gl.ARRAY_BUFFER, 2 * 4 * 4, gl.DYNAMIC_DRAW);

			//Tell him How to Read Data from Buffer Array
			gl.vertexAttribPointer(WebGLMacros.RVG_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false, 0, 0);

			//Enable Attribute Pointer
			gl.enableVertexAttribArray(WebGLMacros.RVG_ATTRIBUTE_TEXTURE0);

			//Unbind Buffer Array
			gl.bindBuffer(gl.ARRAY_BUFFER, null);

	//Stop Creating Cassette
	gl.bindVertexArray(null);

	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);

	///Set Clear Color
	gl.clearColor(0.0, 0.0, 0.0, 1.0);

	//Set Texture
	Smiley_Texture = gl.createTexture();
	Smiley_Texture.image = new Image();
	Smiley_Texture.image.src = "smiley.png";
	Smiley_Texture.image.onload = function()
	{
		gl.bindTexture(gl.TEXTURE_2D, Smiley_Texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);

		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);

		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, Smiley_Texture.image);
		gl.generateMipmap(gl.TEXTURE_2D);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}

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
	//Local Variable Declaration
	var TexCoords;// = new Float32Array();

	//Code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	//Use Shader Program Object
	gl.useProgram(shaderProgramObject_rvg);

	//Create Matrices
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	var translateMatrix = mat4.create();
	var rotateMatrix = mat4.create();

	//Translation fro PYRAMID
	mat4.translate(translateMatrix, translateMatrix, [0.0, 0.0, -4.0]);

	//Multiply Matrices
	modelViewMatrix = translateMatrix;

	mat4.multiply(modelViewProjectionMatrix,
					perspectiveProjectionMatrix_rvg,
					modelViewMatrix);

	//Set Uniform
	gl.uniformMatrix4fv(mvpUniform_rvg, false, modelViewProjectionMatrix);

	//Set Texture
	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, Smiley_Texture);
	gl.uniform1i(textureSamplerUniform_rvg, 0);	

	//Play Cassette
	gl.bindVertexArray(vao_plane_rvg);

	if (PressedDigit_rvg == 1)
	{
		TexCoords = new Float32Array([
										1.0, 1.0,
										0.0, 1.0,
										0.0, 0.0,
										1.0, 0.0
												 ]);
	}

	else if (PressedDigit_rvg == 2)
	{

		TexCoords = new Float32Array([
										0.5, 0.5,
										0.0, 0.5,
										0.0, 0.0,
										0.5, 0.0
												 ]);		
	}

	else if (PressedDigit_rvg == 3)
	{
		TexCoords = new Float32Array([
										2.0, 2.0,
										0.0, 2.0,
										0.0, 0.0,
										2.0, 0.0
												 ]);
	}

	else if (PressedDigit_rvg == 4)
	{
		TexCoords = new Float32Array([
										0.5, 0.5,
										0.5, 0.5,
										0.5, 0.5,
										0.5, 0.5
												 ]);
	}

	if (PressedDigit_rvg > 0 && PressedDigit_rvg < 5)
	{
		gl.uniform1i(defaultColorUniform_rvg, 0);
	}
	else
	{
		gl.uniform1i(defaultColorUniform_rvg, 1);
	}	

	//Bind Buffer Array
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_plane_texcoord_rvg);

	//Fill Data into Buffer Array
	gl.bufferData(gl.ARRAY_BUFFER, TexCoords, gl.DYNAMIC_DRAW);

	//Tell him How to Read Data from Buffer Array
	gl.vertexAttribPointer(WebGLMacros.RVG_ATTRIBUTE_TEXTURE0, 2, gl.FLOAT, false, 0, 0);

	//Enable Attribute Pointer
	gl.enableVertexAttribArray(WebGLMacros.RVG_ATTRIBUTE_TEXTURE0);

	//Unbind Buffer Array
	gl.bindBuffer(gl.ARRAY_BUFFER, null);	

	//Draw Object
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

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

}

function degToRad(degrees)
{
	//Code
	return degrees * (Math.PI / 180.0);
}

function uninitialize()
{
	//Code
	if (vao_plane_rvg)
	{
		gl.deleteVertexArray(vao_plane_rvg);
		vao_plane_rvg = null;
	}

	if (vbo_plane_position_rvg)
	{
		gl.deleteBuffer(vbo_plane_position_rvg);
		vbo_plane_position_rvg = null;
	}

	if (vbo_plane_texcoord_rvg)
	{
		gl.deleteBuffer(vbo_plane_texcoord_rvg);
		vbo_plane_texcoord_rvg = null;
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


