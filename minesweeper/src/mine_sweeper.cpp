#include <mine_sweeper.h>
void MineSweeper::Revert()
{
    auto iter = flipped.begin();
    int tries = flipped.size();
    while(tries > 0)
    {
        if(rand() % tries == 0)
        {
            flipped.erase(iter);
            return;
        }
        ++iter;
        --tries;
    }
}
void MineSweeper::Analyse()
{
    solutions.clear();
    solution_mines = 0;
    solution_safes = 0;
    for(auto& constrain : flipped)
    {
        for(int i = 0; i < adj_max; i++)
        {
            int var = GetVar(constrain.first, i);
            if (var < 0 || flagged.contains(var) || flipped.contains(var) || solutions.contains(var))
            {
                continue;
            }
            assumptions.clear();
            assumptions[var] = true;
            assumption_mines = 1;
            assumption_safes = 0;

            if(!Solve(flipped.begin(), 0, 0))
            {
                solutions[var] = Analysed_Safe;
                solution_safes++;
                continue;
            }
            assumptions.clear();
            assumptions[var] = false;
            assumption_mines = 0;
            assumption_safes = 1;

            if(!Solve(flipped.begin(), 0, 0))
            {
                solutions[var] = Analysed_Mine;
                solution_mines++;                
                continue;
            }
            solutions[var] = Analysed_Both;
        }
    }
}
bool MineSweeper::Solve(std::unordered_map<int, int>::const_iterator current_constrain, int varidx_of_current, int sum_of_current)
{
    //所有条件全部满足
    if (current_constrain == flipped.end())
    {
        return true;
    }
    //当前约束无法满足
    if (varidx_of_current > adj_max || sum_of_current > current_constrain->second)
    {
        return false;
    }
    //雷已超过总数
    if (assumption_mines + solution_mines + flagged.size() > mines)
    {
        return false;
    }
    //雷到不了总数
    if (assumption_safes + solution_safes + flipped.size() + mines > width * height)
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
    if(flipped.contains(var))
    {
        assert(!assumed && !solutions.contains(var));
        return Solve(current_constrain, varidx_of_current + 1, sum_of_current);
    }
    if(flagged.contains(var))
    {  
        assert(!assumed && !solutions.contains(var));
        return Solve(current_constrain, varidx_of_current + 1, sum_of_current + 1);
    }
    if(solutions.contains(var))
    {
        switch(solutions.at(var))
        {
            case Analysed_Safe:
                assert(!assumed);
                return Solve(current_constrain, varidx_of_current + 1, sum_of_current);
            case Analysed_Mine:
                assert(!assumed);
                return Solve(current_constrain, varidx_of_current + 1, sum_of_current + 1);
        }
    }
    //当前变量已假定
    if(assumed)
    {
        return Solve(current_constrain, varidx_of_current + 1, sum_of_current + assumptions[var]);
    }

    assumptions[var] = true;
    assumption_mines++;
    if(Solve(current_constrain, varidx_of_current, sum_of_current)) return true;
    assumptions[var] = false;
    assumption_mines--;
    assumption_safes++;
    if(Solve(current_constrain, varidx_of_current, sum_of_current)) return true;
    //假设不成立
    assumptions.erase(var);
    assumption_safes--;
    return false;
}
std::string MineSweeper::Debug()
{
    std::string result;
    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            int var = j * width + i;
            if(flagged.contains(var))
            {
                result.append("标");
            }
            else if(flipped.contains(var))
            {
                switch(flipped[var])
                {
                case 0:
                    result.append("零");
                    break;
                case 1:
                    result.append("一");
                    break;
                case 2:
                    result.append("二");
                    break;
                case 3:
                    result.append("三");
                    break;
                case 4:
                    result.append("四");
                    break;
                case 5:
                    result.append("五");
                    break;
                case 6:
                    result.append("六");
                    break;
                case 7:
                    result.append("七");
                    break;
                case 8:
                    result.append("八");
                    break;
                }
            }
            else if(solutions.contains(var))
            {
                switch(solutions.at(var))
                {
                case Analysed_Safe:
                    result.append("安");
                    break;
                case Analysed_Both:
                    result.append("危");
                    break;
                case Analysed_Mine:
                    result.append("雷");
                    break;
                }
            }
            else
            {
                result.append("格");
            }
        }
        result.append("\n");
    }
    return result;
}
