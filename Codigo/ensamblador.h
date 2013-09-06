#ifndef ENSAMBLADOR_H_INCLUDED
#define ENSAMBLADOR_H_INCLUDED
/*
********************************************************************************
    Libreria de funciones para ensamblador
    Creadores: Gabriel Ortiz Valdovinos y Alejandro Duarte Sanchez

********************************************************************************
*/
/*
    Taller de programacion de sistemas

    Hecho por:
      - Gabriel Ortiz Valdovinos
      - Alejandro Duarte Sánchez

    version: 05/06/2012
    Hora: 10:54 PM
    practica actual : 10
*/
/*
    1.-inherente
    2.-directo
    3.-extendido
    4.-inmediato
    5.-indizado de 5 bits
    6.-indizado de 9 bits
    7.-indizado de 16 bits
    8.-indizado de pre/incre
    9.-indizado de acumulador
    10.-indizado de 16 bits indirecto
    11.-indizado de acumulador D indirecto
    12.-indizado de pre/decre
    13.-indizado de post/incre
    14.-indizado de post/decre
    15.-relativo 8bits
    16.-relativo 16bits
*/
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <direct.h>

#define TAM 129
#define TRUE 1
#define FALSE 0


struct tabl
{
    string idx5[128], idx9[8], idx16[4], preinc[24], acum[12],ind16b[4];
    string acumd[4], predec[24],posinc[24], posdec[24];
    string cmidx5[128], cmidx9[8], cmidx16[4], cmpreinc[24], cmacum[12],cmind16b[4];
    string cmacumd[4], cmpredec[24], cmposinc[24], cmposdec[24];
};

struct tab
{
    string etiqueta, valor;
};

struct  codop
{
    struct tabop
    {
        string codop,md,cm;
        int operando,tbc,tbpc,stb;

    }t[10];
    int contador;
};



int isEtiqueta(string etiqueta);
void isCodop( string codop );
int isComentario(char caracter);
int leeTabop(int fd2, struct codop c[TAM]);
int localiza( struct codop c[TAM], string codop,int operando,int cont_max,string modo,int stb[15],int tbpc[15],string cm[15]);
int identificar(string operando,string modo,int tbpc[15],int stb[15], int *rango,int *direccionamiento);
int revisaBase(string operando, int *rango);
int isDirConst(string codop,string operando, int *rango);
int isDirReserva( string codop,string operando, int *rango );
void creaTemporal(FILE *temp,string valor, string etiqueta, string codop, string operando);
void creaTabsim(FILE *tabsim,string etiqueta, string valor);
int aceptaCodop(int *org2, string codop);
void revisaTemporal(int tabop, FILE *temp,int cont,struct codop  c[TAM],struct tabl tab, FILE *tabsim,string programa);
struct tabl leeTabla(FILE *tabla, struct tabl tab);
void obtieneCM(int direccionamiento, string operando ,string bytes,struct tabl tab);
void ponerN(string operando,string aux);
int  leeTabsim(FILE *tabsim, struct tab t[100]);
int buscaEtiqueta( struct tab t[100], string etiqueta, string cod_maq, int contr);
int isEtiqueta2(string etiqueta);
int dirConst(string codop);
int dirReserv(string codop);
void creaS0(string programa,FILE ** obj);
void creaS1(string datos,string direccion,FILE ** obj);
void creaS9(FILE **obj);
void obtieneruta(string ruta);

int isEtiqueta(string etiqueta)
{
    int estado=0,band=FALSE,contador=0;
    do
    {
        switch(estado)
        {
            case 0:
                    if(isalpha(etiqueta[contador])==FALSE)
                        printf("\n\tERROR: La ETIQUETA debe de iniciar con letra");
                    estado=1;
                    contador++;
                        break;
            case 1:
                    if(isalpha(etiqueta[contador])==FALSE && isdigit(etiqueta[contador])==FALSE && etiqueta[contador] !='_')
                    {
                        band=TRUE;

                    }
                    contador++;
                    break;

            default:
                    break;

        }
    }while(etiqueta[contador]!='\0');
    if(band==TRUE)
    {
        printf("\n\tERROR: Los caracteres validos en las etiquetas son letras, digitos(0...9)y el guion bajo");
        return FALSE;
    }

    if(contador>8)
    {
         printf("\n\tERROR: La longitud maxima de una ETIQUETA es de ocho caracteres!");
         return FALSE;
    }
    return TRUE;
}
void isCodop( string codop )
{
    int estado=0,band=FALSE,contador=0,band2=FALSE,punto=FALSE;
    do
    {
        switch(estado)
        {
            case 0:
                    if(isalpha(codop[contador])!=TRUE)
                        printf("\n\tERROR: Los CODIGOS DE OPERACION debe de iniciar con letra");
                        estado=1;
                        contador++;
                        break;
            case 1:
                     if(codop[contador]=='.')
                     {
                         punto=TRUE;
                         estado=2;
                     }

                    else if(isalpha(codop[contador])==TRUE)
                        estado=1;
                    else
                        band2=TRUE;
                    contador++;
                    break;
            case 2:
                    if(codop[contador]=='.' && punto==TRUE)
                        band=TRUE;
                    else if(isalpha(codop[contador])!=TRUE)
                            band2=TRUE;
                    contador++;
                    break;

            default:
                    break;

        }
    }while(codop[contador]!='\0');
    if(band==TRUE)
        printf("\n\tERROR: Los CODIGOS DE OPERACION no pueden tener mas de un punto");
    if(band2==TRUE)
        printf("\n\tERROR: Los caracteres validos de los CODIGOS DE OPERACION son letra y el caracter del punto");
    if(contador>5)
        printf("\n\tERROR: La longitud maxima de un CODIGO DE OPERACION es de 5 caracteres");
}
int isComentario(char caracter )
{
    if(caracter==';')
        return TRUE;
    else
        return FALSE;

}

int leeTabop(int fd2, struct codop  c[TAM] )
{
    char caracter;
    int contc, contp;
    int cont=0;
    unsigned i;
    lseek(fd2, 0 ,0 );
    int tot= read(fd2,&caracter,sizeof( char ));
    do
    {
        contp=0;
        while(caracter!='\n' && tot>0)
        {
            contc=0;
                /** CODOP **/
            do
            {

                c[cont].t[contp].codop[contc]=caracter;
                read(fd2,&caracter,sizeof( char ) );
                contc++;
            }while(caracter!='\t');

            c[cont].t[contp].codop[contc]='\0';
            /** OPERANDO **/
            read(fd2,&caracter,sizeof( char ) );


            if(caracter=='0')
                c[cont].t[contp].operando=0;
            else if (caracter== '1')
                c[cont].t[contp].operando=1;
                else
                printf("ERROR");
            /** MODO DIRECCIONAMIENTO **/
            read(fd2,&caracter,sizeof( char ) );

            contc=0;
            do
            {
                c[cont].t[contp].md[contc]=caracter;
                read(fd2,&caracter,sizeof( char ) );
                contc++;
            }while(caracter!='\t');
            for(i=0;i<strlen(c[cont].t[contp].md);i++)
                c[cont].t[contp].md[i]=c[cont].t[contp].md[i+1];
             c[cont].t[contp].md[i-1]='\0';

            /** CODIGO MAQUINA **/
            read(fd2,&caracter,sizeof( char ) );
            contc=0;
            do
            {
                c[cont].t[contp].cm[contc]=caracter;
                read(fd2,&caracter,sizeof( char ) );
                contc++;
            }while(caracter!='\t');
             c[cont].t[contp].cm[contc]='\0';

            /** Total de bytes calculados**/
            read(fd2,&caracter,sizeof( char ) );


             c[cont].t[contp].tbc=atoi(&caracter);
            read(fd2,&caracter,sizeof( char ) );
            /**total de bytes por calcular**/
            read(fd2,&caracter,sizeof( char ) );

            c[cont].t[contp].tbpc=atoi(&caracter);
            read(fd2,&caracter,sizeof( char ) );

            /**suma totsl de bytes**/
            read(fd2,&caracter,sizeof( char ) );
            c[cont].t[contp].stb=atoi(&caracter);
            tot=read(fd2,&caracter,sizeof( char ) );
            tot=read(fd2,&caracter,sizeof( char ));
            contp++;
        }
             c[cont].contador=contp;
             cont++;
    }while(read(fd2,&caracter,sizeof( char ) ) >=0  && tot>0);
    return cont;
}

int localiza( struct codop c[TAM], string codop, int operando,int cont_max,string modo,int stb[15],int tbpc[15], string cm[15])
{

  int i=0,j=cont_max-1,medio;
  int band=0;
  int k,l;

    while(i<=j && band ==0){
        medio=(i+j)/2;
        if(strcmp(codop, c[medio].t[0].codop)==0)
        {
            if( (c[medio].t[0].operando == 1 && operando == 1) ||
                (c[medio].t[0].operando == 0 && operando == 0)   )
            {
                for(l=0;l<70;l++)
                    printf("_");
                for(k=0;k<c[medio].contador;k++)
                {
                    printf("\n%s\t", c[medio].t[k].codop);
                    printf("%i\t", c[medio].t[k].operando);
                    printf("%s\t", c[medio].t[k].md);
                    printf("%s\t", c[medio].t[k].cm);
                    printf("%i\t", c[medio].t[k].tbc);
                    printf("%i\t", c[medio].t[k].tbpc);
                    printf("%i", c[medio].t[k].stb);

                    strcpy(modo,c[medio].t[k].md);

                    if( strcmp(c[medio].t[k].md,"Inmediato")==0 )
                    {
                        stb[0]=c[medio].t[k].stb;
                        tbpc[0]=c[medio].t[k].tbpc;
                        strcpy(cm[0],c[medio].t[k].cm);
                    }
                    else if( strcmp(c[medio].t[k].md,"Directo")==0 )
                    {
                        stb[1]=c[medio].t[k].stb;
                        tbpc[1]=c[medio].t[k].tbpc;
                        strcpy(cm[1],c[medio].t[k].cm);
                    }
                        else if( strcmp(c[medio].t[k].md,"Extendido")==0 )
                        {
                            stb[2]=c[medio].t[k].stb;
                            tbpc[2]=c[medio].t[k].tbpc;
                            strcpy(cm[2],c[medio].t[k].cm);
                        }
                            else if( strcmp(c[medio].t[k].md,"IDX")==0 )
                            {
                                stb[3]=c[medio].t[k].stb;
                                tbpc[3]=c[medio].t[k].tbpc;
                                strcpy(cm[3],c[medio].t[k].cm);
                            }
                                else if( strcmp(c[medio].t[k].md,"IDX1")==0 )
                                {
                                    stb[4]=c[medio].t[k].stb;
                                    tbpc[4]=c[medio].t[k].tbpc;
                                    strcpy(cm[4],c[medio].t[k].cm);
                                }
                                    else if( strcmp(c[medio].t[k].md,"IDX2")==0 )
                                    {
                                        stb[5]=c[medio].t[k].stb;
                                        tbpc[5]=c[medio].t[k].tbpc;
                                        strcpy(cm[5],c[medio].t[k].cm);
                                    }
                                        else if( strcmp(c[medio].t[k].md,"[D,IDX]")==0 )
                                        {
                                            stb[6]=c[medio].t[k].stb;
                                            tbpc[6]=c[medio].t[k].tbpc;
                                            strcpy(cm[6],c[medio].t[k].cm);
                                        }
                                            else if( strcmp(c[medio].t[k].md,"[IDX2]")==0 )
                                            {
                                                stb[7]=c[medio].t[k].stb;
                                                tbpc[7]=c[medio].t[k].tbpc;
                                                strcpy(cm[7],c[medio].t[k].cm);
                                            }
                                                else if( strcmp(c[medio].t[k].md,"Inherente")==0 )
                                                {
                                                    stb[8]=c[medio].t[k].stb;
                                                    tbpc[8]=c[medio].t[k].tbpc;
                                                    strcpy(cm[8],c[medio].t[k].cm);
                                                }
                                                    else if(strcmp(c[medio].t[k].md,"REL")==0)
                                                    {
                                                        stb[9]=c[medio].t[k].stb;
                                                        tbpc[9]=c[medio].t[k].tbpc;
                                                        strcpy(cm[9],c[medio].t[k].cm);
                                                    }
                }//uniconvertor

            }

            else if( c[medio].t[0].operando == 1 && operando == 0 )
                printf("\n\t\tERROR: El CODOP debe tener OPERANDO");
                else if( c[medio].t[0].operando == 0 && operando == 1 )
                        printf("\n\t\tERROR: El CODOP no debe tener OPERANDO");
            band=1;
            return 1;
        }
        else if(strcmp(codop, c[medio].t[0].codop)<0)
                j=medio-1;
        else if(strcmp(codop, c[medio].t[0].codop)>0)
                i=medio+1;
    }
    if(band==0)
    {
        printf("\n");
        for(l=0;l<70;l++)
            printf("_");
        printf("\n\n\t\t=== NO SE ENCONTRO EL CODIGO DE OPERACION === ");
    }

    printf("\n");
    for(l=0;l<70;l++)
        printf("_");
    return 0;
}

int identificar(string operando,string modo, int tbpc[15],int stb[15], int *rango,int *direccionamiento)
{
    int estado=0,band2=0,band=0,band3=0;
    unsigned int i;
    int dec;
    unsigned int contador=0;
    int auxcont=0;
    string aux,aux2;
    char estado_aux[2];
    do
    {
        switch(estado)
        {
            case 0:
                    if(strcmp(strupr(operando),"NULL")==0 &&  strcmp(modo,"Inherente")==0)
                        estado=0;
                    else if( isEtiqueta2(operando)== TRUE  )
                        estado=30;
                        else if( toupper(operando[contador])== 'A' || toupper(operando[contador])== 'B' || toupper(operando[contador])== 'D' )
                            estado=1;
                            else if( operando[contador]=='#' )
                                    estado=4;

                                else if( isdigit(operando[contador])==1 || operando[contador]=='-')
                                {
                                    estado=18;
                                    aux[auxcont]=operando[contador];
                                    auxcont++;
                                    if(operando[contador]=='-')
                                        band=1;
                                }

                                    else if( operando[contador]== '[' )
                                    {
                                        estado=21;
                                    }


                                        else if( operando[contador] == '$')
                                                estado=16;

                                            else if( operando[contador] == '@')
                                                estado=14;

                                                else if( operando[contador] == '%')
                                                        estado=12;
                                                    else if( operando[contador] == ',' )
                                                                estado=34;
                                                        else
                                                            estado=53;

                    break;
            case 1:
                    if( operando[contador] == ',')
                        estado=2;
                    else
                    {
                        printf("\n=== ERROR: caracter invalido debe existir el caracter ',' ===");
                        estado=53;
                    }
                    break;
            case 2:
                    if(toupper(operando[contador])=='X'|| toupper(operando[contador])=='Y')
                    estado=3;
                else if((toupper(operando[contador])=='S' && toupper(operando[contador+1]) =='P')||
                    (toupper(operando[contador])=='P' && toupper(operando[contador+1]) =='C') )
                    {
                        estado=3;
                        contador++;
                    }
                    else
                    {
                        printf("\n=== ERROR los registros validos son: X,Y,SP,PC ===");
                        estado=53;
                    }
                    break;
            case 3://terminal
                    break;
            case 4:
                    if( isdigit(operando[contador]) ==1 )
                    {
                        estado=7;
                        if(operando[contador+1]=='\0' && (operando[contador-1]=='@'|| operando[contador-1]=='$' || operando[contador-1]=='%'))
                        {
                            for(i=1;i<strlen(operando); i++)
                                aux2[i-1]=operando[i];
                            aux2[i-1]='\0';
                            if(operando[contador-1]=='@')
                                dec=toOct(aux2);
                            if(operando[contador-1]=='$')
                                dec=toHex(aux2);
                            if(operando[contador-1]=='%')
                                dec=toBin(aux2);
                            estado=48;
                        }
                    }

                    else if( operando[contador] =='@' )
                            estado=5;
                        else if( operando[contador] == '$' )
                                estado=8;
                            else if( operando[contador] =='%' )
                                    estado=10;
                                else
                                {
                                    printf("\n=== ERROR === las bases numericas validas son: @,$,%% y decimal");
                                    estado=53;
                                }

                    break;
            case 5:
                    if(operando[contador]>='0' && operando[contador]<='7')
                    {
                        estado=6;
                        if(operando[contador+1]=='\0')
                        {
                            for(i=2;i<strlen(operando); i++)
                                aux2[i-2]=operando[i];
                            aux2[i-2]='\0';
                            dec=toHex(aux2);
                            estado=48;
                        }
                    }

                     else if( operando[contador]!='\0')
                    {

                        printf("\n=== ERROR === los numeros validos en la base octal son de 0 a 7");
                        estado=53;
                    }
                    else
                        estado=54;


                    break;
            case 6:
                    if(operando[contador]>='0' && operando[contador]<='7')
                        estado=6;
                    else if(operando[contador] ==',')
                    {
                        printf("\n=== ERROR === modos indizados solo se permiten base decimal");
                        estado=53;
                    }
                    else if( band2 == 0 )
                    {
                        printf("\n=== ERROR === los numeros validos de la base numerica octal son del 0 al 7");
                        estado=53;
                    }
                    if(operando[contador+1] == '\0')
                    {
                        for(i=2;i<strlen(operando); i++)
                                aux2[i-2]=operando[i];
                            aux2[i-2]='\0';
                        if( tbpc[0] == 1  )
                        {
                            if( (dec=toOct(aux2))>= 0 && (dec=toOct(aux2))<= 255 )
                                estado=48;
                            else
                            {
                                printf("\n=== ERROR === Fuera de rango de 0-255");
                                *rango=1;
                                estado=53;
                            }

                        }

                        else if( tbpc[0] == 2 )
                                {
                                    if( toOct(aux2)>=0 && toOct(aux)<=65535 )
                                        estado=49;
                                    else
                                    {
                                        printf("\n=== ERROR === Fuera de rango de 0-65535");
                                        *rango=1;
                                        estado=53;
                                    }

                                }
                        band2=1;
                    }
                    break;
            case 7:
                    if( isdigit(operando[contador])==1 )
                        estado=7;
                    else if(operando[contador] ==',')
                    {
                        printf("\n=== ERROR === modos indizados solo se permiten base decimal");
                        estado=53;
                    }
                    else if(operando[contador]!='\0'  )
                    {
                        printf("\n=== ERROR === los caracteres validos son solo los numeros del 0 al 9");
                        estado=53;
                    }
                    else
                        estado=48;
                    if(operando[contador] == '\0')
                    {
                        for(i=1;i<strlen(operando); i++)
                                aux2[i-1]=operando[i];
                            aux2[i-1]='\0';
                        dec=atoi(aux2);
                        if( tbpc[0] == 1  )
                        {
                            if( dec>= 0 && dec<= 255 )
                                estado=48;
                            else
                            {
                                printf("\n=== ERROR === Fuera de rango de 0-255");
                                *rango=1;
                                estado=53;
                            }

                        }

                        else if( tbpc[0] == 2 )
                                {
                                    if( dec >=0 && dec<=65535 )
                                        estado=49;
                                    else
                                    {
                                        printf("\n=== ERROR === Fuera de rango de 0-65535\n");
                                        *rango=1;
                                        estado=53;
                                    }


                                }

                        band2=1;
                    }
                    break;
            case 8:
                    if( (operando[contador]>='0' && operando[contador]<='9') || (toupper(operando[contador])>='A' && toupper(operando[contador])<='F') )
                    {
                        estado=9;
                        if(operando[contador+1]=='\0')
                        {
                            for(i=1;i<strlen(operando); i++)
                                aux2[i-1]=operando[i];
                            aux2[i-1]='\0';
                            dec=toHex(aux2);
                            estado=48;
                        }

                    }
                    else if( operando[contador]!='\0')
                    {
                        printf("\n=== ERROR === los caracteres validos en la base hexadecimal son: 0 a 9 y A a F");
                        estado=53;
                    }
                    else
                        estado=54;

                    break;
            case 9:
                    if( (operando[contador]>='0' && operando[contador]<='9') || (toupper(operando[contador])>='A' && toupper(operando[contador])<='F') )
                       estado=9;
                    else if(operando[contador] ==',')
                    {
                        printf("\n=== ERROR === modos indizados solo se permiten base decimal");
                        estado=53;
                    }
                    else if( band2 == 0 )
                    {
                        printf("\n=== ERROR === los caracteres validos en la base hexadecimal son: 0 a 9 y A a F");
                        estado=53;
                    }
                    if(operando[contador+1] == '\0')
                    {
                        for(i=2;i<strlen(operando); i++)
                                aux2[i-2]=operando[i];
                            aux2[i-2]='\0';
                        if( tbpc[0] == 1  )
                        {
                            if( (dec=toHex(aux2))>= 0 && (dec=toHex(aux2))<= 255 )
                                estado=48;
                            else
                            {
                                printf("\n=== ERROR === Fuera de rango de 0-255");
                                *rango=1;
                                estado=53;
                            }

                        }

                        else if( tbpc[0] == 2 )
                                {
                                    if( toHex(aux2)>=0 && toHex(aux)<=65535 )
                                        estado=49;
                                    else
                                    {
                                        printf("\n=== ERROR === Fuera de rango de 0-65535");
                                        *rango=1;
                                        estado=53;
                                    }

                                }
                        band2=1;
                    }
                    break;
            case 10:
                    if( operando[contador] =='0' || operando[contador]=='1' )
                        estado=11;
                     else if( operando[contador]!='\0')
                    {
                        printf("\n=== ERROR === los numeros validos son el 1 y el 0");
                        estado=53;
                    }
                    else
                        estado=54;

                    break;
            case 11:
                    if( operando[contador] =='0' || operando[contador]=='1' )
                        estado=11;
                    else if(operando[contador] ==',')
                    {
                        printf("\n=== ERROR === modos indizados solo se permiten base decimal");
                        estado=53;
                    }
                    else if( band2 == 0 )
                    {
                        printf("\n=== ERROR === los numeros validos son el 1y el 0");
                        estado=53;
                    }
                    if(operando[contador+1] == '\0')
                    {
                        for(i=2;i<strlen(operando); i++)
                                aux2[i-2]=operando[i];
                            aux2[i-2]='\0';
                        if( tbpc[0] == 1  )
                        {
                            if( (dec=toBin(aux2))>= 0 && (dec=toBin(aux2)<= 255) )
                                estado=48;
                            else
                            {
                                printf("\n=== ERROR === Fuera de rango de 0-255");
                                *rango=1;
                                estado=53;
                            }

                        }

                        else if( tbpc[0] == 2 )
                                {
                                    if( toBin(aux2)>=0 && toBin(aux2)<=65535 )
                                        estado=49;
                                    else
                                    {
                                        printf("\n=== ERROR === Fuera de rango de 0-65535");
                                        *rango=1;
                                        estado=53;
                                    }

                                }
                        band2=1;
                    }
                    break;
            case 12:
                    if(operando[contador]=='\0')
                    {
                        printf("\n=== ERROR === Hace falta el numero despues de su base");
                        estado=53;
                    }
                    else if( operando[contador] =='0' || operando[contador]=='1' )
                        estado=13;
                    else
                    {
                        printf("\n=== ERROR === los numeros validos son el 1y el 0");
                        estado=53;
                    }
                    if(operando[contador+1]=='\0')
                        {
                            for(i=1;i<strlen(operando); i++)
                                aux2[i-1]=operando[i];
                            aux2[i-1]='\0';
                            dec=toBin(aux2);
                            estado=45;
                        }
                    break;
            case 13:
                    if( (operando[contador] =='0' || operando[contador]=='1')  && operando[contador+1]!='\0')
                        estado=13;
                    else if(operando[contador]!='\0' && (operando[contador+1]!='\0' || (operando[contador] !='0' && operando[contador]!='1')) && operando[contador]!=',' )
                    {
                        printf("\n=== ERROR === los numeros validos son el 1 y el 0");
                        estado=53;
                        band3=1;
                    }
                    if(operando[contador] ==',')
                    {
                        printf("\n=== ERROR === modos indizados solo se permiten base decimal");
                        estado=53;
                    }
                    if (operando[contador+1]=='\0'&& estado!=53)
                    {
                        for(i=1;i<strlen(operando); i++)
                            aux2[i-1]=operando[i];
                        aux2[i-1]='\0';
                        estado=45;

                    }
                    else if(operando[contador] !='0' && operando[contador]!='1' && band3==0 && operando[contador]!=',')
                    {
                        printf("\n=== ERROR === los numeros validos son el 1 y el 0");
                        estado=53;
                    }
                    break;
            case 14:
                    if(operando[contador]=='\0')
                    {
                        printf("\n=== ERROR === Hace falta el numero despues de su base");
                        estado=53;
                    }

                    else if( operando[contador] >='0' || operando[contador]<='7' )
                        estado=15;
                    else
                    {
                        printf("\n=== ERROR === los numeros validos son del 0 al 7");
                        estado=53;
                    }
                    if(operando[contador+1]=='\0')
                        {
                            for(i=1;i<strlen(operando); i++)
                                aux2[i-1]=operando[i];
                            aux2[i-1]='\0';
                            dec=toOct(aux2);
                            estado=45;
                        }
                    break;
            case 15:
                    if( operando[contador] >='0' && operando[contador]<='7' )
                        estado=15;
                    else if(operando[contador] ==',')
                    {
                        printf("\n=== ERROR === modos indizados solo se permiten base decimal");
                        estado=53;
                    }
                    else if( band2 == 0)
                    {
                        printf("\n=== ERROR === los caracteres validos son solo los numeros del 0 al 7");
                        estado=53;
                        band=1;
                    }
                    if(operando[contador+1]=='\0' && band==0)
                        {
                            for(i=1;i<strlen(operando); i++)
                                aux2[i-1]=operando[i];
                            aux2[i-1]='\0';
                            dec=toOct(aux2);
                            estado=45;
                            band2=1;
                        }
                    break;
            case 16:
                    if(operando[contador]=='\0')
                    {
                        printf("\n=== ERROR === Hace falta el numero despues de su base");
                        estado=53;
                    }
                    else if( (operando[contador]>='0' && operando[contador]<='9') || (toupper(operando[contador])>='A' && toupper(operando[contador])<='F') )
                        estado=17;
                    else
                    {
                        printf("\n=== ERROR === los caracteres validos en la base hexadecimal son: 0 al 9 y A a F");
                        estado=53;
                    }
                    if(operando[contador+1]=='\0')
                        {
                            for(i=1;i<strlen(operando); i++)
                                aux2[i-1]=operando[i];
                            aux2[i-1]='\0';
                            dec=toHex(aux2);
                            estado=45;
                        }
                    break;
            case 17:
                    if( ((operando[contador]>='0' && operando[contador]<='9') || (toupper(operando[contador])>='A' && toupper(operando[contador])<='F')) &&  operando[contador+1]!= '\0' )
                        estado=17;
                    else if( operando[contador]!='\0' && ( operando[contador+1]!='\0' || !((operando[contador]>='0' && operando[contador]<='9') || (toupper(operando[contador])>='A' && toupper(operando[contador])<='F')) )  && operando[contador]!=',')
                    {
                        printf("\n=== ERROR === los caracteres validos en la base hexadecimal son: 0 al 9 y A a F");
                        band3=1;
                        estado=53;
                    }
                    if(operando[contador] ==',')
                    {
                        printf("\n=== ERROR === modos indizados solo se permiten base decimal");
                        estado=53;
                    }
                    if(operando[contador+1]=='\0' && estado!=53)
                        {
                            for(i=1;i<strlen(operando); i++)
                                aux2[i-1]=operando[i];
                            aux2[i-1]='\0';
                            dec=toHex(aux2);
                            estado=45;
                        }
                    else if(((operando[contador]<='0' && operando[contador]>='9') && (toupper(operando[contador])<='A' && toupper(operando[contador])>='F')) && band3==0)
                    {
                        printf("\n=== ERROR === los caracteres validos en la base hexadecimal son: 0 al 9 y A a F");
                        estado=53;
                    }
                        break;
            case 18:
                    if(isdigit(operando[contador])==1 )
                    {
                        estado=18;
                        aux[auxcont]=operando[contador];
                        auxcont++;
                        aux[auxcont]='\0';
                        dec=atoi(aux);
                    }

                    else if( ( atoi( aux ) >= -256 && atoi( aux )<= -17 ) ||
                                     ( atoi( aux )>=16 && atoi( aux ) <= 255) )
                            {
                                estado=43;
                                contador--;
                            }


                                else if( atoi(aux) >=256 && atoi(aux)<=65535 && !(isalpha(operando[contador])))
                                        {
                                            estado=40;
                                            contador--;
                                        }
                                    else if( atoi( aux )>= -16 && atoi( aux )<=15 && !(isalpha(operando[contador])))
                                        {
                                            estado=33;
                                            contador--;
                                        }

                                    else if(dec>65535 || dec<-256)
                                        {
                                            estado=53;
                                            printf("\n===Error=== Rango excedido\n");
                                           *rango=1;
                                        }
                                     else if(isdigit(operando[contador]) !=1)
                                        {
                                            printf("\n\t\t=== ERROR=== Caracteres invalidos en base decimal===\n");
                                            estado=53;
                                        }

                    break;
            case 19:
                    if(toupper(operando[contador])=='X'|| toupper(operando[contador])=='Y')
                    estado=20;
                else if((toupper(operando[contador])=='S' && toupper(operando[contador+1]) =='P')||
                    (toupper(operando[contador])=='P' && toupper(operando[contador+1]) =='C') )
                    {
                        estado=20;
                        contador++;
                    }
                    else
                    {
                        printf("\n=== ERROR === los registros validos son: X,Y,SP,PC ===");

                        estado=53;
                    }
                    break;
            case 20://terminal
                    break;
            case 21:
                    if(toupper(operando[contador]=='D')==1 )
                        estado=26;
                    else if( isdigit(operando[contador])==1 )
                    {
                        estado=44;
                        aux[auxcont]=operando[contador];
                        auxcont++;
                        aux[auxcont]='\0';
                    }

                        else
                        {
                            printf("\n=== ERROR === la letra valida en el -acumulador indirecto- es: D o en el -modo indizado indirecto- son los numeros:0 al 9 ");
                            estado=53;
                        }
                    if( toupper(operando[contador]=='D')!=1 &&((atoi( aux )>=0 && atoi( aux )<=65535) && operando[contador+1]==','))
                    {
                        estado = 22;
                    }
                    break;

            case 22:
                if(operando[contador]== ',')
                    estado=23;
                else
                {
                    printf("\n=== ERROR === debe existir el caracter ','");
                    estado=53;
                }
            break;


            case 23:
                if(toupper(operando[contador])=='X'|| toupper(operando[contador])=='Y')
                    estado=24;
                else if((toupper(operando[contador])=='S' && toupper(operando[contador+1]) =='P')||
                    (toupper(operando[contador])=='P' && toupper(operando[contador+1]) =='C') )
                    {
                        estado=24;
                        contador++;
                    }
                else
                {
                    printf("\n=== ERROR === los registros validos son: X,Y,SP,PC ===");
                    estado=53;
                }
            break;


            case 24:
                if(operando[contador]==']')
                    estado=25;
                else
                {
                    printf("\n=== ERROR === caracter invalido: Despues de un registro debe existir ']'");
                    estado=53;
                }
            break;

            case 25://terminal

            break;


            case 26:
                if(operando[contador]== ',')
                    estado=27;
                else
                {
                    printf("\n=== ERROR === debe existir el caracter ','");
                    estado=53;
                }
            break;


            case 27:
                if(toupper(operando[contador])=='X'|| toupper(operando[contador])=='Y')
                    estado=28;
                else if((toupper(operando[contador])=='S' && toupper(operando[contador+1]) =='P')||
                    (toupper(operando[contador])=='P' && toupper(operando[contador+1]) =='C') )
                    {
                        estado=28;
                        contador++;
                    }
                else
                {
                    printf("\n=== ERROR === los registros validos son: X,Y,SP,PC ===");
                    estado=53;
                }
            break;


            case 28:
                if(operando[contador]==']')
                    estado=29;

                else
                {
                    printf("\n=== ERROR === caracter invalido: Despues de un registro debe existir ']'");
                    estado=53;
                }
                break;
            case 29://terminal
                    break;


            case 30:
                    if( strcmp(modo,"REL")==0)
                        estado=31;
                    else if( strcmp(modo,"Inherente")!=0 )
                    {
                        estado=46;
                        dec=256;
                    }

                    else
                    {
                        printf("\n=== ERROR === solo los modos de direccionamiento relativo  puede tener formato de etiqueta");
                        estado=53;
                    }

            break;


            case 31://terminal
            break;


            case 32://terminal
                    if(operando[contador]!='\0')
                    {
                        printf("\n===ERROR=== caracter invalido despues del registro");
                        estado=53;
                    }
            break;

            case 33:
                if(operando[contador]==',')
                    estado=34;
                else if(band==1)
                {
                    printf("\n=== ERROR === Debe existir el caracter ','");
                    estado=53;
                }
            break;

            case 34:
                if(toupper(operando[contador])=='X'|| toupper(operando[contador])=='Y')
                    estado=35;
                else if(toupper(operando[contador])=='S' && toupper(operando[contador+1]) =='P' )
                    {
                        estado=35;
                        contador++;
                    }
                else if( toupper(operando[contador])=='P' && toupper(operando[contador+1]) =='C' )
                    {
                        estado=32;
                        contador++;
                    }
                else if(operando[contador]=='+' || operando[contador]=='-')
                    {
                        estado=37;
                        estado_aux[0]='0';
                        estado_aux[1]=operando[contador];
                    }

                    else
                    {
                        printf("\n=== ERROR === despues de la ',' solo se permiten los registros: x,y ,sp o el signo + y -");
                        estado=53;
                    }

            break;

            case 35://terminal
                if(operando[contador]=='+' || operando[contador]=='-')
                {
                    estado=36;
                    estado_aux[0]='1';
                    estado_aux[1]=operando[contador];
                }
                else if( operando[contador]!='\0')
                {
                    printf("\n=== ERROR === caracter invalido en el modo indizado");
                    estado=53;
                }
            break;

            case 36:
                if(aux[0]<='8' && aux[0]>='1')
                estado=39;
                else
                {
                    printf("\n=== ERROR === los numeros validos en el indizado de pre-post inc o dec son: 1 al 8");
                    estado=53;
                }
            break;


            case 37:
                 if(toupper(operando[contador])=='X'|| toupper(operando[contador])=='Y'||
                    (toupper(operando[contador])=='S' && toupper(operando[contador+1]) =='P') )
                    estado=38;
                else
                    {
                        printf("\n=== ERROR === solo se permiten los registros: x,y ,sp ");
                        estado=38;
                    }
            break;

            case 38:
                if(aux[0]<='8' && aux[0]>='1')
                estado=39;
                else
                {
                    printf("\n=== ERROR === los numeros validos en el indizado de pre-post inc o dec son: 1 al 8");
                    estado=53;
                }
            break;

            case 39://terminal
                    break;

            case 40:
                if(operando[contador]==',')
                    estado=41;

            break;

            case 41:
                if( toupper(operando[contador]) == 'X' || toupper(operando[contador]) == 'Y'  ||
                      ( toupper(operando[contador])=='S' && toupper(operando[contador+1]) =='P' )  ||
                      ( toupper(operando[contador])=='P' && toupper(operando[contador+1]) =='C' ) )
                    estado=42;
                else
                {
                    printf("\n=== ERROR === solo se permiten los registros: x,y ,sp,pc ");
                    estado=53;
                }

            break;

            case 42://terminal
            break;

            case 43:
                if(operando[contador]==',')
                    estado=19;
            break;

            case 44:
                if(isdigit(operando[contador])==1 )
                {
                    estado=44;
                    aux[auxcont]=operando[contador];
                    auxcont++;
                    aux[auxcont]='\0';
                }
                else if(isdigit(operando[contador])==1)
                {
                    printf("\n=== ERROR === caracter invalido: solo se permiten los numeros de 0 al 9");
                    estado=53;
                }
                if( (atoi( aux )>=0 && atoi( aux )<=65535) && operando[contador+1]==',')
                {

                    estado = 22;
                }
                else if( atoi( aux )< 0 || atoi( aux )> 65535 )
                {
                    printf("\n=== ERROR === El rango de del indizado indirecto de 16 bits es de 0 a 65535");
                    *rango=1;
                    estado=53;
                }

            break;

            case 45:
                if(dec<= 255 && dec >=0)
                    estado=46;
                else if( dec>=  256 && dec <= 65535)
                    estado=47;
                    else
                    {
                        printf("=== ERROR === Fuera de rango");
                        *rango=1;
                    }

            break;

            case 46: // terminal
            break;

            case 47:// terminal
                break;
            case 48://terminal
                    break;
            case 49://terminal
                    break;
            case 50:
                    break;
            case 53: //error
                break;

            default:
                    break;
        }
        contador++;
    }while( strlen(operando)+1 >contador  && estado!=53);

    /** Nombre del modo de direccionamiento  **/
    if(estado!=53)
    {
        switch(estado)
        {
            case 0:
                    printf("\t\t\tInherente de %i bytes",stb[8]);
                    *direccionamiento=1;
                    return 8;
                    break;
            case 3:
                    printf("\t\t\tIndizado de acumulador(IDX) de %i bytes",stb[3]);
                    *direccionamiento=9;
                    return 3;
                    break;
            case 20:
                    printf("\t\t\tIndizado de 9 bits,(IDX1) de %i bytes",stb[4]);
                    *direccionamiento=6;
                    return 4;
                    break;
            case 25:
                    printf("\t\t\tIndizado indirecto de 16 bits ([IDX2]) de %i bytes",stb[7]);
                    *direccionamiento=10;
                    return 7;
                    break;
            case 29:
                    printf("\t\t\tIndizado de acumulador indirecto ([D,IDX]) de %i bytes",stb[6]);
                    *direccionamiento=11;
                    return 6;
                    break;
            case 31:
                    if(strcmp(modo,"Extendido")==0)
                    {
                        printf("\t\t\tExtendido de %i bytes",stb[2]);
                        *direccionamiento=3;
                        return 2;
                    }

                    else
                    {
                        if(tbpc[9]==1)
                        {
                            printf("\t\t\tRelativo de 8 bits, de %i bytes ",stb[9]);
                            *direccionamiento=15;
                        }

                        else if( tbpc[9] ==2)
                        {
                            printf("\t\t\tRelativo de 16 bits, de %i bytes ",stb[9]);
                            *direccionamiento=16;
                        }

                        return 9;

                    }

                    break;
            case 32: case 35:
                    printf("\t\t\tIndizado de 5 bits(IDX), de %i bytes ",stb[3]);
                    *direccionamiento=5;
                    return 3;
                    break;
            case 39:
                    if(estado_aux[0]=='0')
                    {
                        if(estado_aux[1]=='+')
                        {
                            printf("\t\t\tIndizado de auto pre incremento de %i bytes",stb[3]);
                            *direccionamiento=8;
                        }

                        else
                        {
                            printf("\t\t\tIndizado de auto pre decremento de %i bytes",stb[3]);
                            *direccionamiento=12;
                        }

                    }
                    else
                    {
                        if(estado_aux[1]=='+')
                        {
                            printf("\t\t\tIndizado de auto post incremento de %i bytes",stb[3]);
                            *direccionamiento=13;
                        }

                        else
                        {
                            printf("\t\t\tIndizado de auto post decremento de %i bytes",stb[3]);
                            *direccionamiento=14;
                        }

                    }
                    return 3;
                    break;
            case 42:
                    printf("\t\t\tIndizado de 16 bits(IDX2), de %i bytes",stb[5]);
                    *direccionamiento=7;
                    return 5;
                    break;
            case 48:
                    printf("\t\t\tInmediato 8 bits de  %i bytes",stb[0]);
                    *direccionamiento=4;
                    return 0;
                    break;
            case 49:
                    printf("\t\t\tInmediato 16 bits de %i bytes",stb[0]);
                    *direccionamiento=4;
                    return 0;
                    break;
            case 33:case 40: case 43: case 46: case 47: case 50:
                    if(dec >=0 && dec<=255)
                    {
                        printf("\t\t\tDirecto de %i bytes",stb[1]);
                        *direccionamiento=2;
                        return 1;
                    }

                    else if( dec >=256 && dec<=65535 )
                    {
                        printf("\t\t\tExtendido de %i bytes",stb[2]);
                        *direccionamiento=3;
                        return 2;
                    }

                    break;
            case 54:
                    printf("\n=== ERROR === Debe existir un caracter valido despues de su base");
                    return -1;
                    break;

            default:
                    break;

        }
    }
    return -1;
}
int revisaBase(string operando, int *rango)
{
    int estado=0,contador=0,contador2=0,dec=0;
    string aux;
    do
    {
        switch(estado)
        {
            case 0:
                    if(operando[contador]>='0' && operando[contador]<='9')
                    {
                        estado=1;
                        aux[contador2]=operando[contador];
                        contador2++;
                        aux[contador2]='\0';
                        if(operando[contador+1]=='\0')
                        {
                            dec=atoi(aux);
                            estado=8;
                        }

                    }

                    else if(operando[contador]=='$')
                        estado=2;
                        else if(operando[contador]=='@')
                            estado=3;
                            else if(operando[contador]=='%')
                                estado=4;
                                else
                                    estado=9;
                    break;
            case 1:
                    if(operando[contador]>='0' && operando[contador]<='9')
                    {
                        estado=1;
                        aux[contador2]=operando[contador];
                            contador2++;
                            aux[contador2]='\0';
                        if(operando[contador+1]=='\0')
                        {
                            if((dec=atoi(operando))>=0 && (dec=atoi(operando))<=65535)
                                estado=8;
                            else
                            {
                                printf("\n\t=== ERROR: Fuera de rango");
                                *rango=1;
                            }

                        }
                    }

                    else
                        estado=9;
                    break;
            case 2:
                    if( (operando[contador]>='0' && operando[contador]<='9') ||
                       (toupper(operando[contador])>='A' && toupper(operando[contador])<='F') )
                       {
                            estado=5;
                            aux[contador2]=operando[contador];
                            contador2++;
                            aux[contador2]='\0';
                            if(operando[contador+1]=='\0')
                            {
                                if((dec=toHex(aux))>=0 && (dec=toHex(aux))<=65535)
                                    estado=8;
                                else
                                {
                                    printf("\n\t=== ERROR: Fuera de rango");
                                    *rango=1;
                                }
                            }
                        }
                    else
                        estado=9;
                    break;
            case 3:
                    if(operando[contador]>='0' && operando[contador]<='7')
                       {
                            estado=6;
                            aux[contador2]=operando[contador];
                            contador2++;
                            aux[contador2]='\0';
                            if(operando[contador+1]=='\0')
                            {
                                if((dec=toOct(aux))>=0 &&(dec=toOct(aux))<=65535)
                                    estado=8;
                                else
                                    {
                                        printf("\n\t=== ERROR: Fuera de rango");
                                        *rango=1;
                                    }

                            }
                        }
                    else
                        estado=9;
                    break;
            case 4:

                    if(operando[contador]=='0' || operando[contador]=='1')
                       {
                            estado=7;
                            aux[contador2]=operando[contador];
                            contador2++;
                            aux[contador2]='\0';
                            if(operando[contador+1]=='\0')
                            {
                                if((dec=toBin(aux))>=0 && (dec=toBin(aux))<=65535)
                                    estado=8;
                                else
                                {
                                    printf("\n\t=== ERROR: Fuera de rango");
                                    *rango=1;
                                }

                            }
                        }
                    else
                        estado=9;
                    break;
            case 5:
                    if( (operando[contador]>='0' && operando[contador]<='9') ||
                       (toupper(operando[contador])>='A' && toupper(operando[contador])<='F') )
                       {
                            estado=5;
                            aux[contador2]=operando[contador];
                            contador2++;
                            aux[contador2]='\0';
                            if(operando[contador+1]=='\0')
                            {
                                if((dec=toHex(aux))>=0 && (dec=toHex(aux))<=65535)
                                    estado=8;
                                else
                                {
                                    printf("\n\t=== ERROR: Fuera de rango");
                                    *rango=1;
                                }

                            }
                        }
                    else
                        estado=9;
                    break;
            case 6:
                    if(operando[contador]>='0' && operando[contador]<='7')
                       {
                            estado=6;
                            aux[contador2]=operando[contador];
                            contador2++;
                            aux[contador2]='\0';
                            if(operando[contador+1]=='\0')
                            {
                                if((dec=toOct(aux))>=0 && (dec=toOct(aux))<=65535)
                                    estado=8;
                                else
                                {
                                    printf("\n\t=== ERROR: Fuera de rango");
                                    *rango=1;
                                }
                            }
                        }
                    else
                        estado=9;
                    break;
            case 7:
                    if(operando[contador]=='0' || operando[contador]=='1')
                       {
                            estado=7;
                            aux[contador2]=operando[contador];
                            contador2++;
                            aux[contador2]='\0';
                            if(operando[contador+1]=='\0')
                            {
                                if((dec=toBin(aux))>=0 && (dec=toBin(aux))<=65535)
                                    estado=8;
                                else
                                {
                                    printf("\n\t=== ERROR: Fuera de rango");
                                    *rango=1;
                                }
                            }
                        }
                    else
                        estado=9;
                    break;
            case 8://terminal
                    break;
            case 9://error
                    break;

            default:
                    break;
        }
        contador++;
    }while(operando[contador]!='\0');

    if(estado==8)
        return dec;
    else if( estado == 9 )
    {
        printf("\n\n\tERROR: no cumple con los formatos de las bases");
        return -1;
    }
    return -1;
}

int isDirConst(string codop,string operando, int *rango)
{
    int dec=0;
    if(strcmp(strupr(codop),"FCC")!=0)
        dec=revisaBase(operando, rango);
    if( dec !=-1 )
    {
       if(strcmp(strupr(codop),"DB")==0 || strcmp(strupr(codop),"DC.B")==0 || strcmp(strupr(codop),"FCB")==0 )
        {
            if(dec>=0 && dec<=255)
                return 1;
        }

        else if(strcmp(strupr(codop),"DW")==0 || strcmp(strupr(codop),"DC.W")==0 || strcmp(strupr(codop),"FDB")==0)
        {
            if( dec>=0 && dec<=65535 )
                    return 2;
        }

            else if(strcmp(strupr(codop),"FCC")==0)
                 {
                    if(operando[0]=='"' && operando[strlen(operando)-1]=='"')
                        return strlen(operando)-2;
                    else
                        return -1;
                 }


    }

    return 0;
}
int isDirReserva( string codop,string operando, int *rango )
{
    int dec=0;
    dec=revisaBase(operando, rango);
    if( dec!=-1 )
    {
        if( strcmp(strupr(codop),"DS")==0 || strcmp(strupr(codop),"DS.B")==0 || strcmp(strupr(codop),"RMB")==0)
        {
            if( dec>=0 && dec<=65535 )
                return dec*1;
        }
        else if( strcmp(strupr(codop),"DS.W")==0 || strcmp(strupr(codop),"RMW")==0)
            {
                if( dec>=0 && dec<=65535 )
                    return dec*2;
            }
    }

    return 0;

}

void creaTemporal(FILE *temp,string valor, string etiqueta, string codop, string operando)
{

        fwrite(valor,strlen(valor),1,temp);
        fwrite("\t",1,1,temp);
        fwrite(etiqueta,strlen(etiqueta),1,temp);
        fwrite("\t",1,1,temp);
        fwrite(codop,strlen(codop),1,temp);
        fwrite("\t",1,1,temp);
        fwrite(operando,strlen(operando),1,temp);
        fwrite("\n",1,1,temp);
}

void creaTabsim(FILE *tabsim,string etiqueta, string valor)
{
        fwrite(etiqueta,strlen(etiqueta),1,tabsim);
        fwrite("\t",1,1,tabsim);
        fwrite(valor,strlen(valor),1,tabsim);
        fwrite("\n",1,1,tabsim);
}

int aceptaCodop(int *org2, string cod)
{

    if(strcmp(strupr(cod),"EQU")==0)
        return  TRUE;

    else if( strcmp(strupr(cod),"ORG")==0 && *org2==FALSE )
    {
        *org2=1;
        return TRUE;
    }
        else if( *org2==TRUE && strcmp(strupr(cod),"ORG")==0)
        {
            return FALSE;
        }
            else if( *org2 == FALSE && (strcmp(strupr(cod),"ORG")!=0 && strcmp(strupr(cod),"EQU")!=0))
            {
                printf("\nERROR: Los codigos de operacion que pueden ir al inicio solo son EQU y ORG\n");
                return FALSE;
            }
                else if(*org2==TRUE && strcmp(strupr(cod),"ORG")!=0)
                    return TRUE;
    return -1;
}

void revisaTemporal(int tabop, FILE *temp,int cont,struct codop  c[TAM],struct tabl tab, FILE *tabsim,string programa)
{
    string codop, operando,modo,cm[15],cod_maq,aux,bytes,bytes2,valor,cod_tab,desplazamiento,hex, s19,codigo="\0",direccion,restante="\0";
    int tam,stb[15],tbpc[15],direccionamiento,rango,pos=0,j,siguiente,optab,resta,bandr=0,s1=0,dec,bandrestante=0;
    char caracter,carac;
    unsigned int contador=0,i,contaux, contr=0,posicion=0;
    struct tab t[100];
    FILE *obj;
    for(i=0;programa[i]!='.';i++)
    {
        s19[i]=programa[i];
    }
    s19[i]='\0';
    strcat(s19,".obj");
    obj= fopen(s19,"w+");

    lseek(tabop, 0, 0);
    fseek(temp, 0,0);
    contr=leeTabsim(tabsim,t)-1;

    creaS0(programa,&obj);
    while(fread(&caracter,sizeof( char ),1,temp ) >0 && tam>0)
    {
        contador=0;
        direccionamiento=0;
        carac='d';
        while(caracter!='\n' && tam>0)
        {
            while(caracter!='\t' &&caracter !=' ')
            {
                valor[contador]=caracter;
                contador++;
                tam=fread(&caracter,sizeof( char ),1,temp );

            }
            fread(&caracter,sizeof( char ),1,temp );
            while(caracter!='\t' &&caracter !=' ')
            {
                tam=fread(&caracter,sizeof( char ),1,temp );
            }
            fread(&caracter,sizeof( char ),1,temp );
            contador=0;
            while(caracter!='\t' && caracter!=' ' && caracter!='\n'&& tam>0)
            {
                codop[contador]=caracter;
                contador++;
                tam=fread(&caracter,sizeof( char ),1,temp );
                if(caracter=='\t' || caracter==' ' || caracter=='\n' || tam<1)
                        codop[contador]='\0';

            }
            while((caracter=='\t' || caracter==' ' )&& tam>0)
                tam=fread(&caracter,sizeof( char ),1,temp );
            contador=0;
            while(caracter!='\n'&& tam>0)
            {
                operando[contador]=caracter;
                tam=fread(&caracter,sizeof( char ),1,temp );
                contador++;
                if(caracter=='\n' || tam==0)
                    operando[contador]='\0';
            }
            if(strcmp(strupr(codop),"ORG")!=0 && strcmp(strupr(codop),"END")!=0 && strcmp(strupr(codop),"EQU")!=0 && dirReserv(codop)==-1)
            {

                /**       se convierte a base hexadecimal el operando        **/
                j=0;
                for(i=0;i<strlen(operando);i++)//se quita los caracteres que indican la base del operando
                {
                    if(operando[i]!='$' && operando[i]!='%' && operando[i]!='@')
                    {
                        aux[j]=operando[i];
                        j++;
                    }

                }
                aux[j]='\0';
                if(operando[0]=='%')//se convierte a hexadecimal si el valor es binario
                {
                    sprintf(hex,"%x",toBin(aux));
                }
                else if(operando[0]=='$')//se hace una copia del hexadecimal
                {
                    strcpy(hex,aux);
                }
                    else if(operando[0]=='@')//se convierte a hexadecimal si el valor es octal
                    {
                        sprintf(hex,"%x",toOct(aux));
                    }
                        else// se convierete a hexadecimal si el valor esta en decimal
                        {
                            sprintf(hex,"%x",atoi(aux));
                        }
                /**************************************************************/
                strcpy(cod_maq,"\0");
                /// se verifica si es una directiva constante si es asi se calcula su codigo maquina
                if( dirConst(codop)==0 )
                {
                    strcpy(cod_maq,"00");
                    for(i=strlen(cod_maq)-1,j=strlen(hex)-1;j>=0;i--,j--)
                    {
                        cod_maq[i]=toupper(hex[j]);
                    }
                }
                else if( dirConst(codop)==1 )
                    {
                        strcpy(cod_maq,"0000");
                        for(i=strlen(cod_maq)-1,j=strlen(hex)-1;j>=0;i--,j--)
                        {
                            cod_maq[i]=toupper(hex[j]);
                        }
                    }
                    else if(dirConst(codop)==2)
                        {
                            for(i=1;i<strlen(operando)-1;i++)
                            {
                                sprintf(hex,"%x",operando[i]);
                                strcat(cod_maq,strupr(hex));
                            }
                        }
                        ///si no se localiza en el tabsim e identifica q tipo de direccionamiento es
                        else
                        {
                            if( strcmp(strupr(operando),"NULL") ==0 && strcmp(strupr(codop),"END")!=0)
                                localiza(c,codop,0,cont,modo,stb,tbpc,cm);
                                else if( strcmp(strupr(operando),"NULL") !=0 && strcmp(strupr(codop),"END")!=0)
                                        localiza(c,codop,1,cont,modo,stb,tbpc,cm);
                            pos=identificar(operando,modo,tbpc,stb,&rango,&direccionamiento);
                            strcpy(cod_maq,cm[pos]);
                        }

                contador=0;
                if(direccionamiento<5 && direccionamiento>0)
                {
                    if(tbpc[pos]==1)
                        strcat(cod_maq,"00");
                    else if(tbpc[pos]==2)
                        strcat(cod_maq,"0000");
                        else if(tbpc[pos]==3)
                            strcat(cod_maq,"000000");
                    if(isEtiqueta2(operando)) // empieza extendido con etiqueta
                    {
                        for(i=0; i<contr;i++)
                        {
                            if( strcmp(operando, t[i].etiqueta) ==0 )
                            {

                                strcpy(cod_maq, cm[pos]);
                                strcat(cod_maq, t[i].valor);

                            }

                        }
                    }
                    else
                    {
                        for(i=0;i<strlen(operando);i++)
                        {
                            if(operando[i]!='$' && operando[i]!='@' && operando[i]!='%'&& operando[i]!='#')
                            {
                                aux[contador]=operando[i];
                                    contador++;
                            }
                                else if((operando[i]=='$' || operando[i]=='@' || operando[i]=='%') && operando[i]!='#')
                                {
                                    carac=operando[i];

                                }
                                    else if(operando[i]!='#')
                                    {
                                        aux[contador]=operando[i];
                                        contador++;
                                    }


                        }
                        aux[contador]='\0';
                        if(carac=='d')
                        {
                            sprintf(bytes,"%x",atoi(aux));
                        }
                        else if(carac=='%')
                        {
                            sprintf(bytes,"%x",toBin(aux));
                        }
                            else if(carac=='@')
                            {
                                sprintf(bytes,"%x",toOct(aux));
                            }
                            else if(carac=='$')
                            {
                                sprintf(bytes,"%x",toHex(aux));
                            }
                          if(bytes[0]!='0')
                            {
                                for(i=strlen(cod_maq)-1,j=strlen(bytes)-1;j>=0;i--,j--)
                                {
                                    cod_maq[i]=toupper(bytes[j]);
                                }
                            }
                    }
                }

                else if(direccionamiento>=5 && direccionamiento<15)
                {
                    if(operando[0]==',')
                    {
                        for(i=strlen(operando);i>0;i--)
                        {
                            operando[i]=operando[i-1];
                        }
                        operando[0]='0';
                    }
                    obtieneCM(direccionamiento,operando,bytes,tab);
                    if(toupper(operando[1])!='D')
                    {
                        contaux=0;
                        for(i=0;operando[i]!=',';i++)
                        {
                            if(operando[i]!='[')
                            {
                                aux[contaux]=operando[i];
                                contaux++;
                            }

                        }
                        aux[contaux]='\0';
                        sprintf(bytes2,"%x",atoi(aux));
                    }

                    if(operando[0]=='-')
                    {
                        contaux=0;
                        for(i=0;i<strlen(bytes2);i++)
                        {
                            if(bytes2[i]!='f')
                            {
                                aux[contaux]=bytes2[i];
                                contaux++;
                            }

                        }
                        aux[contaux]='\0';
                        strcpy(bytes2,aux);
                    }
                    strcat(cod_maq,bytes);
                    if(tbpc[pos]==2)
                        strcat(cod_maq,"00");
                        else if(tbpc[pos]==3)
                            strcat(cod_maq,"0000");

                    if(tbpc[pos]>1)
                        for(i=strlen(cod_maq)-1,j=strlen(bytes2)-1;j>=0;i--,j--)
                                {
                                    cod_maq[i]=toupper(bytes2[j]);
                                }
                }
                    else if(direccionamiento >=15)
                    {
                        if(buscaEtiqueta(t,operando,cod_tab,contr)==TRUE)
                        {
                            optab=toHex(cod_tab);
                            siguiente=toHex(valor)+stb[pos];
                            resta=optab-siguiente;
                            if(direccionamiento==15)
                            {
                                if(resta>=-128 && resta<=127)
                                {
                                    strcat(cod_maq,"00");
                                    sprintf(desplazamiento,"%x",resta);
                                    if(resta<0)
                                    {
                                        desplazamiento[0]=desplazamiento[strlen(desplazamiento)-2];
                                        desplazamiento[1]=desplazamiento[strlen(desplazamiento)-1];
                                        desplazamiento[2]='\0';
                                    }
                                    for(i=strlen(cod_maq)-1,j=strlen(desplazamiento)-1;j>=0;i--,j--)
                                    {
                                        cod_maq[i]=toupper(desplazamiento[j]);
                                    }
                                }
                                else
                                {
                                    printf("\n=== ERROR === el valor del desplazamiento no  se encuentra en el rango permitido -128,127");
                                    bandr=1;
                                }

                            }

                            else if(direccionamiento==16 )
                            {
                                if(resta>=-32768 && resta<=32767)
                                {
                                    strcat(cod_maq,"0000");
                                    sprintf(desplazamiento,"%x",resta);
                                    if(resta<0)
                                    {
                                        desplazamiento[0]=desplazamiento[strlen(desplazamiento)-4];
                                        desplazamiento[1]=desplazamiento[strlen(desplazamiento)-3];
                                        desplazamiento[2]=desplazamiento[strlen(desplazamiento)-2];
                                        desplazamiento[3]=desplazamiento[strlen(desplazamiento)-1];
                                        desplazamiento[4]='\0';
                                    }
                                    for(i=strlen(cod_maq)-1,j=strlen(desplazamiento)-1;j>=0;i--,j--)
                                    {
                                        cod_maq[i]=toupper(desplazamiento[j]);
                                    }
                                }
                                else
                                {
                                    printf("\n=== ERROR === el valor del desplazamiento no  se encuentra en el rango permitido -32768,32767");
                                    bandr=1;
                                }
                            }


                        }
                        else
                        {
                            printf("\n=== ERROR== la etiqueta no fue encontrada");
                        }

                    }
                if(s1==0 && bandrestante!= 1)
                {
                    strcpy(direccion,valor);
                    s1=1;

                }
                bandrestante=0;
                if(bandr!=1)
                {
                    posicion=strlen(codigo);
                    printf("\n******** CODIGO MAQUINA COMPLETO: %s ********\n",cod_maq);
                    strcat(codigo,restante);
                    strcpy(restante,"\0");
                    if(posicion+strlen(cod_maq)<=32)
                        strcat(codigo,cod_maq);
                    else///chekar no esta concatenando bn los caracteres (separandolos lo q es restante y lo q alcanza en el codigo de datos)
                    {
                        j=0;
                        for(i=0;i<strlen(cod_maq);i++)
                        {
                            if(posicion+i<32)
                            {
                                codigo[posicion+i]=cod_maq[i];
                            }

                            else
                            {
                                restante[j]= cod_maq[i];
                                bandrestante=1;
                                j++;
                            }
                        }
                        codigo[strlen(codigo)+i+1]='\0';
                        restante[j]='\0';
                    }
                    system("pause");
                }
                bandr=0;

            }
            if((toupper(codop[0])=='D' && toupper(codop[1])=='S' && strlen(codigo)>0) || strlen(codigo)==32 || strcmp(strupr(codop),"END")==0)
            {
                s1=0;
                if(strcmp(codigo,"\0")!=0)
                {
                    creaS1(codigo,direccion,&obj);
                    dec=(strlen(codigo)/2);
                    strcpy(codigo,"\0");
                }

            }
            if(bandrestante==1)
            {
                dec+=toHex(direccion);
                sprintf(hex,"%x",dec);
                strcpy(direccion,"0000");
                for(i=strlen(direccion)-1,j=strlen(hex)-1;j>=0;i--,j--)
                {
                    direccion[i]=hex[j];
                }
            }
            ///chekar la direccion si es la correcta pero en algun lugar es cambiada



        }

    }

    creaS9(&obj);
}

struct tabl leeTabla(FILE *tabla, struct tabl tab)
{
    int tam;
    char car;
    string caracter;
    int tipo;
    int cont=0,contidx5=0,idx9=0,idx16=0,preinc=0,acum=0, ind16b=0;
    int acumd=0, predec=0, posinc=0, posdec=0;

    fseek(tabla,0, SEEK_SET);
    while(fread(&car,sizeof( char ),1,tabla ) >0 && tam>0)
    {
        cont=0;
        caracter[0]=car;
        tam=fread(&car,sizeof( char ),1,tabla );
        if(car!= '|')
        {
            caracter[1]=car;
            caracter[2]='\0';
        }
        else
        caracter[1]='\0';

        tipo= atoi(caracter);
        tam= fread(&car,sizeof( char ),1,tabla );
        switch(tipo)
        {
            case 5:
                while(car!='\n')
                {
                   cont=0;
                   if(car!='|')
                    {
                        while(car!='&')
                        {
                            tab.idx5[contidx5][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.idx5[contidx5][cont]= '\0';
                        cont=0;
                        tam= fread(&car,sizeof( char ),1,tabla );
                        while(car!='|')
                        {
                            tab.cmidx5[contidx5][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.cmidx5[contidx5][cont]= '\0';
                        tam= fread(&car,sizeof( char ),1,tabla );

                    }
                    contidx5++;

                }


            break;

            case 6:
                while(car!='\n')
                {
                   cont=0;
                   if(car!='|')
                    {
                        while(car!='&')
                        {
                            tab.idx9[idx9][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.idx9[idx9][cont]= '\0';
                        cont=0;
                        tam= fread(&car,sizeof( char ),1,tabla );
                        while(car!='|')
                        {
                            tab.cmidx9[idx9][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.cmidx9[idx9][cont]= '\0';
                        tam= fread(&car,sizeof( char ),1,tabla );

                    }
                    idx9++;

                }
            break;

            case 7:
                while(car!='\n')
                {
                   cont=0;
                   if(car!='|')
                    {
                        while(car!='&')
                        {
                            tab.idx16[idx16][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.idx16[idx16][cont]= '\0';
                        cont=0;
                        tam= fread(&car,sizeof( char ),1,tabla );
                        while(car!='|')
                        {
                            tab.cmidx16[idx16][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.cmidx16[idx16][cont]= '\0';
                        tam= fread(&car,sizeof( char ),1,tabla );

                    }

                idx16++;
                }
            break;

            case 8:
                while(car!='\n')
                {
                   cont=0;
                   if(car!='|')
                    {
                        while(car!='&')
                        {
                            tab.preinc[preinc][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.preinc[preinc][cont]= '\0';
                        cont=0;
                        tam= fread(&car,sizeof( char ),1,tabla );
                        while(car!='|')
                        {
                            tab.cmpreinc[preinc][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.cmpreinc[preinc][cont]= '\0';
                        tam= fread(&car,sizeof( char ),1,tabla );
                    }

                preinc++;
                }
            break;

            case 9:
                while(car!='\n')
                {
                   cont=0;
                   if(car!='|')
                    {
                        while(car!='&')
                        {
                            tab.acum[acum][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.acum[acum][cont]= '\0';
                        cont=0;
                        tam= fread(&car,sizeof( char ),1,tabla );
                        while(car!='|')
                        {
                            tab.cmacum[acum][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.cmacum[acum][cont]= '\0';
                        tam= fread(&car,sizeof( char ),1,tabla );
                    }

                acum++;
                }
            break;

            case 10:
            tam= fread(&car,sizeof( char ),1,tabla );
                while(car!='\n')
                {
                   cont=0;
                   if(car!='|')
                    {
                        while(car!='&')
                        {
                            tab.ind16b[ind16b][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.ind16b[ind16b][cont]= '\0';
                        cont=0;
                        tam= fread(&car,sizeof( char ),1,tabla );
                        while(car!='|')
                        {
                            tab.cmind16b[ind16b][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.cmind16b[ind16b][cont]= '\0';
                        tam= fread(&car,sizeof( char ),1,tabla );
                    }
                ind16b++;
                }
            break;

            case 11:
            tam= fread(&car,sizeof( char ),1,tabla );
                while(car!='\n')
                {
                   cont=0;
                   if(car!='|')
                    {
                        while(car!='&')
                        {
                            tab.acumd[acumd][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.acumd[acumd][cont]= '\0';
                        cont=0;
                        tam= fread(&car,sizeof( char ),1,tabla );
                        while(car!='|')
                        {
                            tab.cmacumd[acumd][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.cmacumd[acumd][cont]= '\0';
                        tam = fread(&car,sizeof( char ),1,tabla );
                    }
                acumd++;
                }
            break;

            case 12:
            tam= fread(&car,sizeof( char ),1,tabla );
                while(car!='\n')
                {
                   cont=0;
                   if(car!='|')
                    {
                        while(car!='&')
                        {
                            tab.predec[predec][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.predec[predec][cont]= '\0';
                        cont=0;
                        tam= fread(&car,sizeof( char ),1,tabla );
                        while(car!='|')
                        {
                            tab.cmpredec[predec][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.cmpredec[predec][cont]= '\0';
                        tam= fread(&car,sizeof( char ),1,tabla );
                    }
                predec++;
                }
            break;

            case 13:
            tam= fread(&car,sizeof( char ),1,tabla );
                while(car!='\n')
                {
                   cont=0;
                   if(car!='|')
                    {
                        while(car!='&')
                        {
                            tab.posinc[posinc][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.posinc[posinc][cont]= '\0';
                        cont=0;
                        tam= fread(&car,sizeof( char ),1,tabla );
                        while(car!='|')
                        {
                            tab.cmposinc[posinc][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.cmposinc[posinc][cont]= '\0';
                        tam= fread(&car,sizeof( char ),1,tabla );
                    }
                posinc++;
                }
            break;

            case 14:
            tam= fread(&car,sizeof( char ),1,tabla );
                while(car!='\n' && tam>0)
                {
                   cont=0;
                   if(car!='|')
                    {
                        while(car!='&')
                        {
                            tab.posdec[posdec][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.posdec[posdec][cont]= '\0';
                        cont=0;
                        tam= fread(&car,sizeof( char ),1,tabla );
                        while(car!='|')
                        {
                            tab.cmposdec[posdec][cont]= car;
                            cont++;
                            tam= fread(&car,sizeof( char ),1,tabla );
                        }
                        tab.cmposdec[posdec][cont]= '\0';
                        tam= fread(&car,sizeof( char ),1,tabla );
                    }
                posdec++;
                }
            break;

            default: printf("ERROR DE LECTURA");
            break;
        }


    }
return tab;

}

void obtieneCM(int direccionamiento, string operando ,string bytes,struct tabl tab)
{

    int i;
    string aux;

    ponerN(operando,aux);
    switch(direccionamiento)
    {
        case 5:
                for(i=0;i<128;i++)
                {
                    if(strcmp(operando,tab.idx5[i])==0)
                    {
                        strcpy(bytes,tab.cmidx5[i]);
                        break;
                    }
                }
                break;
        case 6:
                for(i=0;i<8;i++)
                {
                    if(strcmp(aux,tab.idx9[i])==0)
                    {
                        strcpy(bytes,tab.cmidx9[i]);
                        break;
                    }
                }

                break;
        case 7:
                for(i=0;i<4;i++)
                {
                    if(strcmp(aux,tab.idx16[i])==0)
                    {
                        strcpy(bytes,tab.cmidx16[i]);
                        break;
                    }
                }

                break;
        case 8:
                for(i=0;i<24;i++)
                {
                    if(strcmp(operando,tab.preinc[i])==0)
                    {
                        strcpy(bytes,tab.cmpreinc[i]);
                        break;
                    }
                }

                break;
        case 9:
                for(i=0;i<12;i++)
                {
                    if(strcmp(operando,tab.acum[i])==0)
                    {
                        strcpy(bytes,tab.cmacum[i]);
                        break;
                    }
                }

                break;
        case 10:

                for(i=0;i<4;i++)
                {
                    if(strcmp(aux,tab.ind16b[i])==0)
                    {
                        strcpy(bytes,tab.cmind16b[i]);
                        break;
                    }
                }

                break;
        case 11:
                for(i=0;i<4;i++)
                {

                    if(strcmp(operando,tab.acumd[i])==0)
                    {

                        strcpy(bytes,tab.cmacumd[i]);
                        break;
                    }
                }
                break;
        case 12:
                for(i=0;i<24;i++)
                {
                    if(strcmp(operando,tab.predec[i])==0)
                    {
                        strcpy(bytes,tab.cmpredec[i]);
                        break;
                    }
                }

                break;
        case 13:
                for(i=0;i<24;i++)
                {
                    if(strcmp(operando,tab.posinc[i])==0)
                    {
                        strcpy(bytes,tab.cmposinc[i]);
                        break;
                    }
                }
                break;
        case 14:
                for(i=0;i<24;i++)
                {
                    if(strcmp(operando,tab.posdec[i])==0)
                    {
                        strcpy(bytes,tab.cmposdec[i]);
                        break;
                    }
                }
                break;
        default:
                break;
    }
}
void ponerN(string operando,string aux)
{
    unsigned int i,band=0,cont=0;
    string aux2;
    for(i=0;i<strlen(operando);i++)
    {
        if(operando[i]==','|| band==1)
        {
            aux2[cont]=operando[i];
            band=1;
            cont++;
        }
    }
    aux2[cont]='\0';
    if(operando[0]=='-')
    {
        strcpy(aux,"-n");
    }
    else if(operando[0]=='[')
    {
        strcpy(aux,"[n");
    }
        else
        {
            strcpy(aux,"n");
        }
    strcat(aux,aux2);
}
int  leeTabsim(FILE *tabsim, struct tab t[100])
{
      int tam=0,cont=0;
    char caracter;
    int contr=0;
    fseek(tabsim,0, SEEK_SET );

    do
    {
        tam=fread(&caracter,sizeof(char),1,tabsim);
        while(caracter!='\n' && tam>0)
        {   cont=0;
            while(caracter !=' ' && caracter!= '\t')
            {

                t[contr].etiqueta[cont]=caracter;
                cont++;
                tam=fread(&caracter,sizeof(char),1,tabsim);
            }
            t[contr].etiqueta[cont]='\0';
            tam=fread(&caracter,sizeof(char),1,tabsim);
            cont=0;
            while(caracter!= '\n')
            {

                t[contr].valor[cont]=caracter;
                cont++;
                tam=fread(&caracter,sizeof(char),1,tabsim);
            }
            t[contr].valor[cont]='\0';
        }
        contr++;
    }while(tam>0);
    return contr;
}

int buscaEtiqueta( struct tab t[100], string etiqueta, string cod_maq, int contr)
{
    int i;
    for(i=0; i< contr; i++)
    {
        if (strcmp(etiqueta, t[i].etiqueta)==0)
        {
            strcpy(cod_maq, t[i].valor);
            return 1;
        }
    }
    return 0;
}
int isEtiqueta2(string etiqueta)
{
    int estado=0,band=FALSE,contador=0;
    do
    {
        switch(estado)
        {
            case 0:
                    if(isalpha(etiqueta[contador])==FALSE)
                        return FALSE;
                    estado=1;
                    contador++;
                        break;
            case 1:
                    if(isalpha(etiqueta[contador])==FALSE && isdigit(etiqueta[contador])==FALSE && etiqueta[contador] !='_')
                    {
                        band=TRUE;

                    }
                    contador++;
                    break;

            default:
                    break;

        }
    }while(etiqueta[contador]!='\0');
    if(band==TRUE)
    {
        return FALSE;
    }

    if(contador>8)
    {
         return FALSE;
    }
    return TRUE;
}
int dirConst(string codop)
{
    if(strcmp(strupr(codop),"DB")==0 || strcmp(strupr(codop),"DC.B")==0 || strcmp(strupr(codop),"FCB")==0)
    {
        return 0;
    }
    else if(strcmp(strupr(codop),"DW")==0 || strcmp(strupr(codop),"DC.W")==0 || strcmp(strupr(codop),"FDB")==0)
    {
        return 1;
    }
    else if(strcmp(strupr(codop),"FCC")==0)
    {
        return 2;
    }
    return -1;
}
int dirReserv(string codop)
{
    if(strcmp(strupr(codop),"DS")==0 || strcmp(strupr(codop),"DS.B")==0 || strcmp(strupr(codop),"RMB")==0)
    {
        return 0;
    }
    else if(strcmp(strupr(codop),"DS.W")==0 || strcmp(strupr(codop),"RMW")==0 )
    {
        return 1;
    }

    return -1;
}

void creaS0(string programa,FILE ** obj)
{
    int abs=0;
    int temp,acum=0;
    unsigned i, cont=0;
    string cadena,temporal,checksum,s0,hex,ruta;
    strcpy(s0,"S0");
    while(cont<strlen(programa))
    {
        if (programa[cont]=='\\' || programa[cont]==':')
        abs=1;
        cont++;
    }
    if(abs==0)
    {


        obtieneruta(ruta);
        strcat(ruta,programa);
        temp=strlen(ruta)+2+1+1;
        sprintf(cadena,"%x",temp);
        strcat(s0,cadena);
        strcat(s0,"0000");

        for(i=0;i<strlen(ruta);i++)
        {
            acum+=ruta[i];
            sprintf(hex,"%x",ruta[i]);
           strcat(s0,hex);
        }
        strcat(s0,"0A");
        acum+=temp;
        sprintf(temporal,"%x",65535-acum);
        checksum[0]=temporal[strlen(temporal)-2];
        checksum[1]=temporal[strlen(temporal)-1];
        checksum[2]='\n';
        checksum[3]='\0';
        strcat(s0,checksum);
        fwrite(strupr(s0),strlen(s0),1,*obj);
    }
    else
    {
        temp=strlen(programa)+2+1+1;
        sprintf(cadena,"%x",temp);
        strcat(s0,cadena);
        strcat(s0,"0000");

        for(i=0;i<strlen(programa);i++)
        {
            acum+=programa[i];
            sprintf(hex,"%x",programa[i]);
           strcat(s0,hex);
        }
        strcat(s0,"0A");
        acum+=temp;
        sprintf(temporal,"%x",65535-acum);
        checksum[0]=temporal[strlen(temporal)-2];
        checksum[1]=temporal[strlen(temporal)-1];
        checksum[2]='\n';
        checksum[3]='\0';
        strcat(s0,checksum);
        fwrite(strupr(s0),strlen(s0),1,*obj);
    }

}

void creaS1(string datos,string direccion,FILE ** obj)
{
    string s1,hex,hex2,temp,checksum;
    unsigned int j,contador=0,longitud;
    int i;

    strcpy(s1,"S1");
    longitud=(strlen(datos)/2+3);
    sprintf(hex,"%x",longitud);
    strcpy(hex2,"00");
    j=1;
    for(i=strlen(hex)-1;i>=0;i--)
    {
        hex2[j]=hex[i];
        j--;
    }
    hex2[2]='\0';
    strcat(s1,hex2);
    strcat(s1,direccion);
    strcat(s1,datos);
    //suma byte en byte de los datos
    for(i=0,j=1;j<strlen(datos);i+=2,j+=2)
    {
        temp[0]=datos[i];
        temp[1]=datos[j];
        temp[2]='\0';
        contador+=toHex(temp);
    }
    //se suman al contador los 2 bytes de la direccion
    temp[0]=direccion[0];
    temp[1]=direccion[1];
    temp[2]='\0';
    contador+=toHex(temp);
    temp[0]=direccion[2];
    temp[1]=direccion[3];
    temp[2]='\0';
    contador+=toHex(temp);
    //se suma el byte de la longitud del registro
    contador+=longitud;
    //se saca complemento a 1 del contador luego se convierte a hex
    contador=65535-contador;
    sprintf(checksum,"%x",contador);
    //se toma solo los 2 ultimos caracteres
    checksum[0]=checksum[2];
    checksum[1]=checksum[3];
    checksum[2]='\0';
    strcat(s1,checksum);
    fwrite(strupr(s1),strlen(s1),1,*obj);
    fwrite("\n",1,1,*obj);
}


void obtieneruta(string ruta)
{
    char CurrentPath[_MAX_PATH];
    // use the function to get the path
    getcwd(CurrentPath, _MAX_PATH);
    sprintf(ruta, CurrentPath);
    strcat(ruta,"\\");
}
void creaS9(FILE **obj)
{
    fwrite("S9030000FC",10,1,*obj);
}
#endif // ENSAMBLADOR_H_INCLUDED
