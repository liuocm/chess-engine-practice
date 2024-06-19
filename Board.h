//
//  Board.h
//  Chess
//
//  Created by Liu Martin on 6/24/22.
//

#ifndef BOARD_INCLUDED
#define BOARD_INCLUDED

#include <iostream>
#include <vector>
#include "Piece.h"

class Board
{
public:
    Board();
    ~Board();
    
    void placePieces(int color); // Places pieces in the correct position to start the game.
    
    bool attemptMove(Piece* piece, int proposedR, int proposedC); // Attempts to move piece to (proposedR, proposedC), and returns true if it succeeds.
    bool castle(Piece* king, int proposedR, int proposedC);
    bool enPassant(Piece* pawn, int propsoedR, int proposedC);
    void promotePawn(Piece* pawn, int promotionID);
    
    bool squareInCheck(int row, int col, int attackingColor); // Returns true if this square is in check.
    bool kingSafe(Piece* pieceToMove, int proposedR, int proposedC); // Returns true if the proposed move won't put the king in check.
    bool kingCheckmated(int color); // Returns true if the king of the specified color has been checkmated.
    bool kingStalemated(int color); // Returns true if the king of the specified color has been stalemated.
    bool canCastle(King* king, int proposedR, int proposedC); // Returns true if king can castle.
    bool canEnPassant(Pawn* pawn, int proposedR, int proposedC); // Returns true if pawn can en passant.
    
    // ACCESSORS
    int totalMoves();
    std::vector<Piece*>& pieces(int color); // Returns a reference to m_white or m_black, depending on the specified color.
    Piece*& pieceAtPos(int r, int c); // Returns a reference to the Piece pointer at [r-1][c-1] in m_piecePositions.
    King*& getKing(int color); // Returns a reference to m_whiteKing or m_blackKing, depending on the specified color.

private:
    int m_rows = 8;
    int m_cols = 8;
    int m_totalMoves = 0;
    
    std::vector<Piece*> m_white;
    std::vector<Piece*> m_black;
    King* m_whiteKing;
    King* m_blackKing;
    
    Piece* m_piecePositions[8][8];
};

#endif /* BOARD_INCLUDED */
