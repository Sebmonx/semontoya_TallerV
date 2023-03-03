

#include <stdint.h>

int main(void)
{
    //Operaciones basicas
	uint16_t varOperadores = 12;

	//Operaciones
	varOperadores = varOperadores + 1;
	varOperadores++;
	varOperadores += 1;

	//Modulo
	varOperadores = varOperadores % 2;
	varOperadores %= 2;


	//shift
	uint8_t testVar = 0b00000101;

	testVar = testVar << 1;
	testVar >>= 1;

	testVar = testVar << 4;

	//operadores bitwise
	uint8_t variableA = 0b00111101;
	uint8_t variableB = 0b11111111;

	//or
	uint8_t resultado = variableA | variableB;
	//and
	resultado = variableA & variableB;
	//not
	resultado = ~resultado;
	//xor
	resultado = variableA ^ variableB;

	//varias operaciones
	resultado = ~(0b111 << 2);

	variableA &= resultado;

	variableA |= (0b11 << 6);

}
