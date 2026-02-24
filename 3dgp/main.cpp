#include <iostream>
#include <GL/glew.h>
#include <3dgl/3dgl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;


C3dglProgram program;
bool liiii = true;
bool liiii1 = true;
unsigned vertBuff;
unsigned normBuff;
unsigned indBuff;
float vertices[] = {

 9, 8, -4,   18, 8, -4,   13, 1, 0,
 9, 8,  4,   18, 8,  4,   13, 1, 0,

 9, 8, -4,   9, 8,  4,   13, 1, 0,
 18, 8, -4,   18, 8,  4,  13, 1, 0,

 9, 8, -4,   9, 8,  4,   18, 8, -4,  18, 8, 4,

  9, 8,  4,    18, 8, -4,    18, 8, 4
};

float normals[] = {
	0, 4, -7, 0, 4, -7, 0, 4, -7,
	0, 4,  7, 0, 4,  7, 0, 4,  7,
   -7, 4,  0, -7, 4, 0, -7, 4, 0,
	7, 4,  0,  7, 4, 0,  7, 4, 0,
	0, -1, 0,  0, -1,0, 0,-1,0, 0,-1,0
};

unsigned indices[] = {

0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 13, 14, 15 };

// 3D models
C3dglModel camera;
C3dglModel table;
C3dglModel chicken;
C3dglModel vase;
C3dglModel lamp;

GLuint idTexWood;
GLuint idTexNone;
GLuint clo;

GLuint idTexCube;


// The View Matrix
mat4 matrixView;

// Camera & navigation
float maxspeed = 3.f;	// camera max speed
float accel = 2.f;		// camera acceleration
vec3 _acc(0), _vel(0);	// camera acceleration and velocity vectors
float _fov = 60.f;		// field of view (zoom)




bool init()
{
	
	// Send the texture info to the shaders

	program.sendUniform("texture0", 0);
	
	// Initialise Shaders
	C3dglShader vertexShader;

	C3dglShader fragmentShader;
	
	C3dglBitmap bm;
	C3dglBitmap mm;


	// load Cube Map

	glActiveTexture(GL_TEXTURE1);

	glGenTextures(1, &idTexCube);

	glBindTexture(GL_TEXTURE_CUBE_MAP, idTexCube);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



	//blank
	glGenTextures(1, &idTexNone);

	glBindTexture(GL_TEXTURE_2D, idTexNone);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	BYTE bytes[] = { 255, 255, 255 };

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);

	//oak
	bm.load("models/oak.bmp", GL_RGBA);
	
	
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &idTexWood);

	glBindTexture(GL_TEXTURE_2D, idTexWood);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.getWidth(), bm.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.getBits());

	
	if (!bm.getBits()) return false;

	//cloth
	mm.load("models/cloth.bmp", GL_RGBA);
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &clo);

	glBindTexture(GL_TEXTURE_2D, clo);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mm.getWidth(), mm.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, mm.getBits());
	
	if (!mm.getBits()) return false;

	if (!vertexShader.create(GL_VERTEX_SHADER)) return false;

	if (!vertexShader.loadFromFile("shaders/basic.vert")) return false;

	if (!vertexShader.compile()) return false;


	if (!fragmentShader.create(GL_FRAGMENT_SHADER)) return false;

	if (!fragmentShader.loadFromFile("shaders/basic.frag")) return false;

	if (!fragmentShader.compile()) return false;


	if (!program.create()) return false;

	if (!program.attach(vertexShader)) return false;

	if (!program.attach(fragmentShader)) return false;

	if (!program.link()) return false;

	if (!program.use(true)) return false;

	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!

	// setup lighting


	glutSetVertexAttribCoord3(program.getAttribLocation("aVertex"));

	glutSetVertexAttribNormal(program.getAttribLocation("aNormal"));


	// prepare vertex data

	glGenBuffers(1, &vertBuff);

	glBindBuffer(GL_ARRAY_BUFFER, vertBuff);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	// prepare normal data

	glGenBuffers(1, &normBuff);

	glBindBuffer(GL_ARRAY_BUFFER, normBuff);

	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);


	// prepare indices array

	glGenBuffers(1, &indBuff);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuff);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// load your 3D models here!
	if (!camera.load("models\\camera.3ds")) return false;
	if (!table.load("models\\table.obj")) return false;
	if (!chicken.load("models\\chicken.obj")) return false;
	if (!vase.load("models\\vase.obj")) return false;
	if (!lamp .load("models\\lamp.obj")) return false;


	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1), radians(12.f), vec3(1, 0, 0));
	matrixView *= lookAt(
		vec3(-15.0, 10.0, 10.0),
		vec3(0.0, 0.0, -10.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.18f, 0.25f, 0.22f, 1.0f);   // deep grey background

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Shift to speed up your movement" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << " Press K to turn off the lamp on the left" << endl;
	cout << " Press L to turn off the lamp on the right" << endl;
	cout << endl;

	return true;
}

void renderVase(mat4 matrixView, float time, float deltaTime)
{
	mat4 m;

	//glActiveTexture(GL_TEXTURE1);

	program.sendUniform("reflectionPower", 1.0);
	m = matrixView;
	m = translate(m, vec3(-10.0f, 6, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.2f, 0.2f, 0.2f));


	vase.render(m);



}

void renderScene(mat4& matrixView, float time, float deltaTime)
{

	mat4 m;
	program.sendUniform("textureCubeMap", 1);
	program.sendUniform("reflectionPower", 0.0);
	//light
	program.sendUniform("lightAmbient.color", vec3(0.1, 0.1, 0.1));

	program.sendUniform("materialAmbient", vec3(1.0, 1.0, 1.0));

	//program.sendUniform("lightDir.direction", vec3(1.0, 0.5, 1.0));

	//program.sendUniform("lightDir.diffuse", vec3(0.2, 0.2, 0.2));

	program.sendUniform("materialDiffuse", vec3(0.2, 0.2, 0.6));

	//program.sendUniform("lightDir.diffuse", vec3(0.0, 0.0, 0.0));

	//point light 1
	
	program.sendUniform("lightPoint1.position", vec3(-15.93, 8, -2.1f));
	if (liiii) 
	{
		program.sendUniform("lightPoint1.diffuse", vec3(0.5, 0.5, 0.5));

		program.sendUniform("lightPoint1.specular", vec3(1.0, 1.0, 1.0));
	}
	else
	{
		program.sendUniform("lightPoint1.diffuse", vec3(0.0, 0.0, 0.0));

		program.sendUniform("lightPoint1.specular", vec3(0.0, 0.0, 0));
	}
	//point light 2
	program.sendUniform("lightPoint2.position", vec3(-7.43, 8.5, 2.1f));
	if (liiii1)
	{
		program.sendUniform("lightPoint2.diffuse", vec3(0.5, 0.5, 0.5));

		program.sendUniform("lightPoint2.specular", vec3(1.0, 1.0, 1.0));
	}
	else
	{
		program.sendUniform("lightPoint2.diffuse", vec3(0.0, 0.0, 0.0));

		program.sendUniform("lightPoint2.specular", vec3(0.0, 0.0, 0));
	}
	program.sendUniform("materialSpecular", vec3(0.6, 0.6, 1.0));

	program.sendUniform("shininess", 10.0f);

	program.sendUniform("tex", false); // use a false statement to get rid of texture(s) on objects that dont need it







	// setup material - grey (in this case it now used more to colour/shade the texture(s) below)
	program.sendUniform("materialAmbient", vec3(0.6f, 0.6f, 0.6f));
	program.sendUniform("materialDiffuse", vec3(0.6f, 0.6f, 0.6f));
	program.sendUniform("materialSpecular", vec3(0.6f, 0.6f, 0.6f));


	//chair 1
	

	m = matrixView;
	m = translate(m, vec3(-10.0f, 0, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.008f, 0.008f, 0.008f));

	program.sendUniform("tex", true);
	glBindTexture(GL_TEXTURE_2D, idTexWood); //turn texture on and specifiy which texture to use on object/piece
	table.render(0, m); //renders the wood peice of the chair


	 
	glBindTexture(GL_TEXTURE_2D, clo); // specify texture once again to overwrite the wood texture above
	table.render(1, m); //renders the cushion/pillow
	
	//chair 2
	m = matrixView;
	m = translate(m, vec3(-10.0f, 0, 0.0f));
	m = rotate(m, radians(0.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.008f, 0.008f, 0.008f));


	glBindTexture(GL_TEXTURE_2D, idTexWood); //turn texture on and specifiy which texture to use on object/piece
	table.render(0, m); //renders the wood peice of the chair



	glBindTexture(GL_TEXTURE_2D, clo); // specify texture once again to overwrite the wood texture above
	table.render(1, m); //renders the cushion/pillow

	//chair 3
	m = matrixView;
	m = translate(m, vec3(-8.0f, 0, 0.0f));
	m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.008f, 0.008f, 0.008f));

	glBindTexture(GL_TEXTURE_2D, idTexWood); 
	table.render(0, m); 



	glBindTexture(GL_TEXTURE_2D, clo);
	table.render(1, m); 

	//chair 4
	m = matrixView;
	m = translate(m, vec3(-12.0f, 0, 0.0f));
	m = rotate(m, radians(270.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.008f, 0.008f, 0.008f));


	glBindTexture(GL_TEXTURE_2D, idTexWood); 
	table.render(0, m); 



	glBindTexture(GL_TEXTURE_2D, clo);
	table.render(1, m); 
	
	program.sendUniform("tex", false); // turn off texture so it isn't applied to every object under



	
	//table

	program.sendUniform("tex", true); // turn texture back on 
	glBindTexture(GL_TEXTURE_2D, idTexWood); // specify texture

	m = matrixView;
	m = translate(m, vec3(-10.0f, 0, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.008f, 0.008f, 0.008f));

	
	table.render(2, m);

	program.sendUniform("tex", false); // turn off texture so it isn't applied to every object under

	


	program.sendUniform("materialAmbient", vec3(0.0f, 0.0f, 0.9f));
	program.sendUniform("materialDiffuse", vec3(0.0f, 0.0f, 0.9f));
	program.sendUniform("materialSpecular", vec3(0.0f, 0.0f, 0.9f));
	//vase
	renderVase(matrixView, time, deltaTime);
	
	glActiveTexture(GL_TEXTURE0);

	program.sendUniform("reflectionPower", 0.0);
	

	//upsidedown pyramid


	program.sendUniform("materialAmbient", vec3(0.9f, 0.0f, 0.0f));
	program.sendUniform("materialDiffuse", vec3(0.9f, 0.0f, 0.0f));
	program.sendUniform("materialSpecular", vec3(0.9f, 0.0f, 0.0f));



	
	GLuint attribVertex = program.getAttribLocation("aVertex");

	GLuint attribNormal = program.getAttribLocation("aNormal");

	m = matrixView;
	m = translate(m, vec3(-12.5, 5.9, 0));  //make an origin point near the pyramid
	m = scale(m, vec3(0.2, 0.2, 0.2));  
	m = rotate(m, 1.5f * time, vec3(0, 1, 0)); // rotate
	m = translate(m, vec3(-13, 0, 0)); 
	// return it back to origin, you do this to eliminate the pyramid from spinning around the point 13,1,0 and instead spin in place,
	// origin * (-origin) * rotation = rotation, only 
	
	program.sendUniform("matrixModelView", m); //send data to GPU


	// Enable vertex attribute arrays

	glEnableVertexAttribArray(attribVertex);

	glEnableVertexAttribArray(attribNormal);


	// Bind (activate) the vertex buffer and set the pointer to it

	glBindBuffer(GL_ARRAY_BUFFER, vertBuff);

	glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);


	// Bind (activate) the normal buffer and set the pointer to it

	glBindBuffer(GL_ARRAY_BUFFER, normBuff);

	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);


	// Draw triangles – using index buffer

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuff);

	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);



	// Disable arrays

	glDisableVertexAttribArray(attribVertex);

	glDisableVertexAttribArray(attribNormal);


	//yellow colour


	program.sendUniform("materialAmbient", vec3(0.9f, 0.9f, 0.0f));
	program.sendUniform("materialDiffuse", vec3(0.9f, 0.9f, 0.0f));
	program.sendUniform("materialSpecular", vec3(0.9f, 0.9f, 0.0f));
	
	//chicken

	m = matrixView;
	m = translate(m, vec3(-12.5f, 8.5, 0.1f));
	m = rotate(m, radians(100.f), vec3(0.0f, 1.0f, 0.0f));
	m = rotate(m, 1.5f * time, vec3(0, 1, 0));
	m = scale(m, vec3(0.04f, 0.04f, 0.04f));


	chicken.render(m);

	// lamp

	m = matrixView;
	m = translate(m, vec3(-14.5f, 6.09, -2.1f));
	m = scale(m, vec3(0.04f, 0.04f, 0.04f));

	lamp.render(m);

	program.sendUniform("materialAmbient", vec3(0.9f, 0.9f, 0.9f));
	program.sendUniform("materialDiffuse", vec3(0.3f, 0.3f, 0.3f));
	program.sendUniform("materialSpecular", vec3(0.9f, 0.9f, 0.9f));
	
	//sphere/bulb one
	m = matrixView;

	m = translate(m, vec3(-15.93, 8.5, -2.1f));

	m = scale(m, vec3(0.17f, 0.17f, 0.17f));

	program.sendUniform("matrixModelView", m);

	if (liiii) 
	{
		program.sendUniform("emissive", vec3(1.0, 1.0, 1.0));
	}
	
	else 
	{ 
		program.sendUniform("emissive", vec3(0.0, 0.0, 0.0)); 
	}
	glutSolidSphere(1, 32, 32);
	program.sendUniform("emissive", vec3(0.0, 0.0, 0.0));

	
	//yellow
	program.sendUniform("materialAmbient", vec3(0.9f, 0.9f, 0.0f));
	program.sendUniform("materialDiffuse", vec3(0.9f, 0.9f, 0.0f));
	program.sendUniform("materialSpecular", vec3(0.9f, 0.9f, 0.0f));
	// lamp 2

	m = matrixView;
	m = translate(m, vec3(-6, 6.09, 2.1f));
	m = scale(m, vec3(0.04f, 0.04f, 0.04f));

	lamp.render(m);

	program.sendUniform("materialAmbient", vec3(0.9f, 0.9f, 0.9f));
	program.sendUniform("materialDiffuse", vec3(0.3f, 0.3f, 0.3f));
	program.sendUniform("materialSpecular", vec3(0.9f, 0.9f, 0.9f));

	//sphere/bulb two
	m = matrixView;

	m = translate(m, vec3(-7.43, 8.5, 2.1f));

	m = scale(m, vec3(0.17f, 0.17f, 0.17f));

	program.sendUniform("matrixModelView", m);

	if (liiii1)
	{
		program.sendUniform("emissive", vec3(1.0, 1.0, 1.0));
	}

	else
	{
		program.sendUniform("emissive", vec3(0.0, 0.0, 0.0));
	}
	
	glutSolidSphere(1, 32, 32);
	program.sendUniform("emissive", vec3(0.0, 0.0, 0.0));
	
	
	
// setup material - green

	program.sendUniform("materialAmbient", vec3(0.0f, 0.9f, 0.0f));
	program.sendUniform("materialDiffuse", vec3(0.0f, 0.9f, 0.0f));
	program.sendUniform("materialSpecular", vec3(0.0f, 0.9f, 0.0f));

	// teapot
	m = matrixView;
	m = translate(m, vec3(-7.0f, 6.7, 0.0f));
	m = rotate(m, radians(0.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.4f, 0.4f, 0.4f));
	// the GLUT objects require the Model View Matrix setup
	program.sendUniform("matrixModelView", m);
	glutSolidTeapot(2.0);

	

	

}


void prepareCubeMap(float x, float y, float z, float time, float deltaTime)

{

	// Store the current viewport in a safe place

	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);

	int w = viewport[2];

	int h = viewport[3];


	// setup the viewport to 256x256, 90 degrees FoV (Field of View)

	glViewport(0, 0, 256, 256);

	program.sendUniform("matrixProjection", perspective(radians(90.f), 1.0f, 0.02f, 1000.0f));


	// render environment 6 times

	program.sendUniform("reflectionPower", 0.0);

	for (int i = 0; i < 6; ++i)

	{

		// clear background

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// setup the camera

		const GLfloat ROTATION[6][6] =

		{ // at up

		{ 1.0, 0.0, 0.0, 0.0, -1.0, 0.0 }, // pos x

		{ -1.0, 0.0, 0.0, 0.0, -1.0, 0.0 }, // neg x

		{ 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 }, // pos y

		{ 0.0, -1.0, 0.0, 0.0, 0.0, -1.0 }, // neg y

		{ 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 }, // poz z

		{ 0.0, 0.0, -1.0, 0.0, -1.0, 0.0 } // neg z

		};

		mat4 matrixView2 = lookAt(

			vec3(x, y, z),

			vec3(x + ROTATION[i][0], y + ROTATION[i][1], z + ROTATION[i][2]),

			vec3(ROTATION[i][3], ROTATION[i][4], ROTATION[i][5]));


		// send the View Matrix

		program.sendUniform("matrixView", matrixView);


		// render scene objects - all but the reflective one

		glActiveTexture(GL_TEXTURE0);

		renderScene(matrixView2, time, deltaTime);


		// send the image to the cube texture

		glActiveTexture(GL_TEXTURE1);

		glBindTexture(GL_TEXTURE_CUBE_MAP, idTexCube);

		glCopyTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, 0, 0, 256, 256, 0);

	}

	// restore the matrixView, viewport and projection

	void onReshape(int w, int h);

	onReshape(w, h);

}


void onRender()
{
	// these variables control time & animation
	static float prev = 0;
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;	// time since start in seconds
	float deltaTime = time - prev;						// time since last frame
	prev = time;										// framerate is 1/deltaTime
	prepareCubeMap(-10.0f, 6, 0.0f, time, deltaTime);
	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	_vel = clamp(_vel + _acc * deltaTime, -vec3(maxspeed), vec3(maxspeed));
	float pitch = getPitch(matrixView);
	matrixView = rotate(translate(rotate(mat4(1),
		pitch, vec3(1, 0, 0)),	// switch the pitch off
		_vel * deltaTime),		// animate camera motion (controlled by WASD keys)
		-pitch, vec3(1, 0, 0))	// switch the pitch on
		* matrixView;
	
	program.sendUniform("matrixView", matrixView);
	
	// render the scene objects
	renderScene(matrixView, time, deltaTime);

	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}

// called before window opened or resized - to setup the Projection Matrix
void onReshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 matrixProjection = perspective(radians(_fov), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix
	program.sendUniform("matrixProjection", matrixProjection);
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': _acc.z = accel; break;
	case 's': _acc.z = -accel; break;
	case 'a': _acc.x = accel; break;
	case 'd': _acc.x = -accel; break;
	case 'e': _acc.y = accel; break;
	case 'q': _acc.y = -accel; break;
	case 'k': liiii = !liiii; break;
	case 'l': liiii1 = !liiii1; break;

	}

	

}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': _acc.z = _vel.z = 0; break;
	case 'a':
	case 'd': _acc.x = _vel.x = 0; break;
	case 'q':
	case 'e': _acc.y = _vel.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();

	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
void onMouse(int button, int state, int x, int y)
{
	glutSetCursor(state == GLUT_DOWN ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
	if (button == 1)
	{
		_fov = 60.0f;
		onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}

// handle mouse move
void onMotion(int x, int y)
{
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

	// find delta (change to) pan & pitch
	float deltaYaw = 0.005f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
	float deltaPitch = 0.005f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

	if (abs(deltaYaw) > 0.3f || abs(deltaPitch) > 0.3f)
		return;	// avoid warping side-effects

	// View = Pitch * DeltaPitch * DeltaYaw * Pitch^-1 * View;
	constexpr float maxPitch = radians(80.f);
	float pitch = getPitch(matrixView);
	float newPitch = glm::clamp(pitch + deltaPitch, -maxPitch, maxPitch);
	matrixView = rotate(rotate(rotate(mat4(1.f),
		newPitch, vec3(1.f, 0.f, 0.f)),
		deltaYaw, vec3(0.f, 1.f, 0.f)),
		-pitch, vec3(1.f, 0.f, 0.f))
		* matrixView;
}

void onMouseWheel(int button, int dir, int x, int y)
{
	_fov = glm::clamp(_fov - dir * 5.f, 5.0f, 175.f);
	onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

int main(int argc, char** argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("3DGL Scene: First Example");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		C3dglLogger::log("GLEW Error {}", (const char*)glewGetErrorString(err));
		return 0;
	}
	C3dglLogger::log("Using GLEW {}", (const char*)glewGetString(GLEW_VERSION));

	// register callbacks
	glutDisplayFunc(onRender);
	glutReshapeFunc(onReshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutMouseWheelFunc(onMouseWheel);

	C3dglLogger::log("Vendor: {}", (const char*)glGetString(GL_VENDOR));
	C3dglLogger::log("Renderer: {}", (const char*)glGetString(GL_RENDERER));
	C3dglLogger::log("Version: {}", (const char*)glGetString(GL_VERSION));
	C3dglLogger::log("");

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		C3dglLogger::log("Application failed to initialise\r\n");
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}

