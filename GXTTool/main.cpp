#include "IVGXT.h"
#include <iostream>
#include <exception>

int main(int argc, char **argv)
{
	IVGXT inst;

	//不带参数 读取自身目录的GTA4.txt生成chinese.gxt
	//文件夹形式的文本与gxt互转

	try
	{
		inst.process2args("D:/american.gxt", "D:/GTA4EN");
		inst.process0arg();
/*
		if (argc == 1)
		{
			inst.process0arg();
		}
		else if (argc == 3)
		{
			inst.process2args(argv[1], argv[2]);
		}
		else
		{
			std::cout << "命令行使用不正确。" << std::endl;
		}*/
	}
	catch (std::exception* e)
	{
		std::cout << e->what() << std::endl;
	}
	
	return 0;
}
