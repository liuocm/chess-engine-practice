//
//  Piece.cpp
//  Chess
//
//  Created by Liu Martin on 6/24/22.
//

#include "Piece.h"
#include "Board.h"
#include "globals.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////////////////////
// PIECE BASE CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
Piece::Piece(int row, int col, int color, int pieceID, Board* b)
: m_row(row), m_col(col), m_color(color), m_pieceID(pieceID), m_board(b)
{}

Piece::~Piece()
{}

bool Piece::notBlocked (int proposedR, int proposedC)
{
    if (row() == proposedR)     // Horizontal movement
    {
        for (int c = min(col(), proposedC) + 1; c < max(col(), proposedC); c++)
        {
            if (board()->pieceAtPos(proposedR, c) != nullptr)
            {
                return false;
            }
        }
    }
    else if (col() == proposedC)    // Vertical movement
    {
        for (int r = min(row(), proposedR) + 1; r < max(row(), proposedR); r++)
        {
            if (board()->pieceAtPos(r, proposedC) != nullptr)
            {
                return false;
            }
        }
    }
    else if (abs(proposedR - row()) == abs(proposedC - col()))  // Diagonal movement
    {
        int rDir = (proposedR - row() > 0) ? NORTH : SOUTH;
        int cDir = (proposedC - col() > 0) ? EAST : WEST;
       
        int r = row() + rDir;
        int c = col() + cDir;
        
        while (r != proposedR || c != proposedC)
        {
            if (board()->pieceAtPos(r, c) != nullptr)
            {
                return false;
            }
            r += rDir;
            c += cDir;
        }
    }
    return true;    // Calling this function on a Knight will always return true
}

bool Piece::noPieceOverlap (int proposedR, int proposedC)
{
    Piece* pieceAtPosition = board()->pieceAtPos(proposedR, proposedC);
    return pieceAtPosition == nullptr || pieceAtPosition->color() != color();
}

bool Piece::movePossible(int proposedR, int proposedC)
{
    return mpAdherent(proposedR, proposedC) && notBlocked(proposedR, proposedC) && noPieceOverlap(proposedR, proposedC) && board()->kingSafe(this, proposedR, proposedC);
}

// MUTATORS
void Piece::updatePos(int proposedR, int proposedC)
{
    m_row = proposedR;
    m_col = proposedC;
}

void Piece::incrementMoves()
{
    m_numMoves++;
}

void Piece::setAliveStatus(bool alive)
{
    m_alive = alive;
}

// ACCESSORS
int Piece::row() const
{
    return m_row;
}

int Piece::col() const
{
    return m_col;
}

int Piece::numMoves() const
{
    return m_numMoves;
}

bool Piece::alive() const
{
    return m_alive;
}

int Piece::color() const
{
    return m_color;
}

int Piece::pieceID() const
{
    return m_pieceID;
}

Board* Piece::board() const
{
    return m_board;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// KING DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
King::King(int row, int col, int color, Board* b)
 : Piece(row, col, color, 0 + color, b)
{}

bool King::mpAdherent(int proposedR, int proposedC)
{
    if (numMoves() == 0 && row() == proposedR && abs(proposedC - col()) == 2 && board()->canCastle(this, proposedR, proposedC)) // Castle Movement Pattern
    {
        return true;
    }
    else if ((abs(proposedR - row()) <= 1 && abs(proposedC - col()) <= 1))
    {
        return true;
    }
    else
    {
        return false;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
// QUEEN DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
Queen::Queen(int row, int col, int color, Board* b)
: Piece(row, col, color, 2 + color, b)
{}

bool Queen::mpAdherent(int proposedR, int proposedC)
{
    return row() == proposedR || col() == proposedC || abs(proposedR - row()) == abs(proposedC - col());
}


////////////////////////////////////////////////////////////////////////////////////////////////
// ROOK DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
Rook::Rook(int row, int col, int color, Board* b)
: Piece(row, col, color, 4 + color, b)
{}

bool Rook::mpAdherent(int proposedR, int proposedC)
{
    return row() == proposedR || col() == proposedC;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// BISHOP DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
Bishop::Bishop(int row, int col, int color, Board* b)
: Piece(row, col, color, 6 + color, b)
{}

bool Bishop::mpAdherent(int proposedR, int proposedC)
{
    return abs(proposedR - row()) == abs(proposedC - col());
}


////////////////////////////////////////////////////////////////////////////////////////////////
// KNIGHT DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
Knight::Knight(int row, int col, int color, Board* b)
: Piece(row, col, color, 8 + color, b)
{}

bool Knight::mpAdherent(int proposedR, int proposedC)
{
    return (abs(row() - proposedR) == 2 && abs(col() - proposedC) == 1) || (abs(col() - proposedC) == 2 && abs(row() - proposedR) == 1);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// PAWN DERIVED CLASS
////////////////////////////////////////////////////////////////////////////////////////////////
Pawn::Pawn(int row, int col, int color, Board* b)
: Piece(row, col, color, 10 + color, b)
{}

void Pawn::setPrevMoveNum(int moveNum)
{
    m_prevMoveNum = moveNum;
}

int Pawn::prevMoveNum() const
{
    return m_prevMoveNum;
}

bool Pawn::mpAdherent(int proposedR, int proposedC)
{
    int dir = color() ? SOUTH : NORTH; // BLACK evaluates to true, WHITE evaluates to false
    
    if (proposedC == col() && (proposedR == row() + dir || (numMoves() == 0 && proposedR == row() + 2 * dir))) // Moving up 1 or 2 squares
    {
        return true;
    }
    else if (proposedR == row() + dir && abs(proposedC - col()) == 1 && board()->pieceAtPos(proposedR, proposedC) != nullptr && board()->pieceAtPos(proposedR, proposedC)->color() != color()) // Capturing diagonally
    {
        return true;
    }
    else if (board()->canEnPassant(this, proposedR, proposedC)) // En Passant
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Pawn::notBlocked(int proposedR, int proposedC)
{
    for (int r = min(row(), proposedR) + 1; r < max(row(), proposedR); r++) // Return false if any pieces are in the way
    {
        if (board()->pieceAtPos(r, proposedC) != nullptr)
        {
            return false;
        }
    }
    if (proposedC == col() && board()->pieceAtPos(proposedR, proposedC) != nullptr) // If Pawn is moving straight, return false if (proposedR, proposedC) is occupied by a piece of the opposite color
    {
        return false;
    }
    else
    {
        return true;
    }
}
