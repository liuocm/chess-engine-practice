//
//  Engine.cpp
//  Chess
//
//  Created by Liu Martin on 7/6/22.
//

#include "Engine.h"
#include "Board.h"
#include <cstdlib>
#include <math.h>
#include <GLUT/glut.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR/DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////
Engine::Engine()
 : b(new Board)
{
    if (!loadAssets())
    {
        cerr << "FAILURE LOADING ASSETS" << endl;
        exit(-999);
    }
}

Engine::~Engine()
{
    delete b;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// CALLBACKS/RUN
////////////////////////////////////////////////////////////////////////////////////////////////
// void keyboardEventCallback(unsigned char key, int x, int y) {}
// void specialKeyboardEventCallback(int key, int x, int y) {}
// glutKeyboardFunc(keyboardEventCallback);
// glutSpecialFunc(specialKeyboardEventCallback);

void reshapeCallback(int w, int h)
{
}

void displayCallback()
{
    Eng().displayControl();
}

void mouseCallback(int button, int state, int x, int y)
{
    Eng().mouseControl(button, state, x, y);
}

void Engine::run(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    glutInitWindowSize(1200, 847);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Chess");
    glViewport(VP_BORDER, VP_BORDER, VP_HEIGHT, VP_WIDTH);
    
    glutReshapeFunc(reshapeCallback);
    glutDisplayFunc(displayCallback);
    glutIdleFunc(displayCallback);
    
    glutMouseFunc(mouseCallback);

    glutMainLoop();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// CONTROL
////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::displayControl()
{
    switch(m_gameState)
    {
        case 0:
            displayMenu();
            break;
        case 1:
            displayGameplay();
            break;
        case 2:
            displayEndScreen(false);
            break;
        case 3:
            displayEndScreen(true);
            break;
    }
}

void Engine::mouseControl(int button, int state, int x, int y)
{
    switch (m_gameState) {
        case 0:
            mouseMenu(button, state, x, y);
            break;
        case 1:
            if (m_ppMenu)
            {
                mousePawnPromotion(button, state, x, y);
            }
            else
            {
                mouseGameplay(button, state, x, y);
            }
            break;
        case 2:
        case 3:
            mouseEndScreen(button, state, x, y);
            break;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY
////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::displayMenu()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    drawBackdrop();
    drawMainMenu();
    
    glutSwapBuffers();
}

void Engine::displayGameplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawBoard();
    drawPieces();
    if (m_ppMenu)
    {
        drawPawnPromotion();
    }
    
    glutSwapBuffers();
}

void Engine::displayEndScreen(bool draw)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    drawBackdrop();
    drawEndScreen(draw);
    
    glutSwapBuffers();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// MOUSE INPUT
////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::mouseMenu(int button, int state, int x, int y)
{
    x -= VP_BORDER;
    y -= VP_BORDER;
    
    // Click on Player vs. Player
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x >= 35 && x <= 315 && y >= 280 && y <= 420)
    {
        m_gameState = 1;
    }
}

void Engine::mouseGameplay(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x >= VP_BORDER && x <= VP_WIDTH + VP_BORDER && y >= VP_BORDER && y <= VP_HEIGHT + VP_BORDER)
    {
        x -= VP_BORDER;
        y -= VP_BORDER;
        int newR = 9 - (y / 87.5);
        int newC = x / 87.5 + 1;
        
        // Clicking a square that is already selected
        if (selectionToggled && newR == selectedR && newC == selectedC)
        {
            selectionToggled = !selectionToggled;
            selectedR = -1;
            selectedC = -1;
        }
        // Clicking a square when none is selected
        else if (!selectionToggled)
        {
            selectionToggled = !selectionToggled;
            selectedR = newR;
            selectedC = newC;
        }
        // Clicking a different square from the one that is currently selected
        else if (selectionToggled && (newR != selectedR || newC != selectedC))
        {
            Piece* piece = b->pieceAtPos(selectedR, selectedC);
            if (piece != nullptr && b->totalMoves() % 2 == piece->color() && b->attemptMove(piece, newR, newC))
            {
                selectionToggled = false;
                lastSelR = newR;
                lastSelC = newC;
                
                selectedR = -1;
                selectedC = -1;
                if (b->kingCheckmated(b->totalMoves() % 2))
                {
                    m_gameState = 2;
                }
                else if (b->kingStalemated(b->totalMoves() % 2))
                {
                    m_gameState = 3;
                }
            }
            else
            {
                selectedR = newR;
                selectedC = newC;
            }
        }
    }
}

void Engine::mousePawnPromotion(int button, int state, int x, int y)
{
    x -= VP_BORDER;
    y -= VP_BORDER;
        
    int row = 9 - (y / 87.5);
    int col = (x / 87.5) + 1;
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && abs(lastSelR - row) <= 3 && lastSelC == col)
    {
        int promotionID = 2 + (2 * (abs(lastSelR) - row)) + ((b->totalMoves() + 1) % 2);
        b->promotePawn(b->pieceAtPos(lastSelR, lastSelC), promotionID);
        ppMenu(false);
    }
}

void Engine::mouseEndScreen(int button, int state, int x, int y)
{
    x -= VP_BORDER;
    y -= VP_BORDER;
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x >= 280 && x <= 420 && y >= 420 && y <= 490)
    {
        exit(EXIT_SUCCESS);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
// OTHER
////////////////////////////////////////////////////////////////////////////////////////////////
void Engine::drawPieces()
{
    // TEXTURE
    glEnable(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    // Makes background for texture transparent (as opposed to black)
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    const float PIECE_SIZE = 0.20;
    
    // Draw white
    for (int i = 0; i < b->pieces(WHITE).size(); i++)
    {
        if (b->pieces(WHITE)[i]->alive())
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pieceWidth, pieceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data[b->pieces(WHITE)[i]->pieceID()]);
            
            float xCoord = (0.25 * b->pieces(WHITE)[i]->col() - 1.0) - (0.25 - PIECE_SIZE)/2;
            float yCoord = (0.25 * b->pieces(WHITE)[i]->row() - 1.0) - (0.25 - PIECE_SIZE)/2;
            
            glBegin(GL_QUADS);
            glTexCoord2f(1.0, 1.0); glVertex2f(xCoord, yCoord);
            glTexCoord2f(0.0, 1.0); glVertex2f(xCoord - PIECE_SIZE,  yCoord);
            glTexCoord2f(0.0, 0.0); glVertex2f(xCoord - PIECE_SIZE, yCoord - PIECE_SIZE);
            glTexCoord2f(1.0, 0.0); glVertex2f(xCoord, yCoord - PIECE_SIZE);
            glEnd();
        }
    }
    
    // Draw black
    for (int i = 0; i < b->pieces(BLACK).size(); i++)
    {
        if (b->pieces(BLACK)[i]->alive())
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pieceWidth, pieceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data[b->pieces(BLACK)[i]->pieceID()]);
            
            float xCoord = (0.25 * b->pieces(BLACK)[i]->col() - 1.0) - (0.25 - PIECE_SIZE)/2;
            float yCoord = (0.25 * b->pieces(BLACK)[i]->row() - 1.0) - (0.25 - PIECE_SIZE)/2;
            
            glBegin(GL_QUADS);
            glTexCoord2f(1.0, 1.0); glVertex2f(xCoord, yCoord);
            glTexCoord2f(0.0, 1.0); glVertex2f(xCoord - PIECE_SIZE,  yCoord);
            glTexCoord2f(0.0, 0.0); glVertex2f(xCoord - PIECE_SIZE, yCoord - PIECE_SIZE);
            glTexCoord2f(1.0, 0.0); glVertex2f(xCoord, yCoord - PIECE_SIZE);
            glEnd();
        }
    }
    glDisable(GL_TEXTURE_2D);
}

void Engine::drawBoard()
{
    float coordY = -0.75;
    for (int r = 1; r <= 8; r++)
    {
        float coordX = -0.75;
        for (int c = 1; c <= 8; c++)
        {
            float red;
            float green;
            float blue;
            if (selectionToggled && r == selectedR && c == selectedC && b->pieceAtPos(selectedR, selectedC) != nullptr) // Selected Square
            {
                if (r % 2 == c % 2) // Black squares
                {
                    red = 106.0/255.0;
                    green = 111.0/255.0;
                    blue = 65.0/255.0;
                }
                else // White squares
                {
                    red = 134.0/255.0;
                    green = 151.0/255.0;
                    blue = 105.0/255.0;
                }
            }
            else
            {
                if (r % 2 == c % 2) // Black squares
                {
                    red = 181.0/255.0;
                    green = 136.0/255.0;
                    blue = 99.0/255.0;
                }
                else // White squares
                {
                    red = 240.0/255.0;
                    green = 217.0/255.0;
                    blue = 181.0/255.0;
                }
            }
            
            glColor3f(red, green, blue);
            
            glBegin(GL_QUADS);
            glVertex2f(coordX - 0.25, coordY);
            glVertex2f(coordX - 0.25, coordY - 0.25);
            glVertex2f(coordX, coordY - 0.25);
            glVertex2f(coordX, coordY);
            glEnd();
            
            coordX += 0.25;
        }
        coordY += 0.25;
    }
    if (selectionToggled == true)
    {
        drawPossibleMoves(b->pieceAtPos(selectedR, selectedC));
    }
}

void Engine::drawPossibleMoves(Piece* piece)
{
    if (piece == nullptr)
    {
        return;
    }
    
    for (int r = 1; r <= 8; r++)
    {
        for (int c = 1; c <= 8; c++)
        {
            if (b->totalMoves() % 2 == piece->color() && piece->movePossible(r, c))
            {
                float red;
                float green;
                float blue;
                if (r % 2 == c % 2)
                {
                    red = 106.0/255.0;
                    green = 111.0/255.0;
                    blue = 65.0/255.0;
                }
                else
                {
                    red = 134.0/255.0;
                    green = 151.0/255.0;
                    blue = 105.0/255.0;
                }
                if (b->pieceAtPos(r, c) == nullptr)
                {
                    // radius, and center coordinate of octagon
                    float octRad = 0.03125;
                    float octX = 0.25 * c - 1.00 - 0.125;
                    float octY = 0.25 * r - 1.00 - 0.125;
                    // The distance to the diag along the x/y axis -- a^2 + b^2 = octRad^2; 2a^2 = octRad^2; a = sqrt(octRad^2)/sqrt(2)
                    float octDiag = sqrt(octRad * octRad)/sqrt(2.0);
                    
                    glColor3f(red, green, blue);
                    glBegin(GL_POLYGON);
                    glVertex2f(octX, octY + octRad);
                    glVertex2f(octX - octDiag, octY + octDiag);
                    glVertex2f(octX - octRad, octY);
                    glVertex2f(octX - octDiag, octY - octDiag);
                    glVertex2f(octX, octY - octRad);
                    glVertex2f(octX + octDiag, octY - octDiag);
                    glVertex2f(octX + octRad, octY);
                    glVertex2f(octX + octDiag, octY + octDiag);
                    glEnd();
                }
                else
                {
                    float triBase = 0.05;
                    
                    float topRightX = 0.25 * c - 1.0;
                    float topRightY = 0.25 * r - 1.0;
                    
                    float topLeftX = 0.25 * (c - 1) - 1.0;
                    float topLeftY = 0.25 * r - 1.0;
                    
                    float botLeftX = 0.25 * (c - 1) - 1.0;
                    float botLeftY = 0.25 * (r - 1) - 1.0;
                    
                    float botRightX = 0.25 * c - 1.0;
                    float botRightY = 0.25 * (r-1) - 1.0;

                    glColor3f(red, green, blue);
                    
                    glBegin(GL_TRIANGLES);
                    glVertex2f(topRightX, topRightY);
                    glVertex2f(topRightX - triBase, topRightY);
                    glVertex2f(topRightX, topRightY - triBase);
                    
                    glVertex2f(topLeftX, topLeftY);
                    glVertex2f(topLeftX, topLeftY - triBase);
                    glVertex2f(topLeftX + triBase, topLeftY);
                    
                    glVertex2f(botLeftX, botLeftY);
                    glVertex2f(botLeftX + triBase, botLeftY);
                    glVertex2f(botLeftX, botLeftY + triBase);
                    
                    glVertex2f(botRightX, botRightY);
                    glVertex2f(botRightX - triBase, botRightY);
                    glVertex2f(botRightX, botRightY + triBase);
                    glEnd();
                }
            }
        }
    }
}

void Engine::drawPawnPromotion()
{
    float xCoord = 0.25 * lastSelC - 1.0;
    float yCoord;
    if (lastSelR == 8)
    {
        yCoord = 1.0;
    }
    else
    {
        yCoord = 0.0;
    }
    
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(xCoord, yCoord);
    glVertex2f(xCoord - 0.25, yCoord);
    glVertex2f(xCoord - 0.25, yCoord - 1.0);
    glVertex2f(xCoord, yCoord - 1.0);
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    // Makes background for texture transparent (as opposed to black)
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    const float PIECE_SIZE = 0.20;
    
    if (lastSelR == 8)
    {
        yCoord = 1.0;
    }
    else
    {
        yCoord = -0.75;
    }
    
    xCoord -= ((0.25 - PIECE_SIZE)/2.0);
    yCoord -= ((0.25 - PIECE_SIZE)/2.0);
    for (int i = 1; i <= 4; i++)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pieceWidth, pieceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data[(i * 2) + ((b->totalMoves() - 1) % 2)]);
                
        glBegin(GL_QUADS);
        glTexCoord2f(1.0, 1.0); glVertex2f(xCoord, yCoord);
        glTexCoord2f(0.0, 1.0); glVertex2f(xCoord - PIECE_SIZE,  yCoord);
        glTexCoord2f(0.0, 0.0); glVertex2f(xCoord - PIECE_SIZE, yCoord - PIECE_SIZE);
        glTexCoord2f(1.0, 0.0); glVertex2f(xCoord, yCoord - PIECE_SIZE);
        glEnd();
        
        float dir = (-2.0 * (b->totalMoves() % 2) + 1);
        yCoord += (0.25 * dir);
    }
    glDisable(GL_TEXTURE_2D);
}

void Engine::drawBackdrop()
{
    glColor3f(1.0, 1.0, 1.0);
    
    glBegin(GL_QUADS);
    glVertex2f(1, 1);
    glVertex2f(-1, 1);
    glVertex2f(-1, -1);
    glVertex2f(1, -1);
    glEnd();
}

void Engine::drawMainMenu()
{
    float buttonW = 0.8;
    float buttonH = 0.4;

    glEnable(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Makes background for texture transparent (as opposed to black)
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, buttonWidth, buttonHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data[12]);
    
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex2f(-0.1, 0.2);
    glTexCoord2f(0.0, 1.0); glVertex2f(-0.1 - buttonW, 0.2);
    glTexCoord2f(0.0, 0.0); glVertex2f(-0.1 - buttonW, 0.2 - buttonH);
    glTexCoord2f(1.0, 0.0); glVertex2f(-0.1, 0.2 - buttonH);
    glEnd();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, buttonWidth, buttonHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data[13]);

    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex2f(0.9, 0.2);
    glTexCoord2f(0.0, 1.0); glVertex2f(0.9 - buttonW, 0.2);
    glTexCoord2f(0.0, 0.0); glVertex2f(0.9 - buttonW, 0.2 - buttonH);
    glTexCoord2f(1.0, 0.0); glVertex2f(0.9, 0.2 - buttonH);
    glEnd();


    glDisable(GL_TEXTURE_2D);
}

void Engine::drawEndScreen(bool draw)
{
    int textureIndex;
    if (draw)
    {
        textureIndex = 16;
    }
    else
    {
        textureIndex = 14 + ((b->totalMoves() + 1) % 2);
    }
    glEnable(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Makes background for texture transparent (as opposed to black)
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // OUTCOME BANNER
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, buttonWidth, buttonHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data[textureIndex]);
    
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex2f(0.6, 0.6);
    glTexCoord2f(0.0, 1.0); glVertex2f(-0.6, 0.6);
    glTexCoord2f(0.0, 0.0); glVertex2f(-0.6, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex2f(0.6, 0.0);
    glEnd();
    
    // EXIT BUTTON
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, buttonWidth, buttonHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data[17]);
    
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex2f(0.2, -0.2);
    glTexCoord2f(0.0, 1.0); glVertex2f(-0.2, -0.2);
    glTexCoord2f(0.0, 0.0); glVertex2f(-0.2, -0.4);
    glTexCoord2f(1.0, 0.0); glVertex2f(0.2, -0.4);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

bool Engine::loadAssets()
{
    // LOAD IMAGES
    stbi_set_flip_vertically_on_load(true);
    texture_data[0] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/king_white.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    texture_data[1] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/king_black.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    
    texture_data[2] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/queen_white.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    texture_data[3] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/queen_black.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    
    texture_data[4] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/rook_white.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    texture_data[5] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/rook_black.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    
    texture_data[6] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/bishop_white.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    texture_data[7] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/bishop_black.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    
    texture_data[8] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/knight_white.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    texture_data[9] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/knight_black.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    
    texture_data[10] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/pawn_white.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    texture_data[11] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/pawn_black.png", &pieceWidth, &pieceHeight, &nrChannels, 0);
    
    texture_data[12] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/button_vsPlayer.png", &buttonWidth, &buttonHeight, &nrChannels, 0);
    texture_data[13] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/button_vsComputer.png", &buttonWidth, &buttonHeight, &nrChannels, 0);
    
    texture_data[14] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/banner_whiteWins.png", &bannerWidth, &bannerHeight, &nrChannels, 0);
    texture_data[15] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/banner_blackWins.png", &bannerWidth, &bannerHeight, &nrChannels, 0);
    texture_data[16] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/banner_draw.png", &bannerWidth, &bannerHeight, &nrChannels, 0);
    texture_data[17] = stbi_load("/Users/liumartin/Dropbox/Chess/Chess/assets/button_exit.png", &buttonWidth, &buttonHeight, &nrChannels, 0);
    
    for (int i = 0; i < NUM_TEXTURES; i++)
    {
        if (!texture_data[i])
        {
            std::cerr << "Texture at texture_data[" << i << "] not found!" << std::endl;
            return false;
        }
    }
    return true;
}

void Engine::ppMenu(bool status)
{
    m_ppMenu = status;
}
