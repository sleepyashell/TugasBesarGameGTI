#include "Bot.h"

#include <cstdlib> 
#include <cmath>    

#include "World.h"
#include "Jumpscare.h"

EnemyBot ghostBots[NUM_FLOORS];




const int NUM_NODES = 3;
Waypoint nodes[NUM_NODES] = {
    { 2.0f	},  
    { 33.0f	},  
    { 50.0f	}   
};




void initBot(){
	
    for (int i = 0; i < NUM_FLOORS; i++){
        
        if (i == 1){	
            ghostBots[i].x = nodes[0].x;
            ghostBots[i].visionX = 1.0f;
            ghostBots[i].movingRight = true;
            ghostBots[i].targetNodeIndex = 1;
        } else{ 
            ghostBots[i].x = nodes[2].x;
            ghostBots[i].visionX = -1.0f;
            ghostBots[i].movingRight = false;
            ghostBots[i].targetNodeIndex = 1; 
        }  
        
        ghostBots[i].y = (i * 5) + 0.8f;
        ghostBots[i].z = 2.0f;
        ghostBots[i].speed = 0.05f;  
        ghostBots[i].isChasing = false;
        ghostBots[i].isSearching = false;
    }
}






bool scanForPlayer(int botIdx){
    int playerFloor = (int)(playerY / FLOOR_HEIGHT); 

    
    if (botIdx == playerFloor && playerZ >= 0.0f){
        float distanceX = std::abs(playerX - ghostBots[botIdx].x);
        float distanceZ = std::abs(playerZ - ghostBots[botIdx].z);
        float totalDistance = sqrtf(distanceX * distanceX + distanceZ * distanceZ);
        float toPlayerX = playerX - ghostBots[botIdx].x;
        float dotProduct = ghostBots[botIdx].visionX * toPlayerX;

        if (dotProduct > 0.0f){
            if (totalDistance < 5.0f) return true;
        } else{
            if (totalDistance < 2.5f) return true;
        }
    } 
    return false;
}


void corridorPatrol(int botIdx){
    Waypoint nodeKiri  = nodes[0];
    Waypoint nodeKanan = nodes[2];

    
    if (ghostBots[botIdx].z < 2.0f){
        ghostBots[botIdx].z += ghostBots[botIdx].speed;
        if (ghostBots[botIdx].z > 2.0f) ghostBots[botIdx].z = 2.0f;
        return;
    }

	
    if (ghostBots[botIdx].movingRight){
        float stepX = ghostBots[botIdx].x + ghostBots[botIdx].speed;
        if (stepX < nodeKanan.x && !checkCollision(stepX, ghostBots[botIdx].z)){
            ghostBots[botIdx].x = stepX;
        } else{
            ghostBots[botIdx].movingRight = false;
        }
    } else { 
        float stepX = ghostBots[botIdx].x - ghostBots[botIdx].speed;
        if (stepX > nodeKiri.x && !checkCollision(stepX, ghostBots[botIdx].z)){
            ghostBots[botIdx].x = stepX;
        } else{
            ghostBots[botIdx].movingRight = true;
        }
    }
}


void chase(int botIdx, int playerFloor){
    float distanceX = std::abs(playerX - ghostBots[botIdx].x);
    float distanceZ = std::abs(playerZ - ghostBots[botIdx].z);

    if (ghostBots[botIdx].currentFloor == playerFloor){
        float nextX = ghostBots[botIdx].x;
        float nextZ = ghostBots[botIdx].z;
        
        int roomIdx = getRoomIndexFromX(playerX);
        if (roomIdx < 0){
            roomIdx = getRoomIndexFromX(ghostBots[botIdx].x);
        }
        
        if (roomIdx != 1 && roomIdx != 3 && roomIdx >= 4){
            if (ghostBots[botIdx].z < 0.0f && playerZ >= 0.0f){
                roomIdx = (int)(ghostBots[botIdx].x / 8.0f);
            }
        }
        float targetDoorX = doorInfos[roomIdx].x + (doorInfos[roomIdx].w * 0.5f);
        
        if (playerZ < 0.0f && ghostBots[botIdx].z >= 0.0f) {
            if (std::abs(ghostBots[botIdx].x - targetDoorX) > 0.1f){
                if (ghostBots[botIdx].x < targetDoorX){ 
                    nextX += ghostBots[botIdx].speed; 
                    ghostBots[botIdx].movingRight = true; 
                }
                else if (ghostBots[botIdx].x > targetDoorX){ 
                    nextX -= ghostBots[botIdx].speed; 
                    ghostBots[botIdx].movingRight = false; 
                }
            } 
            else if (ghostBots[botIdx].z > 0.0f) {
                nextZ -= ghostBots[botIdx].speed;
            }
        }
        
        else if (playerZ >= 0.0f && ghostBots[botIdx].z < 0.0f){
            if (std::abs(ghostBots[botIdx].x - targetDoorX) > 0.1f){
                if (ghostBots[botIdx].x < targetDoorX){ 
                    nextX += ghostBots[botIdx].speed; 
                    ghostBots[botIdx].movingRight = true; 
                }
                else if (ghostBots[botIdx].x > targetDoorX){ 
                    nextX -= ghostBots[botIdx].speed; 
                    ghostBots[botIdx].movingRight = false; 
                }
            }
            else if (ghostBots[botIdx].z < 2.0f){
                nextZ += ghostBots[botIdx].speed;
            }
        }
        
        else {
            if (ghostBots[botIdx].x < playerX){ 
                nextX += ghostBots[botIdx].speed; 
                ghostBots[botIdx].movingRight = true; 
            }
            else if (ghostBots[botIdx].x > playerX){ 
                nextX -= ghostBots[botIdx].speed; 
                ghostBots[botIdx].movingRight = false; 
            }
            
            if (ghostBots[botIdx].z < playerZ){
                nextZ += ghostBots[botIdx].speed;
            }
            else if (ghostBots[botIdx].z > playerZ){
                nextZ -= ghostBots[botIdx].speed;
            }
        }
        if (!checkCollision(nextX, ghostBots[botIdx].z)) ghostBots[botIdx].x = nextX;
        if (!checkCollision(ghostBots[botIdx].x, nextZ)) ghostBots[botIdx].z = nextZ;
    }
    
    if (distanceX < 0.8f && distanceZ < 1.0f && ghostBots[botIdx].currentFloor == playerFloor){
        jumpscareManager.startJumpscare();
    }
}


void updateBot(){
    int playerFloor = (int)(playerY / FLOOR_HEIGHT); 
    for (int i = 0; i < NUM_FLOORS; i++){
        ghostBots[i].visionX = (ghostBots[i].movingRight) ? 1.0f : -1.0f;
        
        float dX = playerX - ghostBots[i].x;
        float dZ = playerZ - ghostBots[i].z;
        float totalDistance = sqrtf(dX * dX + dZ * dZ);

        if (ghostBots[i].isChasing) {
            int playerFloorNow = (int)(playerY / FLOOR_HEIGHT); 
            
            bool playerInStairArea = (playerX >= 32.0f && playerX <= 36.0f && playerZ < 0.8f);
            
            if (ghostBots[i].currentFloor != playerFloorNow || playerInStairArea){
                ghostBots[i].isChasing = false;
                ghostBots[i].isSearching = false;
                ghostBots[i].speed = 0.05f;
            }
            
            else if (totalDistance > 8.0f){
                ghostBots[i].isChasing = false;   
                ghostBots[i].isSearching = false;
                ghostBots[i].speed = 0.05f;
            }
        } else{
            bool playerSpotted = scanForPlayer(i);

            if (playerSpotted) {
                ghostBots[i].isChasing = true;
                ghostBots[i].isSearching = false;
                ghostBots[i].speed = 0.065f;
            } else if (ghostBots[i].isSearching) {
                ghostBots[i].isSearching = false;
            }
        }

        if (ghostBots[i].isChasing) {
            chase(i, playerFloor);
        } 
        else {
            corridorPatrol(i);
        }
    }
}


void drawBot() {
    glEnable(GL_COLOR_MATERIAL);

    for (int i = 0; i < NUM_FLOORS; i++){
        float hover = sin(glutGet(GLUT_ELAPSED_TIME) * 0.005f + i)* 0.08f;
        float sway = sin(glutGet(GLUT_ELAPSED_TIME) * 0.01f + i) * 8.0f;

        glPushMatrix();
            glTranslatef(
                ghostBots[i].x,
                ghostBots[i].y + hover,
                ghostBots[i].z
            );

            if (ghostBots[i].movingRight)
                glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
            else
                glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

            
            if (ghostBots[i].isChasing)
                glColor3f(0.45f, 0.0f, 0.0f);
            else
                glColor3f(0.12f, 0.12f, 0.12f);

            glPushMatrix();
                glScalef(0.45f, 1.4f, 0.25f);
                glutSolidCube(1.0f);
            glPopMatrix();

            
            glPushMatrix();

                glTranslatef(0.0f, 1.0f, 0.0f);
                glRotatef(sway, 0.0f, 1.0f, 0.0f);

                glColor3f(0.08f, 0.08f, 0.08f);

                glPushMatrix();
                    glScalef(0.5f, 0.5f, 0.5f);
                    glutSolidCube(1.0f);
                glPopMatrix();

                
                glColor3f(1.0f, 0.0f, 0.0f);

                glPushMatrix();
                    glTranslatef(-0.10f, 0.05f, 0.26f);
                    glutSolidSphere(0.05f, 8, 8);
                glPopMatrix();

                
                glPushMatrix();
                    glTranslatef(0.10f, 0.05f, 0.26f);
                    glutSolidSphere(0.05f, 8, 8);
                glPopMatrix();

                
                glColor3f(0.0f, 0.0f, 0.0f);

                glPushMatrix();
                    glTranslatef(0.0f, -0.10f, 0.26f);
                    glScalef(0.25f, 0.08f, 0.05f);
                    glutSolidCube(1.0f);
                glPopMatrix();

            glPopMatrix();

            
            glColor3f(0.10f, 0.10f, 0.10f);

            glPushMatrix();
                glTranslatef(-0.35f, 0.15f, 0.0f);
                glScalef(0.12f, 1.3f, 0.12f);
                glutSolidCube(1.0f);
            glPopMatrix();

            
            glPushMatrix();
                glTranslatef(0.35f, 0.15f, 0.0f);
                glScalef(0.12f, 1.3f, 0.12f);
                glutSolidCube(1.0f);
            glPopMatrix();

            
            glPushMatrix();
                glTranslatef(-0.12f, -1.0f, 0.0f);
                glScalef(0.12f, 0.9f, 0.12f);
                glutSolidCube(1.0f);
            glPopMatrix();

            
            glPushMatrix();
                glTranslatef(0.12f, -1.0f, 0.0f);
                glScalef(0.12f, 0.9f, 0.12f);
                glutSolidCube(1.0f);
            glPopMatrix();

            
            glColor3f(0.7f, 0.0f, 0.0f);

            glPushMatrix();
                glTranslatef(-0.35f, -0.55f, 0.0f);
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                glutSolidCone(0.05f, 0.18f, 6, 2);
            glPopMatrix();

            
            glPushMatrix();
                glTranslatef(0.35f, -0.55f, 0.0f);
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                glutSolidCone(0.05f, 0.18f, 6, 2);
            glPopMatrix();
        glPopMatrix();
    }
    glDisable(GL_COLOR_MATERIAL);
}
