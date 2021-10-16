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
	RVG_ATTRIBUTE_NORMAL:1,
	RVG_ATTRIBUTE_NORMAL:2,
	RVG_ATTRIBUTE_TEXTURE0:3,
};

///////////////////// For Per Vertex /////////////////////
var vertexShaderObject_pv_rvg;
var fragmentShaderObject_pv_rvg;
var shaderProgramObject_pv_rvg;

var LaUniform_pv_rvg, LdUniform_pv_rvg, LsUniform_pv_rvg;
var KaUniform_pv_rvg, KdUniform_pv_rvg, KsUniform_pv_rvg;
var shininessUniform_pv_rvg;
var lightPositionUniform_pv_rvg;

var modelMatrixUniform_pv_rvg;
var viewMatrixUniform_pv_rvg;
var perspectiveProjectionUniform_pv_rvg;
var LKeyPressedUniform_pv_rvg; 

///////////////////// For Per Fragment /////////////////////
var vertexShaderObject_pf_rvg;
var fragmentShaderObject_pf_rvg;
var shaderProgramObject_pf_rvg;

var LaUniform_pf_rvg, LdUniform_pf_rvg, LsUniform_pf_rvg;
var KaUniform_pf_rvg, KdUniform_pf_rvg, KsUniform_pf_rvg;
var shininessUniform_pf_rvg;
var lightPositionUniform_pf_rvg;

var modelMatrixUniform_pf_rvg;
var viewMatrixUniform_pf_rvg;
var perspectiveProjectionUniform_pf_rvg;
var LKeyPressedUniform_pf_rvg; 

///////////////////// Common /////////////////////
var sphere_rvg = null;

var bLight_rvg;
var bPerVertex_rvg = true;
var bPerFragment_rvg = false;

var lightPosition_rvg = [100.0, 100.0, 100.0, 1.0];
var materialShininess_rvg = 128.0;

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
		case 76:
			if (bLight_rvg == true)
			{
				bLight_rvg = false;
			}
			else
			{
				bLight_rvg = true;
			}
			break;

		case 84: 	//T or t
			if (bPerVertex_rvg == true)
			{
				bPerVertex_rvg = false;
				bPerFragment_rvg = true;
			}
			else
			{
				bPerFragment_rvg = false;
				bPerVertex_rvg = true;
			}
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


//////////////////////////////////////////////////////// PER VERTEX //////////////////////////////////////////////////////////
	// ****************** Vertex Shader ******************
	//Create Shader
	vertexShaderObject_pv_rvg = gl.createShader(gl.VERTEX_SHADER);

	//Provide Source Code to Shader
	var vertexShaderSource_pv_rvg =
									"#version 300 es" +
									"\n" +
									"in vec4 vPosition_pv;" +
									"in vec3 vNormal_pv;" +
									
									"uniform vec3 u_la_pv;" +
									"uniform vec3 u_ld_pv;" +
									"uniform vec3 u_ls_pv;" +
									"uniform vec4 u_light_position_pv;" +
									
									"uniform vec3 u_ka_pv;" +
									"uniform vec3 u_kd_pv;" +
									"uniform vec3 u_ks_pv;" +
									"uniform mediump float u_materialShininess_pv;" +

									"uniform mat4 u_modelMatrix_pv;" +
									"uniform mat4 u_viewMatrix_pv;" +
									"uniform mat4 u_projectionMatrix_pv;" +

									"uniform mediump int u_lKeyPressed_pv;" +

									"out vec3 fong_ads_light_pv;" +

									"void main(void)" +
									"{"	+
										"if(u_lKeyPressed_pv == 1)" +
										"{" +
											"vec4 eye_coordinates = (u_viewMatrix_pv * u_modelMatrix_pv * vPosition_pv);" +
											"vec3 transformed_normal_pv = normalize(mat3(u_viewMatrix_pv * u_modelMatrix_pv) * vNormal_pv);" +
											"vec3 light_direction_pv = normalize(vec3(u_light_position_pv - eye_coordinates));" +
											"vec3 reflection_vector = reflect(-light_direction_pv, transformed_normal_pv);" +
											"vec3 view_vector_pv = normalize(-eye_coordinates.xyz);" +
											"vec3 ambient = u_la_pv * u_ka_pv;" +
											"vec3 diffuse = u_ld_pv * u_kd_pv * max(dot(light_direction_pv, transformed_normal_pv), 0.0);" +
											"vec3 specular = u_ls_pv * u_ks_pv * pow(max(dot(reflection_vector, view_vector_pv), 0.0), u_materialShininess_pv);" +
											"fong_ads_light_pv = ambient + diffuse + specular;" +
										"}" +
										"else" +
										"{" +
											"fong_ads_light_pv = vec3(1.0, 1.0, 1.0);" +
										"}" +
										"gl_Position = u_projectionMatrix_pv * u_viewMatrix_pv * u_modelMatrix_pv * vPosition_pv;" +
									"}";

	//Feed to Shader Object
	gl.shaderSource(vertexShaderObject_pv_rvg, vertexShaderSource_pv_rvg);

	//Compile Shader
	gl.compileShader(vertexShaderObject_pv_rvg);

	//Shader Compilation Error Checking
	if (gl.getShaderParameter(vertexShaderObject_pv_rvg, gl.COMPILE_STATUS) == false)
	{
		var error_rvg = gl.getShaderInfoLog(vertexShaderObject_pv_rvg);
		if (error_rvg.length > 0)
		{
			alert(error_rvg);
			uninitialize();
		}
	}
	else
	{
		console.log("PER VERTEX : Vertex Shader Compilation is done Successfully.");
	}

	// ****************** Fragment Shader ******************
	//Create Shader
	fragmentShaderObject_pv_rvg = gl.createShader(gl.FRAGMENT_SHADER);

	//Provide Source Code to Shader
	var fragmentShaderSource_pv_rvg =
								"#version 300 es" +
								"\n" +
								"precision highp float;" +
								"in vec3 fong_ads_light_pv;" +
								"out vec4 fragColor_pv;" +
								"void main(void)" +
								"{"	+
									"fragColor_pv = vec4(fong_ads_light_pv, 1.0);" +
								"}";

	//Feed to Shader Object
	gl.shaderSource(fragmentShaderObject_pv_rvg, fragmentShaderSource_pv_rvg);

	//Compile Shader
	gl.compileShader(fragmentShaderObject_pv_rvg);

	//Shader Compilation Error Checking
	if (gl.getShaderParameter(fragmentShaderObject_pv_rvg, gl.COMPILE_STATUS) == false)
	{
		var error_rvg = gl.getShaderInfoLog(fragmentShaderObject_pv_rvg);
		if (error_rvg.length > 0)
		{
			alert(error_rvg);
			uninitialize();
		}
	}
	else
	{
		console.log("PER VERTEX : Fragment Shader Compilation is done Successfully.");
	}

	//Shader Linking Code
	//1)Create the Shader Program which capable of linking Shader.
	shaderProgramObject_pv_rvg = gl.createProgram();

	//2)Attach whichever Shaders you have to this Shader Program Object.
	gl.attachShader(shaderProgramObject_pv_rvg, vertexShaderObject_pv_rvg);
	gl.attachShader(shaderProgramObject_pv_rvg, fragmentShaderObject_pv_rvg);

	//3)(Pre-Linking) Bind with Shader Program Object with Attributes
	gl.bindAttribLocation(shaderProgramObject_pv_rvg, WebGLMacros.RVG_ATTRIBUTE_POSITION, "vPosition_pv");
	gl.bindAttribLocation(shaderProgramObject_pv_rvg, WebGLMacros.RVG_ATTRIBUTE_NORMAL, "vNormal_pv");

	//4)Then, Link all those attached Shader at once.
	gl.linkProgram(shaderProgramObject_pv_rvg);

	//Shader Linkage Error Checking
	if (!gl.getProgramParameter(shaderProgramObject_pv_rvg, gl.LINK_STATUS))
	{
		var error_rvg = gl.getProgramInfoLog(shaderProgramObject_pv_rvg);
		if (error_rvg.length > 0)
		{
			alert(error_rvg);
			uninitialize();
		}
	}
	else
	{
		console.log("PER VERTEX : Program Shader Object Compilation is done Successfully.");
	}

	//Get MVP Uniform Location
	LaUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_la_pv");
	LdUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_ld_pv");
	LsUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_ls_pv");
	lightPositionUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_light_position_pv");

	KaUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_ka_pv");
	KdUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_kd_pv");
	KsUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_ks_pv");
	shininessUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_materialShininess_pv");

	LKeyPressedUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_lKeyPressed_pv");

	modelMatrixUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_modelMatrix_pv");
	viewMatrixUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_viewMatrix_pv");
	perspectiveProjectionUniform_pv_rvg = gl.getUniformLocation(shaderProgramObject_pv_rvg, "u_projectionMatrix_pv");

//////////////////////////////////////////////////////// PER FRAGMENT ////////////////////////////////////////////////////////
	// ****************** Vertex Shader ******************
	//Create Shader
	vertexShaderObject_pf_rvg = gl.createShader(gl.VERTEX_SHADER);

	//Provide Source Code to Shader
	var vertexShaderSource_pf_rvg =
									"#version 300 es" +
									"\n" +
									"in vec4 vPosition_pf;" +
									"in vec3 vNormal_pf;" +

									"out vec3 transformed_normal_pf;" +
									"out vec3 light_direction_pf;" +
									"out vec3 view_vector_pf;" +
									
									"uniform vec4 u_light_position_pf;" +
									"uniform mediump int u_lKeyPressed_pf;" +

									"uniform mat4 u_modelMatrix_pf;" +
									"uniform mat4 u_viewMatrix_pf;" +
									"uniform mat4 u_projectionMatrix_pf;" +

									"void main(void)" +
									"{"	+
										"if(u_lKeyPressed_pf == 1)" +
										"{" +
											"vec4 eye_coordinates = (u_viewMatrix_pf * u_modelMatrix_pf * vPosition_pf);" +
											"transformed_normal_pf = mat3(u_viewMatrix_pf * u_modelMatrix_pf) * vNormal_pf;" +
											"light_direction_pf = vec3(u_light_position_pf - eye_coordinates);" +
											"view_vector_pf = (-eye_coordinates.xyz);" +
										"}" +
										"gl_Position = u_projectionMatrix_pf * u_viewMatrix_pf * u_modelMatrix_pf * vPosition_pf;" +
									"}";

	//Feed to Shader Object
	gl.shaderSource(vertexShaderObject_pf_rvg, vertexShaderSource_pf_rvg);

	//Compile Shader
	gl.compileShader(vertexShaderObject_pf_rvg);

	//Shader Compilation Error Checking
	if (gl.getShaderParameter(vertexShaderObject_pf_rvg, gl.COMPILE_STATUS) == false)
	{
		var error_rvg = gl.getShaderInfoLog(vertexShaderObject_pf_rvg);
		if (error_rvg.length > 0)
		{
			alert(error_rvg);
			uninitialize();
		}
	}
	else
	{
		console.log("PER FRAGMENT : Vertex Shader Compilation is done Successfully.");
	}

	// ****************** Fragment Shader ******************
	//Create Shader
	fragmentShaderObject_pf_rvg = gl.createShader(gl.FRAGMENT_SHADER);

	//Provide Source Code to Shader
	var fragmentShaderSource_pf_rvg =
									"#version 300 es" +
									"\n" +
									"precision highp float;" +
									"out vec4 fragColor_pf;" +

									"in vec3 transformed_normal_pf;" +
									"in vec3 light_direction_pf;" +
									"in vec3 view_vector_pf;" +
									
									"uniform vec3 u_la_pf;" +
									"uniform vec3 u_ld_pf;" +
									"uniform vec3 u_ls_pf;" +

									"uniform vec3 u_ka_pf;" +
									"uniform vec3 u_kd_pf;" +
									"uniform vec3 u_ks_pf;" +
									"uniform float u_materialShininess_pf;" +
									"vec3 fong_ads_light_pf;" +

									"uniform mediump int u_lKeyPressed_pf;" +

									"void main(void)" +
									"{"	+

									"if(u_lKeyPressed_pf == 1)" +
										"{" +
											"vec3 normalized_tranformed_normal = normalize(transformed_normal_pf);" +
											"vec3 normalized_light_direction = normalize(light_direction_pf);" +
											"vec3 normalized_view_vector = normalize(view_vector_pf);" +
											"vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tranformed_normal);" +
											"vec3 ambient = u_la_pf * u_ka_pf;" +
											"vec3 diffuse = u_ld_pf * u_kd_pf * max(dot(normalized_light_direction, normalized_tranformed_normal), 0.0);" +
											"vec3 specular = u_ls_pf * u_ks_pf * pow(max(dot(reflection_vector, normalized_view_vector), 0.0), u_materialShininess_pf);" +
											"fong_ads_light_pf = ambient + diffuse + specular;" +
										"}" +
										"else" +
										"{" +
											"fong_ads_light_pf = vec3(1.0, 1.0, 1.0);" +
										"}" +
										"fragColor_pf = vec4(fong_ads_light_pf, 1.0);" +
									"}";

	//Feed to Shader Object
	gl.shaderSource(fragmentShaderObject_pf_rvg, fragmentShaderSource_pf_rvg);

	//Compile Shader
	gl.compileShader(fragmentShaderObject_pf_rvg);

	//Shader Compilation Error Checking
	if (gl.getShaderParameter(fragmentShaderObject_pf_rvg, gl.COMPILE_STATUS) == false)
	{
		var error_rvg = gl.getShaderInfoLog(fragmentShaderObject_pf_rvg);
		if (error_rvg.length > 0)
		{
			alert(error_rvg);
			uninitialize();
		}
	}
	else
	{
		console.log("PER FRAGMENT : Fragment Shader Compilation is done Successfully.");
	}

	//Shader Linking Code
	//1)Create the Shader Program which capable of linking Shader.
	shaderProgramObject_pf_rvg = gl.createProgram();

	//2)Attach whichever Shaders you have to this Shader Program Object.
	gl.attachShader(shaderProgramObject_pf_rvg, vertexShaderObject_pf_rvg);
	gl.attachShader(shaderProgramObject_pf_rvg, fragmentShaderObject_pf_rvg);

	//3)(Pre-Linking) Bind with Shader Program Object with Attributes
	gl.bindAttribLocation(shaderProgramObject_pf_rvg, WebGLMacros.RVG_ATTRIBUTE_POSITION, "vPosition_pf");
	gl.bindAttribLocation(shaderProgramObject_pf_rvg, WebGLMacros.RVG_ATTRIBUTE_NORMAL, "vNormal_pf");

	//4)Then, Link all those attached Shader at once.
	gl.linkProgram(shaderProgramObject_pf_rvg);

	//Shader Linkage Error Checking
	if (!gl.getProgramParameter(shaderProgramObject_pf_rvg, gl.LINK_STATUS))
	{
		var error_rvg = gl.getProgramInfoLog(shaderProgramObject_pf_rvg);
		if (error_rvg.length > 0)
		{
			alert(error_rvg);
			uninitialize();
		}
	}
	else
	{
		console.log("PER FRAGMENT : Program Shader Object Compilation is done Successfully.");
	}

	//Get MVP Uniform Location
	LaUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_la_pf");
	LdUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_ld_pf");
	LsUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_ls_pf");
	lightPositionUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_light_position_pf");

	KaUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_ka_pf");
	KdUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_kd_pf");
	KsUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_ks_pf");
	shininessUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_materialShininess_pf");

	LKeyPressedUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_lKeyPressed_pf");

	modelMatrixUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_modelMatrix_pf");
	viewMatrixUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_viewMatrix_pf");
	perspectiveProjectionUniform_pf_rvg = gl.getUniformLocation(shaderProgramObject_pf_rvg, "u_projectionMatrix_pf");

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

	//Vertices Array Declaration
	sphere_rvg = new Mesh();
	makeSphere(sphere_rvg, 2.0, 50, 50);

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

	if (bPerVertex_rvg == true)
	{
		//Use Shader Program Object
		gl.useProgram(shaderProgramObject_pv_rvg);

		if (bLight_rvg == true)
		{
			gl.uniform1i(LKeyPressedUniform_pv_rvg, 1);
				
			gl.uniform3f(LaUniform_pv_rvg, 0.0, 0.0, 0.0, 1.0);
			gl.uniform3f(LdUniform_pv_rvg, 1.0, 1.0, 1.0, 1.0);
			gl.uniform3f(LsUniform_pv_rvg, 1.0, 1.0, 1.0, 1.0);

			gl.uniform3f(KaUniform_pv_rvg, 0.0, 0.0, 0.0, 1.0);
			gl.uniform3f(KdUniform_pv_rvg, 1.0, 1.0, 1.0, 1.0);
			gl.uniform3f(KsUniform_pv_rvg, 1.0, 1.0, 1.0, 1.0);

			gl.uniform1f(shininessUniform_pv_rvg, materialShininess_rvg);
			gl.uniform4fv(lightPositionUniform_pv_rvg, lightPosition_rvg);
		}
		else
		{
			gl.uniform1i(LKeyPressedUniform_pv_rvg, 0);
		}
	}
	else
	{
		//Use Shader Program Object
		gl.useProgram(shaderProgramObject_pf_rvg);

		if (bLight_rvg == true)
		{
			gl.uniform1i(LKeyPressedUniform_pf_rvg, 1);
				
			gl.uniform3f(LaUniform_pf_rvg, 0.0, 0.0, 0.0, 1.0);
			gl.uniform3f(LdUniform_pf_rvg, 1.0, 1.0, 1.0, 1.0);
			gl.uniform3f(LsUniform_pf_rvg, 1.0, 1.0, 1.0, 1.0);

			gl.uniform3f(KaUniform_pf_rvg, 0.0, 0.0, 0.0, 1.0);
			gl.uniform3f(KdUniform_pf_rvg, 1.0, 1.0, 1.0, 1.0);
			gl.uniform3f(KsUniform_pf_rvg, 1.0, 1.0, 1.0, 1.0);

			gl.uniform1f(shininessUniform_pf_rvg, materialShininess_rvg);
			gl.uniform4fv(lightPositionUniform_pf_rvg, lightPosition_rvg);
		}
		else
		{
			gl.uniform1i(LKeyPressedUniform_pv_rvg, 0);
		}
	}

	//Create Matrices
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();
	var translateMatrix = mat4.create();
	
	mat4.translate(translateMatrix, translateMatrix, [0.0, 0.0, -7.0]);
	
	//Multiply Matrices
	modelMatrix = translateMatrix;

	projectionMatrix = perspectiveProjectionMatrix_rvg;


	if (bPerVertex_rvg == true)
	{
		//Set Uniform
		gl.uniformMatrix4fv(modelMatrixUniform_pv_rvg, false, modelMatrix);
		gl.uniformMatrix4fv(viewMatrixUniform_pv_rvg, false, viewMatrix);
		gl.uniformMatrix4fv(perspectiveProjectionUniform_pv_rvg, false, projectionMatrix);
	}
	else
	{
		//Set Uniform
		gl.uniformMatrix4fv(modelMatrixUniform_pf_rvg, false, modelMatrix);
		gl.uniformMatrix4fv(viewMatrixUniform_pf_rvg, false, viewMatrix);
		gl.uniformMatrix4fv(perspectiveProjectionUniform_pf_rvg, false, projectionMatrix);		
	}

	//Play Cassette
	sphere_rvg.draw();

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
	if (sphere_rvg)
	{
		sphere_rvg.deallocate();
	}

	if (shaderProgramObject_pv_rvg)
	{
		if (fragmentShaderObject_pv_rvg)
		{
			gl.detachShader(shaderProgramObject_pv_rvg, fragmentShaderObject_pv_rvg);
			gl.deleteShader(fragmentShaderObject_pv_rvg);
			fragmentShaderObject_pv_rvg = null;
		}

		if (vertexShaderObject_pv_rvg)
		{
			gl.detachShader(shaderProgramObject_pv_rvg, vertexShaderObject_pv_rvg);
			gl.deleteShader(vertexShaderObject_pv_rvg);
			vertexShaderObject_pv_rvg = null;
		}

		gl.deleteProgram(shaderProgramObject_pv_rvg);
		shaderProgramObject_pv_rvg = null;
	}

	if (shaderProgramObject_pf_rvg)
	{
		if (fragmentShaderObject_pf_rvg)
		{
			gl.detachShader(shaderProgramObject_pf_rvg, fragmentShaderObject_pf_rvg);
			gl.deleteShader(fragmentShaderObject_pf_rvg);
			fragmentShaderObject_pf_rvg = null;
		}

		if (vertexShaderObject_pf_rvg)
		{
			gl.detachShader(shaderProgramObject_pf_rvg, vertexShaderObject_pf_rvg);
			gl.deleteShader(vertexShaderObject_pf_rvg);
			vertexShaderObject_pf_rvg = null;
		}

		gl.deleteProgram(shaderProgramObject_pf_rvg);
		shaderProgramObject_pf_rvg = null;
	}
}


