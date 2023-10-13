#include "mine_sweeper.h"
int MineSweeper::Success(int var)
{
    int result = 0;
    
    Assumption assumption = MakeAssumption();
    if (map[var] != State_Evaluated_Safe)
    {
        assumption.With(var, false);
    }
    bool possible = Possible(assumption, constrains.begin());
    assert(possible);
    for (int adj = 0; adj < adj_max; adj++)
    {
        int adj_pos = GetAdjPos(var, adj);
        if (adj_pos == -1)
        {
            continue;
        }
        switch(map.at(adj_pos))
        {
        case State_Flagged:
        case State_Evaluated_Mine:
            result++;
        case State_Flipped:
        case State_Evaluated_Safe:
            continue;
        default:
            break;
        }

        switch(assumption.data.at(adj_pos))
        {
        case Assumptions_Mine:
            result++;
        case Assumptions_Safe:
            continue;
        default:
            break;
        }
        int decided_mine_count = evaluated_mines + flagged_count + assumption.mine_count;
        if (decided_mine_count == mines)
        {
            //没必要继续统计了
            //assumption.safe_count++;
            continue;
        }
        int decided_safe_count = evaluated_safes + constrains.size() + assumption.safe_count;
        bool isMine = rand() % (width * height - decided_mine_count - decided_safe_count) < mines - decided_mine_count;
        (isMine ? assumption.mine_count : assumption.safe_count)++;
        result += isMine;
    }
    return result;
}
void MineSweeper::Fail(int var, bool isMine)
{
    Assumption assumption = MakeAssumption();
    if(isMine && map.at(var) != State_Evaluated_Mine)
    {
        assumption.With(var, true);
    }
    if (!isMine && map.at(var) != State_Evaluated_Safe)
    {
        assumption.With(var, false);
    }
    bool possible = Possible(assumption, constrains.begin());
    assert(possible);
    for(int i = 0; i < width * height; i++)
    {
        switch(map.at(i))
        {
        case State_Unevaluated:
            if (assumption.data.at(i) == Assumptions_Mine)
            {
                map.at(i) = State_Lose;
                break;
            }
            [&](){
                int decided_mine_count = evaluated_mines + flagged_count + assumption.mine_count;
                int decided_safe_count = evaluated_safes + constrains.size() + assumption.safe_count;
                bool isMine = rand() % (width * height - decided_mine_count - decided_safe_count) < mines - decided_mine_count;
                if (isMine)
                {
                    assumption.mine_count++;
                    map.at(i) = State_Lose;
                }
                else
                {
                    assumption.safe_count++;
                }
            }();
            break;
        case State_Evaluated_Uncertain:
            if(assumption.data.at(i) == Assumptions_Mine)
            {
                map.at(i) = State_Lose;
            }
            break;
        case State_Evaluated_Mine:
            map.at(i) = State_Lose;
            break;
        }
    }
}

void MineSweeper::Analyse(bool incremental)
{
    for(auto& state: map)
    {
        switch(state)
        {
            
        case State_Evaluated_Safe:
        case State_Evaluated_Mine:
            if(incremental)
            {
                break;
            }
        case State_Evaluated_Uncertain:
            state = State_Unevaluated;
            break;
        default:
            break;
        }
    }
    if(!incremental)
    {
        evaluated_mines = 0;
        evaluated_safes = 0;
    }
    evaluated_uncertains = 0;

    for(auto& constrain : constrains)
    {
        for(int adj = 0; adj < adj_max; adj++)
        {
            int pos = GetAdjPos(constrain.first, adj);
            if(pos < 0 || map.at(pos) != State_Unevaluated)
            {
                //无需计算
                continue;
            }
            if(!Possible(MakeAssumption().With(pos, true), constrains.begin()))
            {
                //不可能是雷
                map.at(pos) = State_Evaluated_Safe;
                evaluated_safes++;
                continue;
            }
            if(!Possible(MakeAssumption().With(pos, false), constrains.begin()))
            {
                //只可能是雷
                map.at(pos) = State_Evaluated_Mine;
                evaluated_mines++;
                continue;
            }
            map.at(pos) = State_Evaluated_Uncertain;
            evaluated_uncertains++;
        }
    }

}
bool MineSweeper::Possible(MineSweeper::Assumption& assumption, std::unordered_map<int, int>::const_iterator current_constrain)
{
    //所有条件全部满足
    if (current_constrain == constrains.end())
    {
        return true;
    }
    int flexible_pos[adj_max];
    int flexible_count = 0;
    int flexible_mines = current_constrain->second;
    for(int i = 0; i < adj_max; i++)
    {
        int pos = GetAdjPos(current_constrain->first, i);
        if(pos < 0)
        {
            continue;
        }

        switch(map.at(pos))
        {
        case State_Flagged:
        case State_Evaluated_Mine:
            flexible_mines--;
        case State_Flipped:
        case State_Evaluated_Safe:
            continue;
        }

        switch(assumption.data.at(pos))
        {
        case Assumptions_Mine:
            flexible_mines--;
        case Assumptions_Safe:
            continue;
        }

        flexible_pos[flexible_count] = pos;
        flexible_count++;
    }

    if(flexible_mines < 0 || flexible_mines > mines - evaluated_mines - flagged_count - assumption.mine_count)
    {
        return false;
    }
    int flexible_safes = flexible_count - flexible_mines;
    if(flexible_safes < 0 || flexible_safes > width * height - mines - evaluated_safes - (int)constrains.size() - assumption.safe_count)
    {
        return false;
    }
    //randomize
    for(int i = 0; i < flexible_count; i++)
    {
        std::swap(flexible_pos[i], flexible_pos[rand()%(flexible_count - i)]);
    }

    assumption.mine_count += flexible_mines;
    assumption.safe_count += flexible_safes;
    ++current_constrain;
    if (flexible_mines == 0)
    {
        for (int i = 0; i < flexible_count; i++)
        {
            assumption.data.at(flexible_pos[i]) = Assumptions_Safe;
        }

        if (Possible(assumption, current_constrain)) return true;
    }
    else
    {
        //gosper's hack
        for(int16_t cur = (1 << flexible_mines) - 1; cur < (1 << flexible_count); cur = (cur + (cur & -cur)) | (((cur ^ (cur + (cur & -cur))) >> 2) / (cur & -cur)))
        {
            for(int i = 0 ; i < flexible_count; i++)
            {
                assumption.data.at(flexible_pos[i]) = (cur & (1 << i))? Assumptions_Mine : Assumptions_Safe;
            }
            if(Possible(assumption, current_constrain)) return true;
        }
    }

    for(int i = 0; i < flexible_count; i++)
    {
        assumption.data.at(flexible_pos[i]) = Assumptions_Null;
    }
    //--current_constrain;
    assumption.safe_count -= flexible_safes;
    assumption.mine_count -= flexible_mines;

    //无解 当前假设不成立
    return false;    
}