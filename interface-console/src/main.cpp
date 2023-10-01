#include <mine_sweeper.h>
#include <ctime>
#include <cstdio>
int main()
{
    system("chcp 65001");
    srand(time(0));
    int w;
    int h;
    int count;
    printf("conf(8x8,20)");
    scanf("%dx%d,%d", &w, &h,&count);
    printf("map(s to stop):\n");
    std::string map;
    while(true)
    {
        char c = getchar();
        if(c == 's')
        {
            break;
        }
        map += c;
    }
    MineSweeper game;
    game.Load(w, h, count, map.c_str());

    while(true)
    {
        printf("\n%s\n Your action:", game.Debug().c_str());
        int x;
        int y;
        char c;
        scanf("%d, %d, %c", &x, &y, &c);
        switch(c)
        {
        case 'o':
            game.Flip(x,y);
            break;
        case 'f':
            game.Flag(x,y);
            break;
        }
    }
}