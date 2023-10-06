#include <mine_sweeper.h>
int MineSweeper::Success(int var)
{
    int result = 0;
    
    assumptions.clear();
    assumption_mines = 0;
    assumption_safes = 0;
    if (map[var] != State_Evaluated_Safe)
    {
        assumptions[var] = false;
        assumption_safes++;
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

        if(assumptions.contains(pos))
        {
            result += assumptions[pos];
            continue;
        }
        int decided_mine_count = evaluated_mines + flagged_count + assumption_mines;
        int decided_safe_count = evaluated_safes + constrains.size() + assumption_safes;
        bool isMine = rand() % (width * height - decided_mine_count - decided_safe_count) < mines - decided_mine_count;
        (isMine ? assumption_mines : assumption_safes)++;
        //assumptions[pos] = isMine;
        result += isMine;
    }
    return result;
}
void MineSweeper::Fail(int var, bool isMine)
{
    assumptions.clear();
    assumption_mines = 0;
    assumption_safes = 0;
    if(isMine && map.at(var) != State_Evaluated_Mine)
    {
        assumptions[var] = true;
        assumption_mines++;
    }
    if (!isMine && map.at(var) != State_Evaluated_Safe)
    {
        assumptions[var] = false;
        assumption_safes++;
    }
    assert(Solve(constrains.begin(), 0, 0));
    for(int i = 0; i < width * height; i++)
    {
        if(map.at(i) == State_Unevaluated)
        {

            int decided_mine_count = evaluated_mines + flagged_count + assumption_mines;
            int decided_safe_count = evaluated_safes + constrains.size() + assumption_safes;
            bool isMine = rand() % (width * height - decided_mine_count - decided_safe_count) < mines - decided_mine_count;
            (isMine ? assumption_mines : assumption_safes)++;
            //assumptions[pos] = isMine;
            if (isMine)
            {
                map.at(i) = State_Lose;
            }
        }
        else if(map.at(i) == State_Evaluated_Uncertain)
        {
            assert(assumptions.contains(i));
            if(assumptions.at(i))
            {
                map.at(i) = State_Lose;
            }
        }
        else if(map.at(i) == State_Evaluated_Mine)
        {
            map.at(i) = State_Lose;
        }
    }
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
    evaluated_uncertains = 0;
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
            assumptions.clear();
            assumptions[var] = true;
            assumption_mines = 1;
            assumption_safes = 0;
            if(!Solve(constrains.begin(), 0, 0))
            {
                //不可能是雷
                map[var] = State_Evaluated_Safe;
                evaluated_safes++;
                continue;
            }
            assumptions.clear();
            assumptions[var] = false;
            assumption_mines = 0;
            assumption_safes = 1;
            if(!Solve(constrains.begin(), 0, 0))
            {
                //只可能是雷
                map[var] = State_Evaluated_Mine;
                evaluated_mines++;                
                continue;
            }
            map[var] = State_Evaluated_Uncertain;
            evaluated_uncertains++;
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
    if (assumption_mines + evaluated_mines + flagged_count > mines)
    {
        return false;
    }
    //雷到不了总数
    if (assumption_safes + evaluated_safes + constrains.size() + mines > width * height)
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
    

    bool assumed = assumptions.contains(var);
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
    if(assumptions.contains(var))
    {
        return Solve(current_constrain, varidx_of_current + 1, sum_of_current + assumptions.at(var));
    }
    switch (rand() % 2)
    {
    case 0:
        assumptions[var] = true;
        assumption_mines++;
        if(Solve(current_constrain, varidx_of_current, sum_of_current)) return true;
        assumption_mines--;

        assumptions[var] = false;
        assumption_safes++;
        if(Solve(current_constrain, varidx_of_current, sum_of_current)) return true;
        assumption_safes--;
        break;
    
    case 1:
        assumptions[var] = false;
        assumption_safes++;
        if(Solve(current_constrain, varidx_of_current, sum_of_current)) return true;
        assumption_safes--;

        assumptions[var] = true;
        assumption_mines++;
        if(Solve(current_constrain, varidx_of_current, sum_of_current)) return true;
        assumption_mines--;
        break;
    }
    //无解 当前假设不成立
    assumptions.erase(var);
    return false;
}