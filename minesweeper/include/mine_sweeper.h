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
    enum State
    {
        State_Unevaluated,
        State_Flagged,
        State_Flipped,
        State_Evaluated_Safe,
        State_Evaluated_Uncertain,
        State_Evaluated_Mine,
        State_Lose
    };

    static constexpr int adj_max = 8;

    int width = 0;
    int height = 0;
    int mines = 0;
    int max_flipped = 0;
    
    std::vector<State> map;
    int flagged_count = 0;
    int evaluated_mines = 0;
    int evaluated_safes = 0;
    int evaluated_uncertains = 0;
    std::unordered_map<int, bool> assumptions;
    int assumption_safes = 0;
    int assumption_mines = 0;
    std::unordered_map<int, int> constrains;

    bool Solve(std::unordered_map<int, int>::const_iterator current_constrain, int varidx_of_current, int sum_of_current);
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
    int Success(int var);
    void Fail(int var, bool isMine);
public:
    void Analyse();
    bool Load(int w, int h, int m, int f, const char* s)
    {
        width = w;
        height = h;
        mines = m;
        max_flipped = f;

        map.resize(width * height);
        flagged_count = 0;
        evaluated_mines = 0;
        evaluated_safes = 0;
        evaluated_uncertains = 0;
        assumptions.clear();
        assumption_mines = 0;
        assumption_safes = 0;
        constrains.clear();

        for(int i = 0; i < width * height; i++, s++)
        {
            if(*s >= '0' && *s <= '8')
            {
                map[i] = State_Flipped;
                constrains[i] = *s - '0';
                continue;
            }
            switch(*s)
            {
            case 'f':
                map[i] = State_Flagged;
                flagged_count++;
                continue;
            case 'e':
                map[i] = State_Unevaluated;
                continue;
            default:
                break;
            }
            i--;
        }
        if(!constrains.empty() && !Solve(constrains.begin(), 0, 0))
        {
            return false;
        }
        Analyse();
        return true;
    }
    
    
    OpResult Flip(int var)
    {
        if(var >= map.size())
        {
            return OpResult_Invalid;
        }
        switch(map[var])
        {
        case State_Unevaluated:
            assumptions.clear();
            assumptions[var] = 0;
            assumption_mines = 1;
            assumption_safes = 0;
            if(!Solve(constrains.begin(), 0, 0))
            {
                break;
            }
        case State_Evaluated_Uncertain:
            if (evaluated_safes > 0)
            {
                Fail(var, true);
                return OpResult_Lose;
            }
        case State_Evaluated_Safe:
            break;
        case State_Flagged:
        case State_Flipped:
            return OpResult_Invalid;
        case State_Evaluated_Mine:
            Fail(var, true);
            return OpResult_Lose;
        }
        constrains[var] = Success(var);
        map[var] = State_Flipped;
        if(constrains.size() <= max_flipped)
        {
            return OpResult_Success;
        }
        int tries = constrains.size() - 1;
        for(auto it = constrains.begin(); it != constrains.end(); ++it)
        {
            if(it->first == var)
            {
                continue;
            }
            if(rand() % tries-- == 0)
            {
                map[it->first] = State_Unevaluated;
                constrains.erase(it);
                return OpResult_Success;
            }
        }
        //不可能走到这
        assert(false);
        return OpResult_Invalid;
    }
    OpResult Flag(int var)
    {
        if(var >= map.size())
        {
            return OpResult_Invalid;
        }
        switch(map[var])
        {
        case State_Flagged:
        case State_Flipped:
            return OpResult_Invalid;
        case State_Unevaluated:
            assumptions.clear();
            assumptions[var] = 0;
            assumption_mines = 0;
            assumption_safes = 1;
            if(!Solve(constrains.begin(), 0, 0))
            {
                map[var] = State_Flagged;
                flagged_count++;
                break;
            }
        case State_Evaluated_Uncertain:
        case State_Evaluated_Safe:
            Fail(var, false);
            return OpResult_Lose;
        case State_Evaluated_Mine:
            map[var] = State_Flagged;
            flagged_count++;
            break;
        }
        if(flagged_count == mines)
        {
            return OpResult_Win;
        }
        return OpResult_Success;
    }
    OpResult Revert(int var)
    {
        if(var >= map.size())
        {
            return OpResult_Invalid;
        }
        switch(map[var])
        {
        case State_Unevaluated:
        case State_Evaluated_Uncertain:
        case State_Evaluated_Safe:
        case State_Evaluated_Mine:
            return OpResult_Invalid;
        case State_Flagged:
            map[var] = State_Unevaluated;
            flagged_count--;
            break;
        case State_Flipped:
            map[var] = State_Unevaluated;
            constrains.erase(var);
            break;
        }
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
        return mines - flagged_count;
    }
    int FlagCount() const
    {
        return flagged_count;
    }
    int FlipCount() const
    {
        return constrains.size();
    }
    char GetState(int var) const
    {
        switch(map[var])
        {
        case State_Unevaluated:    
            return 'e';
        case State_Flagged:
            return 'f';
        case State_Flipped:
            return '0' + constrains.at(var);
        case State_Evaluated_Safe:
            return 's';
        case State_Evaluated_Uncertain:
            return 'w';
        case State_Evaluated_Mine:
            return 'm';
        case State_Lose:
            return 'b';
        }
        assert(false);
        return '\0';
    }
};