/*
********************************************************************************
    Libreria de conversiones bases Hexadecimal, Octal y Binario a Decimal
    Creadores: Gabriel Ortiz Valdovinos y Alejandro Duarte Sanchez

********************************************************************************
*/



#ifndef CONVERTION_H_INCLUDED
#define CONVERTION_H_INCLUDED

#include<ctype.h>
#include<string.h>
#include<math.h>

typedef char string[200];

int toHex(string hex);
int toOct(string octal);
int toBin(string bin);

int toHex(string hex)
{
    short int cont;
    unsigned aux=0;
    unsigned potencia=0;
    int dec=0;
    cont=strlen(hex)-1;

    do
    {
        aux=pow(16,potencia);
        if( isdigit(hex[cont]) == 1 )
        {
            dec+=(hex[cont]-48)*aux;
        }

        else if( (hex[cont]>='a' && hex[cont]<='f') || (hex[cont]>='A' && hex[cont]<='F'))
            {
                switch( toupper(hex[cont]) )
                {
                    case 'A':
                            dec+=(10*aux);
                            break;
                    case 'B':
                            dec+=(11*aux);
                            break;
                    case 'C':
                            dec+=(12*aux);
                            break;
                    case 'D':
                            dec+=(13*aux);
                            break;
                    case 'E':
                            dec+=(14*aux);
                            break;
                    case 'F':
                            dec+=(15*aux);
                            break;
                    default:
                            break;
                }
            }
        cont--;
        potencia++;

    }while( cont >= 0 );

    return dec;
}


int toOct(string octal)
{
    short int cont;
    unsigned aux=0;
    unsigned potencia=0;
    int dec=0;
    cont=strlen(octal)-1;


    do
    {
        aux=pow(8,potencia);


        if( (octal[cont]>='0' && octal[cont]<='7') )
            {
                dec+=((octal[cont]-48)*aux);

            }
        else
        {
            printf("\nSe encontraron caracteres invalidos\n");
            return 0;
        }

        cont--;
        potencia++;

    }while( cont >= 0 );


    return dec;
}


int toBin(string bin)
{
    short int cont;
    unsigned aux=0;
    unsigned potencia=0;
    int dec=0;
    cont=strlen(bin)-1;

    do
    {
        aux=pow(2,potencia);


        if( (bin[cont]=='0' || bin[cont]=='1') )
            {
                dec+=((bin[cont]-48)*aux);

            }
        else
        {
            printf("\nSe encontraron caracteres invalidos\n");
            return 0;
        }
        cont--;
        potencia++;

    }while( cont >= 0 );


    return dec;
}


#endif // CONVERTION_H_INCLUDED
