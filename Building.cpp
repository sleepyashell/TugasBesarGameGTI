#include "Building.h"

#include "Material.h"
#include "Drawing.h"
#include "World.h"
#include "Texture.h"

void drawCorridor(float width, float depth, float currentFloor){
    bindTexture(TEX_FLOOR_TILE);
    matFloor();  
    float uMax = width / 2.0f; 
    float vMax = depth / 2.0f;
    if (currentFloor == 0){
    	glBegin(GL_QUADS);
	        glNormal3f(0, 1, 0);
	        glTexCoord2f(0,    0);    glVertex3f(-6.0f,     0.05f,  0);
	        glTexCoord2f(uMax, 0);    glVertex3f(width, 0.05f,  0);
	        glTexCoord2f(uMax, vMax); glVertex3f(width, 0.05f, -depth);
	        glTexCoord2f(0,    vMax); glVertex3f(-6.0f,     0.05f, -depth);
	    glEnd();
    } else{
    	glBegin(GL_QUADS);
	        glNormal3f(0, 1, 0);
	        glTexCoord2f(0,    0);    glVertex3f(0.0f,     0.05f,  0);
	        glTexCoord2f(uMax, 0);    glVertex3f(width, 0.05f,  0);
	        glTexCoord2f(uMax, vMax); glVertex3f(width, 0.05f, -depth);
	        glTexCoord2f(0,    vMax); glVertex3f(0.0f,     0.05f, -depth);
	    glEnd();
    }
    unbindTexture();
    
    glPushMatrix();
        glTranslatef(0, 4.0f, 0);
        matFloor();   drawBlock(width, 0.05f, depth);
        glTranslatef(0, 0.05f, 0);
        matConcrete(); drawBlock(width, 0.95f, depth);  
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0, 4.0f, 0);
        matFloor();   drawBlock(width, 0.05f, depth);
        glTranslatef(0, 0.05f, 0);
        matConcrete(); drawBlock(width, 0.95f, depth);  
    glPopMatrix();

	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);
	
	if (currentFloor > 0){
		glPushMatrix();
			matConcrete();
	        drawBlock(0.2f, 1.0f, 4.0f);
	    glPopMatrix();
	}
	
    glPushMatrix();
		matConcrete();
        drawBlock(width, 1.0f, 0.2f);
    glPopMatrix();
    
    glPushMatrix();
    	glTranslatef(51.8f, 0.0f, 0.0f);
        matConcrete();
        drawBlock(0.2f, 1.0f, 4.0f);
    glPopMatrix();

    const float segment  = 8.0f;
    const float pillarW = 0.4f;
    
    int jumlahSegmen = (int)((width + 4) / segment);
    glPushMatrix();
        matPillar();
        drawBlock(pillarW, 4.0f, 0.2f);
    glPopMatrix();
    for (int i = 1; i < jumlahSegmen; i++) {
        float x = i * segment - (pillarW / 2.0f);
        if (i >= 5){
        	glPushMatrix();
	            glTranslatef(x-4.0f, 0.0f, 0.0f);
	            matPillar();
	            drawBlock(pillarW, 4.0f, 0.2f);
	        glPopMatrix();
        }
        else {
        	glPushMatrix();
	            glTranslatef(x, 0.0f, 0.0f);
	            matPillar();
	            drawBlock(pillarW, 4.0f, 0.2f);
	        glPopMatrix();
        }
    }
    glPushMatrix();
        glTranslatef(width - pillarW, 0.0f, 0.0f);
        matPillar();
        drawBlock(pillarW, 4.0f, 0.2f);
    glPopMatrix();

}

void drawGate(){
    const float gateX    = -6.0f;
    const float gateZ    = 2.0f;
    const float gateWidth = 4.0f;
    const float pillarSize = 0.8f;
    glPushMatrix();
        glTranslatef(gateX, 0.0f, gateZ - gateWidth/2.0f);
        matConcrete();
        drawBlock(pillarSize, 5.0f, -pillarSize);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(gateX, 0.0f, gateZ + gateWidth/2.0f);
        matConcrete();
        drawBlock(pillarSize, 5.0f, pillarSize);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(gateX, 5.0f, gateZ + gateWidth/2.0f);
        matConcrete();
        drawBlock(0.8f, 0.5f, gateWidth);
    glPopMatrix();

    int nJeruji = 9;
    float startZ = gateZ - (gateWidth / 2.0f); 
    float endZ   = gateZ + (gateWidth / 2.0f);
    
    float jerujiAreaStart = startZ + 0.8f;
    float jerujiAreaEnd   = endZ - 0.8f;
    float jerujiWidth     = jerujiAreaEnd - jerujiAreaStart;
    
    float gapJeruji = jerujiWidth / (float)(nJeruji - 1);

    for (int i = 0; i < nJeruji; i++) {
        glPushMatrix();
            glTranslatef(gateX + 0.35f, 0.15f, jerujiAreaStart + (i * gapJeruji));
            matPillar();
            drawBlock(0.12f, 4.0f, 0.12f);
        glPopMatrix();
    }
    glPushMatrix();
        glTranslatef(gateX + 0.35f, 4.0f, jerujiAreaEnd); 
        matPillar();
        drawBlock(0.12f, 0.12f, jerujiWidth);
    glPopMatrix();
	
    glPushMatrix();
        glTranslatef(gateX + 0.35f, 0.15f, jerujiAreaEnd); 
        matPillar();
        drawBlock(0.12f, 0.12f, jerujiWidth);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(gateX, 0.0f, 0.0f); 
        matPillar();
        drawBlock(6.0f, 1.0f, 0.2f);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(gateX, 0.0f, 4.0f); 
        matPillar();
        drawBlock(6.0f, 1.0f, 0.2f);
    glPopMatrix();
}

void drawOuterWalls()
{
    matConcrete();
    glPushMatrix();
        glTranslatef(-6.0f, 0.0f, 15.0f);
        drawBlock(64.0f, 2.5f, 0.5f);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-6.0f, 0.0f, -25.0f);
        drawBlock(64.0f, 2.5f, 0.5f);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-6.0f, 0.0f, 4.0f);
        drawBlock(0.5f, 2.5f, -11.0f);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-6.0f, 0.0f, 0.0f);
        drawBlock(0.5f, 2.5f, 25.0f);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(58.0f, 0.0f, 15.0f);
        drawBlock(0.5f, 2.5f, 40.0f);
    glPopMatrix();
}

void drawRoomInterior(float width, float depth) {
    bindTexture(TEX_FLOOR_TILE);
    matFloor();  
    float uMax = width / 2.0f;
    float vMax = depth / 2.0f;
    
    glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0,    0);    glVertex3f(0,     0.05f,  0);
        glTexCoord2f(uMax, 0);    glVertex3f(width, 0.05f,  0);
        glTexCoord2f(uMax, vMax); glVertex3f(width, 0.05f, -depth);
        glTexCoord2f(0,    vMax); glVertex3f(0,     0.05f, -depth);
    glEnd();
    
    unbindTexture();
    drawDeskSet(width, depth);
    
    bindTexture(TEX_WALL);
    matConcrete();
    
	glPushMatrix();
		glTranslatef(0, 0, -0.01f);
		drawWallBlock(0.2f, 4.0f, 9.99f);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(7.8f, 0, -0.01f);
		drawWallBlock(0.2f, 4.0f, 9.99f);
	glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0, 0, -depth);
		drawWallBlock(width, 4.0f, 0.2f);
    glPopMatrix();
    unbindTexture();
    
    glPushMatrix();
        glTranslatef(0, 4.0f, 0);
        matFloor();    drawBlock(width, 0.05f, depth);
        glTranslatef(0, 0.05f, 0);
        matConcrete(); drawBlock(width, 0.95f, depth);
    glPopMatrix();
}

void drawStairArea(float width, float depth, bool isLastFloor, float offsetY) {
	if (offsetY == 0.0f) {
        bindTexture(TEX_FLOOR_TILE);
        matFloor();  
        glBegin(GL_QUADS);
            glNormal3f(0, 1, 0);
            glTexCoord2f(0, 0);    glVertex3f(0,     0.05f,  0);
            glTexCoord2f(2, 0);    glVertex3f(width, 0.05f,  0);
            glTexCoord2f(2, 3.25); glVertex3f(width, 0.05f, -6.5f);
            glTexCoord2f(0, 3.25); glVertex3f(0,     0.05f, -6.5f);
        glEnd();
        unbindTexture();
    }
    glPushMatrix();
        glTranslatef(0, 0, -6.5);
        bindTexture(TEX_WALL);
        matConcrete(); drawWallBlock(width, FLOOR_HEIGHT, 0.2f);
        unbindTexture();
    glPopMatrix();

	if (isLastFloor) {
		glPushMatrix();
            glTranslatef(0, 0.0f, -1);
            matFloor(); drawBlock(2.15f, 1.0f, 0.2f);
        glPopMatrix();
		
        glPushMatrix();
            glTranslatef(0, 4.0f, 0);
            matFloor();    drawBlock(width, 0.05f, depth);
            glTranslatef(0, 0.05f, 0);
            matConcrete(); drawBlock(width, 0.95f, depth);
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0, 0.05f, 0);
            bindTexture(TEX_FLOOR_TILE);
            glBegin(GL_QUADS);
                glNormal3f(0, 1, 0);
                glTexCoord2f(0, 0);    glVertex3f(0, 0,  0);
                glTexCoord2f(2, 0);    glVertex3f(4, 0,  0);
                glTexCoord2f(2, 0.5);   glVertex3f(4, 0, -1);
                glTexCoord2f(0, 0.5);   glVertex3f(0, 0, -1);
            glEnd();
            unbindTexture();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(0, -10.0f, 0);
            matFloor(); drawBlock(width, 0.05f, depth);
        glPopMatrix();
        return;
    }
    
    int currentFloor = (int)(offsetY / FLOOR_HEIGHT) + 1;
    if (currentFloor == 1) {
        glPushMatrix();
        	matFloor(); 
	        glTranslatef(1.85f, 0.0f, -1);
			drawBlock(0.15f, 1.0f, 0.2f);
			glTranslatef(0.0f, 2.4f, -3.6);
			drawBlock(0.3f, 1.0f, 0.2f);
	    glPopMatrix();
    } else if (currentFloor == 2) {
        glPushMatrix();
	        glTranslatef(1.85f, 0.0f, -1);
	        matFloor(); drawBlock(0.3f, 1.0f, 0.2f);
	    glPopMatrix();
        matFloor(); 
    }
    float xLeft  = 1.85f;
    float xRight = 2.0f;

    matFloor(); 
    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(xLeft, 0.0f,        -1.2f);
        glVertex3f(xLeft, 2.4f,        -4.6f);
        glVertex3f(xLeft, 2.4f + 1.0f, -4.6f);
        glVertex3f(xLeft, 0.0f + 1.0f, -1.2f);
        
        glNormal3f(1.0f, 0.0f, 0.0f); 
        glVertex3f(xRight, 0.0f,        -1.2f);
        glVertex3f(xRight, 2.4f,        -4.6f);
        glVertex3f(xRight, 2.4f + 1.0f, -4.6f);
        glVertex3f(xRight, 0.0f + 1.0f, -1.2f);

        glNormal3f(0.0f, 0.9f, 0.4f); 
        glVertex3f(xLeft,  0.0f + 1.0f, -1.2f);
        glVertex3f(xRight, 0.0f + 1.0f, -1.2f);
        glVertex3f(xRight, 2.4f + 1.0f, -4.6f);
        glVertex3f(xLeft,  2.4f + 1.0f, -4.6f);
    glEnd();
    
    xLeft  = 2.0f;
    xRight = 2.15f;

    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(xLeft, 2.4f,        -4.6f);
        glVertex3f(xLeft, FLOOR_HEIGHT - 0.2f, -1.2f);
        glVertex3f(xLeft, FLOOR_HEIGHT + 1.0f, -1.2f);
        glVertex3f(xLeft, 2.4f + 1.0f, -4.6f);

        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(xRight, 2.4f,        -4.6f);
        glVertex3f(xRight, FLOOR_HEIGHT - 0.2f, -1.2f);
        glVertex3f(xRight, FLOOR_HEIGHT + 1.0f, -1.2f);
        glVertex3f(xRight, 2.4f + 1.0f, -4.6f);

        glNormal3f(0.0f, 0.9f, -0.4f);
        glVertex3f(xLeft,  2.4f + 1.0f, -4.6f);
        glVertex3f(xRight, 2.4f + 1.0f, -4.6f);
        glVertex3f(xRight, FLOOR_HEIGHT + 1.0f, -1.2f);
        glVertex3f(xLeft,  FLOOR_HEIGHT + 1.0f, -1.2f);
    glEnd();
    
    for (int i = 0; i < 13; i++) {
        glPushMatrix();
            glTranslatef(0.0f, (float)i * 0.2f, -1.0f - (float)i * 0.3f);
            matStair(); drawBlock(2.0f, 0.2f, 0.3f);
        glPopMatrix();
    }
    glPushMatrix();
        glTranslatef(0.0f, 2.4f, -4.9f);
        bindTexture(TEX_FLOOR_TILE);
        matFloor();  
        glBegin(GL_QUADS);
            glNormal3f(0, 1, 0);
            glTexCoord2f(0, 0);    glVertex3f(0, 0.2f,  0);
            glTexCoord2f(2, 0);    glVertex3f(4, 0.2f,  0);
            glTexCoord2f(2, 1);    glVertex3f(4, 0.2f, -2);
            glTexCoord2f(0, 1);    glVertex3f(0, 0.2f, -2);
        glEnd();
        unbindTexture();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0, 0.05f, 0);
        bindTexture(TEX_FLOOR_TILE);
        matFloor();  
        glBegin(GL_QUADS);
            glNormal3f(0, 1, 0);
            glTexCoord2f(0, 0);    glVertex3f(0, 0,  0);
            glTexCoord2f(2, 0);    glVertex3f(4, 0,  0);
            glTexCoord2f(2, 0.5);   glVertex3f(4, 0, -1);
            glTexCoord2f(0, 0.5);   glVertex3f(0, 0, -1);
        glEnd();
        unbindTexture();
    glPopMatrix();

    for (int i = 0; i < 13; i++) {
        glPushMatrix();
            glTranslatef(2.0f, (-(float)(i + 1) * 0.2f) + FLOOR_HEIGHT, -1.0f - (float)i * 0.3f);
            matStair(); drawBlock(2.0f, 0.2f, 0.3f);
        glPopMatrix();
    }
}

void drawDoubleDoor(float alpha) {
    if (alpha < 1.0f) {
        glDisable(GL_LIGHTING);
        glPushMatrix();
            glTranslatef(0.02f, 0, -0.05f);
            glColor4f(0.45f, 0.28f, 0.15f, alpha);
            drawBlock(1.0f, 2.5f, 0.1f);
            glTranslatef(0.85f, 1.0f, 0.02f);
            glColor4f(0.8f, 0.8f, 0.78f, alpha);
            drawBlock(0.05f, 0.4f, 0.05f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(1.03f, 0, -0.05f);
            glColor4f(0.45f, 0.28f, 0.15f, alpha);
            drawBlock(1.0f, 2.5f, 0.1f);
            glTranslatef(0.1f, 1.0f, 0.02f);
            glColor4f(0.8f, 0.8f, 0.78f, alpha);
            drawBlock(0.05f, 0.4f, 0.05f);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    } else {
        glPushMatrix();
            glTranslatef(0.02f, 0, -0.05f);
            matDoor();   drawBlock(1.0f, 2.5f, 0.1f);
            glTranslatef(0.85f, 1.0f, 0.02f);
            matHandle(); drawBlock(0.05f, 0.4f, 0.05f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(1.03f, 0, -0.05f);
            matDoor();   drawBlock(1.0f, 2.5f, 0.1f);
            glTranslatef(0.1f, 1.0f, 0.02f);
            matHandle(); drawBlock(0.05f, 0.4f, 0.05f);
        glPopMatrix();
    }
}

void drawFrontWall(float width, float depth, float startX, bool isFlipped,
                   float floorY) {
    
    bool  insideZ   = (playerZ < 0.0f);
    bool  insideX   = (playerX >= startX && playerX <= startX + width);
    bool  sameFloor = (playerY >= floorY && playerY < floorY + FLOOR_HEIGHT);
    float alpha     = (insideZ && insideX && sameFloor) ? 0.25f : 1.0f;  

    if (alpha < 1.0f) {
        glDepthMask(GL_FALSE);
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    
    float doorX = isFlipped ? 5.0f : 1.0f;

    
    if (alpha < 1.0f) {
        glColor4f(0.7f, 0.7f, 0.65f, alpha);
        
        drawBlock(doorX, 4.0f, 0.2f);
        
        glPushMatrix();
            glTranslatef(doorX + 2.0f, 0, 0);
            drawBlock(width - (doorX + 2.0f), 4.0f, 0.2f);
        glPopMatrix();
        
        glPushMatrix();
            glTranslatef(doorX, 2.5f, 0);
            drawBlock(2.0f, 1.5f, 0.2f);
        glPopMatrix();
    } else {
        bindTexture(TEX_WALL);
        matConcrete(); 
        drawWallBlock(doorX, 4.0f, 0.2f);
        glPushMatrix();
            glTranslatef(doorX + 2.0f, 0, 0);
            matConcrete(); 
            drawWallBlock(width - (doorX + 2.0f), 4.0f, 0.2f);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(doorX, 2.5f, 0);
            matConcrete(); 
            drawWallBlock(2.0f, 1.5f, 0.2f);
        glPopMatrix();
    }
    int currentFloor = (int)(floorY / FLOOR_HEIGHT);
    int roomIndex = getRoomIndexFromX(startX + 0.1f); 
    
    if (roomIndex >= 0 && roomIndex < NUM_ROOMS_PER_FLOOR && roomIndex != 3) {
        float doorAngle = doorAnims[currentFloor][roomIndex].currentAngle;
        
        glPushMatrix();
            
            glTranslatef(doorX, 0, 0);
            
            drawAnimatedDoor(0, 0, 0, 2.0f, 2.5f, doorAngle, isFlipped, alpha);
        glPopMatrix();
    } else {
        
        glPushMatrix();
            glTranslatef(doorX, 0, 0);
            drawDoubleDoor(alpha);
        glPopMatrix();
    }
    
    if (alpha < 1.0f) {
        glEnable(GL_LIGHTING);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
    
    if (alpha == 1.0f) {
        unbindTexture(); 
    }
}

void drawOneLantai(float offsetY, int currentFloor, bool hasStairDown) {
	bool isLastFloor  = (currentFloor == NUM_FLOORS - 1);
	glPushMatrix();
    glTranslatef(0, offsetY, 0);
        glPushMatrix();
            glTranslatef(0, 0, 4);
            drawCorridor(52, 4, currentFloor);
        glPopMatrix();

        glPushMatrix();
            for (int i = 0; i < 4; i++) {
                drawRoomInterior(8, 10);
                glTranslatef(8, 0, 0);
            }
            drawStairArea(4, 10, isLastFloor, offsetY);
            
            glTranslatef(4, 0, 0);
            for (int i = 0; i < 2; i++) {
                drawRoomInterior(8, 10);
                glTranslatef(8, 0, 0);
            }
        glPopMatrix();
        
        glPushMatrix();
            drawFrontWall(8, 10,  0, false, offsetY); glTranslatef(8, 0, 0);
            drawFrontWall(8, 10,  8, true,  offsetY); glTranslatef(8, 0, 0);
            drawFrontWall(8, 10, 16, false, offsetY); glTranslatef(8, 0, 0);
            drawFrontWall(8, 10, 24, true,  offsetY); glTranslatef(12, 0, 0); 
            drawFrontWall(8, 10, 36, false, offsetY); glTranslatef(8, 0, 0);
            drawFrontWall(8, 10, 44, false, offsetY);
        glPopMatrix();

    glPopMatrix();

	for (int room = 0; room < 4; room++) {
        drawRoomPosters(currentFloor, room);
    }
}

void drawRuangGedung() {
    for (int f = 0; f < NUM_FLOORS; f++) {
        bool hasStairDown = (f > 0);
        drawOneLantai(f * FLOOR_HEIGHT, f, hasStairDown);
    }
}

void drawLockedDoorMarker(float alpha) {
    glDisable(GL_LIGHTING);
    glColor4f(0.15f, 0.15f, 0.18f, alpha);
    
    glPushMatrix();
        glTranslatef(0.5f, 1.2f, 0.08f);
        drawBlock(1.0f, 0.08f, 0.06f);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0.9f, 0.8f, 0.08f);
        drawBlock(0.12f, 0.5f, 0.06f);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0.85f, 1.0f, 0.1f);
        glColor4f(0.25f, 0.25f, 0.28f, alpha);
        glutSolidCube(0.15f);
    glPopMatrix();
    
    glColor4f(0.6f, 0.1f, 0.1f, alpha * 0.8f);
    glPushMatrix();
        glTranslatef(0.5f, 1.8f, 0.08f);
        drawBlock(0.8f, 0.15f, 0.04f);
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
}

void drawAnimatedDoor(float x, float y, float z, 
                      float width, float height, 
                      float angle, bool flipped, float alpha) {
    if (alpha < 1.0f) {
        glDisable(GL_LIGHTING);
        glDepthMask(GL_FALSE);
    }
    float halfWidth = width / 2.0f;
    
    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(angle, 0.0f, 1.0f, 0.0f);
        if (alpha < 1.0f) {
            glColor4f(0.45f, 0.28f, 0.15f, alpha);
        } else {
            matDoor();
        }
        drawBlock(halfWidth, height, 0.1f);
        glPushMatrix();
            glTranslatef(halfWidth - 0.15f, height * 0.4f, 0.05f);
            if (alpha < 1.0f) {
                glColor4f(0.8f, 0.8f, 0.78f, alpha);
            } else {
                matHandle();
            }
            drawBlock(0.05f, 0.4f, 0.05f);
        glPopMatrix();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(x + width, y, z);
        glRotatef(-angle, 0.0f, 1.0f, 0.0f);
        glTranslatef(-halfWidth, 0, 0);
        
        if (alpha < 1.0f) {
            glColor4f(0.45f, 0.28f, 0.15f, alpha);
        } else {
            matDoor();
        }
        drawBlock(halfWidth, height, 0.1f);
        
        glPushMatrix();
            glTranslatef(0.15f, height * 0.4f, 0.05f);
            if (alpha < 1.0f) {
                glColor4f(0.8f, 0.8f, 0.78f, alpha);
            } else {
                matHandle();
            }
            drawBlock(0.05f, 0.4f, 0.05f);
        glPopMatrix();
    glPopMatrix();

    if (alpha < 1.0f) {
        glEnable(GL_LIGHTING);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
}
