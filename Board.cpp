//
//  Board.cpp
//  Chess
//
//  Created by Liu Martin on 6/24/22.
//

#include "Board.h"
#include "globals.h"
#include "Engine.h"
#include <string>
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////////////////////
Board::Board()
{
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            m_piecePositions[row][col] = nullptr;
        }
    }
    placePieces(WHITE);
    placePieces(BLACK);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////////////////////
Board::~Board()
{
    while (!m_white.empty())
    {
        m_white.pop_back();
    }
    while (!m_black.empty())
    {
        m_black.pop_back();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// placePieces
////////////////////////////////////////////////////////////////////////////////////////////////
void Board::placePieces(int color)
{
    vector<Piece*>* pieceSet = color ? &m_black : &m_white;
    King** kingPointer = color ? &m_blackKing : &m_whiteKing;
    int pawnRow = color ? 7 : 2;
    int backRow = color ? 8 : 1;

    for (int col = 1; col <= 8; col++) // Pawns
    {
        Piece* p = new Pawn(pawnRow, col, color, this);
        pieceSet->push_back(p);
        m_piecePositions[pawnRow - 1][col - 1] = p;
    }
    
    for (int col = 1; col <= 8; col++) // Other Pieces
    {
        Piece* p;
        switch (col)
        {
            case 1:
            case 8:
                p = new Rook(backRow, col, color, this);
                break;
            case 2:
            case 7:
                p = new Knight(backRow, col, color, this);
                break;
            case 3:
            case 6:
                p = new Bishop(backRow, col, color, this);
                break;
            case 4:
                p = new Queen(backRow, col, color, this);
                break;
            case 5:
                *kingPointer = new King(backRow, col, color, this);
                p = *kingPointer;
                break;
        }
        
        pieceSet->push_back(p);
        m_piecePositions[backRow - 1][col - 1] = p;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// attemptMove
////////////////////////////////////////////////////////////////////////////////////////////////
bool Board::attemptMove(Piece* piece, int proposedR, int proposedC)
{
    if (piece->movePossible(proposedR, proposedC))
    {
        // Special moves (Castle, En Passant)
        if (piece->pieceID() == KING_ID + piece->color() && piece->row() == proposedR && abs(piece->col() - proposedC) == 2) // Moving two spaces and movePossible == true imples the King is castling
        {
            return castle(piece, proposedR, proposedC);
        }
        if (piece->pieceID() == PAWN_ID + piece->color() && piece->col() != proposedC && pieceAtPos(proposedR, proposedC) == nullptr) // Moving diagonally to an unoccupied space implies En Passant
        {
            return enPassant(piece, proposedR, proposedC);
        }
        
        // Update board, mark a piece as CAPTURED if necessary, increment the piece's m_numMoves variable
        m_piecePositions[piece->row() - 1][piece->col() - 1] = nullptr;
        if (pieceAtPos(proposedR, proposedC) != nullptr)
        {
            pieceAtPos(proposedR, proposedC)->setAliveStatus(false);
        }
        piece->updatePos(proposedR, proposedC);
        m_piecePositions[proposedR - 1][proposedC - 1] = piece;
        piece->incrementMoves();
        
        // If the piece is a pawn: check for a pawn promotion, update the pawn's m_prevMoveNum variable
        if (piece->pieceID() == PAWN_ID + piece->color())
        {
            if(piece->row() == (piece->color() ? 1 : 8))
            {
                Eng().ppMenu(true);
            }
            static_cast<Pawn*>(piece)->setPrevMoveNum(totalMoves());
        }
        
        // Increment the board's m_totalMoves variable
        m_totalMoves++;
        return true;
    }
    return false;
}

bool Board::castle(Piece* king, int proposedR, int proposedC)
{
    // Find the direction the king is moving in (neg or pos), find the appropriate rook (based on dir)
    int dir = (proposedC - king->col()) / 2;
    int rookC = (dir == WEST ? 1 : 8);
    Piece* rook = pieceAtPos(proposedR, rookC);
    
    // Update the king's position on the board, update the king's internal position
    m_piecePositions[king->row() - 1][king->col() - 1] = nullptr;
    king->updatePos(king->row(), proposedC);
    m_piecePositions[proposedR - 1][proposedC - 1] = king;
        
    // Update the rook's position on the board, update the rook's internal position
    m_piecePositions[rook->row() - 1][rookC - 1] = nullptr;
    rook->updatePos(rook->row(), proposedC - dir);
    m_piecePositions[proposedR - 1][(proposedC - dir) - 1] = rook;
        
    // Increment m_numMoves variable for both the king and the rook, update the board's m_totalMoves
    king->incrementMoves();
    rook->incrementMoves();
    m_totalMoves++;
    return true;
}

bool Board::enPassant(Piece* pawn, int proposedR, int proposedC)
{
    // Remove the captured pawn to DEAD, remove it from the board
    int takenR = pawn->row();
    int takenC = proposedC;
    pieceAtPos(takenR, takenC)->setAliveStatus(false);
    m_piecePositions[takenR - 1][takenC - 1] = nullptr;
    
    // Update the pawn's position on the board, update the pawn's internal position
    m_piecePositions[pawn->row() - 1][pawn->col() - 1] = nullptr;
    pawn->updatePos(proposedR, proposedC);
    m_piecePositions[proposedR - 1][proposedC - 1] = pawn;
    
    // Increment the pawn's m_numMoves variable, set the pawn's m_prevMoveNum variable, increment the board's m_totalMoves variable
    pawn->incrementMoves();
    static_cast<Pawn*>(pawn)->setPrevMoveNum(totalMoves());
    m_totalMoves++;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// promotePawn
////////////////////////////////////////////////////////////////////////////////////////////////
void Board::promotePawn(Piece* pawn, int promotionID)
{
    int promotionR = pawn->row();
    int promotionC = pawn->col();
    int color = pawn->color();
    
    Piece* promotedPiece;
    switch(promotionID)
    {
        case 2:
        case 3:
            promotedPiece = new Queen(promotionR, promotionC, color, this);
            break;
        case 4:
        case 5:
            promotedPiece = new Rook(promotionR, promotionC, color, this);
            break;
        case 6:
        case 7:
            promotedPiece = new Bishop(promotionR, promotionC, color, this);
            break;
        case 8:
        case 9:
            promotedPiece = new Knight(promotionR, promotionC, color, this);
            break;
        default:
            exit(999);
    }
    
    pawn->setAliveStatus(false);
    m_piecePositions[promotionR - 1][promotionC - 1] = promotedPiece;
    (color ? m_black : m_white).push_back(promotedPiece);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// squareInCheck
////////////////////////////////////////////////////////////////////////////////////////////////
bool Board::squareInCheck(int row, int col, int attackingColor)
{
    for (int r = 1; r <= 8; r++)
    {
        for (int c = 1; c <= 8; c++)
        {
            Piece* pieceAtPosition = pieceAtPos(r, c);
            // Intentionally left out kingSafe and opted for 3/4 of the conditions in movePossible()
            if (pieceAtPosition != nullptr &&
                pieceAtPosition->color() == attackingColor &&
                pieceAtPosition->mpAdherent(row, col) &&
                pieceAtPosition->notBlocked(row, col) &&
                pieceAtPosition->noPieceOverlap(row, col))
            {
                return true;
            }
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// kingSafe
////////////////////////////////////////////////////////////////////////////////////////////////
bool Board::kingSafe(Piece* pieceToMove, int proposedR, int proposedC)
{
    bool safe;
    int initR = pieceToMove->row();
    int initC = pieceToMove->col();
    
    m_piecePositions[initR - 1][initC - 1] = nullptr;
    Piece* pieceAtPropPos = m_piecePositions[proposedR - 1][proposedC - 1];
    m_piecePositions[proposedR - 1][proposedC - 1] = pieceToMove;
    
    int kingR = getKing(pieceToMove->color())->row();
    int kingC = getKing(pieceToMove->color())->col();

    if (pieceToMove->pieceID() == KING_ID + pieceToMove->color()) // If pieceToMove is a king, return true if king isn't moving into check
    {
        kingR = proposedR;
        kingC = proposedC;
    }
    
    safe = !squareInCheck(kingR, kingC, (pieceToMove->color() ? WHITE : BLACK));
    
    m_piecePositions[initR - 1][initC - 1] = pieceToMove;
    m_piecePositions[proposedR - 1][proposedC - 1] = pieceAtPropPos;
    return safe;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// kingCheckmated
////////////////////////////////////////////////////////////////////////////////////////////////
bool Board::kingCheckmated(int color)
{
    King* king = getKing(color);
    if (!squareInCheck(king->row(), king->col(), (color ? WHITE : BLACK)))
    {
        return false;
    }
    for (int r = 1; r <= 8; r++)
    {
        for (int c = 1; c <= 8; c++)
        {
            for (int i = 0; i < pieces(color).size(); i++)
                
            if (pieces(color)[i]->alive() && pieces(color)[i]->movePossible(r, c))
            {
                return false;
            }
        }
    }
    if (color == BLACK)
    {
        cout << "WHITE WINS" << endl;
    }
    else
    {
        cout << "BLACK WINS" << endl;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// kingStalemated()
////////////////////////////////////////////////////////////////////////////////////////////////
bool Board::kingStalemated(int color)
{
    King* king = getKing(color);
    if (squareInCheck(king->row(), king->col(), (color ? WHITE : BLACK)))
    {
        return false;
    }
    for (int r = 1; r <= 8; r++)
    {
        for (int c = 1; c <= 8; c++)
        {
            for (int i = 0; i < pieces(color).size(); i++)
                
            if (pieces(color)[i]->alive() && pieces(color)[i]->movePossible(r, c))
            {
                return false;
            }
        }
    }
    cout << "IT'S A DRAW" << endl;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// canCastle
////////////////////////////////////////////////////////////////////////////////////////////////
bool Board::canCastle(King* king, int proposedR, int proposedC)
{
    int dir = (proposedC - king->col()) / 2;
    int rookC = (dir == WEST ? 1 : 8);
    
    if (king->numMoves() != 0 ||
        !king->notBlocked(king->row(), rookC) ||
        pieceAtPos(king->row(), rookC) == nullptr ||
        pieceAtPos(king->row(), rookC)->pieceID() != ROOK_ID + king->color() ||
        pieceAtPos(king->row(), rookC)->numMoves() != 0)
    {
        return false;
    }
    
    for (int c = king->col(); c != king->col() + 3 * dir; c += dir)
    {
        if (squareInCheck(king->row(), c, (king->color() ? WHITE : BLACK)))
        {
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// canEnPassant
////////////////////////////////////////////////////////////////////////////////////////////////
bool Board::canEnPassant(Pawn* pawn, int proposedR, int proposedC)
{
    if (pawn->numMoves() == 3 &&
        pawn->row() == (pawn->color() ? 4 : 5) &&
        proposedR - pawn->row() == (pawn->color() ? SOUTH : NORTH) &&
        abs(proposedC - pawn->col()) == 1 &&
        pieceAtPos(pawn->row(), proposedC) != nullptr &&
        pieceAtPos(pawn->row(), proposedC)->pieceID() == B_PAWN_ID - pawn->color() &&
        pieceAtPos(pawn->row(), proposedC)->numMoves() == 1 &&
        static_cast<Pawn*>(pieceAtPos(pawn->row(), proposedC))->prevMoveNum() == totalMoves() - 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// ACCESSORS
////////////////////////////////////////////////////////////////////////////////////////////////
int Board::totalMoves()
{
    return m_totalMoves;
}

std::vector<Piece*>& Board::pieces(int color)
{
    return (color ? m_black : m_white);
}

Piece*& Board::pieceAtPos(int r, int c)
{
    return m_piecePositions[r - 1][c - 1];
}

King*& Board::getKing(int color)
{
   return (color ? m_blackKing : m_whiteKing);
}
