//----------------------------------------------------------------------------//
//               █      █                                                     //
//               ████████                                                     //
//             ██        ██                                                   //
//            ███  █  █  ███        GameCore.cpp                              //
//            █ █        █ █        CorePuzzle16                              //
//             ████████████                                                   //
//           █              █       Copyright (c) 2016                        //
//          █     █    █     █      AmazingCow - www.AmazingCow.com           //
//          █     █    █     █                                                //
//           █              █       N2OMatt - n2omatt@amazingcow.com          //
//             ████████████         www.amazingcow.com/n2omatt                //
//                                                                            //
//                  This software is licensed as GPLv3                        //
//                 CHECK THE COPYING FILE TO MORE DETAILS                     //
//                                                                            //
//    Permission is granted to anyone to use this software for any purpose,   //
//   including commercial applications, and to alter it and redistribute it   //
//               freely, subject to the following restrictions:               //
//                                                                            //
//     0. You **CANNOT** change the type of the license.                      //
//     1. The origin of this software must not be misrepresented;             //
//        you must not claim that you wrote the original software.            //
//     2. If you use this software in a product, an acknowledgment in the     //
//        product IS HIGHLY APPRECIATED, both in source and binary forms.     //
//        (See opensource.AmazingCow.com/acknowledgment.html for details).    //
//        If you will not acknowledge, just send us a email. We'll be         //
//        *VERY* happy to see our work being used by other people. :)         //
//        The email is: acknowledgment_opensource@AmazingCow.com              //
//     3. Altered source versions must be plainly marked as such,             //
//        and must not be misrepresented as being the original software.      //
//     4. This notice may not be removed or altered from any source           //
//        distribution.                                                       //
//     5. Most important, you must have fun. ;)                               //
//                                                                            //
//      Visit opensource.amazingcow.com for more open-source projects.        //
//                                                                            //
//                                  Enjoy :)                                  //
//----------------------------------------------------------------------------//

//Header
#include "../include/GameCore.h"
//std
#include <algorithm>
#include <sstream>

//Usings
USING_NS_COREPUZZLE16;

// Constants / Enums / Typedefs //
const int GameCore::kUnlimitedMoves = -1;

// CTOR/DTOR //
GameCore::GameCore(int width, int height, int maxMoves, int seed) :
    //m_board - Init in initBoard().
    m_status       (CoreGame::Status::Continue),
    m_movesCount   (0),
    m_maxMovesCount(maxMoves),
    m_random       (seed)
{
    initBoard(width, height);
}


// Public Methods //
void GameCore::moveRow(int index, HorizontalDir dir)
{
    if(m_status != CoreGame::Status::Continue)
        return;

    //Assume Left first.
    CoreCoord::Coord start(index, 0);
    CoreCoord::Coord end  (index, getWidth() - 1);
    int incr = 1;

    //Adjust for Right move.
    if(dir == HorizontalDir::Right)
        std::swap(start, end);

    moveHelper(start, end);
}

void GameCore::moveCol(int index, VerticalDir dir)
{
    if(m_status != CoreGame::Status::Continue)
        return;

    //Assume Up first.
    CoreCoord::Coord start(0, index);
    CoreCoord::Coord end  (getHeight() - 1, index);

    //Adjust for Down move.
    if(dir == VerticalDir::Down)
        std::swap(start, end);

    moveHelper(start, end);
}


const GameCore::Board& GameCore::getBoard() const
{
    return m_board;
}

int GameCore::getValueAt(const CoreCoord::Coord &coord) const
{
    return m_board[coord.y][coord.x];
}


CoreGame::Status GameCore::getStatus() const
{
    return m_status;
}


int GameCore::getMovesCount() const
{
    return m_movesCount;
}

int GameCore::getMaxMovesCount() const
{
    return m_maxMovesCount;
}

int GameCore::getRemainingMovesCount() const
{
    if(m_maxMovesCount == GameCore::kUnlimitedMoves)
        return GameCore::kUnlimitedMoves;

    return m_maxMovesCount - m_movesCount;
}


int GameCore::getWidth() const
{
    return m_board[0].size();
}

int GameCore::getHeight() const
{
    return m_board.size();
}


int GameCore::getSeed() const
{
    return m_random.getSeed();
}

bool GameCore::isUsingRandomSeed() const
{
    return m_random.isUsingRandomSeed();
}


std::string GameCore::ascii() const
{
    std::stringstream ss;

    for(const auto &line : m_board)
    {
        for(const auto &value : line)
            ss << value << " ";
        ss << std::endl;
    };

    return ss.str();
}

// Private Methods //
void GameCore::initBoard(int width, int height)
{
    //Init a temp vector with all values.
    std::vector<int> tmp(width * height);
    std::iota(std::begin(tmp), std::end(tmp), 0);

    //Shuffle the temp vector.
    std::shuffle(std::begin(tmp), std::end(tmp),
                 m_random.getNumberGenerator());

    //Add the values into the board.
    m_board.reserve(height);
    for(int i = 0; i < height; ++i)
    {
        m_board.push_back(std::vector<int>(width));
        std::copy_n(std::begin(tmp) + i * width,
                    width,
                    std::begin(m_board[i]));
    }
}

void GameCore::checkStatus()
{
    //Check if all values are sorted.
    int prevValue = -1;
    int sorted    = true;

    for(const auto &line : m_board)
    {
        for(const auto &value : line)
        {
            if(prevValue > value)
            {
                sorted = false;
                break;
            }
            prevValue = value;
        }
    }

    //Player sort all values - Game Won
    if(sorted)
    {
        m_status = CoreGame::Status::Victory;
        return;
    }

    //Values aren't sorted, check if player
    //reach the max moves allowed.
    if(getRemainingMovesCount() == 0)
    {
        m_status = CoreGame::Status::Defeat;
    }
}

void GameCore::putValueAt(const CoreCoord::Coord &coord, int value)
{
    m_board[coord.y][coord.x] = value;
}

void GameCore::moveHelper(const CoreCoord::Coord start,
                          const CoreCoord::Coord end)
{

    CoreCoord::Coord coordToPut = end;
    int valueToPut = getValueAt(start);
    CoreCoord::Coord incr;

    if(start.x != end.x)
        incr.x = (start.x > end.x) ? -1 : +1;
    else if(start.y != end.y)
        incr.y = (start.y > end.y) ? -1 : +1;

    //Swap the middle values.
    for(CoreCoord::Coord curr = start; curr != end ; curr += incr)
        putValueAt(curr, getValueAt(curr + incr));

    //Swap the edge values.
    putValueAt(coordToPut, valueToPut);

    ++m_movesCount;
    checkStatus();
}
