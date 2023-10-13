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
    enum Assumptions{
        Assumptions_Null,
        Assumptions_Safe,
        Assumptions_Mine
    };
    struct Assumption
    {
        std::vector<Assumptions> data;
        int safe_count;
        int mine_count;
        Assumption& With(int pos, bool isMine)
        {
            data.at(pos) = isMine ? Assumptions_Mine : Assumptions_Safe;
            (isMine ? mine_count : safe_count)++;
            return *this;
        }
        Assumption& Without(int pos)
        {
            switch (data.at(pos))
            {
            default:
                break;
            case Assumptions_Mine:
                mine_count--;
                break;
            case Assumptions_Safe:
                safe_count--;
                break;
            }
            data.at(pos) = Assumptions_Null;
            return *this;
        }
    };
    static constexpr int adj_max = 8;

    int width = 0;
    int height = 0;
    int mines = 0;
    int max_flipped = 0;
    
    std::vector<State> map;
    std::unordered_map<int, int> constrains;
    int flagged_count = 0;
    int evaluated_mines = 0;
    int evaluated_safes = 0;
    int evaluated_uncertains = 0;
    Assumption MakeAssumption()
    {
        Assumption result;
        result.data.resize(width * height);
        result.safe_count = 0;
        result.mine_count = 0;
        return result;
    }

    int GetAdjPos(int pos, int adj)
    {
        int x = pos % width;
        int y = pos / width;
        switch(adj)
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
    
    bool Possible(Assumption& assumption, std::unordered_map<int, int>::const_iterator current_constrain);

    int Success(int var);
    void Fail(int var, bool isMine);
public:
    void Analyse(bool incremental);
    void Load(int w, int h, int m, int f, const char* s)
    {
        width = w;
        height = h;
        mines = m;
        max_flipped = f;

        map.resize(width * height);
        constrains.clear();
        flagged_count = 0;
        evaluated_mines = 0;
        evaluated_safes = 0;
        evaluated_uncertains = 0;

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

        Analyse(false);
    }
    
    
    OpResult Flip(int pos)
    {
        if(pos >= map.size())
        {
            return OpResult_Invalid;
        }
        switch(map.at(pos))
        {
        case State_Unevaluated:
            if(!Possible(MakeAssumption().With(pos, true), constrains.begin()))
            {
                break;
            }
        case State_Evaluated_Uncertain:
            if (evaluated_safes > 0)
            {
                Fail(pos, true);
                return OpResult_Lose;
            }
            break;
        case State_Evaluated_Safe:
            evaluated_safes--;
            break;
        case State_Flagged:
        case State_Flipped:
            return OpResult_Invalid;
        case State_Evaluated_Mine:
            Fail(pos, true);
            return OpResult_Lose;
        }
        constrains[pos] = Success(pos);
        map.at(pos) = State_Flipped;
        if(constrains.size() <= max_flipped)
        {
            Analyse(true);
            return OpResult_Success;
        }
        int tries = constrains.size() - 1;
        for(auto it = constrains.begin(); it != constrains.end(); ++it)
        {
            if(it->first == pos)
            {
                continue;
            }
            if(rand() % tries-- == 0)
            {
                map[it->first] = State_Unevaluated;
                constrains.erase(it);
                Analyse(false);
                return OpResult_Success;
            }
        }
        //不可能走到这
        assert(false);
        return OpResult_Invalid;
    }
    OpResult Flag(int pos)
    {
        if(pos >= map.size())
        {
            return OpResult_Invalid;
        }
        switch(map.at(pos))
        {
        case State_Flagged:
        case State_Flipped:
            return OpResult_Invalid;
        case State_Unevaluated:
            if(!Possible(MakeAssumption().With(pos, false), constrains.begin()))
            {
                map[pos] = State_Flagged;
                flagged_count++;
                break;
            }
        case State_Evaluated_Uncertain:
        case State_Evaluated_Safe:
            Fail(pos, false);
            return OpResult_Lose;
        case State_Evaluated_Mine:
            map.at(pos) = State_Flagged;
            evaluated_mines--;
            flagged_count++;
            break;
        }

        if(flagged_count == mines)
        {
            return OpResult_Win;
        }
        return OpResult_Success;
    }
    OpResult Revert(int pos)
    {
        if(pos >= map.size())
        {
            return OpResult_Invalid;
        }
        switch(map.at(pos))
        {
        case State_Unevaluated:
        case State_Evaluated_Uncertain:
        case State_Evaluated_Safe:
        case State_Evaluated_Mine:
        case State_Flagged:
            return OpResult_Invalid;
            // map.at(pos) = State_Unevaluated;
            // flagged_count--;
            // break;
        case State_Flipped:
            for(int i = 0; i < adj_max; i++)
            {
                int adj_pos = GetAdjPos(pos, i);
                if(adj_pos == -1)
                {
                    continue;
                }
                State state = map.at(adj_pos);
                switch(state)
                {
                case State_Evaluated_Uncertain:
                case State_Evaluated_Mine:
                    return OpResult_Invalid;
                }
            }
            map.at(pos) = State_Unevaluated;
            constrains.erase(pos);
            break;
        }
        Analyse(false);
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
    char GetState(int pos) const
    {
        switch(map.at(pos))
        {
        case State_Unevaluated:    
            return 'e';
        case State_Flagged:
            return 'f';
        case State_Flipped:
            return '0' + constrains.at(pos);
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