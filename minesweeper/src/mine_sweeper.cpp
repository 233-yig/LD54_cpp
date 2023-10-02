#include <mine_sweeper.h>
int MineSweeper::Evaluate(int var)
{
    int result = 0;
    
    assumption_safes.clear();
    assumption_mines.clear();
    if (map[var] != State_Evaluated_Safe)
    {
        assumption_safes.insert(var);
    }
    assert(Solve(constrains.begin(), 0, 0));
    for (int i = 0; i < adj_max; i++)
    {
        int pos = GetVar(var, i);
        if (pos == -1)
        {
            continue;
        }
        switch(map[pos])
        {
        case State_Unevaluated:
        case State_Evaluated_Uncertain:
            break;
        case State_Flagged:
        case State_Evaluated_Mine:
            result++;
            continue;
        case State_Flipped:
        case State_Evaluated_Safe:
            continue;
        }

        if(assumption_mines.contains(pos))
        {
            result++;
            continue;
        }
        if(assumption_safes.contains(pos))
        {
            continue;
        }
        int unevaluated_count = width * height - evaluated_mines - evaluated_safes  - flagged_count - constrains.size() - assumption_mines.size() - assumption_safes.size() ;
        int unevaluated_mine_count = mines - evaluated_mines - flagged_count - assumption_mines.size();
        bool isMine = rand() % unevaluated_count < unevaluated_mine_count;
        (isMine ? assumption_mines : assumption_safes).insert(var);
        result += isMine;
    }
    return result;
}
void MineSweeper::Analyse()
{
    for(auto& state: map)
    {
        switch(state)
        {
        case State_Evaluated_Safe:
        case State_Evaluated_Uncertain:
        case State_Evaluated_Mine:
            state = State_Unevaluated;
            break;
        default:
            break;
        }
    }
    evaluated_mines = 0;
    evaluated_safes = 0;
    for(auto& constrain : constrains)
    {
        for(int i = 0; i < adj_max; i++)
        {
            int var = GetVar(constrain.first, i);
            if (var < 0 || map[var] != State_Unevaluated)
            {
                //无需计算
                continue;
            }
            assumption_safes.clear();
            assumption_mines.clear();
            assumption_mines.insert(var);
            if(!Solve(constrains.begin(), 0, 0))
            {
                //不可能是雷
                map[var] = State_Evaluated_Safe;
                evaluated_safes++;
                continue;
            }
            assumption_safes.clear();
            assumption_mines.clear();
            assumption_safes.insert(var);
            if(!Solve(constrains.begin(), 0, 0))
            {
                //不可能不是雷
                map[var] = State_Evaluated_Mine;
                evaluated_mines++;                
                continue;
            }
            map[var] = State_Evaluated_Uncertain;
        }
    }
}
bool MineSweeper::Solve(std::unordered_map<int, int>::const_iterator current_constrain, int varidx_of_current, int sum_of_current)
{
    //所有条件全部满足
    if (current_constrain == constrains.end())
    {
        return true;
    }
    //当前约束无法满足
    if (varidx_of_current > adj_max || sum_of_current > current_constrain->second)
    {
        return false;
    }
    //雷已超过总数
    if (assumption_mines.size() + evaluated_mines + flagged_count > mines)
    {
        return false;
    }
    //雷到不了总数
    if (assumption_safes.size() + evaluated_safes + constrains.size() + mines > width * height)
    {
        return false;
    }
    
    //当前约束已满足
    if(varidx_of_current == adj_max && sum_of_current == current_constrain->second)
    {
        return Solve(++current_constrain, 0, 0);
    }
    
    int var = GetVar(current_constrain->first, varidx_of_current);
    //当前格子超出边界
    if(var < 0)
    {
        return Solve(current_constrain, varidx_of_current + 1, sum_of_current);
    }
    

    bool assumed = assumption_safes.contains(var) || assumption_mines.contains(var);
    //当前变量已确定
    switch(map[var])
    {
    case State_Unevaluated:
    case State_Evaluated_Uncertain:
        break;
    case State_Flipped:
    case State_Evaluated_Safe:
        assert(!assumed);
        return Solve(current_constrain, varidx_of_current + 1, sum_of_current);
    case State_Flagged:
    case State_Evaluated_Mine:
        assert(!assumed);
        return Solve(current_constrain, varidx_of_current + 1, sum_of_current + 1);
    }
    //当前变量已假定
    if(assumption_safes.contains(var))
    {
        return Solve(current_constrain, varidx_of_current + 1, sum_of_current);
    }
    if(assumption_mines.contains(var))
    {
        return Solve(current_constrain, varidx_of_current + 1, sum_of_current + 1);
    }
    assumption_mines.insert(var);
    if(Solve(current_constrain, varidx_of_current, sum_of_current)) return true;
    assumption_mines.erase(var);

    assumption_safes.insert(var);
    if(Solve(current_constrain, varidx_of_current, sum_of_current)) return true;
    assumption_safes.erase(var);
    //无解 当前假设不成立
    return false;
}