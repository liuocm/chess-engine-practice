//
//  Piece.h
//  Chess
//
//  Created by Liu Martin on 6/24/22.
//

#ifndef PIECE_INCLUDED
#define PIECE_INCLUDED

#include <iostream>
#include <string>
class Board;


////////////////////////////////////////////////////////////////////////////////////////////////
// PIECE BASE CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
class Piece
{
public:
    Piece(int row, int col, int color, int pieceID, Board* b);
    virtual ~Piece();
    
    virtual bool mpAdherent(int proposedR, int proposedC) = 0; // Returns true if (proposedR, proposedC) adheres to the piece's movement pattern.
    virtual bool notBlocked (int proposedR, int proposedC); // Returns true if the squares between the piece and (propsoedR, proposedC) are not occupied.
    bool noPieceOverlap (int proposedR, int proposedC); // Returns true if there is not a piece of the same color at (proposedR, proposedC).
    bool movePossible(int proposedR, int proposedC); // Returns true if the move to (proposedR, proposedC) is mpAdherent, notBlocked, noPieceOverlap, and kingSafe.
    
    // MUTATORS
    void updatePos(int proposedR, int proposedC);
    void incrementMoves();
    void setAliveStatus(bool alive);
    
    // ACCESSORS
    int row() const;
    int col() const;
    int numMoves() const;
    bool alive() const;
    int color() const;
    int pieceID() const;
    Board* board() const;
    
private:
    int m_row;
    int m_col;
    int m_numMoves = 0;
    bool m_alive = true;
    int m_color;
    int m_pieceID;
    Board* m_board;
};


////////////////////////////////////////////////////////////////////////////////////////////////
// KING DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
class King : public Piece
{
public:
    King(int row, int col, int color, Board* b);
    virtual bool mpAdherent(int proposedR, int proposedC);
};


////////////////////////////////////////////////////////////////////////////////////////////////
// QUEEN DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
class Queen : public Piece
{
public:
    Queen(int row, int col, int color, Board* b);
    virtual bool mpAdherent(int proposedR, int proposedC);
};


////////////////////////////////////////////////////////////////////////////////////////////////
// ROOK DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
class Rook : public Piece
{
public:
    Rook(int row, int col, int color, Board* b);
    virtual bool mpAdherent(int proposedR, int proposedC);
};


////////////////////////////////////////////////////////////////////////////////////////////////
// BISHOP DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
class Bishop : public Piece
{
public:
    Bishop(int row, int col, int color, Board* b);
    virtual bool mpAdherent(int proposedR, int proposedC);
};


////////////////////////////////////////////////////////////////////////////////////////////////
// KNIGHT DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
class Knight : public Piece
{
public:
    Knight(int row, int col, int color, Board* b);
    virtual bool mpAdherent(int proposedR, int proposedC);
};


////////////////////////////////////////////////////////////////////////////////////////////////
// PAWN DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
class Pawn : public Piece
{
public:
    Pawn(int row, int col, int color, Board* b);
    void setPrevMoveNum(int moveNum); // The last time this piece was moved (with 0 corresponding to the first move)
    int prevMoveNum() const;
    virtual bool mpAdherent(int proposedR, int proposedC);
    
    // OVERRIDE
    virtual bool notBlocked (int proposedR, int proposedC); // Pawn can also be blocked by pieces of the opposite color
    
private:
    int m_prevMoveNum = -1;
};

#endif /* PIECE_INCLUDED */
