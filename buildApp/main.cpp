#include "BuildAppConfig.h"
#include "Helpers/GetAddressOf.h"

#include <iostream>

void Alloc(int** pptr)
{
	*pptr = new int(123);
}

int main()
{
	int* rawPtr = nullptr;
	Alloc(Helpers::GetAddressOf(rawPtr));

	while (true)
	{
		std::unique_ptr<int> uPtr;
		std::shared_ptr<int> sPtr;

		Alloc(Helpers::GetAddressOf(uPtr));
		Alloc(Helpers::GetAddressOf(sPtr));
	}
	

	return 0;
}
