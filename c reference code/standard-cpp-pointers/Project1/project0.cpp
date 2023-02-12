#include "project0.h"
#include <stdio.h>
using namespace std;
//#define MAX 3
constexpr int returnConst()
{
	const int MAX = 3;
	return MAX;
}
int main()
{
	// a long hexadecimal number that represents a memory address
	int intVar = 8; // actual variable declaration
	int* intPoint = nullptr; // declare integer pointer variable and initialize with default null pointer
	intPoint = &intVar; // assign the address of intVar to pointer variable intPoint
	printf("Address of a variable intVar: %x\n", &intVar);
	printf("Address of the variable intVar stored in a pointer intPoint: %x\n", intPoint);
	printf("Address of the pointer intPoint: %x\n", &intPoint);
	printf("Value of the pointer intPoint: %x\n", *intPoint);
	printf("\n");
	char charArr[8]{ 'k', 'h', 'x', 'r', 'b', ' ', '8' };
	printf("charArr[] (s)= %s\n", charArr);
	printf("charArr[] (x)= %x\n", charArr);
	printf("charArr[] (&x)= %x\n", &charArr);
	printf("charArr[] (*x)= %x\n", *charArr); // k in hex is 6b
	printf("charArr[] (*c)= %c\n", *charArr); // k
	char* charPoint = &charArr[0];
	printf("&charPoint[0]: %x\n", &charPoint[0]);
	printf("charPoint: %x\n", charPoint);
	printf("&charPoint: %x\n", &charPoint);
	printf("*charPoint: %c\n", *charPoint);
	printf("charArr: %x\n", charArr);
	printf("*charArr: %c\n", *charArr);
	printf("&charArr: %x\n", &charArr);
	printf("&charArr[0]: %x\n", &charArr[0]);
	printf("char* charPoint = &charArr[0]: %x\n", charPoint);
	charPoint = &charArr[1];
	printf("char* charPoint = &charArr[1]: %x\n", charPoint);
	printf("char* charPoint = &charArr[0](&): %x\n", &charPoint);
	printf("char* charPoint = &charArr[0](*): %c\n", *charPoint);
	printf("&charArr[1]: %x\n", &charArr[1]);
	printf("*charArr: %c\n", *charArr);
	printf("*(charPoint+2): %c\n", *(charPoint + 2));
	printf("\n");
	int* ptr = NULL;
	printf("The value of ptr is: %x\n", ptr);
	long* longPtr = nullptr;
	printf("The value of longPtr is: %x\n", longPtr);
	double* doublePtr = nullptr;
	printf("The value of doublePtr is: %x\n", doublePtr);

	if (ptr)
	{
		/* succeeds if p is not null */
	}
	if (!ptr)
	{
		/* succeeds if p is null */
	}

	printf("\n");
	int var[] = { 10, 100, 200 };
	int i, * dyn; /* let us have array address in pointer */
	dyn = var;
	for (i = 0; i < returnConst(); i++) {
		printf("Address of var[%d] = %x\n", i, dyn);
		printf("Value of var[%d] = %d\n", i, *dyn); /* move to the next location by 4 bytes as the elemenst of the array are of type int */
		dyn++;
	}
}

/*
Data Type		Format Specifier	Minimal Range							Typical Bit Size
unsigned char		%c			0 to +255							8
char			%c			-127 to +127							8
signed char		%c			-127 to +127							8
int			%d, %i			-32,767 to +32,767						16 or 32
unsigned int		%u			0 to +65,535							16 or 32
signed int		%d, %i			Same as int							Same as int 16 or 32
short int		%hd			-32,767 to +32,767						16
unsigned short int	%hu			0 to +65,535							16
signed short int	%hd			Same as	short int						16
long int		%ld, %li		-2,147,483,647 to +2,147,483,647				32
long long int		%lld, %lli		-(263 – 1) to 263 – 1 (It will be added by the C99 standard)	64
signed long int		%ld, %li		Same as long int						32
unsigned long int	%lu			0 to +4,294,967,295						32
unsigned long long int	%llu			264 – 1 (It will be added by the C99 standard)			64
float			%f			1E-37 to 1E+37 along with six digits of the precisions here	32
double			%lf			1E-37 to 1E+37 along with six digits of the precisions here	64
long double		%Lf			1E-37 to 1E+37 along with six digits of the precisions here	80
*/
