/*
 * ModBUS_Chelo.h
 *
 *  Created on: 20 may. 2021
 *      Author: Marcelo
 */

#ifndef MODBUS_CHELO_H_
#define MODBUS_CHELO_H_



#endif /* MODBUS_CHELO_H_ */

#include "main.h"

#define CLIENTE  0
#define SERVIDOR 1

struct MBUS
{
	uint8_t  _address;								//Canal  o direccion ModBUS

	uint8_t	 _mode;									//Funciona como servidor mode = 1 o cliente mode =0
	uint16_t _timeout;								//Timeout para esperar respuesta desde el Servidor
	uint8_t  _w_answer;								//Mensaje enviado aguardando respuesta

	uint8_t  _01_ReadCoils_EN;						//Habilitar codigo de función  01 0=DESACTIVADO 1=ACTIVADO
	uint8_t  _02_ReadDiscreteInputs_EN;				//Habilitar codigo de función  02 0=DESACTIVADO 1=ACTIVADO
	uint8_t  _03_ReadHoldingRegisters_EN;			//Habilitar codigo de función  03 0=DESACTIVADO 1=ACTIVADO
	uint8_t  _04_ReadInputRegisters_EN;				//Habilitar codigo de función  04 0=DESACTIVADO 1=ACTIVADO
	uint8_t  _05_WriteSingleCoil_EN;				//Habilitar codigo de función  05 0=DESACTIVADO 1=ACTIVADO
	uint8_t  _06_WriteSingleRegister_EN;			//Habilitar codigo de función  06 0=DESACTIVADO 1=ACTIVADO
	uint8_t  _15_WriteMultipleCoils_EN;				//Habilitar codigo de función  0F 0=DESACTIVADO 1=ACTIVADO
	uint8_t  _16_WriteMultipleRegisters_EN;			//Habilitar codigo de función  10 0=DESACTIVADO 1=ACTIVADO

	char _MBUS_RCVD[384];							//Vector ModBUS Recibido
	int  _n_MBUS_RCVD;								//Cdad de elementos del Vector ModBUS Recibido
	char _MBUS_2SND[384];							//Vector ModBUS a Enviar
	int  _n_MBUS_2SND;								//Cdad de elementos del Vector ModBUS a Enviar


	uint32_t _Coils;								//Son las salidas del I/O Afectado por comandos 01 05 15
	uint32_t _Inputs;								//Son las entradas del I/O Afectado por comandos 02
	uint8_t  _Holding_Registers[254];				//Registros de Aplicación Afectado por comandos 03 06 16
	uint8_t  _InputRegisters[254];					//Registro de entradas i/o Afectado por comandos 04


	uint8_t  _01_ReadCoils_QTY;						//Cantidad de Coils disponibles MAX32
	uint16_t _01_ReadCoils_StrtAddr;				//Dirección de inicio para consultar registros
	uint16_t _01_ReadCoils_QTYReg;					//Cantidad de registros a c

	uint8_t  _02_ReadDiscreteInputs_QTY;			//Cantidad de Entradas disponibles MAX32
	uint16_t _02_ReadDiscreteInputs_StrtAddr;		//Dirección de inicio para consultar registros
	uint16_t _02_ReadDiscreteInputs_QTYReg;			//Cantidad de registros a c

	uint8_t  _03_ReadHoldingRegisters_QTY;			//Cantidad de Entradas disponibles MAX por modbus 127 este dispositivo 64
	uint16_t _03_ReadHoldingRegisters_StrtAddr;		//Dirección de inicio para consultar registros
	uint16_t _03_ReadHoldingRegisters_QTYReg;		//Cantidad de registros a c

	uint8_t  _04_ReadInputRegisters_QTY;			//Cantidad de Entradas disponibles MAX por modbus 127 este dispositivo 64
	uint16_t _04_ReadInputRegisters_StrtAddr;		//Dirección de inicio para consultar registros
	uint16_t _04_ReadInputRegisters_QTYReg;			//Cantidad de registros a c

	uint8_t  _05_WriteSingleCoil_QTY;				//Cantidad de Coils disponibles MAX32
	uint16_t _05_WriteSingleCoil_StrtAddr;			//Dirección de inicio para consultar registros
	uint16_t _05_WriteSingleCoil_State;				//Cantidad de registros a c

	uint8_t  _15_WriteMultipleCoils_QTY;			//Cantidad de Coils disponibles MAX32
	uint16_t _15_WriteMultipleCoils_StrtAddr;		//Dirección de inicio para consultar registros
	uint16_t _15_WriteMultipleCoils_QTYReg;			//Cantidad de registros a c

	uint8_t  _06_WriteSingleRegister_QTY;			//Cantidad de Entradas disponibles MAX por modbus 127 este dispositivo 64
	uint16_t _06_WriteSingleRegister_Addr;			//Dirección de inicio para consultar registros
	uint16_t _06_WriteSingleRegister_Value;			//Cantidad de registros a c

	uint8_t  _16_WriteMultipleRegisters_QTY;		//Cantidad de Entradas disponibles MAX por modbus 127 este dispositivo 64
	uint16_t _16_WriteMultipleRegisters_StrtAddr;	//Dirección de inicio para consultar registros
	uint16_t _16_WriteMultipleRegisters_QTYReg;		//Cantidad de registros a c

};


int ModBUS(struct MBUS *);

/****************************************************************
*	       		Procesa el telegrama ModBUS						*
*	       														*
*		Devuelve el codigo de error si no lo puede procesar		*
****************************************************************/

int ModBUS_Check(char *, int);

/****************************************************************
*	       Determina si lo recibido es ModBUS					*
*	       														*
*		1 = ModBUS    0= No corresponde a ModBUS
*
*		Char = vector recibido      int= cantidad de elemntos 	*
****************************************************************/

int ModBUS_Check_tid(struct MBUS *);
/****************************************************************
* Determina si el transaction id y el comando 					*
*	 corresponde al enviado										*
*		1 = ModBUS    0= No corresponde el tid					*
*																*
*		Char = vector recibido     							 	*
****************************************************************/

void ModBUS_Config(struct MBUS *);

/****************************************************************
*	Setea varaiables y habilitaciones de funciones 				*
*																*
****************************************************************/
int ModBUS_F01_Assign(struct MBUS *, uint8_t, uint16_t);

/****************************************************************
*	Asigna valores a la Función 01 ReadCoils					*
*																*
*	void ModBUS_F03_Assign(struct MBUS *, uint8_t, uint16_t);	*
*																*
*	int			:	0 = OK  1=ERROR								*
*	struct MBUS	:	Estructura de datos MBUS					*
*	uint8_t		:	Dirección donde se guarda el dato			*
*	uint16_t	:	Dato a alamacenar							*
*																*
****************************************************************/
int ModBUS_F02_Assign(struct MBUS *, uint8_t, uint16_t);

/****************************************************************
*	Asigna valores a la Función 02 ReadDiscreteInputs	  		*
*																*
*	void ModBUS_F03_Assign(struct MBUS *, uint8_t, uint16_t);	*
*																*
*	int			:	0 = OK  1=ERROR								*
*	struct MBUS	:	Estructura de datos MBUS					*
*	uint8_t		:	Dirección donde se guarda el dato			*
*	uint16_t	:	Dato a alamacenar							*
*																*
****************************************************************/

int ModBUS_F03_Assign(struct MBUS *, uint8_t, uint16_t);

/****************************************************************
*	Asigna valores a la Función 03 ReadHoldginRegisters  		*
*																*
*	void ModBUS_F03_Assign(struct MBUS *, uint8_t, uint16_t);	*
*																*
*	int			:	0 = OK  1=ERROR								*
*	struct MBUS	:	Estructura de datos MBUS					*
*	uint8_t		:	Dirección donde se guarda el dato			*
*	uint16_t	:	Dato a alamacenar							*
*																*
****************************************************************/
int ModBUS_F04_Assign(struct MBUS *, uint8_t, uint16_t);

/****************************************************************
*	Asigna valores a la Función 04 ReadInputRegisters  		*
*																*
*	void ModBUS_F03_Assign(struct MBUS *, uint8_t, uint16_t);	*
*																*
*	int			:	0 = OK  1=ERROR								*
*	struct MBUS	:	Estructura de datos MBUS					*
*	uint8_t		:	Dirección donde se guarda el dato			*
*	uint16_t	:	Dato a alamacenar							*
*																*
****************************************************************/
uint16_t ModBUS_F03_Read(struct MBUS *, uint8_t );

uint16_t ModBUS_F04_Read(struct MBUS *, uint8_t );

ModBUS_F03_Request(struct MBUS *m2, uint16_t , uint8_t);
/****************************************************************
*	Genera trama modbus a enviar 						  		*
*																*
*	ModBUS_F03_Request(struct MBUS *m2, uint16_t , uint8_t);	*
*																*
*	 															*
*	struct MBUS	:	Estructura de datos MBUS					*
*	uint16_t	:	Dirección donde de incio					*
*	uint8_t	:	cantidad de datos 								*
*																*
****************************************************************/
