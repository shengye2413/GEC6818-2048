#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <time.h>
int game[4][4]={
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};//定义4*4数组
int fd;
int *p;	
void init_lcd()
{
    fd = open("/dev/fb0",O_RDWR);
	if(fd == -1)
	{
		printf("open lcd fail:\n");
		perror("");
		return ;
	}
    p = mmap(NULL,480*800*4,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
	if(p==NULL)
	{
		printf("mmap fail:\n");
		perror("");
		return ;
	}
}//打开显示屏文件

void destroy_lcd()
{
    munmap(p,480*800*4);
	close(fd);
}//解除映射关闭文件

void display_point(int i,int j,int color)
{
	if(i>=0 && i<480 && j>=0 && j<800)
		*(p+800*i+j) = color;
}//显示一个点，该点的位置  第i行，第j列，颜色color

void clear(int color)
{
	int i,j;
	for(i=0;i<480;i++)
	{
		for(j=0;j<800;j++)
		{
			display_point(i,j,color);
		}
	}
}//清屏：把显示屏变为 color

void display_bmp(const char *pathname,int x0,int y0)
{
	int bmpfd = open(pathname,O_RDONLY);
	if(bmpfd == -1)
	{
		printf("open fail:\n");
		perror("");
		return ;
	}
	lseek(bmpfd,0x12,SEEK_SET);
    //光标定位到宽度位置
	int width;
	read(bmpfd,&width,4);
	int height;
	read(bmpfd,&height,4);
    //读取高度和宽度
	//printf("width=%d,height=%d\n",width,height);
	lseek(bmpfd,54,SEEK_SET);
    //定位光标到像素数组位置
	char buf[width*height*3];
	read(bmpfd,buf,width*height*3);
    //读取各个像素点保存在 buf中
	int i,j;
	int color;
	int n=0;
	unsigned char r,g,b;
	for(i=height-1;i>=0;i--)
	{
		for(j=0;j<width;j++)
		{
			b = buf[n++];
			g = buf[n++];
			r = buf[n++];
			color = r << 16 | g << 8 | b;
			display_point(i+y0,j+x0,color);
		}
	}
    close(bmpfd);
}//在特定位置显示图像

void display_game()
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			char pathname[100];
			sprintf(pathname,"/df/%d.bmp",game[i][j]);
			display_bmp(pathname,190+105*j,30+105*i);
		}
	}
}//显示4*4的游戏界面

void set_rand()
{
	int data;
	if(rand()%5 == 0)
		data = 4;
	else
		data = 2;
	int x,y;
	while(1)
	{
		x = rand()%4;//随机生成行坐标
		y = rand()%4;//随机生成列坐标
		if(game[x][y] == 0)
		{
			game[x][y] = data;
			break;
		}
	}
}//随机把一个0变成2或者4

void init_game()
{
	init_lcd();//初始化屏幕
	clear(0xb0e0e6);//清屏 
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			game[i][j] = 0;
		}
	}
    srand(time(NULL));
	set_rand();
	set_rand();
    set_rand();
	set_rand();
}//随机出现四个数字 2或者4

int up()
{
    int i,j,k;
    int flag=0;
    for(j=0;j<4;j++)
    {
        for(i=0;i<3;i++)
        {
            for(k=i+1;k<4;k++)
            {
                if (game[k][j]!=0)
                //下面的是否为0
                {
                    if(game[i][j]==0)
                    {
                        game[i][j]=game[k][j];
                        game[k][j]=0;
                        flag=1;
                    }//上面为0
                    else
                    {
                        if (game[i][j]==game[k][j])
                        {
                            game[i][j]+=game[k][j];
                            game[k][j]=0;
                            flag=1;
                            break;
                            //break终止防止连续合并
                        }
                        else
                        break;
                        //防止隔行合并
                    }//上面不为0
                }
            }
        }//当前列上面的数与下面的数作比较
    }
    return flag;
}//定义上划规则

int down()
{
    int i,j,k;
    int flag=0;
    for(j=0;j<4;j++)
    {
        for(k=3;k>=0;k--)
        {
            for(i=k-1;i>=0;i--)
            {
                if (game[i][j]!=0)
                //上面的是否为0
                {
                    if(game[k][j]==0)
                    {
                        game[k][j]=game[i][j];
                        game[i][j]=0;
                        flag=1;
                    }//下面为0
                    else
                    {
                        if (game[k][j]==game[i][j])
                        {
                            game[k][j]+=game[i][j];
                            game[i][j]=0;
                            flag=1;
                            break;
                            //break终止防止连续合并
                        }
                        else
                        break;
                        //防止隔行合并
                    }//下面不为0
                }
            }
        }//当前列下面的数与上面的数作比较
    }
    return flag;
}//定义下划规则

int left()
{
    int i,j,k;
    int flag=0;
    for(i=0;i<4;i++)
    {
        for(j=0;j<3;j++)
        {
            for(k=j+1;k<4;k++)
            {
                if (game[i][k]!=0)
                //右面的是否为0
                {
                    if(game[i][j]==0)
                    {
                        game[i][j]=game[i][k];
                        game[i][k]=0;
                        flag=1;
                    }//左面为0
                    else
                    {
                        if (game[i][j]==game[i][k])
                        {
                            game[i][j]+=game[i][k];
                            game[i][k]=0;
                            flag=1;
                            break;
                            //break终止防止连续合并
                        }
                        else
                        break;
                        //防止隔行合并
                    }//左面不为0
                }
            }
        }//当前列右面的数与左面的数作比较
    }
    return flag;
}//定义左划规则

int right()
{   int flag=0;
    int i,j,k;
    for(i=0;i<4;i++)
    {
        for(k=3;k>0;k--)
        {
            for(j=k-1;j>=0;j--)
            {
                if (game[i][j]!=0)
                //左面的是否为0
                {
                    if(game[i][k]==0)
                    {
                        game[i][k]=game[i][j];
                        game[i][j]=0;
                        flag=1;
                    }//右面为0
                    else
                    {
                        if (game[i][j]==game[i][k])
                        {
                            game[i][k]+=game[i][j];
                            game[i][j]=0;
                            flag=1;
                            break;
                            //break终止防止连续合并
                        }
                        else
                        break;
                        //防止隔行合并
                    }//右面不为0
                }
            }
        }//当前列左面的数与右面的数作比较
    }
    return flag;
}//定义右划规则

int gameover()
{
    int over=0;
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            if(game[i][j]==0)
                over=1;
        }
    }
    return over;
}//判断数组中的值是否为0

int main()
{
    init_game();
    //初始化游戏
    //while(getchar()!='\0')
    while (getchar()!='\0')
    {
        display_game();
        //显示界面
        char ch;
        ch=getchar();
        //72:↑,80:↓,75:←,77:→ 
        if(ch=='w')
        {
            int flag=up();
            if(flag==1)
            set_rand();
            //当发生有效移动的时候，生成随机数
            if(flag==0)
            {
                int over=gameover();
                if(over==0)
                {
                    clear(0xb0e0e6);
                    display_bmp("/df/game_over.bmp",0,0);
                    break;
                }//判断当未发生有效移动的时候，数组是否都不为0
            }
            display_game();
        }
        else if(ch=='s')
        {
            int flag=down();
            if(flag==1)
            set_rand();
            if(flag==0)
            {
               int over=gameover();
                if(over==0)
                {
                    clear(0xb0e0e6);
                    display_bmp("/df/game_over.bmp",0,0);
                    break;
                }
            }          
            display_game();
        }
        else if(ch=='a')
        {
            int flag=left();
            if(flag==1)
            set_rand();
            if(flag==0)
            {
               int over=gameover();
                if(over==0)
                {
                    clear(0xb0e0e6);
                    display_bmp("/df/game_over.bmp",0,0);
                    break;
                }
            }          
            display_game();
        }
        else if(ch=='d')
        {

            int flag=right();
            if(flag==1)
            set_rand();
            if(flag==0)
            {
                int over=gameover();
                if(over==0)
                {
                    clear(0xb0e0e6);
                    display_bmp("/df/game_over.bmp",0,0);
                    break;
                }
            }          
            display_game();
        }
    }
    destroy_lcd();
    return 0;
}