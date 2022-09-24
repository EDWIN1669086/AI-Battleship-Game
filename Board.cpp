#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <vector>

using namespace std;



class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
    struct ship_des
    {
        char symbol;
        int timesHit;
        int id;
    };
    char m_board[MAXROWS][MAXCOLS];
    const Game& m_game;
    vector<ship_des> ship_desvect;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    clear(); //sets all positions to '.'
}

void BoardImpl::clear()
{
    for (int i = 0; i < m_game.rows(); i++)
    {
        for (int j = 0; j < m_game.cols(); j++)
        {
            m_board[i][j] = '.';
        }
    }
}

//blocks exactly half of the positions in the board
void BoardImpl::block()
{
    int counter = 0;
    while (counter != (m_game.rows()*m_game.cols())/2)
    {
        int rowpos = randInt(m_game.rows());
        int colpos = randInt(m_game.cols());
        if (m_board[rowpos][colpos] == '.')
        {
            m_board[rowpos][colpos] = '-';
            counter++;
        }
    }
}

 

void BoardImpl::unblock()
{
    for (int i = 0; i < m_game.rows(); i++)
    {
        for (int j = 0; j < m_game.cols(); j++)
        {
            if (m_board[i][j] == '-')
            {
                m_board[i][j] = '.';
            }
        }
    }
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId > m_game.nShips())
    {
        return false;
    }
    if (m_game.isValid(topOrLeft) == false)
    {
        return false;
    }
    for (int i = 0; i < ship_desvect.size(); i++)
    {
        if (ship_desvect[i].id == shipId) //checks if the id for the ship has already been placed
        {
            return false;
        }
    }
    
    //since ship is valid, iniitialize the members of that ship
    ship_des temp;
    temp.symbol = m_game.shipSymbol(shipId);
    temp.timesHit = 0;
    temp.id = shipId;
    
    
    if (dir == HORIZONTAL)
    {
        if (topOrLeft.c + m_game.shipLength(shipId) > m_game.cols()) //checks if the whole ship can fit horizontally on the board
        {
            return false;
        }
        for (int i = 0; i < m_game.shipLength(shipId); i++)
        {
            if (m_board[topOrLeft.r][topOrLeft.c + i] != '.') //checks if position has been taken already
            {
                return false;
            }
        }
        for (int i = 0; i < m_game.shipLength(shipId); i++)
        {
            m_board[topOrLeft.r][topOrLeft.c + i] = m_game.shipSymbol(shipId); //adds ship to board
        }
    }
    else //dir == VERTICAL
    {
        if (topOrLeft.r + m_game.shipLength(shipId) > m_game.rows()) //checks if the whole ship can fit vertically on the board
        {
            return false;
        }
        for (int i = 0; i < m_game.shipLength(shipId); i++)
        {
            if (m_board[topOrLeft.r + i][topOrLeft.c] != '.') //checks if position has been taken already
            {
                return false;
            }
        }
        for (int i = 0; i < m_game.shipLength(shipId); i++)
        {
            m_board[topOrLeft.r + i][topOrLeft.c] = m_game.shipSymbol(shipId); //adds ship to board
        }
    }
    ship_desvect.push_back(temp); //adds ship to ship vector
    return true;
}


bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if (m_game.isValid(topOrLeft) == false)
    {
        return false;
    }
    bool exists = false;
    int index = 0;
    for (int i = 0; i < ship_desvect.size(); i++)
    {
        if (ship_desvect[i].id == shipId) //checks to see if the shipId already exists in the ship vector
        {
            exists = true;
            index = i;
            
        }
    }
    if (exists == false)
    {
        return false;
    }
    
    if (dir == HORIZONTAL)
    {
        if (topOrLeft.c + m_game.shipLength(shipId) > m_game.cols()) //check if orientation of points is valid
        {
            return false;
        }
        for (int i =0; i < m_game.shipLength(shipId); i++)
        {
            if (m_board[topOrLeft.r][topOrLeft.c+i] != m_game.shipSymbol(shipId)) //checks if correct ship symbol
            {
                return false;
            }
        }
        
        for (int i =0; i < m_game.shipLength(shipId); i++)
        {
            m_board[topOrLeft.r][topOrLeft.c+i] = '.'; //erases the ship from the board
        }
        ship_desvect.erase(ship_desvect.begin() + index); // removes ship from ship vector
    }
    else //dir == VERTICAL
    {
        if (topOrLeft.r + m_game.shipLength(shipId) > m_game.rows()) //if orientation of points is valid
        {
            return false;
        }
        for (int i =0; i < m_game.shipLength(shipId); i++)
        {
            if (m_board[topOrLeft.r+i][topOrLeft.c] != m_game.shipSymbol(shipId))
            {
                return false;
            }
        }
        
        for (int i =0; i < m_game.shipLength(shipId); i++)
        {
            m_board[topOrLeft.r+i][topOrLeft.c] = '.';
        }
        ship_desvect.erase(ship_desvect.begin() + index); //removes the ship from the ship vector
    }
    
    return true;
}

 
void BoardImpl::display(bool shotsOnly) const
{
    cout << "  ";
    for (int i = 0; i < m_game.cols(); i++)
    {
        cout << i;
    }
    cout << endl;
    
    for (int j = 0; j < m_game.rows(); j++)
    {
        cout << j << " ";
        for (int k = 0; k < m_game.cols(); k++)
        {
            if (shotsOnly == false)
            {
                cout << m_board[j][k]; //print the board raw
            }
            else
            {
                if (m_board[j][k] == 'X' || m_board[j][k] == 'o')
                {
                    cout << m_board[j][k];
                }
                else {
                    cout << '.';
                }
            }
        }
        cout << endl;
     }
}


bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    shotHit = false;
    shipDestroyed = false;
    if (m_game.isValid(p) == false || m_board[p.r][p.c] == 'X' || m_board[p.r][p.c] == 'o') //checks if invalid or already attacked point
    {
        return false;
    }
    
    else if (m_board[p.r][p.c] != '.') // if it is an undamaged part of a ship like 'A'
    {
        int id_index = 100;
        shotHit = true;
        
        for (int i = 0; i < ship_desvect.size(); i++)
        {
            if (m_game.shipSymbol(ship_desvect[i].id) == m_board[p.r][p.c]) //checks if correct ship symbol at Point p on board
            {
                id_index = i;
                break;
            }
        }
        
        m_board[p.r][p.c] = 'X'; //set the ship point to damaged
        ship_desvect[id_index].timesHit += 1;
        
        if (ship_desvect[id_index].timesHit == m_game.shipLength(id_index)) //if entire ship is destroyed
        {
            shipDestroyed = true;
            shipId = ship_desvect[id_index].id; //sets shipId when ship is destroyed
        }
    }
    else{ //if the valid shot missed
        m_board[p.r][p.c] = 'o';
    }
    
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
    for (int i = 0; i < ship_desvect.size(); i++)
    {
        if (ship_desvect[i].timesHit != m_game.shipLength(i)) //if one not sunk...
        {
            return false;
        }
    }
    return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
