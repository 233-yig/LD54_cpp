#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <string>
enum OpResult
{
    OpResult_Invalid,
    OpResult_Success,
    OpResult_Lose,
    OpResult_Win,
    OpResult_Count
};
class MineSweeper
{
    enum Analysed
    {
        Analysed_Safe,
        Analysed_Both,
        Analysed_Mine,
        Analysed_Count,
    };

    static constexpr int adj_max = 8;
    
    std::unordered_set<int> flagged;
    std::unordered_map<int, int> flipped;
    std::unordered_map<int, bool> assumptions;
    int assumption_mines;
    int assumption_safes;
    std::unordered_map<int, Analysed> solutions;
    int solution_mines;
    int solution_safes;
    int width;
    int height;
    int mines;

    int GetVar(int pos, int offset)
    {
        int x = pos % width;
        int y = pos / width;
        switch(offset)
        {
            case 0:
                if(x == 0 || y == 0) break;
                return pos - width - 1;
            case 1:
                if(y == 0) break;
                return pos - width;
            case 2:
                if(x + 1 == width || y == 0) break;
                return pos - width + 1;
            case 3:
                if(x == 0) break;
                return pos - 1;
            case 4:
                if(x + 1 == width) break;
                return pos + 1;
            case 5:
                if(x == 0 || y + 1 == height) break;
                return pos + width - 1;
            case 6:
                if(y + 1 == height) break;
                return pos + width;
            case 7:
                if(x + 1 == width || y + 1 == height) break;
                return pos + width + 1;
        }
        return -1;
    }
    bool Solve(std::unordered_map<int, int>::const_iterator current_constrain, int varidx_of_current, int sum_of_current);
    void Analyse();
    void Revert();
public:
    void Load(int w, int h, int count, const char* map)
    {
        width = w;
        height = h;
        mines = count;
        solution_mines = 0;
        solution_safes = 0;
        assumption_mines = 0;
        assumption_safes = 0;
        for(int i = 0; i < width * height; i++)
        {
            if(*map >= '0' && *map <= '8')
            {
                flipped[i] = *map - '0';
            }
            else if(*map =='f')
            {
                flagged.insert(i);
            }
            else if(*map != 'e')
            {
                i--;
            }
            map++;
        }
        Analyse();
    }
    OpResult Flip(int x, int y)
    {
        int var = x + width * y;
        if(flagged.contains(var)) return OpResult_Invalid;
        if(flipped.contains(var)) return OpResult_Invalid;
        if(!solutions.contains(var)) return OpResult_Invalid;
        if(solutions.at(var) == Analysed_Mine) return OpResult_Lose;
        if (solutions.at(var) == Analysed_Both && solution_safes > 0) return OpResult_Lose;
        assumptions.clear();
        assumption_mines = 0;
        assumption_safes = 0;
        if (solutions.at(var) != Analysed_Safe)
        {
            assumptions[var] = false;
            assumption_safes = 1;
        }
        assert(Solve(flipped.begin(), 0, 0));
        
        flipped[var] = 0;
        for (int i = 0; i < adj_max; i++)
        {
            int pos = GetVar(var, i);
            if (pos == -1)
            {
                continue;
            }
            if (flipped.contains(pos))
            {
                continue;
            }
            if (flagged.contains(pos))
            {
                flipped.at(var)++;
                continue;
            }
            if (solutions.contains(pos))
            {
                if (solutions.at(pos) == Analysed_Mine)
                {
                    flipped.at(var)++;
                }
                if (solutions.at(pos) == Analysed_Safe)
                {
                    continue;
                }

            }
            if (!assumptions.contains(pos))
            {
                assumptions[pos] = rand() % (width * height - flipped.size() - flagged.size() - solution_mines - solution_safes - assumption_mines - assumption_safes) <= mines - solution_mines - assumption_mines - flagged.size();
            }
            if (assumptions.at(pos) == true)
            {
                flipped.at(var)++;
                continue;
            }
        }
        
        Revert();
        Analyse();
        return OpResult_Success;
    }
    OpResult Flag(int x, int y)
    {
        int var = x + width * y;
        if(flagged.contains(var)) return OpResult_Invalid;
        if(flipped.contains(var)) return OpResult_Invalid;
        if (!solutions.contains(var) || solutions.at(var) != Analysed_Mine) return OpResult_Lose;
        flagged.insert(var);
        Revert();
        if(flagged.size() == mines)
        {
            return OpResult_Win;
        }
        Analyse();
        return OpResult_Success;
    }

    
    int Width() const
    {
        return width;
    }
    int Height() const
    {
        return height;
    }
    int Mines() const
    {
        return mines - flagged.size();
    }
    char GetState(int var) const
    {
        if(flipped.contains(var))
        {
            return '0' + flipped.at(var);
        }
        if(flagged.contains(var))
        {
            return 'f';
        }
        if(solutions.contains(var))
        {
            switch(solutions.at(var))
            {
            case Analysed_Safe:
                return 's';
            case Analysed_Both:
                return 'w';
            case Analysed_Mine:
                return 'm';
            }
        }
        return 'e';

    }

    std::string Debug();
};