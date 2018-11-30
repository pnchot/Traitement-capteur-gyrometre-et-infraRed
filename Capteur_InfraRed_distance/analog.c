

/* 
 * File:   main.c
 * Author: Geoffrey and Denis
 *
 * Created on 30 fevrier 2018
 *
 */

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  Includes  //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <24fj128ga202.h>//on importe le module spécifique à notre DSPic
#include <stdio.h>// Importation des fonctions/constantes/macros des opérations d'entree sortie a notre DSPic
#include <stdlib.h>// Importation des fonctions operatoires de bases
#include <math.h>// on importe les fonctions mathématiques

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////  FUSES  ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#fuses ICSP1      // ICD uses PGC1/PGD1 pins
#fuses NOJTAG     // JTAG disabled
#fuses DEBUG      // Debug mode for use with ICD
#fuses NOWDT      // Watchdog timer enabled/disabled by user software
#fuses HS,NOPROTECT
//#fuses NOALTI2C1  // I2C1 mapped to SDA1/SCL1 pins

////////////////////////////////////////////////////////////////////////////////
///////////////////////////  DEFINE  ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX(x,y)  ((x > y) ? x : y) //on définit les macros min et max
#define MIN(x,y)  ((x < y) ? x : y)



////////////////////////////////////////////////////////////////////////////////
///////////////////////////  DEVICE  ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// A verifier si cela est utile et que ça marche bien.
#device ADC=10

////////////////////////////////////////////////////////////////////////////////
///////////////////////////  USE  //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#use delay(clock=10000000)//pour pouvoir manipuler le temps

/*Definition des differents I2C avec les conventions suivantes :
  - le bus0 est le bus du Pi
  - le pin1 est l'adresse du capteur avant 1
  - le pin2 est l'adresse du capteur avant 2
*/
/* IL FAUT PENSER A REGLER LES PINS : Ce commentaire est a retirer

#define bus0 0x01
#define pin1 0x02
#define pin2 0x03
#define adr1 0x21

#USE I2C(SLAVE, SCL=PIN_B8, SDA=PIN_B9,ADDRESS=adr1, FORCE_HW) //On definit la connexion I2C avec le Pi

*/

////////////////////////////////////////////////////////////////////////////////
////////////////////  VARIABLES GLOBALES  //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int1 etat_capteur = 1; //1 les capteurs mesure les distances, 0 les capteurs s'arrêtent
unsigned int16 distance_1 = 0;
unsigned int16 distance_2 = 0;

/////////////////////////////////////////////////////////////////////////////////
////////  Mesure d'une longueur par un capteur spécifique  //////////////////////
/////////////////////////////////////////////////////////////////////////////////

/* La numerotation des capteurs est :
  - le 1 est le capteur 1
  - le 2 est le capteur 2
*/

int mesure(int numero)
{
	//On commence par determiner quel est le capteur a interroger
	BYTE pin;
	if (numero==1)
	{
		pin=pin1;
	}
	else //if (numero==2)
	{
		pin=pin2;
	}
	float m = 19.8;
	float b = 0.228;
	long volt = read_adc();
	
	//On converti le temps du pulse en distance selon la formule de la datasheet (en mm)
	int distance = (int) m/(volt-b)*10; //c'est en mm. Voir datasheet pour la conversion.
	return distance;
}

//////////////////////////////////////////////////////////////////////////////
///  Calcule la distance devant le robot en faisant plusieures mesures et  ///
///////////////  en utilisant les deux capteurs a l'avant  ///////////////////
//////////////////////////////////////////////////////////////////////////////

int distance_1()
{
	int compteur;
	int nombre_de_mesure = 3;
	int distance1 = 0;
	//On lit le port analogique 0
	set_adc_channel(0);
	for (compteur=0, compteur<nombre_de_mesure, compteur++)
	{
		distance1 += mesure(1); //1 est le code pour le capteur avant droit
		delay_us(10);
	}
	int distance = (int) distance1/nombre_de_mesure;
	return distance;
}

//////////////////////////////////////////////////////////////////////////////
///  Calcule la distance derriere le robot en faisant plusieures mesures  ////
//////////////////////////////////////////////////////////////////////////////

int distance_2()
{
	int compteur;
	int nombre_de_mesure = 3;
	int distance2 = 0;
	// on lit le port analogique 1
	set_adc_channel(1);
	for (compteur=0, compteur<nombre_de_mesure, compteur++)
	{
		distance2 += mesure(2); //1 est le code pour le capteur avant droit
		delay_us(10);
	}
	int distance = (int) distance2/nombre_de_mesure;
	return distance;
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////////  Communication I2C  //////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Variables

/*
0 : addresse reçue, 
1 a 0x7F : byte de données reçu, 1 etant le premier, 2 le second, ....
0x80 : demande de réponse, 
0x81 a 0xFF : reponses a envoyer. Si 0x81, alors un byte de donnee a deja ete envoye. S'incremente a chaque envoit, commme pour la reception.
*/

BYTE data[8]; // Les données reçues 
//BYTE write[10];

BYTE state = 1; // L'état du bus I2C(0 : addresse reçue, 1 : byte de données reçu, 0x80 : demande de réponse)

int16 buff_i2c;

#INT_SI2C

/* 
La reponse aura la forme : [0, 0, distance_av poids fort, distance_av poids faible, distance_ar poids fort, distance_ar poids faible, 0, ...]
*/

void si2c_interrupt() 
{
	state = i2c_isr_state();
	if(state>=0x80)
	{
		if(state < 0x82)
		{
			i2c_write(data[state-0x80]);
		}
		else if(state == 0x82)
		{
			i2c_write((int16)distance_1);
		}
		else if(state == 0x83)
		{
			i2c_write((int16) (((int8)distance_1) >> 8));
		}
		else if(state == 0x84)
		{
			i2c_write((int16)distance_2);
		}
		else if(state == 0x85)
		{
			i2c_write((int16) (((int8)distance_2) >> 8));
		}
		else
		{
			i2c_write(0x00);
		}
	}	
	
}


void main()
{
	//On active les interuptions physiques du PIC
	enable_interrupts(INT_SI2C);
	enable_interrupts(INTR_GLOBAL);
	//On configure la clock du convertisseur analogique numerique
	setup_adc(ADC_CLOCK_INTERNAL);
	//On selectionne le port A + les entrees A00 et A01
	setup_adc_ports(sAN0|sAN1, VSS_VDD);
	// //on touche a des trucs.... Des histoires d'horloges et de synchro.
	// setup_adc(ADC_CLOCK_DIV_4|ADC_TAD_MUL_8);
	
	delay_ms(10);
	
	
	int8 i = 0;
	for(i=0; i < 8; i++)
	{
		data[i]=0;
	}
	
	//On calcule constamment la distance a l'avant et a l'arriere du robot.
	while (etat_capteur == 1)
	{
		distance_1 = distance_1();
		distance_2 = distance_2();
		delay_ms(50);
	}
}
	
	
	



