#pragma once
#define MYICON 2912
#define TEXTURE_WOOD 9012

GLuint Texture_Wood;

void drawBench();

void drawBench()
{
	glColor3f(0.305f, 0.203f, 0.180f);			//Darkest Brown
	/////////////////////////////////////// Plank 1 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);
	
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, 0.38f);		//A
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.05f, 0.21f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, 0.21f);		//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, 0.05f, 0.38f);		//D
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, 0.38f);		//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, -0.10f, 0.21f);		//F

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, 0.21f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, -0.10f, 0.38f);		//H
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, 0.38f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, 0.38f);		//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, 0.38f);		//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, 0.38f);		//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, 0.21f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, 0.21f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, 0.21f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, 0.21f);		//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, 0.38f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.05f, 0.21f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.00f, -0.10f, 0.21f);		//F
	
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, 0.38f);		//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.00f, 0.05f, 0.38f);		//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, 0.21f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, 0.21f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, -0.10f, 0.38f);		//H
	glEnd();
	
	/////////////////////////////////////// Plank 2 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, 0.16f);		//B
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.05f, -0.01f);		//I
	
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, -0.01f);		//J

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, 0.05f, 0.16f);		//C
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, 0.16f);		//F

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, -0.10f, -0.01f);		//L

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, -0.01f);		//K

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, -0.10f, 0.16f);		//G
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, 0.16f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, 0.16f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, 0.16f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, 0.16f);		//F
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, -0.01f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, -0.01f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, -0.01f);		//K

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, -0.01f);		//L
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, 0.16f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.05f, -0.01f);		//I

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.00f, -0.10f, -0.01f);		//L

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.00f, -0.10f, 0.16f);		//F
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.00f, 0.05f, 0.16f);		//C

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, -0.01f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, -0.01f);		//K

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, -0.10f, 0.16f);		//G
	glEnd();
	
	/////////////////////////////////////// Plank 3 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, -0.06f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.05f, -0.23f);		//M

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, -0.23f);		//N

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, 0.05f, -0.06f);		//J
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, -0.06f);		//L

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, -0.10f, -0.23f);		//P

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, -0.23f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, -0.10f, -0.06f);		//K
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, -0.06f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, -0.06f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, -0.06f);		//K

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, -0.06f);		//L
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, -0.23f);		//M

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, -0.23f);		//N

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, -0.23f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, -0.23f);		//P
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.05f, -0.06f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.05f, -0.23f);		//M

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.00f, -0.10f, -0.23f);		//P

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, -0.10f, -0.06f);		//L
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.00f, 0.05f, -0.06f);		//J

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.05f, -0.23f);		//N

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, -0.10f, -0.23f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, -0.10f, -0.06f);		//K
	glEnd();
	
	/////////////////////////////////////// Plank 4 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.10f, 0.2686f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, -0.10f, 0.1186f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, 0.1186f);		//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.10f, 0.2686f);		//D
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.65f, 0.1966f);		//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, -0.65f, 0.0466f);		//F

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.65f, 0.0466f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.65f, 0.1966f);		//H
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.10f, 0.2686f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, 0.2686f);		//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.65f, 0.1966f);		//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.65f, 0.1966f);		//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.10f, 0.1186f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, 0.1186f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.65f, 0.0466f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.65f, 0.0466f);		//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.10f, 0.2686f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, -0.10f, 0.1186f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.70f, -0.65f, 0.0466f);		//F

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.65f, 0.1966f);		//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.10f, 0.2686f);		//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, 0.1186f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.65f, 0.0466f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.65f, 0.1966f);		//H
	glEnd();


	/////////////////////////////////////// Plank 5 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.10f, 0.2686f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, -0.10f, 0.1186f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, 0.1186f);		//K

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.10f, 0.2686f);		//L
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.65f, 0.1966f);		//M

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, -0.65f, 0.0466f);		//N
	
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.65f, 0.0466f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.65f, 0.1966f);		//P
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.10f, 0.2686f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, 0.2686f);		//L

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.65f, 0.1966f);		//P

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.65f, 0.1966f);		//M
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.10f, 0.1186f);		//J

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, 0.1186f);		//K

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.65f, 0.0466f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.65f, 0.0466f);		//N
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.10f, 0.2686f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, -0.10f, 0.1186f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.70f, -0.65f, 0.0466f);		//N

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.65f, 0.1966f);		//M
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, -0.10f, 0.2686f);		//L

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, 0.1186f);		//K

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.65f, 0.0466f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.65f, 0.1966f);		//P
	glEnd();

	/////////////////////////////////////// Plank 6 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, 0.800f, -0.300f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, 0.800f, -0.450f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, 0.800f, -0.450f);		//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, 0.800f, -0.300f);		//D
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.65f, -0.200f);		//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, -0.65f, -0.350f);		//F

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.65f, -0.350f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.65f, -0.200f);		//H
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, 0.800f, -0.3000f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, 0.800f, -0.3000f);		//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.650f, -0.2000f);		//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.650f, -0.2000f);		//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, 0.800f, -0.450f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, 0.800f, -0.450f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.650f, -0.350f);	//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.650f, -0.350f);	//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, 0.800f, -0.3000f);	//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, 0.800f, -0.450f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.70f, -0.650f, -0.350f);	//F

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.650f, -0.2000f);	//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, 0.800f, -0.3000f);	//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, 0.800f, -0.450f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.650f, -0.350f);	//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.650f, -0.2000f);	//H
	glEnd();


	/////////////////////////////////////// Plank 7 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, 0.800f, -0.300f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, 0.800f, -0.450f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, 0.800f, -0.450f);		//K

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, 0.800f, -0.300f);		//L
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.650f, -0.200f);		//M

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, -0.650f, -0.350f);		//N

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.650f, -0.350f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.650f, -0.200f);		//P
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, 0.800f, -0.300f);		//I

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, 0.800f, -0.300f);		//L

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.650f, -0.200f);		//P

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.650f, -0.200f);		//M
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, 0.800f, -0.450f);		//J

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, 0.800f, -0.450f);		//K

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.650f, -0.350f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.650f, -0.350f);		//N
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, 0.800f, -0.3000f);		//I
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, 0.800f, -0.450f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.70f, -0.650f, -0.350f);		//N

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.650f, -0.200f);		//M
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, 0.800f, -0.300f);		//L

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, 0.800f, -0.450f);		//K

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.650f, -0.350f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.650f, -0.200f);		//P
	glEnd();

	/////////////////////////////////////// Plank 8 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.10f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, -0.330f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.85f, -0.10f, -0.330f);		//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.85f, -0.10f, 0.295f);		//D
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.20f, 0.295f);		//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.20f, -0.330f);		//F

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.85f, -0.20f, -0.330f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.85f, -0.20f, 0.295f);		//H
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.10f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.85f, -0.10f, 0.295f);		//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.85f, -0.20f, 0.295f);		//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.20f, 0.295f);		//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.10f, -0.330f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.85f, -0.10f, -0.330f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.85f, -0.20f, -0.330f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.20f, -0.330f);		//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.10f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.10f, -0.330f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.20f, -0.330f);		//F

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.20f, 0.2233f);		//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.85f, -0.10f, 0.295f);		//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.85f, -0.10f, -0.330f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.85f, -0.20f, -0.330f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.85f, -0.20f, 0.295f);		//H
	glEnd();

	/////////////////////////////////////// Plank 9 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.10f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, -0.10f, -0.300f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.65f, -0.10f, -0.300f);		//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.65f, -0.10f, 0.295f);		//D
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.20f, 0.295f);		//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, -0.20f, -0.300f);		//F

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.65f, -0.20f, -0.300f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.65f, -0.20f, 0.295f);		//H
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.10f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.65f, -0.10f, 0.295f);		//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.65f, -0.20f, 0.295f);		//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.20f, 0.295f);		//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.10f, -0.300f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.65f, -0.10f, -0.300f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.65f, -0.20f, -0.300f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.20f, -0.300f);		//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.10f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, -0.10f, -0.300f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.70f, -0.20f, -0.300f);		//F

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.20f, 0.295f);		//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.65f, -0.10f, 0.295f);		//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.65f, -0.10f, -0.300f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.65f, -0.20f, -0.300f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.65f, -0.20f, 0.295f);		//H
	glEnd();

	
	/////////////////////////////////////// Plank 10 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.65f, -0.10f, 0.295f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.65f, -0.10f, -0.300f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.70f, -0.10f, -0.300f);		//K

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.10f, 0.295f);		//L
	glEnd();


	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.65f, -0.20f, 0.295f);		//M

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.65f, -0.20f, -0.300f);		//N

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.70f, -0.20f, -0.300f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.20f, 0.295f);		//P
	glEnd();


	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.65f, -0.10f, 0.295f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, -0.10f, 0.295f);		//L

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.70f, -0.20f, 0.295f);		//P

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.65f, -0.20f, 0.295f);		//M
	glEnd();


	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.65f, -0.10f, -0.300f);		//J

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, -0.10f, -0.300f);		//K

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.70f, -0.20f, -0.300f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.65f, -0.20f, -0.300f);		//N
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.65f, -0.10f, 0.295f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.65f, -0.10f, -0.300f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.65f, -0.20f, -0.300f);		//N

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.65f, -0.20f, 0.295f);		//M
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.10f, 0.295f);		//L

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, -0.10f, -0.300f);		//K

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.70f, -0.20f, -0.300f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.20f, 0.295f);		//P
	glEnd();

	/////////////////////////////////////// Plank 11 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.85f, -0.10f, 0.295f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.85f, -0.10f, -0.330f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, -0.330f);		//K

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.10f, 0.295f);		//L
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.85f, -0.20f, 0.295f);		//M

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.85f, -0.20f, -0.330f);		//N

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.20f, -0.330f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.20f, 0.295f);		//P
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.85f, -0.10f, 0.295f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, 0.295f);		//L

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.20f, 0.295f);		//P

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.85f, -0.20f, 0.295f);		//M
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.85f, -0.10f, -0.330f);		//J

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, -0.330f);		//K

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.20f, -0.330f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.85f, -0.20f, -0.330f);		//N
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.85f, -0.10f, 0.295f);		//I

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.85f, -0.10f, -0.330f);		//J

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.85f, -0.20f, -0.330f);		//N

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.85f, -0.20f, 0.295f);		//M
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, -0.10f, 0.295f);		//L

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.10f, -0.330f);		//K

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.20f, -0.330f);		//O

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.20f, 0.295f);		//P
	glEnd();
	
	
	/////////////////////////////////////// Plank 12 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.850f, -0.200f);	//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.850f, -0.300f);	//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.850f, -0.300f);	//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, 0.850f, -0.200f);	//D
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.650f, -0.200f);	//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.650f, -0.300f);	//F

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.650f, -0.300f);	//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, 0.650f, -0.200f);	//H
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.850f, -0.200f);	//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.850f, -0.200f);	//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.650f, -0.200f);	//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, 0.650f, -0.200f);	//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.850f, -0.300f);	//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.850f, -0.300f);	//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.650f, -0.300f);	//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, 0.650f, -0.300f);	//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.850f, -0.200f);	//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.850f, -0.300f);	//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.00f, 0.650f, -0.300f);	//F

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, 0.650f, -0.200f);	//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.00f, 0.850f, -0.200f);	//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.850f, -0.300f);	//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.650f, -0.300f);	//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, 0.650f, -0.200f);	//H
	glEnd();

	/////////////////////////////////////// Plank 13 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.600f, -0.200f);	//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.600f, -0.300f);	//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.600f, -0.300f);	//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, 0.600f, -0.200f);	//D
	glEnd();


	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.400f, -0.200f);	//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.400f, -0.300f);	//F

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.400f, -0.300f);	//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, 0.400f, -0.200f);	//H
	glEnd();


	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.600f, -0.200f);	//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.600f, -0.200f);	//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.600f, -0.200f);	//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, 0.600f, -0.200f);	//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.600f, -0.250f);	//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.600f, -0.250f);	//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.400f, -0.250f);	//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, 0.400f, -0.250f);	//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.00f, 0.600f, -0.200f);	//A
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.00f, 0.600f, -0.300f);	//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.00f, 0.400f, -0.300f);	//F

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.00f, 0.400f, -0.200f);	//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.00f, 0.600f, -0.200f);	//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.00f, 0.600f, -0.300f);	//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.00f, 0.400f, -0.300f);	//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.00f, 0.400f, -0.200f);	//H
	glEnd();

	/////////////////////////////////////// Plank 14 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.55f, -0.42f);	//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.85f, -0.55f, -0.42f);	//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.85f, -0.55f, 0.295f);		//D
	glEnd();


	glNormal3f(1.0f, -1.0f, 1.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.65f, 0.1966f);		//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.65f, -0.42f);	//F

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.85f, -0.65f, -0.42f);	//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.85f, -0.65f, 0.295f);		//H
	glEnd();


	glNormal3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.85f, -0.55f, 0.295f);		//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.85f, -0.65f, 0.295f);		//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.65f, 0.295f);		//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.55f, -0.42f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.85f, -0.55f, -0.42f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.85f, -0.65f, -0.42f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.65f, -0.42f);		//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.80f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.80f, -0.55f, -0.42f);	//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.80f, -0.65f, -0.42f);	//F

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.80f, -0.65f, 0.295f);		//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.85f, -0.55f, 0.295f);		//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.85f, -0.55f, -0.42f);	//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.85f, -0.65f, -0.42f);	//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.85f, -0.65f, 0.295f);		//H
	glEnd();
	
	/////////////////////////////////////// Plank 15 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.55f, -0.42f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.85f, -0.55f, -0.42f);		//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.85f, -0.55f, 0.295f);		//D
	glEnd();


	glNormal3f(1.0f, -1.0f, 1.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, -0.65f, 0.295f);		//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.65f, -0.42f);		//F

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.85f, -0.65f, -0.42f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.85f, -0.65f, 0.295f);		//H
	glEnd();


	glNormal3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.85f, -0.55f, 0.295f);		//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.85f, -0.65f, 0.295f);		//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.65f, 0.295f);		//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, -0.55f, -0.42f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.85f, -0.55f, -0.42f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.85f, -0.65f, -0.42f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.65f, -0.42f);		//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.80f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.80f, -0.55f, -0.42f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.80f, -0.65f, -0.42f);		//F

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.80f, -0.65f, 0.295f);		//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.85f, -0.55f, 0.295f);		//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.85f, -0.55f, -0.42f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.85f, -0.65f, -0.42f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.85f, -0.65f, 0.295f);		//H
	glEnd();
	
	/////////////////////////////////////// Plank 16 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, -0.55f, -0.42f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.65f, -0.55f, -0.42f);		//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.65f, -0.55f, 0.295f);		//D
	glEnd();


	glNormal3f(1.0f, -1.0f, 1.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.65f, 0.295f);		//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, -0.65f, -0.42f);		//F

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.65f, -0.65f, -0.42f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.65f, -0.65f, 0.295f);		//H
	glEnd();


	glNormal3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.65f, -0.55f, 0.295f);		//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.65f, -0.65f, 0.295f);		//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.65f, 0.295f);		//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.55f, -0.42f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.65f, -0.55f, -0.42f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.65f, -0.65f, -0.42f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.65f, -0.42f);		//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.70f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.70f, -0.55f, -0.42f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.70f, -0.65f, -0.42f);		//F

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.70f, -0.65f, 0.295f);		//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.65f, -0.55f, 0.295f);		//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.65f, -0.55f, -0.42f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.65f, -0.65f, -0.42f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.65f, -0.65f, 0.295f);		//H
	glEnd();
	
	/////////////////////////////////////// Plank 17 ///////////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, Texture_Wood);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);		//Upper Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, -0.55f, -0.42f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.65f, -0.55f, -0.42f);		//C

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.65f, -0.55f, 0.295f);		//D
	glEnd();


	glNormal3f(1.0f, -1.0f, 1.0f);
	glBegin(GL_POLYGON);		//Lower Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.65f, 0.295f);		//E

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, -0.65f, -0.42f);		//F

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.65f, -0.65f, -0.42f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.65f, -0.65f, 0.295f);		//H
	glEnd();


	glNormal3f(1.0f, 0.0f, 1.0f);
	glBegin(GL_POLYGON);		//Front Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.65f, -0.55f, 0.295f);		//D

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.65f, -0.65f, 0.295f);		//H

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.65f, 0.295f);		//E
	glEnd();


	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_POLYGON);		//Back Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.55f, -0.42f);		//B

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.65f, -0.55f, -0.42f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.65f, -0.65f, -0.42f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.65f, -0.42f);		//F
	glEnd();


	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Left Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.70f, -0.55f, 0.295f);		//A

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.70f, -0.55f, -0.42f);		//B

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.70f, -0.65f, -0.42f);		//F

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.70f, -0.65f, 0.295f);		//E
	glEnd();


	glNormal3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);		//Right Plane
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.65f, -0.55f, 0.295f);		//D

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.65f, -0.55f, -0.42f);		//C

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-0.65f, -0.65f, -0.42f);		//G

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-0.65f, -0.65f, 0.295f);		//H
	glEnd();
}