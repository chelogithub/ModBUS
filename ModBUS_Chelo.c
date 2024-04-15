/*
 * ModBUS_Chelo.c
 *
 *  Created on: 22 may. 2021
 *      Author: Marcelo
 */
#include "ModBUS_Chelo.h"

void ModBUS_Config(struct MBUS *m)
{
	m->_address=1;							//Canal  o direccion ModBUS

	m->_mode=0;								//Defino modo de trabajo, cliente o servidor.
	m->_timeout=500;						//Timeout para la recepción de respuesta.

	m->_01_ReadCoils_EN=1;					//Habilito Funcion 1
	m->_01_ReadCoils_QTY=32;				//Defino cantidad de Coils disponibles en la funcion 1
	m->_Coils=0xF0F0AF5A;

	m->_02_ReadDiscreteInputs_EN=1;			//Habilito la funcion 2
	m->_02_ReadDiscreteInputs_QTY=32;		//Defino cantidad de Discrete Inputs disponibles en la funcion 2
	m->_Inputs=0xA5FA0F0F;

	m->_03_ReadHoldingRegisters_EN=1;		//Habilito la funcion 3
	m->_03_ReadHoldingRegisters_QTY=16;		//Defino cantidad de registros disponibles en la funcion 3

	m->_04_ReadInputRegisters_EN=1;			//Habilito la funcion 4
	m->_04_ReadInputRegisters_QTY=16;		//Defino cantidad de registros disponibles en la funcion 4

	m->_05_WriteSingleCoil_EN=1;			//Habilito la funcion 5
	m->_05_WriteSingleCoil_QTY=32;			//Defino cantidad de registros disponibles en la funcion 5

	m->_06_WriteSingleRegister_EN=1;		//Habilito la funcion 06

	m->_15_WriteMultipleCoils_EN=1;			//Habilito la funcion 15
	m->_15_WriteMultipleCoils_QTY=32;		//Defino cantidad de registros disponibles en la funcion 5

	m->_16_WriteMultipleRegisters_EN=1;		//Habilito la funcion 16
	m->_16_WriteMultipleRegisters_QTY=32;	//Defino cantidad de registros disponibles en la funcion 5
}

int ModBUS_Check(char *a, int b)
{
	//Identifico protocolo ModBus con a[2]+a[3]==0 y coherencia en los datos con a[5]+6==b
	if((a[5]+6==b)&&(a[2]+a[3]==0))
	{
		return(1);
	}
	else
	{
		return(0);
	}
}

int ModBUS_Check_tid(struct MBUS *m2)
{

	if( (m2->_MBUS_2SND[0] == m2->_MBUS_RCVD[0])&&
		(m2->_MBUS_2SND[1] == m2->_MBUS_RCVD[1])&&
		(m2->_MBUS_2SND[2] == m2->_MBUS_RCVD[2])&&
		(m2->_MBUS_2SND[3] == m2->_MBUS_RCVD[3])&&
		(m2->_MBUS_2SND[7] == m2->_MBUS_RCVD[7]))
	{
		return(1);
	}
	else
	{

		return(0);
	}
}

int ModBUS_F03_Assign(struct MBUS *m2, uint8_t address, uint16_t data)
{

	uint8_t c=0;
	//	Analizo que el rango de direcciones sea el correcto
	if( (address) > m2->_03_ReadHoldingRegisters_QTY)
	{
		return(1);
	}
	else
	{
		m2->_Holding_Registers[(address)+1]=data;
		c=data>>8;
		m2->_Holding_Registers[(address)]=c;

		return(0);
	}
}

int ModBUS_F04_Assign(struct MBUS *m2, uint8_t address, uint16_t data)
{
	uint8_t c=0;
	//	Analizo que el rango de direcciones sea el correcto
	if( (address) > m2->_04_ReadInputRegisters_QTY)
	{
		return(1);
	}
	else
	{
		m2->_InputRegisters[(address)+1]=data;
		c=data>>8;
		m2->_InputRegisters[(address)]=c;

		return(0);
	}
}

uint16_t ModBUS_F03_Read(struct MBUS *m2, uint8_t address)
{
	uint16_t n=0;
	uint8_t c=0;
	//	Analizo que el rango de direcciones sea el correcto
	if( (address) > m2->_03_ReadHoldingRegisters_QTY)
	{
		return(1);
	}
	else
	{
		n=m2->_Holding_Registers[(address*2)];
		n=n<<8;
		n|=m2->_Holding_Registers[(address*2)+1];
		return(n);
	}
}

uint16_t ModBUS_F04_Read(struct MBUS *m2, uint8_t address)
{
	uint16_t n=0;
	uint8_t c=0;
	//	Analizo que el rango de direcciones sea el correcto
	if( (address) > m2->_03_ReadHoldingRegisters_QTY)
	{
		return(1);
	}
	else
	{
		n=m2->_Holding_Registers[(address*2)];
		n=n<<8;
		n|=m2->_Holding_Registers[(address*2)+1];
		return(n);
	}
}

ModBUS_F03_Request(struct MBUS *m2, uint16_t addr , uint8_t qty)
{
	// Se deberia asignar variable inremental para el transaction id
	m2->_MBUS_2SND[9]= addr && 0x0F;
	addr&=0xF0;
	addr=addr>>8;
	addr&=0x0F;

	m2->_MBUS_2SND[0]=0x00;
	m2->_MBUS_2SND[1]=0x00;
	m2->_MBUS_2SND[2]=0x00;
	m2->_MBUS_2SND[3]=0x00;
	m2->_MBUS_2SND[4]=0x00;
	m2->_MBUS_2SND[5]=0x06;
	m2->_MBUS_2SND[6]=m2->_address;
	m2->_MBUS_2SND[7]=0x03;
	m2->_MBUS_2SND[8]=addr;

	m2->_MBUS_2SND[10]=0x00;
	m2->_MBUS_2SND[11]=qty;
	m2->_n_MBUS_2SND=12;			//Conteo de bytes a transmitir

}

int ModBUS(struct MBUS *m3)
{

	if(m3->_mode == SERVIDOR)
	{
		if (ModBUS_Check(m3->_MBUS_RCVD,m3->_n_MBUS_RCVD))//if (m3->_n_MBUS_RCVD>=10)  //Garantizo longitud minima de telegram MBUS
		{
			switch(m3->_MBUS_RCVD[7])
			{
				case 0x01:	//Funcion 01 ReadCoils
				{

					//-----------------------------------------------------//
					// Los Read Coils solo pueden ser leídos por la        //
					// Funcion 01										   //
					//-----------------------------------------------------//

					//-----------------------------------------------------//
					//		Para todos los casos se devuelve el mismo	   //
					//		encabezado de respuesta						   //
					//-----------------------------------------------------//
					m3->_MBUS_2SND[4]=m3->_MBUS_RCVD[4];
					m3->_MBUS_2SND[3]=m3->_MBUS_RCVD[3];
					m3->_MBUS_2SND[2]=m3->_MBUS_RCVD[2];
					m3->_MBUS_2SND[1]=m3->_MBUS_RCVD[1];
					m3->_MBUS_2SND[0]=m3->_MBUS_RCVD[0];

					int qty_reg=0;
					if  ( m3->_01_ReadCoils_EN == 1 ) //Debe estar habilitado el código de funcion
					{


							m3->_01_ReadCoils_StrtAddr = m3->_MBUS_RCVD[8];
							m3->_01_ReadCoils_StrtAddr = m3->_01_ReadCoils_StrtAddr<<8;
							m3->_01_ReadCoils_StrtAddr &= 0xFF00;
							m3->_01_ReadCoils_StrtAddr |= m3->_MBUS_RCVD[9];

							m3->_01_ReadCoils_QTYReg = m3->_MBUS_RCVD[10];
							m3->_01_ReadCoils_QTYReg = m3->_01_ReadCoils_QTYReg<<8;
							m3->_01_ReadCoils_QTYReg &= 0xFF00;
							m3->_01_ReadCoils_QTYReg |= m3->_MBUS_RCVD[11];



							if ( m3->_01_ReadCoils_QTYReg <=  m3->_01_ReadCoils_QTY) //Cdad pedida menor a igual que disponible
							{
								if ( (m3->_01_ReadCoils_StrtAddr + m3->_01_ReadCoils_QTYReg) <=  m3->_01_ReadCoils_QTY) //Dirección inicial + cantidad menor a igual que disponible
								{
								int div=0, resto=0, bytes=0,pos=0;
								uint32_t coils=0;
									// Verificado el mensaje, se procesa.

									coils = (m3->_Coils >> m3->_01_ReadCoils_StrtAddr);
									//Debo determinar la cantidad de bytes a enviar

									div=m3->_01_ReadCoils_QTYReg/8;
									resto=m3->_01_ReadCoils_QTYReg%8;

									if(resto==0)
									{
										bytes=div;
									}
									else
									{
										bytes=div+1;
									}

									m3->_MBUS_2SND[8]=bytes;


									while(bytes > 0)
									{
										m3->_MBUS_2SND[8+bytes]=coils;
										coils=coils>>8;
										bytes--;
									}


									m3->_MBUS_2SND[7]=0x01;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=m3->_MBUS_2SND[8]+3; //Cdad de datos

									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);

								}
								else
								{
									m3->_MBUS_2SND[8]=0x02;
									m3->_MBUS_2SND[7]=0x81;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=0x03; //Cdad de datos

									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
									return(2);//Error excepcion 02
								}

							}
							else
							{
								m3->_MBUS_2SND[8]=0x03;
								m3->_MBUS_2SND[7]=0x81;
								m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
								m3->_MBUS_2SND[5]=0x03; //Cdad de datos

								m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
								return(3);//Erroe excepcion 03
							}


					}
					else
					{
						m3->_MBUS_2SND[8]=0x01;
						m3->_MBUS_2SND[7]=0x81;
						m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
						m3->_MBUS_2SND[5]=0x03; //Cdad de datos

						m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
						return(1);//Error excepcion 01
					}
				}
				break;
				case 0x02:	//Funcion 01 ReadCoils
				{

					//-----------------------------------------------------//
					// Los Read Coils solo pueden ser leídos por la        //
					// Funcion 01										   //
					//-----------------------------------------------------//

					//-----------------------------------------------------//
					//		Para todos los casos se devuelve el mismo	   //
					//		encabezado de respuesta						   //
					//-----------------------------------------------------//
					m3->_MBUS_2SND[4]=m3->_MBUS_RCVD[4];
					m3->_MBUS_2SND[3]=m3->_MBUS_RCVD[3];
					m3->_MBUS_2SND[2]=m3->_MBUS_RCVD[2];
					m3->_MBUS_2SND[1]=m3->_MBUS_RCVD[1];
					m3->_MBUS_2SND[0]=m3->_MBUS_RCVD[0];

					int qty_reg=0;
					if  ( m3->_02_ReadDiscreteInputs_EN == 1 ) //Debe estar habilitado el código de funcion
					{


							m3->_02_ReadDiscreteInputs_StrtAddr = m3->_MBUS_RCVD[8];
							m3->_02_ReadDiscreteInputs_StrtAddr = m3->_02_ReadDiscreteInputs_StrtAddr<<8;
							m3->_02_ReadDiscreteInputs_StrtAddr &= 0xFF00;
							m3->_02_ReadDiscreteInputs_StrtAddr |= m3->_MBUS_RCVD[9];

							m3->_02_ReadDiscreteInputs_QTYReg = m3->_MBUS_RCVD[10];
							m3->_02_ReadDiscreteInputs_QTYReg = m3->_02_ReadDiscreteInputs_QTYReg<<8;
							m3->_02_ReadDiscreteInputs_QTYReg &= 0xFF00;
							m3->_02_ReadDiscreteInputs_QTYReg |= m3->_MBUS_RCVD[11];



							if ( m3->_02_ReadDiscreteInputs_QTYReg <=  m3->_02_ReadDiscreteInputs_QTY) //Cdad pedida menor a igual que disponible
							{
								if ( (m3->_02_ReadDiscreteInputs_StrtAddr + m3->_02_ReadDiscreteInputs_QTYReg) <=  m3->_02_ReadDiscreteInputs_QTY) //Dirección inicial + cantidad menor a igual que disponible
								{
								int div=0, resto=0, bytes=0,pos=0;
								uint32_t coils=0;
									// Verificado el mensaje, se procesa.

									coils = (m3->_Inputs >> m3->_02_ReadDiscreteInputs_StrtAddr);
									//Debo determinar la cantidad de bytes a enviar

									div=m3->_02_ReadDiscreteInputs_QTYReg/8;
									resto=m3->_02_ReadDiscreteInputs_QTYReg%8;

									if(resto==0)
									{
										bytes=div;
									}
									else
									{
										bytes=div+1;
									}

									m3->_MBUS_2SND[8]=bytes;


									while(bytes > 0)
									{
										m3->_MBUS_2SND[8+bytes]=coils;
										coils=coils>>8;
										bytes--;
									}


									m3->_MBUS_2SND[7]=0x02;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=m3->_MBUS_2SND[8]+3; //Cdad de datos

									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);

								}
								else
								{
									m3->_MBUS_2SND[8]=0x02;
									m3->_MBUS_2SND[7]=0x82;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=0x03; //Cdad de datos

									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
									return(2);//Error excepcion 02
								}

							}
							else
							{
								m3->_MBUS_2SND[8]=0x03;
								m3->_MBUS_2SND[7]=0x82;
								m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
								m3->_MBUS_2SND[5]=0x03; //Cdad de datos

								m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
								return(3);//Erroe excepcion 03
							}


					}
					else
					{
						m3->_MBUS_2SND[8]=0x01;
						m3->_MBUS_2SND[7]=0x82;
						m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
						m3->_MBUS_2SND[5]=0x03; //Cdad de datos

						m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
						return(1);//Error excepcion 01
					}
				}
				break;
				case 0x03:	//Funcion 03 ReadHoldingRegisters
				{

					//-----------------------------------------------------//
					// Los Holding Registers pueden ser escritos o leidos  //
					// vía ModBUS pues es info que puede ser alterada por  //
					// una aplicación.									   //
					//-----------------------------------------------------//

					//-----------------------------------------------------//
					//		Para todos los casos se devuelve el mismo	   //
					//		encabezado de respuesta						   //
					//-----------------------------------------------------//
					m3->_MBUS_2SND[4]=m3->_MBUS_RCVD[4];
					m3->_MBUS_2SND[3]=m3->_MBUS_RCVD[3];
					m3->_MBUS_2SND[2]=m3->_MBUS_RCVD[2];
					m3->_MBUS_2SND[1]=m3->_MBUS_RCVD[1];
					m3->_MBUS_2SND[0]=m3->_MBUS_RCVD[0];

					int qty_reg=0;
					if  ( m3->_03_ReadHoldingRegisters_EN == 1 ) //Debe estar habilitado el código de funcion
					{


							m3->_03_ReadHoldingRegisters_StrtAddr = m3->_MBUS_RCVD[8];
							m3->_03_ReadHoldingRegisters_StrtAddr = m3->_03_ReadHoldingRegisters_StrtAddr<<8;
							m3->_03_ReadHoldingRegisters_StrtAddr &= 0xFF00;
							m3->_03_ReadHoldingRegisters_StrtAddr |= m3->_MBUS_RCVD[9];

							m3->_03_ReadHoldingRegisters_QTYReg = m3->_MBUS_RCVD[10];
							m3->_03_ReadHoldingRegisters_QTYReg = m3->_03_ReadHoldingRegisters_QTYReg<<8;
							m3->_03_ReadHoldingRegisters_QTYReg &= 0xFF00;
							m3->_03_ReadHoldingRegisters_QTYReg |= m3->_MBUS_RCVD[11];



							if ( m3->_03_ReadHoldingRegisters_QTYReg <=  m3->_03_ReadHoldingRegisters_QTY) //Cdad pedida menor a igual que disponible
							{
								if ( (m3->_03_ReadHoldingRegisters_StrtAddr + m3->_03_ReadHoldingRegisters_QTYReg) <=  m3->_03_ReadHoldingRegisters_QTY) //Dirección inicial + cantidad menor a igual que disponible
								{
								// Verificado el mensaje, se procesa.

									while(qty_reg < m3->_03_ReadHoldingRegisters_QTYReg)
									{

									 m3->_MBUS_2SND[2*qty_reg + 9]=m3->_Holding_Registers[m3->_03_ReadHoldingRegisters_StrtAddr*2 + 2*qty_reg];
									 m3->_MBUS_2SND[2*qty_reg + 10]=m3->_Holding_Registers[m3->_03_ReadHoldingRegisters_StrtAddr*2 + 2*qty_reg+1];

										qty_reg++; //Incremento la cantidad de registros

									}

									m3->_MBUS_2SND[8]=m3->_03_ReadHoldingRegisters_QTYReg*2;
									m3->_MBUS_2SND[7]=0x03;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=m3->_MBUS_2SND[8]+3; //Cdad de datos


									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);

								}
								else
								{
									m3->_MBUS_2SND[8]=0x02;
									m3->_MBUS_2SND[7]=0x83;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=0x03; //Cdad de datos

									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
									return(2);//Error excepcion 02
								}

							}
							else
							{
								m3->_MBUS_2SND[8]=0x03;
								m3->_MBUS_2SND[7]=0x83;
								m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
								m3->_MBUS_2SND[5]=0x03; //Cdad de datos

								m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
								return(3);//Erroe excepcion 03
							}


					}
					else
					{
						m3->_MBUS_2SND[8]=0x01;
						m3->_MBUS_2SND[7]=0x83;
						m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
						m3->_MBUS_2SND[5]=0x03; //Cdad de datos

						m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
						return(1);//Error excepcion 01
					}
				}
				break;
				case 0x04:	//Funcion 04 ReadInputRegisters
				{

					//-----------------------------------------------------//
					// Los Input Registers solo pueden ser leidos dado que //
					// generalmente provienen de algun dispositivo I/O     //
					//-----------------------------------------------------//

					//-----------------------------------------------------//
					//		Para todos los casos se devuelve el mismo	   //
					//		encabezado de respuesta						   //
					//-----------------------------------------------------//
					m3->_MBUS_2SND[4]=m3->_MBUS_RCVD[4];
					m3->_MBUS_2SND[3]=m3->_MBUS_RCVD[3];
					m3->_MBUS_2SND[2]=m3->_MBUS_RCVD[2];
					m3->_MBUS_2SND[1]=m3->_MBUS_RCVD[1];
					m3->_MBUS_2SND[0]=m3->_MBUS_RCVD[0];

					int qty_reg=0;
					if  ( m3->_04_ReadInputRegisters_EN == 1 ) //Debe estar habilitado el código de funcion
					{


							m3->_04_ReadInputRegisters_StrtAddr = m3->_MBUS_RCVD[8];
							m3->_04_ReadInputRegisters_StrtAddr = m3->_04_ReadInputRegisters_StrtAddr<<8;
							m3->_04_ReadInputRegisters_StrtAddr &= 0xFF00;
							m3->_04_ReadInputRegisters_StrtAddr |= m3->_MBUS_RCVD[9];

							m3->_04_ReadInputRegisters_QTYReg = m3->_MBUS_RCVD[10];
							m3->_04_ReadInputRegisters_QTYReg = m3->_04_ReadInputRegisters_QTYReg<<8;
							m3->_04_ReadInputRegisters_QTYReg &= 0xFF00;
							m3->_04_ReadInputRegisters_QTYReg |= m3->_MBUS_RCVD[11];



							if ( m3->_04_ReadInputRegisters_QTYReg <=  m3->_04_ReadInputRegisters_QTY) //Cdad pedida menor a igual que disponible
							{
								if ( (m3->_04_ReadInputRegisters_StrtAddr + m3->_04_ReadInputRegisters_QTYReg) <=  m3->_04_ReadInputRegisters_QTY) //Dirección inicial + cantidad menor a igual que disponible
								{
								// Verificado el mensaje, se procesa.

									while(qty_reg < m3->_04_ReadInputRegisters_QTYReg)
									{

									 m3->_MBUS_2SND[2*qty_reg + 9]=m3->_InputRegisters[m3->_04_ReadInputRegisters_StrtAddr*2 + 2*qty_reg];
									 m3->_MBUS_2SND[2*qty_reg + 10]=m3->_InputRegisters[m3->_04_ReadInputRegisters_StrtAddr*2 + 2*qty_reg+1];

										qty_reg++; //Incremento la cantidad de registros

									}

									m3->_MBUS_2SND[8]=m3->_04_ReadInputRegisters_QTYReg*2;
									m3->_MBUS_2SND[7]=0x04;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=m3->_MBUS_2SND[8]+3; //Cdad de datos


									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);

								}
								else
								{
									m3->_MBUS_2SND[8]=0x02;
									m3->_MBUS_2SND[7]=0x84;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=0x03; //Cdad de datos

									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
									return(2);//Error excepcion 02
								}

							}
							else
							{
								m3->_MBUS_2SND[8]=0x03;
								m3->_MBUS_2SND[7]=0x84;
								m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
								m3->_MBUS_2SND[5]=0x03; //Cdad de datos

								m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
								return(3);//Erroe excepcion 03
							}


					}
					else
					{
						m3->_MBUS_2SND[8]=0x01;
						m3->_MBUS_2SND[7]=0x84;
						m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
						m3->_MBUS_2SND[5]=0x03; //Cdad de datos

						m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
						return(1);//Error excepcion 01
					}
				}
				break;
				case 0x05:	//Funcion 03 ReadHoldingRegisters
							{

								//-----------------------------------------------------//
								// Los Holding Registers pueden ser escritos o leidos  //
								// vía ModBUS pues es info que puede ser alterada por  //
								// una aplicación.									   //
								//-----------------------------------------------------//

								//-----------------------------------------------------//
								//		Para todos los casos se devuelve el mismo	   //
								//		encabezado de respuesta						   //
								//-----------------------------------------------------//
								m3->_MBUS_2SND[4]=m3->_MBUS_RCVD[4];
								m3->_MBUS_2SND[3]=m3->_MBUS_RCVD[3];
								m3->_MBUS_2SND[2]=m3->_MBUS_RCVD[2];
								m3->_MBUS_2SND[1]=m3->_MBUS_RCVD[1];
								m3->_MBUS_2SND[0]=m3->_MBUS_RCVD[0];

								int qty_reg=0;
								if  ( m3->_05_WriteSingleCoil_EN == 1 ) //Debe estar habilitado el código de funcion
								{


										m3->_05_WriteSingleCoil_StrtAddr = m3->_MBUS_RCVD[8];
										m3->_05_WriteSingleCoil_StrtAddr = m3->_05_WriteSingleCoil_StrtAddr<<8;
										m3->_05_WriteSingleCoil_StrtAddr &= 0xFF00;
										m3->_05_WriteSingleCoil_StrtAddr |= m3->_MBUS_RCVD[9];

										m3->_05_WriteSingleCoil_State = m3->_MBUS_RCVD[10];
										m3->_05_WriteSingleCoil_State = m3->_05_WriteSingleCoil_State<<8;
										m3->_05_WriteSingleCoil_State &= 0xFF00;
										m3->_05_WriteSingleCoil_State |= m3->_MBUS_RCVD[11];



										if ( m3->_05_WriteSingleCoil_StrtAddr <=  (m3->_05_WriteSingleCoil_QTY-1)) //Cdad pedida menor a igual que disponible
										{
											if ( (m3->_05_WriteSingleCoil_State == 0x0000)||(m3->_05_WriteSingleCoil_State == 0xFF00)) //Dirección inicial + cantidad menor a igual que disponible
											{
											// Verificado el mensaje, se procesa.

												if  (m3->_05_WriteSingleCoil_State == 0xFF00)
												{
													m3->_Coils |= 1 << m3->_05_WriteSingleCoil_StrtAddr;
												}
												else
												{
													m3->_Coils &= 0 << m3->_05_WriteSingleCoil_StrtAddr;
												}

												m3->_MBUS_2SND[11]=m3->_MBUS_RCVD[11];
												m3->_MBUS_2SND[10]=m3->_MBUS_RCVD[10];
												m3->_MBUS_2SND[9]=m3->_MBUS_RCVD[9];
												m3->_MBUS_2SND[8]=m3->_MBUS_RCVD[8];
												m3->_MBUS_2SND[7]=0x05;
												m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
												m3->_MBUS_2SND[5]=m3->_MBUS_2SND[8]+6; //Cdad de datos


												m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);

											}
											else
											{
												m3->_MBUS_2SND[8]=0x02;
												m3->_MBUS_2SND[7]=0x85;
												m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
												m3->_MBUS_2SND[5]=0x03; //Cdad de datos

												m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
												return(2);//Error excepcion 02
											}

										}
										else
										{
											m3->_MBUS_2SND[8]=0x03;
											m3->_MBUS_2SND[7]=0x85;
											m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
											m3->_MBUS_2SND[5]=0x03; //Cdad de datos

											m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
											return(3);//Erroe excepcion 03
										}


								}
								else
								{
									m3->_MBUS_2SND[8]=0x01;
									m3->_MBUS_2SND[7]=0x85;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=0x03; //Cdad de datos

									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
									return(1);//Error excepcion 01
								}
							}
				break;
				case 0x06:	//Funcion 06 WriteSingleRegisters
				{


					//-----------------------------------------------------//
					//		Para todos los casos se devuelve el mismo	   //
					//		encabezado de respuesta						   //
					//-----------------------------------------------------//
					m3->_MBUS_2SND[4]=m3->_MBUS_RCVD[4];
					m3->_MBUS_2SND[3]=m3->_MBUS_RCVD[3];
					m3->_MBUS_2SND[2]=m3->_MBUS_RCVD[2];
					m3->_MBUS_2SND[1]=m3->_MBUS_RCVD[1];
					m3->_MBUS_2SND[0]=m3->_MBUS_RCVD[0];

					uint16_t qty_reg=0, start_address=0;
					if  ( m3->_06_WriteSingleRegister_EN == 1 ) //Debe estar habilitado el código de funcion
					{


							m3->_06_WriteSingleRegister_Addr = m3->_MBUS_RCVD[8];
							m3->_06_WriteSingleRegister_Addr = m3->_06_WriteSingleRegister_Addr<<8;
							m3->_06_WriteSingleRegister_Addr &= 0xFF00;
							m3->_06_WriteSingleRegister_Addr |= m3->_MBUS_RCVD[9];

							start_address=m3->_06_WriteSingleRegister_Addr;

							m3->_06_WriteSingleRegister_Value = m3->_MBUS_RCVD[10];
							m3->_06_WriteSingleRegister_Value = m3->_06_WriteSingleRegister_Value<<8;
							m3->_06_WriteSingleRegister_Value &= 0xFF00;
							m3->_06_WriteSingleRegister_Value |= m3->_MBUS_RCVD[11];



							if ((m3->_06_WriteSingleRegister_Value <=  0xFFFF)&&(m3->_06_WriteSingleRegister_Value >=  0x0000)) //Cdad pedida menor a igual que disponible
							{
								if ( m3->_06_WriteSingleRegister_Addr  <=  m3->_06_WriteSingleRegister_QTY) //Dirección inicial + cantidad menor a igual que disponible
								{
								// Verificado el mensaje, se procesa.

										m3->_Holding_Registers[start_address] = m3->_MBUS_RCVD[10];
										m3->_Holding_Registers[start_address + 1] = m3->_MBUS_RCVD[11];


									m3->_MBUS_2SND[11]=m3->_MBUS_RCVD[11];
									m3->_MBUS_2SND[10]=m3->_MBUS_RCVD[10];
									m3->_MBUS_2SND[9]=m3->_MBUS_RCVD[9];
									m3->_MBUS_2SND[8]=m3->_MBUS_RCVD[8];
									m3->_MBUS_2SND[7]=0x06;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=0x06; //Cdad de datos


									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);

								}
								else
								{
									m3->_MBUS_2SND[8]=0x02;
									m3->_MBUS_2SND[7]=0x86;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=0x03; //Cdad de datos

									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
									return(2);//Error excepcion 02
								}

							}
							else
							{
								m3->_MBUS_2SND[8]=0x03;
								m3->_MBUS_2SND[7]=0x86;
								m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
								m3->_MBUS_2SND[5]=0x03; //Cdad de datos

								m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
								return(3);//Erroe excepcion 03
							}


					}
					else
					{
						m3->_MBUS_2SND[8]=0x01;
						m3->_MBUS_2SND[7]=0x86;
						m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
						m3->_MBUS_2SND[5]=0x03; //Cdad de datos

						m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
						return(1);//Error excepcion 01
					}
				}
				break;

				case 0x0F:	//Funcion 15 Write Multiple Coils
				{

					//-----------------------------------------------------//
					// Los Input Registers solo pueden ser leidos dado que //
					// generalmente provienen de algun dispositivo I/O     //
					//-----------------------------------------------------//

					//-----------------------------------------------------//
					//		Para todos los casos se devuelve el mismo	   //
					//		encabezado de respuesta						   //
					//-----------------------------------------------------//
					m3->_MBUS_2SND[4]=m3->_MBUS_RCVD[4];
					m3->_MBUS_2SND[3]=m3->_MBUS_RCVD[3];
					m3->_MBUS_2SND[2]=m3->_MBUS_RCVD[2];
					m3->_MBUS_2SND[1]=m3->_MBUS_RCVD[1];
					m3->_MBUS_2SND[0]=m3->_MBUS_RCVD[0];

					int qty_reg=0;
					if  ( m3->_15_WriteMultipleCoils_EN == 1 ) //Debe estar habilitado el código de funcion
					{


							m3->_15_WriteMultipleCoils_StrtAddr = m3->_MBUS_RCVD[8];
							m3->_15_WriteMultipleCoils_StrtAddr = m3->_15_WriteMultipleCoils_StrtAddr<<8;
							m3->_15_WriteMultipleCoils_StrtAddr &= 0xFF00;
							m3->_15_WriteMultipleCoils_StrtAddr |= m3->_MBUS_RCVD[9];

							m3->_15_WriteMultipleCoils_QTYReg = m3->_MBUS_RCVD[10];
							m3->_15_WriteMultipleCoils_QTYReg = m3->_15_WriteMultipleCoils_QTYReg<<8;
							m3->_15_WriteMultipleCoils_QTYReg &= 0xFF00;
							m3->_15_WriteMultipleCoils_QTYReg |= m3->_MBUS_RCVD[11];



							if ( m3->_15_WriteMultipleCoils_QTYReg <=  m3->_15_WriteMultipleCoils_QTY) //Cdad pedida menor a igual que disponible
							{
								if ( (m3->_15_WriteMultipleCoils_StrtAddr + m3->_15_WriteMultipleCoils_QTYReg) <=  m3->_15_WriteMultipleCoils_QTY) //Dirección inicial + cantidad menor a igual que disponible
								{
								// Verificado el mensaje, se procesa.
								uint32_t data=0x00000000,cdad_bytes=0,cdad_datos=0, mascara=0x00000000;

									cdad_bytes=m3->_MBUS_RCVD[12];


									while(cdad_bytes > 0)
									{
										data=data<<8;
										data|=m3->_MBUS_RCVD[12+cdad_bytes];
										cdad_bytes--;
									}
									data=data << m3->_15_WriteMultipleCoils_StrtAddr; //Dejo los datos listo para la OR de Transferencia

									while (cdad_datos < m3->_15_WriteMultipleCoils_QTYReg)
									{
										mascara |= (1 << (m3->_15_WriteMultipleCoils_StrtAddr + cdad_datos));
										cdad_datos++;
									}

									mascara = ~ mascara;

									m3->_Coils &= mascara;


									data &= ~mascara;

									m3->_Coils |= data;

									m3->_MBUS_2SND[11]=m3->_MBUS_RCVD[11];
									m3->_MBUS_2SND[10]=m3->_MBUS_RCVD[10];
									m3->_MBUS_2SND[9]=m3->_MBUS_RCVD[9];
									m3->_MBUS_2SND[8]=m3->_MBUS_RCVD[8];
									m3->_MBUS_2SND[7]=0x0F;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=m3->_MBUS_2SND[8]+6; //Cdad de datos


									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);

								}
								else
								{
									m3->_MBUS_2SND[8]=0x02;
									m3->_MBUS_2SND[7]=0x8F;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=0x03; //Cdad de datos

									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
									return(2);//Error excepcion 02
								}

							}
							else
							{
								m3->_MBUS_2SND[8]=0x03;
								m3->_MBUS_2SND[7]=0x8F;
								m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
								m3->_MBUS_2SND[5]=0x03; //Cdad de datos

								m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
								return(3);//Erroe excepcion 03
							}


					}
					else
					{
						m3->_MBUS_2SND[8]=0x01;
						m3->_MBUS_2SND[7]=0x8F;
						m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
						m3->_MBUS_2SND[5]=0x03; //Cdad de datos

						m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
						return(1);//Error excepcion 01
					}
				}
				break;
				case 0x10:	//Funcion 16 WriteMultipleRegisters
				{


					//-----------------------------------------------------//
					//		Para todos los casos se devuelve el mismo	   //
					//		encabezado de respuesta						   //
					//-----------------------------------------------------//
					m3->_MBUS_2SND[4]=m3->_MBUS_RCVD[4];
					m3->_MBUS_2SND[3]=m3->_MBUS_RCVD[3];
					m3->_MBUS_2SND[2]=m3->_MBUS_RCVD[2];
					m3->_MBUS_2SND[1]=m3->_MBUS_RCVD[1];
					m3->_MBUS_2SND[0]=m3->_MBUS_RCVD[0];

					uint16_t qty_reg=0, start_address=0;
					if  ( m3->_16_WriteMultipleRegisters_EN == 1 ) //Debe estar habilitado el código de funcion
					{


							m3->_16_WriteMultipleRegisters_StrtAddr = m3->_MBUS_RCVD[8];
							m3->_16_WriteMultipleRegisters_StrtAddr = m3->_16_WriteMultipleRegisters_StrtAddr<<8;
							m3->_16_WriteMultipleRegisters_StrtAddr &= 0xFF00;
							m3->_16_WriteMultipleRegisters_StrtAddr |= m3->_MBUS_RCVD[9];

							start_address= (2 * m3->_16_WriteMultipleRegisters_StrtAddr);

							m3->_16_WriteMultipleRegisters_QTYReg = m3->_MBUS_RCVD[10];
							m3->_16_WriteMultipleRegisters_QTYReg = m3->_16_WriteMultipleRegisters_QTYReg<<8;
							m3->_16_WriteMultipleRegisters_QTYReg &= 0xFF00;
							m3->_16_WriteMultipleRegisters_QTYReg |= m3->_MBUS_RCVD[11];



							if ( m3->_16_WriteMultipleRegisters_QTYReg <=  m3->_16_WriteMultipleRegisters_QTY) //Cdad pedida menor a igual que disponible
							{
								if ( (m3->_16_WriteMultipleRegisters_StrtAddr + m3->_16_WriteMultipleRegisters_QTYReg) <=  m3->_16_WriteMultipleRegisters_QTY) //Dirección inicial + cantidad menor a igual que disponible
								{
								// Verificado el mensaje, se procesa.

									while(qty_reg < m3->_MBUS_RCVD[12])
									{
										m3->_Holding_Registers[start_address + qty_reg] = m3->_MBUS_RCVD[13 + qty_reg];
										//Esto era para vectores de UINT16_T
										//m3->_Holding_Registers[start_address + qty_reg] = m3->_Holding_Registers[start_address + qty_reg]<<8;
										//m3->_Holding_Registers[start_address + qty_reg] &= 0xFF00;
										qty_reg++;
										m3->_Holding_Registers[start_address + qty_reg] = m3->_MBUS_RCVD[13 + qty_reg];
										qty_reg++;

									}
									m3->_MBUS_2SND[11]=m3->_MBUS_RCVD[11];
									m3->_MBUS_2SND[10]=m3->_MBUS_RCVD[10];
									m3->_MBUS_2SND[9]=m3->_MBUS_RCVD[9];
									m3->_MBUS_2SND[8]=m3->_MBUS_RCVD[8];
									m3->_MBUS_2SND[7]=0x10;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=0x06; //Cdad de datos


									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);

								}
								else
								{
									m3->_MBUS_2SND[8]=0x02;
									m3->_MBUS_2SND[7]=0x90;
									m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
									m3->_MBUS_2SND[5]=0x03; //Cdad de datos

									m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
									return(2);//Error excepcion 02
								}

							}
							else
							{
								m3->_MBUS_2SND[8]=0x03;
								m3->_MBUS_2SND[7]=0x90;
								m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
								m3->_MBUS_2SND[5]=0x03; //Cdad de datos

								m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
								return(3);//Erroe excepcion 03
							}


					}
					else
					{
						m3->_MBUS_2SND[8]=0x01;
						m3->_MBUS_2SND[7]=0x90;
						m3->_MBUS_2SND[6]=0x01; //En teoría es el canal
						m3->_MBUS_2SND[5]=0x03; //Cdad de datos

						m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
						return(1);//Error excepcion 01
					}
				}
				break;
				default:
				{	// Comando no soportado
					//-----------------------------------------------------//
					//		Para todos los casos se devuelve el mismo	   //
					//		encabezado de respuesta						   //
					//-----------------------------------------------------//
					m3->_MBUS_2SND[8]=0x01;
					m3->_MBUS_2SND[7]=m3->_MBUS_RCVD[7] + 0x80;
					m3->_MBUS_2SND[6]=m3->_MBUS_RCVD[6];
					m3->_MBUS_2SND[5]=0x03;
					m3->_MBUS_2SND[4]=m3->_MBUS_RCVD[4];
					m3->_MBUS_2SND[3]=m3->_MBUS_RCVD[3];
					m3->_MBUS_2SND[2]=m3->_MBUS_RCVD[2];
					m3->_MBUS_2SND[1]=m3->_MBUS_RCVD[1];
					m3->_MBUS_2SND[0]=m3->_MBUS_RCVD[0];

					m3->_n_MBUS_2SND=(m3->_MBUS_2SND[5]+6);
					return(1);

				}
				break;
			}


		}
		else
		{
			return(1);
		}
	}
	else if (m3->_mode == CLIENTE)
	{
		if(ModBUS_Check(m3->_MBUS_RCVD,m3->_n_MBUS_RCVD))   //Verifico si el vector recibido es ModBUS
		{
			if(ModBUS_Check_tid(m3))
			{

				switch (m3->_MBUS_RCVD[7])
				{
					case 0x03: // Si el comando fue un ReadHoldingRegisters
					{
						uint16_t start_address = 0;
						uint8_t qty=0;

						start_address = m3->_MBUS_2SND[8];
						start_address = start_address << 8 ;
						start_address |= m3->_MBUS_2SND[9];

						qty = m3->_MBUS_2SND[11];

						for (int i=0;i< qty; i++)
						{
							m3->_Holding_Registers[(start_address*2)+(2*i)]=m3->_MBUS_RCVD[9+2*i];
							m3->_Holding_Registers[(start_address*2)+(2*i) + 1]=m3->_MBUS_RCVD[10+2*i];

						}


					}
					break;
					default :
					{

					}
				}



				int qty = m3->_MBUS_RCVD[5]-m3->_MBUS_RCVD[8];

			}
			else
			{
				switch(m3->_MBUS_RCVD[7])
				{
					case 0x81:
					{
						//return=m3->_MBUS_RCVD[8];
					}
					break;
					case 0x82:
					{
						//return=m3->_MBUS_RCVD[8];
					}
					break;
					case 0x83:
					{
						//return=m3->_MBUS_RCVD[8];
					}
					break;
					case 0x84:
					{
						//return=m3->_MBUS_RCVD[8];
					}
					break;
					case 0x85:
					{
						//return=m3->_MBUS_RCVD[8];
					}
					break;
					case 0x86:
					{
						//return=m3->_MBUS_RCVD[8];
					}
					break;
					case 0x95:
					{
						//return=m3->_MBUS_RCVD[8];
					}
					break;
					case 0x96:
					{
						//return=m3->_MBUS_RCVD[8];
					}
					break;
				}


				//ERROR
			}
		}
	}


}

/**********************************************************
   Verificar_CRC16 Devuelve 1 si CRC OK y 0 si CRC ERROR
   Se requiere Vector y cdad de elementos a calcular CRC
**********************************************************/

uint8_t Verificar_CRC16(unsigned char *ptr, int count)
	{
	unsigned int crc;
    char i;

 	   crc = 0xffff;
 	   while (--count >= 0)
  	  {
  	      crc = crc ^ (unsigned int) *ptr++;
  	      i = 8;
   	    	 do
    	   		 {
    	        	if (crc & 0x0001)
   	           		    crc = (crc >> 1) ^ 0xA001;
  	          			else
                			crc = (crc >> 1);
        		 } while(--i);
    	}
	if (crc==0x0000)
		return(1);
		else
		return(0);
	}

/*************************************************************************
   Agregar_CRC16aTXVect() Agrega los 2 bytes al vector
   Se requiere Vector y cdad de elementos a calcular CRC
   Vector resultante posee 2 posiciones mas debido a las palabras de CRC
*************************************************************************/

void  Agregar_CRC16aTXVect(unsigned char *ptr, int count)
	{
		unsigned int crc;
   		char i;
		int count2=count;

 		   crc = 0xffff;
 		   while (--count >= 0)
  		  {
	  	      crc = crc ^ (unsigned int) *ptr++;
	  	      i = 8;
   	    	 do
    	   	 {
    	        	if (crc & 0x0001)
   	           		    crc = (crc >> 1) ^ 0xA001;
  	          			else
                			crc = (crc >> 1);
        	} while(--i);
    	};
	*ptr++=*&crc;

	while (count2>count)
	{
		count++;
		*ptr--;
	}
	count2++;
		   crc = 0xffff;
 		   while (--count2 >= 0)
  		  {
	  	      crc = crc ^ (unsigned int) *ptr++;
	  	      i = 8;
   	    	 do
    	   	 {
    	        	if (crc & 0x0001)
   	           		    crc = (crc >> 1) ^ 0xA001;
  	          			else
                			crc = (crc >> 1);
        	} while(--i);

			};
	*ptr++=*&crc;
	}
/**********************************************************************************
   uint8_t porcessAnswerModbusRTU(struct MBUS *m3,unsigned char *vect, int items)
   Debo verificar que lo recibido es modBus y cotejar con la configuración si es para
   este dispositivo.
***********************************************************************************/
uint8_t processAnswerModbusRTU(struct MBUS *inst_m485)
{
	if((inst_m485->_MBUS_RCVD[PDU_QTY_BYTE]+5==inst_m485->_n_MBUS_RCVD))
	{
		switch(inst_m485->_MBUS_RCVD[PDU_FCODE])
		{
		case HOLDING_REG:		//Si el código de la función el Holding Registers
			{
				for(int i=PDU_DATA; i<= (PDU_DATA + inst_m485->_MBUS_RCVD[PDU_QTY_BYTE]);i++)
				{
					inst_m485->_Holding_Registers[i-PDU_DATA]= inst_m485->_MBUS_RCVD[i];
				}
			}
		break;
		}

	}

}

void  createCommandRTU(struct MBUS *inst_m485, uint8_t addr , uint8_t comando, uint16_t startAddress, uint16_t qty )
{
	uint16_t num=0;

	inst_m485->_MBUS_2SND[0]=addr;
	inst_m485->_MBUS_2SND[1]=comando;
	num=startAddress;
	inst_m485->_MBUS_2SND[3]=num;
	num=num>>8;
	inst_m485->_MBUS_2SND[2]=num;
	num=qty;
	inst_m485->_MBUS_2SND[5]=num;
	num=num>>8;
	inst_m485->_MBUS_2SND[4]=num;
	Agregar_CRC16aTXVect(inst_m485->_MBUS_2SND,6);
	inst_m485->_n_MBUS_2SND=8;
}
