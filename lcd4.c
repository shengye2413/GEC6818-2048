#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h> 
#include <sys/mman.h>
int fd;
int * p;
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
int main()
{
	init_lcd();
	clear(0xb0e0e6);
	char photo[16][100];
	strcpy(photo[0],"/df/0.bmp");
	strcpy(photo[1],"/df/2.bmp");
	strcpy(photo[2],"/df/4.bmp");
	strcpy(photo[3],"/df/8.bmp");
	strcpy(photo[4],"/df/16.bmp");
	strcpy(photo[5],"/df/32.bmp");
	strcpy(photo[6],"/df/64.bmp");
	strcpy(photo[7],"/df/128.bmp");
	strcpy(photo[8],"/df/256.bmp");
	strcpy(photo[9],"/df/512.bmp");
	strcpy(photo[10],"/df/1024.bmp");
	strcpy(photo[11],"/df/2048.bmp");
	strcpy(photo[12],"/df/4096.bmp");
	strcpy(photo[13],"/df/8192.bmp");
	strcpy(photo[14],"/df/16384.bmp");
	strcpy(photo[15],"/df/0.bmp");
	//定义数组存储图片地址
	int height,width;
	for(int a=0;a<16;a++)
	{
		int bmpfd= open(photo[a],O_RDONLY);
		if(bmpfd == -1)
		{
			printf("open fail:\n");
			perror("");
			return 0;
		}
		//判断打开是否失败
		lseek(bmpfd,0x12,SEEK_SET);//光标定位到宽度位置
		read(bmpfd,&width,4);//读取宽度
		//lseek(bmpfd,0x16,SEEK_SET);//光标定位到宽度位置
		read(bmpfd,&height,4);//读取高度
		printf("width=%d,height=%d\n",width,height);
		//有了宽度和高度，就可以循环显示像素点了
		lseek(bmpfd,54,SEEK_SET);//定位光标到像素数组位置
		char buf[width*height*3];
		read(bmpfd,buf,width*height*3);//读取各个像素点保存在 buf中
		//显示到左上角
		int i,j,c=0,d=0;
		int color;
		int n=0;
		unsigned char r,g,b;
		//for(i=height+c-1;i>=c;i--)
		//{
		//	for(j=d*width;j<width*(d+1);j++)
		//	{
		//		//color = 这张图片对应位置的颜色
		//		b = buf[n++];
		//		g = buf[n++];
		//		r = buf[n++];
		//		color = r << 16 | g << 8 | b;
		//		display_point(i,j,color);
		//		d++;
		//		if(j>799)
		//		d=0;
		//	}	
		//}
		for(i=height+c-1;i>=c;i--)
		{
			for(j=d;j<d+width;j++)
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
		d=width+d;
		if(width>800-d)
		{
			c=height+c;
			//对x重新置零
			d=0;
		}
	}
	destroy_lcd();
	return 0;
}