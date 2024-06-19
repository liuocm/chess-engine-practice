//
//  Engine.h
//  Chess
//
//  Created by Liu Martin on 7/6/22.
//

#ifndef ENGINE_INCLUDED
#define ENGINE_INCLUDED

#include <iostream>
#include "globals.h"
class Board;
class Piece;

class Engine
{
public:
    // CONSTRUCTOR/DESTRUCTOR
    Engine();
    ~Engine();
    
    // RUN
    void run(int argc, char* argv[]);
    
    // CONTROLS
    void displayControl();
    void mouseControl(int button, int state, int x, int y);
    
    // DISPLAY
    void displayMenu();
    void displayGameplay();
    void displayEndScreen(bool draw);
    
    // MOUSE INPUT
    void mouseMenu(int button, int state, int x, int y);
    void mouseGameplay(int button, int state, int x, int y);
    void mousePawnPromotion(int button, int state, int x, int y);
    void mouseEndScreen(int button, int state, int x, int y);
    
    // Gameplay Display Functions
    void drawPieces();
    void drawBoard();
    void drawPossibleMoves(Piece* piece);
    void drawPawnPromotion();
    
    // Menu/End Display Functions
    void drawBackdrop();
    void drawMainMenu();
    void drawEndScreen(bool draw);
    
    // Load Assets
    bool loadAssets();
    
    void ppMenu(bool status);
    
    // Get Instance
    static Engine& getInstance()
    {
        static Engine instance;
        return instance;
    }
    
private:
    Board* b;
    unsigned char* texture_data[NUM_TEXTURES];
    
    int m_gameState = 0;
    bool m_ppMenu = false;
    
    int pieceWidth;
    int pieceHeight;
    
    int buttonHeight;
    int buttonWidth;
    
    int bannerHeight;
    int bannerWidth;
    
    int nrChannels;
    
    int selectedR = -1;
    int selectedC = -1;
    int selectionToggled = false;
    
    int lastSelR = -1;
    int lastSelC = -1;
};

inline Engine& Eng()
{
    return Engine::getInstance();
}

#endif /* ENGINE_INCLUDED */
