#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h> 
#include <sys/mman.h>
int fd;
int bmpfd;
int * p;
void show(int bmpfd);
//初始化显示屏，在显示屏上显示东西之前调用该函数
void init_lcd()
{
	//打开文件
	fd = open("/dev/fb0",O_RDWR);
	if(fd == -1)
	{
		printf("open lcd fail:\n");
		perror("");
		return ;
	}
	//映射 
	p = mmap(NULL,480*800*4,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
	if(p==NULL)
	{
		printf("mmap fail:\n");
		perror("");
		return ;
	}
}
//销毁屏幕资源，在使用完显示屏之后调用一次
void destroy_lcd()
{
	//解除映射并关闭文件
	munmap(p,480*800*4);
	close(fd);
}
//显示一个点，该点的位置  第i行，第j列，颜色color
void display_point(int i,int j,int color)
{
	if(i>=0 && i<480 && j>=0 && j<800)
		*(p+800*i+j) = color;
}
//清屏：把显示屏变为 color
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
}
//将图片对应点的颜色赋值给像素点的方法,并把打印的图片放在左上角,怎么样实现移动排放图片
int main()
{
	init_lcd();
	int width;
	int height;
	//定义一个二维数组，存放图片地址
	char path[16][50];
	strcpy(path[0],"/df/2.bmp");
	strcpy(path[1],"/df/4.bmp");
	strcpy(path[2],"/df/8.bmp");
	strcpy(path[3],"/df/2.bmp");
	strcpy(path[4],"/df/2.bmp");
	strcpy(path[5],"/df/2.bmp");
	strcpy(path[6],"/df/2.bmp");
	strcpy(path[7],"/df/2.bmp");
	strcpy(path[8],"/df/2.bmp");
	strcpy(path[9],"/df/2.bmp");
	strcpy(path[10],"/df/2.bmp");
	strcpy(path[11],"/df/2.bmp");
	strcpy(path[12],"/df/2.bmp");
	strcpy(path[13],"/df/2.bmp");
	strcpy(path[14],"/df/2.bmp");
	strcpy(path[15],"/df/2.bmp");
	//循环16遍将地址取出来放到open函数的对应参数中
	//定义一个图片打印位置的初始值,0,0为左上角
	int x=0;
	int y=0;
	for (int a = 0; a < 16; a++) 
	{
		int bmpfd = open(path[a],O_RDONLY);
		if(bmpfd == -1)
		{
			printf("open bmp fail:\n");
			perror("");
			return 0;
		}
		//将图片在左上角打印,x,y为打印的初始位置
		//bmp_copy(bmpfd,x,y,width,height);
		//读取宽高并打印//有了宽度和高度，就可以循环显示像素点了
		lseek(bmpfd,0x12,SEEK_SET);//光标定位到宽度位置
		read(bmpfd,&width,4);//读取宽度
		//lseek(bmpfd,0x16,SEEK_SET);//光标定位到宽度位置
		read(bmpfd,&height,4);//读取高度
		printf("width=%d,height=%d\n",width,height);
		lseek(bmpfd,54,SEEK_SET);//定位光标到像素数组位置
		char buf[width*height*3];
		read(bmpfd,buf,width*height*3);//读取各个像素点保存在 buf中
		int i,j;
		int color;
		int n=0;
		unsigned char r,g,b;
		//i控制行变，j控制列，通过控制两个for循环可以控制打印区域
		for(i=height+y-1;i>=y;i--)
		{
			for(j=x;j<x+width;j++)
			{
				//中间为上色区域，不用管
				//color = 这张图片对应位置的颜色
				b = buf[n++];
				g = buf[n++];
				r = buf[n++];
				color = r << 16 | g << 8 | b;
				display_point(i,j,color);
			}
		}
		//变换打印的初始位置，确定打印的范围是否超过限制,长度超过800换第二行
		x=width+x;
		if(width>800-x)
		{
			y=height+y;
			//对x重新置零
			x=0;
		}
	}
	destroy_lcd();
	return 0;
}
