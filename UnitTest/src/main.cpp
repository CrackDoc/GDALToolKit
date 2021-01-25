#include <stdlib.h>
#include <iostream>
#include "XUnitTestBase.h"

int main(int argc, char** argv)
{
	XUnitTestFrame::getInstancePtr()->exec();

	std::cout << XUnitTestFrame::getInstancePtr()->toString() << std::endl;

	system("pause");

	return 0;
}