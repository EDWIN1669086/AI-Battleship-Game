#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

#include <vector>
using namespace std;


class GameImpl
{
  public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const; //size of vector of ships
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
    
  private:
    int m_rows;
    int m_cols;
    struct ship {
        int m_length;
        char m_symbol;
        string m_name;
    };
    vector<ship> shipvect;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
    m_rows = nRows;
    m_cols = nCols;
}

int GameImpl::rows() const
{
    return m_rows;
}

int GameImpl::cols() const
{
    return m_cols;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
    //checks if the point is in bounds
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols())); //returns a random valid Point
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    ship temp; //new ship
    temp.m_length = length;
    temp.m_symbol = symbol;
    temp.m_name = name;
    shipvect.push_back(temp); //add new ship to vector of ships
    return true;
}

int GameImpl::nShips() const
{
    return int(shipvect.size()); //casted to int; returns size of ship vector
}

int GameImpl::shipLength(int shipId) const
{
    return shipvect[shipId].m_length;
}

char GameImpl::shipSymbol(int shipId) const
{
    return shipvect[shipId].m_symbol;
}

string GameImpl::shipName(int shipId) const
{
    return shipvect[shipId].m_name;
}

 
Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    if (p1->isHuman())
    {
        cout << p1->name() << " must place " << nShips() << " ships." << endl;
        p1->placeShips(b1); //initializes the board, assigning p1 to b1
    }
    else
    {
        if (p1->placeShips(b1) == false) {return nullptr;} //returns nullptr if could not place the ships
    }
    
    if (p2->isHuman())
    {
        cout << p2->name() << " must place " << nShips() << " ships." << endl;
        p2->placeShips(b2); //initializes the board, assigning p2 to b2
    }
    else
    {
        if (p2->placeShips(b2) == false) {return nullptr;} //returns nullptr if could not place the ships
    }
    
    int shipId;
    bool shipDestroyed;
    bool shotHit;
    Point attacked;
    
    while (true)
    {
        cout << p1->name() << "'s turn. Board for " << p2->name() << ":" << endl;
        b2.display(p1->isHuman()); //p1 is attacking, so they need to see p2's board
        attacked = p1->recommendAttack();
        if (b2.attack(attacked, shotHit, shipDestroyed, shipId) == true)
        {
            if (shotHit == true)
            {
                if (shipDestroyed)
                {
                    cout << p1->name() << " attacked (" << attacked.r << "," << attacked.c << ") and destroyed the " << p1->game().shipName(shipId) << ", resulting in:" << endl;
                }
                else
                {
                    cout << p1->name() << " attacked " << "(" << attacked.r << "," << attacked.c << ") and hit something, resulting in:" << endl;
                }
                
            }
            else
            {
                cout << p1->name() << " attacked (" << attacked.r << "," << attacked.c << ") and missed, resulting in:" << endl;
            }
            
            b2.display(p1->isHuman()); //only display after every valid attack
            if(b2.allShipsDestroyed() == true) //if player 1 won
            {
                cout << p1->name() << " wins!" << endl;
                return p1;
            }
            p1->recordAttackResult(attacked, true, shotHit, shipDestroyed, shipId);
        }
        else //attacked at an already attacked or out of bounds spot
        {
            cout << p1->name() << " wasted a shot at " << "(" << attacked.r << "," << attacked.c << ")." << endl;
            p1->recordAttackResult(attacked, false, shotHit, shipDestroyed, shipId);
        }
        p2->recordAttackByOpponent(attacked);
        
        if (shouldPause == true)
        {
            cout << "Press enter to continue: ";
            string s;
            getline(cin,s);
        }
        
        
        //player 2's turn
        cout << p2->name() << "'s turn. Board for " << p1->name() << ":" << endl;
        b1.display(p2->isHuman()); //p2 is attacking, so he needs to see p1's board
        attacked = p2->recommendAttack();
        if (b1.attack(attacked, shotHit, shipDestroyed, shipId) == true)
        {
            if (shotHit == true)
            {
                if (shipDestroyed)
                {
                    cout << p2->name() << " attacked (" << attacked.r << "," << attacked.c << ") and destroyed the " << p2->game().shipName(shipId) << ", resulting in:" << endl;
                }
                else
                {
                    cout << p2->name() << " attacked " << "(" << attacked.r << "," << attacked.c << ") and hit something, resulting in:" << endl;
                }
                
            }
            else
            {
                cout << p2->name() << " attacked (" << attacked.r << "," << attacked.c << ") and missed, resulting in:" << endl;
            }
            
            b1.display(p2->isHuman()); //only display after every valid attack
            if(b1.allShipsDestroyed() == true) //player 1 won
            {
                cout << p2->name() << " wins!" << endl;
                return p2;
            }
            p2->recordAttackResult(attacked, true, shotHit, shipDestroyed, shipId);
        }
        else //attacked at an already attacked or out of bounds spot
        {
            cout << p2->name() << " wasted a shot at " << "(" << attacked.r << "," << attacked.c << ")." << endl;
            p2->recordAttackResult(attacked, false, shotHit, shipDestroyed, shipId);
        }
        p1->recordAttackByOpponent(attacked);
        
        if (shouldPause == true)
        {
            cout << "Press enter to continue: ";
            string s;
            getline(cin,s);
        }
   
    }
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

