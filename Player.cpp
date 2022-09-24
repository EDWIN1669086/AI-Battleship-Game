#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>

#include <vector>
using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL)) //placeShip(point, id, direction)
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class HumanPlayer: public Player
{
    public:
    HumanPlayer(string nm, const Game& g);
    virtual bool isHuman() const;
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
};

HumanPlayer::HumanPlayer(string nm, const Game& g):Player(nm,g) {}
bool HumanPlayer::isHuman() const {return true;}

bool HumanPlayer::placeShips(Board& b)
{
    Direction d;
    string direction;
    int row;
    int col;
    for (int i =0; i < game().nShips(); i++)
    {
        b.display(false);
        
        while (true)
        {
            cout << "Enter h or v for direction of " << game().shipName(i) << " (length " << game().shipLength(i) << "): ";
            getline(cin,direction);
            if (direction.at(0) != 'h' && direction.at(0) != 'v')
            {
                cout << "Direction must be h or v." << endl;
            }
            else
            {
                break;
            }
        }
        
        if (direction.at(0) == 'h')
        {
            d = HORIZONTAL;
        }
        else
        {
            d = VERTICAL;
        }
        
        //now that we have the proper direction, h or v
        while (true)
        {
            if (d == HORIZONTAL)
            {
                cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
            }
            else {
                cout << "Enter row and column of topmost cell (e.g., 3 5): ";
            }
            
            
            if (getLineWithTwoIntegers(row,col) == true)
            {
                if (b.placeShip(Point(row,col), i, d) == true)
                {
                    break; //break when placeShip and row/col are valid
                }
                else
                {
                    cout << "The ship cannot be placed there." << endl;
                }
            }
            else
            {
                cout << "You must enter two integers." << endl;
            }
        }
    }
    return true;
}

Point HumanPlayer::recommendAttack()
{
    int row;
    int col;
    
    while (true) //loops until two valid ints are inputted
    {
        cout << "Enter the row and column to attack (e.g., 3 5): ";
        if (getLineWithTwoIntegers(row,col) == true)
        {
            Point p(row,col);
            return p;
        }
        else
        {
            cout << "You must enter two integers." << endl;
        }
    }
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    // HumanPlayer completely ignores the result of any attack
}

void HumanPlayer::recordAttackByOpponent(Point p)
{
    //HumanPlayer completely ignores what the opponent does
}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player
{
  public:
    MediocrePlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    
    bool recursive(Board& b, int shipId);
    
    bool newPoint(Point p);
  private:
    vector<Point> prevMoves; //vector that stores all the previous Points of the player
    int playerState;
    int crossPoints;
    Point center;
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g): Player(nm,g)
{
    playerState = 1;
    crossPoints = 0;
}

bool MediocrePlayer::placeShips(Board& b)
{
    //place the ships...
    
    for (int i = 0; i < 50; i++) //tries to place the ships 50 times if necessary
    {
        b.block();
        if (recursive(b, 0) == true){
            b.unblock();
            return true;
        }
        b.unblock();
    }

    return false;
}
 
 
bool MediocrePlayer::recursive(Board& b, int shipId)
{
    if (shipId == game().nShips()) // if the shipId goes out of bounds.. all ships are placed
    {
        return true;
    }
    
    for (int r = 0; r < game().rows(); r++)
    {
        for (int c = 0; c < game().cols(); c++)
        {
            if (b.placeShip(Point(r,c), shipId, HORIZONTAL) == true) //tries to place the ships in the horizontal dir first
            {
                if (recursive(b, shipId+1) == true) //if function continues to work placing the remaining ships...
                {
                    return true;
                }
                else
                {
                    b.unplaceShip(Point(r,c), shipId, HORIZONTAL);
                }
            }
            if (b.placeShip(Point(r,c), shipId, VERTICAL) == true){ //tries to place the ships in the vertical dir next
                if (recursive(b, shipId+1) == true) //if function continues to work placing the remaining ships...
                {
                    return true;
                }
                else {
                    b.unplaceShip(Point(r,c), shipId, VERTICAL);
                }
            }
        }
    }
    return false;
}

Point MediocrePlayer::recommendAttack()
{
    if (playerState == 1) //has not hit a new ship yet (or just destroyed one).. so randomly attack
    {
        Point p;
        while (true)
        {
            p.r = randInt(game().rows());
            p.c = randInt(game().cols());
            if (newPoint(p) == true)
            {
                break;
            }
        }
        //exit while loop, meaning found a point that has not been previously hit
        prevMoves.push_back(p);
        return p;
    }
    
    else //if playerState == 2
    {
        int row = 0;
        int col = 0;
        
        while (true) //attacks in a cross-like pattern with a new, valid point
        {
            int direction = randInt(2); // 0 means horizontal, 1 means vertical
            int displacement = randInt(9)-4;
            
            if (direction == 0) //horizontal
            {
                row = center.r + displacement;
                col = center.c;
            }
            else //vertical
            {
                row = center.r;
                col = center.c + displacement;
            }
            
            if (game().isValid(Point(row,col)) && newPoint(Point(row,col)))
            {
                crossPoints--; //the # of available crossPoints decreases by 1
                break;
            }
        }
        prevMoves.push_back(Point(row,col));
        return Point(row,col);
    }
}

bool MediocrePlayer::newPoint(Point p) //checks to see if the Point p is not one of the previous points in the vector
{
    for (int i = 0; i < prevMoves.size(); i++)
    {
        if (p.r == prevMoves[i].r && p.c == prevMoves[i].c){
            return false;
        }
    }
    return true;
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    if (shotHit == true)
    {
        if (shipDestroyed == true)
        {
            playerState = 1;
            return;
        }
        
        if (playerState == 1)
        {
            center.r = p.r;
            center.c = p.c;
            
            crossPoints = 0;
            for (int i = 0; i < 9; i++) //initializes the # of crossPoints
            {
                if (game().isValid(Point(center.r, center.c-4+i)) && newPoint(Point(center.r,center.c-4+i))) //horizontal
                {
                    crossPoints++;
                }
                if (game().isValid(Point(center.r-4+i, center.c)) && newPoint(Point(center.r-4+i,center.c))) //vertical
                {
                    crossPoints++;
                }
            }
            playerState = 2;
        }
        //checks to see if all the crossPoints have been hit already
        if (crossPoints == 0)
        {
            playerState = 1;
        }
    }
}

void MediocrePlayer::recordAttackByOpponent(Point p)
{
    //this function does nothing for the Mediocre class
}

//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer: public Player
{
public:
  GoodPlayer(string nm, const Game& g);
  virtual bool placeShips(Board& b);
  virtual Point recommendAttack();
  virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                              bool shipDestroyed, int shipId);
  virtual void recordAttackByOpponent(Point p);
  
  bool recursive(Board& b, int shipId);
    
private:
    int playerState;
    int numMoves;
    Direction dir; //direction of the located ship
    Point end1;
    Point end2;
    bool end1Reached;
    bool end2Reached;
    bool falseDestruction;
    int m_board[MAXROWS][MAXCOLS]; //board for recording misses/hits/sunken ships
};

GoodPlayer::GoodPlayer(string nm, const Game& g): Player(nm,g)
{
    playerState = 1;
    numMoves = 0;
    dir = HORIZONTAL;
    end2Reached = false;
    falseDestruction = false;
    
    //initialize the board to record attacks to zeroes
    for (int r = 0; r < game().rows(); r++)
    {
        for (int c = 0; c < game().cols(); c++)
        {
            m_board[r][c] = 0;
        }
    }
}

bool GoodPlayer::placeShips(Board& b)
{
    //place the ships...
    for (int i = 0; i < 50; i++)
    {
        b.block();
        if (recursive(b, 0) == true){
            b.unblock();
            return true;
        }
        b.unblock();
    }

    return false;
}

bool GoodPlayer::recursive(Board& b, int shipId)
{
    if (shipId == game().nShips()) // if the shipId goes out of bounds.. all ships are placed
    {
        return true;
    }
    
    for (int r = 0; r < game().rows(); r++)
    {
        for (int c = 0; c < game().cols(); c++)
        {
            if (b.placeShip(Point(r,c), shipId, HORIZONTAL) == true)
            {
                if (recursive(b, shipId+1) == true)
                {
                    return true;
                }
                else
                {
                    b.unplaceShip(Point(r,c), shipId, HORIZONTAL);
                }
            }
            if (b.placeShip(Point(r,c), shipId, VERTICAL) == true){
                if (recursive(b, shipId+1) == true)
                {
                    return true;
                }
                else {
                    b.unplaceShip(Point(r,c), shipId, VERTICAL);
                }
            }
        }
    }
    return false;
}


Point GoodPlayer::recommendAttack()
{
    int row = game().rows()/2;
    int col = game().cols()/2;
    
    if (playerState == 1) //no ship hit yet; scanning...
    {
        int numLoops = 0;
        numMoves++;
        if (numMoves < 13) // if true...attack the first half of the board
        {
            while (true)
            {
                numLoops++;
                row = randInt(game().rows()/2);
                col = randInt(game().cols());
                if (numLoops == 50) //in case mostly all the points in top half have been attacked
                {
                   numMoves = 14;
                   break;
                }
                if (m_board[row][col] == 0) //if found a new, valid point...
                {
                    break;
                }
            }
            
            
        }
        else //start attacking the whole board
        {
            while (true)
            {
                row = randInt(game().rows());
                col = randInt(game().cols());
                if (m_board[row][col] == 0) //if found a new, valid point...
                {
                    break;
                }
            }
        }
        return Point(row,col);
    }
    else if (playerState == 3) //if direction of ship is known...
    {
        if (dir == HORIZONTAL)
        {
            if (end1Reached == false && game().isValid(Point(end1.r, end1.c-1)) && m_board[end1.r][end1.c -1] == 0) //horizontal left attack
            {
                return Point(end1.r, end1.c-1);
            }
            else
            {
                end1Reached = true;
            }
            if (end2Reached == false && game().isValid(Point(end2.r, end2.c + 1 )) && m_board[end2.r][end2.c + 1] == 0) //horizontal right attack
            {
                return Point(end2.r, end2.c+1);
            }
            else
            {
                end2Reached = true;
            }
        }
        else if (dir == VERTICAL)
        {
            if (end1Reached == false && game().isValid(Point(end1.r-1, end1.c)) && m_board[end1.r-1][end1.c] == 0) //vertically up attack
            {
                return Point(end1.r-1, end1.c);
            }
            else
            {
                end1Reached = true;
            }
            if (end2Reached == false && game().isValid(Point(end2.r+1, end2.c)) && m_board[end2.r+1][end2.c] == 0) //vertically down attack
            {
                return Point(end2.r+1, end2.c);
            }
            else
            {
                end2Reached = true;
            }
        }
        playerState = 2;
        falseDestruction = true;
    }
    
    if(playerState == 2) //attacks in a mini-cross->bigger-cross based on found endpoint(s) to find the direction
    {
        if (falseDestruction == true) //if in "odd detector" condition
        {
            for (int r = 0; r < game().rows(); r++)
            {
                for (int c = 0; c < game().cols(); c++)
                {
                    if (m_board[r][c] == 2) //if ship is hit but not sunk
                    {
                        end1.r = r;
                        end1.c = c;
                        
                        end2.r = r;
                        end2.c = c;
                    }
                }
            }
        }
        for (int i = 1; i < 5; i++) //basically the cross method from mediocre player but gradual for max effect
        {
            if (game().isValid(Point(end1.r - i, end1.c)) && m_board[end1.r - i][end1.c] == 0) //up
            {
                return Point(end1.r-i, end1.c);
            }
            else if (game().isValid(Point(end1.r, end1.c - i)) && m_board[end1.r][end1.c - i] == 0) //left
            {
                return Point(end1.r, end1.c-i);
            }
            else if (game().isValid(Point(end2.r + i, end1.c)) && m_board[end2.r + i][end2.c] == 0) //down
            {
                return Point(end2.r+i, end2.c);
            }
            else if (game().isValid(Point(end2.r, end2.c + i)) && m_board[end2.r][end2.c + i] == 0) //right
            {
                return Point(end2.r, end2.c+i);
            }
        }
        falseDestruction = true;
        return Point(randInt(game().rows()),randInt(game().cols()));
    }
    return Point(row,col);
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    if (validShot == false) {return;}
    
    if (shotHit == true) //if Point p hit a ship
    {
        if (shipDestroyed == true) //the ship has been hit and sunk
        {
            if (end1.c == p.c)
            {
                dir = VERTICAL;
            }
            else
            {
                dir = HORIZONTAL;
            }
            
            if (dir == VERTICAL)
            {
                if (p.r < end1.r) //if the last point is one above end1.r
                {
                    for (int i = 0; i < game().shipLength(shipId); i++)
                    {
                        m_board[p.r + i][p.c] = 3;
                    }
                }
                else
                {
                    //mark the ship as sunk on the 2D board array
                    for (int i = 0; i < game().shipLength(shipId); i++)
                    {
                        m_board[p.r - i][p.c] = 3;
                    }
                }
            }
            else if (dir == HORIZONTAL)
            {
                if (p.c < end1.c) //if the last point is one to the left of end1.c
                {
                    for (int i = 0; i < game().shipLength(shipId); i++)
                    {
                        m_board[p.r][p.c+i] = 3;
                    }
                }
                else
                {
                    //mark the ship as sunk on the 2D board array
                    for (int i = 0; i < game().shipLength(shipId); i++)
                    {
                        m_board[p.r][p.c-i] = 3;
                    }
                }
            }
            
            if (falseDestruction == true) //if still in "odd detector" condition
            {
                bool notAllDestroyed = true;
                for (int r = 0; r < game().rows(); r++)
                {
                    for (int c = 0; c < game().cols(); c++)
                    {
                        if (m_board[r][c] == 2)
                        {
                            notAllDestroyed = false;
                        }
                    }
                }
                
                if (notAllDestroyed == true) //if passes condition..then in normal conditions
                {
                    falseDestruction = false;
                }
            }
            
            //reset the endpoints for next ship
            end1Reached = false;
            end2Reached = false;
            
            if (falseDestruction == true) //if still not properly sunk, return to state 2
            {
                playerState = 2;
            }
            else //if normal condition sunk, then return to random point hitting in playerState 1
            {
                playerState = 1;
            }
            return;
        }
        else //the ship has been hit but not destroyed...  when hit something, check the bounds...
        {
            m_board[p.r][p.c] = 2;
            if (playerState == 1) // if it was randomly searching and got first hit...
            {
                playerState = 2;
                end1.r  = p.r;
                end1.c = p.c;
                
                end2.r = p.r;
                end2.c = p.c;
            }
            else if (playerState == 2) //if it was finding the right direction...
            {
                if (p.r < end1.r) // if p.r is below the first hit -> then vertical dir
                {
                    end1.r = p.r;
                    dir = VERTICAL;
                }
                else if (p.r > end1.r) //if p.r is above the first hit --> then vertical dir
                {
                    end2.r = p.r;
                    dir = VERTICAL;
                }
                else if (p.c < end1.c) //if p.c is left of the first hit --> then horizontal dir
                {
                    end1.c = p.c;
                    dir = HORIZONTAL;
                }
                else                   //if p.c is left of the first hit --> then horizontal dir
                {
                    end2.c = p.c;
                    dir = HORIZONTAL;
                }
                playerState = 3;
                return;
            }
            
            if (playerState == 3)
            {
                if (dir == HORIZONTAL) //end1 to end2 spans from left to right
                {
                    if (end1.c == 0 || m_board[end1.r][end1.c-1] == 1)
                    {
                        end1Reached = true;
                    }
                    if (end2.c == game().cols()-1 || m_board[end2.r][end2.c+1] == 1)
                    {
                        end2Reached = true;
                    }
                }
                if (dir == VERTICAL) //end1 to end2 spans from up to down
                {
                    if (end1.r == 0 || m_board[end1.r-1][end1.c] == 1)
                    {
                        end1Reached = true;
                    }
                    if (end2.r == game().rows()-1 || m_board[end2.r+1][end2.c] == 1)
                    {
                        end2Reached = true;
                    }
                }
                
                if (end1Reached == false) //update first endpoint if not found
                {
                    end1.r = p.r;
                    end1.c = p.c;
                }
                else //update second endpoint if not found
                {
                    end2.r = p.r;
                    end2.c = p.c;
                }
                if (end1Reached == true && end2Reached == true) //if both endpoints have been found
                {
                    falseDestruction = true;
                    playerState = 2;
                    return;
                }
            }
            
        }
    }
    else //the player missed
    {
        m_board[p.r][p.c] = 1;
    }
}

void GoodPlayer::recordAttackByOpponent(Point p){}



//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}
