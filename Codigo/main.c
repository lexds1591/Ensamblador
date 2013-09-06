/*
    Taller de programacion de sistemas

    Hecho por:
      - Gabriel Ortiz Valdovinos
      - Alejandro Duarte Sánchez

    version: 05/06/2012
    Hora: 10:54 PM
*/
///Practica Actual: 10

/*******************  BOX de comentario  ***************************************


*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#include "conversion.h"
#include "arbol.h"
#include "ensamblador.h"



int main()
{
    int fd,fd2;
    struct  codop c[TAM];
    int cont,tbpc[15],nb,org,dec,pos,bandorg,bandcom,bandDesp,bandrang,valido;
    string modo;
    string resp;
    int contador,bandend,tam,band2=FALSE,stb[15],DIR_INIC,CONTLOC,direccionamiento;
    string etiqueta,codop,operando,contloc,buffer,cm[15];
    int i,cl,org2,bandc,et_duplicada;
    TApt arbol;
    int rango;
    FILE *tabla;
    struct tabl tab;

    char caracter;
    //pedir archivo
    string temporal,programa;
    FILE  *temp,*tabsim;
    tabla = fopen("tabla.txt","r");
    fd2 = open(  "TABOP.txt", 2);
    do
    {
    /*********  inicializar   *******/
        inicializa(&arbol);
        nb=0;
        org=FALSE;
        dec=0;
        pos=-1;
        bandorg=0;
        bandcom=0;
        bandDesp=0;
        bandrang=0;
        valido=0;
        bandend=0;
        tam=1;
        band2=FALSE;
        DIR_INIC=0;
        CONTLOC=0;
        direccionamiento=0;
        strcpy(etiqueta,"NULL");
        strcpy(codop,"NULL");
        strcpy(operando,"NULL");
        org2=0;
        bandc=0;
        et_duplicada=0;
        rango=0;
        strcpy(contloc,"0000");
    /**********************************/
        printf("\nIntroduce el nombre del archivo a abrir con su extension: ");
        gets(programa);

        for(i=0;programa[i]!='.';i++)
            temporal[i]=programa[i];
        temporal[i]='\0';

        strcat(temporal,"_tmp.txt");
        temp = fopen(temporal,"w+");
        tabsim= fopen("TABSIM.txt","w+");

        if( (fd = open( programa , 2)) > 0 && temp!=NULL)
        {
            if(tabla==NULL)
            {
                printf("No se pudo abrir tabla.txt\n");

                system("pause");
                return EXIT_FAILURE;

            }
            else
            {
                tab= leeTabla(tabla, tab);

            }

            if( (fd2 >0))
            {

                cont=leeTabop(fd2,c);
                lseek(fd,0,0);

                while(read(fd,&caracter,sizeof( char ) ) >0 && tam>0)
                {
                    //inicializar
                    contador=0;
                    bandorg=0;
                    bandcom=0;
                    tam=1;
                    nb=0;
                    et_duplicada=0;
                    rango=0;
                    strcpy(contloc,"0000");
                    strcpy(etiqueta,"NULL");
                    strcpy(codop,"NULL");
                    strcpy(operando,"NULL");
                    /*//////////////*/
                    if(caracter!='\n')
                        lseek(fd,-1,SEEK_CUR);

                     while(caracter!='\n' && tam>0)
                     {
                         tam=read(fd,&caracter,sizeof( char ) );

                         if(isComentario(caracter)== TRUE)
                         {
                            printf("\nCOMENTARIO\n");
                            while( caracter !='\n' )
                            read(fd, &caracter, sizeof(char));
                            bandcom=1;

                        }
                        else
                        {

                            while(caracter!='\t' && caracter!=' ' && caracter !='\n')
                                {
                                    etiqueta[contador]=caracter;
                                    contador++;
                                    tam=read(fd,&caracter,sizeof( char ) );
                                    if(caracter=='\t' || caracter==' ' || caracter !='\n')
                                        etiqueta[contador]='\0';
                                }
                            while(caracter=='\t' || caracter==' ')
                                tam=read(fd,&caracter,sizeof( char ) );
                                contador=0;
                            while(caracter!='\t' && caracter!=' ' && caracter!='\n'&& tam>0)
                            {
                                codop[contador]=caracter;
                                contador++;
                                tam=read(fd,&caracter,sizeof( char ) );
                                if(caracter=='\t' || caracter==' ' || caracter=='\n' || tam<1)
                                        codop[contador]='\0';

                            }
                            while((caracter=='\t' || caracter==' ' )&& tam>0)
                                tam=read(fd,&caracter,sizeof( char ) );
                            contador=0;

                            while(caracter!='\n'&& tam>0)
                            {
                                operando[contador]=caracter;
                                tam=read(fd,&caracter,sizeof( char ) );
                                contador++;
                                if(caracter=='\n' || tam==0)
                                    operando[contador]='\0';
                            }

                            printf("\nETIQUETA: %s",etiqueta);
                            if(strcmp(etiqueta,"NULL")!=0)
                                if(isEtiqueta(etiqueta)==1)
                                {
                                    inserta ( etiqueta, &arbol,&et_duplicada );//busca si existe una ETIQUETA repetida
                                    //printf("\nETiqueta: %i",et_duplicada);

                                }

                            printf("\nCODOP: %s",codop);
                            if(strcmp(strupr(codop),"END")==0  && bandend==0)
                            {
                                bandend=1; //activa q ya existe un END
                                if(strcmp(operando,"NULL")!=0)
                                    band2=TRUE;
                            }
                            else if(strcmp(strupr(codop),"END") == 0  && bandend==TRUE)
                                printf("\n\n\t=== ERROR === La directiva END solo puede existir una vez");


                            else if(strcmp(strupr(codop),"NULL")!=0)
                                    isCodop(codop);
                                    else
                                        printf("\nERROR: Siempre debe de haber un codigo de operacion");

                            printf("\nOPERANDO: %s\n",operando);
                            bandc= aceptaCodop(&org2,codop);
                            if(band2==TRUE)
                                printf("\tERROR: no debe de haber operando despues del END\n\n");

                            //identifica si es EQU
                            if(strcmp(strupr(codop),"EQU")==0 )
                            {
                                if( strcmp(strupr(etiqueta),"NULL")!=0)
                                {
                                    if(strcmp(strupr(operando),"NULL")!=0)
                                    {
                                       if((dec=revisaBase(operando, &rango))!=1)
                                          if(dec<0 && dec>65535)
                                          {
                                              printf("\n\n\t=== ERROR === Fuera de rango");
                                              rango=1;
                                          }

                                    }
                                    else
                                        printf("\n\n\t=== ERROR == Debe existir una operando en el codop: EQU");
                                }
                                else
                                    printf("\n\n\t=== ERROR == Debe existir una etiqueta en el codop: EQU");

                            }//identifica si es una directiva  valida
                                else if( strcmp(strupr(codop),"DB")==0 || strcmp(strupr(codop),"DC.B")==0 || strcmp(strupr(codop),"FCB")==0 ||
                                       strcmp(strupr(codop),"DW")==0 || strcmp(strupr(codop),"DC.W")==0 || strcmp(strupr(codop),"FDB")==0 ||
                                        strcmp(strupr(codop),"FCC")==0)
                                    {
                                        nb=isDirConst(codop,operando, &rango);
                                        if(nb!=-1)
                                        {
                                            valido=1;
                                            printf("\n\t Es Directiva de constantes");
                                        }

                                        else
                                            printf("\n\t\t=== ERROR ===: Formato incorrecto en FCC debe estar entre comillas ");
                                    }
                                    else if( strcmp(strupr(codop),"DS")==0 || strcmp(strupr(codop),"DS.B")==0 || strcmp(strupr(codop),"RMB")==0 ||
                                            strcmp(strupr(codop),"DS.W")==0 || strcmp(strupr(codop),"RMW")==0 )
                                        {
                                            nb=isDirReserva(codop,operando, &rango);
                                            printf("\n\t Es Directiva de reserva de espacio en memoria");
                                            valido=1;
                                        }
                                        else if( strcmp(strupr(codop),"END")!=0 && strcmp(strupr(codop),"ORG")!=0)
                                        {
                                            ///localiza en el tabop el codop
                                            if( strcmp(strupr(operando),"NULL") ==0 && strcmp(strupr(codop),"END")!=0)
                                                valido=localiza(c,codop,0,cont,modo,stb,tbpc,cm);
                                                else if( strcmp(strupr(operando),"NULL") !=0 && strcmp(strupr(codop),"END")!=0)
                                                    valido=localiza(c,codop,1,cont,modo,stb,tbpc,cm);
                                                printf("\nValido: %i",valido);
                                            printf("\n\n");
                                            pos=identificar(operando,modo,tbpc,stb, &rango,&direccionamiento);
                                            if(pos!=-1)///se identifica el tipo de operando y se asigna la suma total de bytes
                                            {

                                                nb=stb[pos];

                                            }
                                            printf("\n\n");
                                            //system("pause");
                                        }
                                            else if( strcmp(strupr(codop),"ORG") == 0 && org==FALSE )
                                            {
                                                org=TRUE;
                                                bandorg=TRUE;
                                                DIR_INIC=revisaBase(operando, &rango);
                                                if( strcmp(strupr(operando),"NULL")!=0 && DIR_INIC!=-1)//identifica si es ORG valido
                                                {
                                                    CONTLOC=DIR_INIC;
                                                }
                                                else if(  strcmp(strupr(operando),"NULL")==0 )
                                                {
                                                    printf("\n\n\t=== ERROR === : Debe existir codigo de operacion en ORG");
                                                    org=FALSE;
                                                }
                                                else if(DIR_INIC==-1)
                                                    org=FALSE;
                                                if( strcmp(strupr(etiqueta),"NULL")!=0)
                                                {
                                                    printf("\n\t === ERROR === ORG no permite etiqueta\n");
                                                    org=FALSE;
                                                }

                                            }
                                            else if(strcmp(strupr(codop),"ORG") == 0 && org==TRUE)
                                                printf("\n\t=== Error === Se encontro mas de un ORG\n");

                        }


                     }
                        //printf("\nBandcom: %i\nBanend: %i\n BandDesp: %i\nCONTLOC: %i\nbandrang:%i", bandcom, bandend, bandDesp,CONTLOC,bandrang);
                        if( bandcom!=1 && (bandend!=1 || bandDesp==0)&&
                           (CONTLOC>=0 && CONTLOC<=65535 ) && bandrang==0  ) //escribe en el archivo temporal y TAMSIM
                        {
                            if(strcmp(strupr(codop),"EQU")!=0)
                                sprintf(buffer,"%x",CONTLOC);
                            else if(strcmp(strupr(codop),"EQU")==0 )
                                    sprintf(buffer,"%x",dec);
                            cl=3;
                            for(i=strlen(buffer)-1;i>=0;i--)
                            {

                                contloc[cl]=toupper(buffer[i]);
                                cl--;
                            }
                            if(nb!=-1 && bandc==1 && et_duplicada==0 )
                            {
                                //printf("\nCODOP: %s\nOperando: %s\nRango: %i\n", codop, operando, rango);

                                if(((strcmp(strupr(codop),"ORG")==0 && strcmp(strupr(operando),"NULL")!=0 )||valido==1 || strcmp(strupr(codop),"END")==0) && rango==0)
                                    creaTemporal(temp,contloc,etiqueta,codop,operando);

                                    //printf("\nOPERANDOasda: %s", operando);
                                else if( (strcmp(strupr(etiqueta),"NULL")!=0 && strcmp(strupr(codop),"EQU")==0)
                                       && (strcmp(strupr(codop),"EQU")==0 && strcmp(strupr(operando),"NULL")!=0) && rango==0)
                                                creaTemporal(temp,contloc,etiqueta,codop,operando);
                            }
                            else if(et_duplicada!=0)
                                nb=0;

                            if(strcmp(strupr(etiqueta),"NULL")!=0 && nb!=-1 &&
                               bandc==1 && et_duplicada==0)
                               {
                                   if((((strcmp(strupr(etiqueta),"NULL")!=0 && strcmp(strupr(codop),"EQU")==0)
                                   &&(strcmp(strupr(codop),"EQU")==0 && strcmp(strupr(operando),"NULL")!=0)) ||valido==1)&& rango==0)
                                        creaTabsim(tabsim,etiqueta,contloc);
                               }

                            if(bandDesp==0 && bandend==0)
                                CONTLOC+=nb;// se asigna al contador de localidades la suma total de bytes
                            if(bandend==1 && strcmp(strupr(codop),"END")!=0)
                                bandDesp=1;
                        }
                        else if(CONTLOC>65535 && bandrang==0)
                        {
                            printf("\n\t=== ERROR ===: contador de localidades fuera de rango");
                            bandrang=1;
                        }
                }
                if(bandend!=1)
                    printf("\nERROR: No se encontro el END\n\n");
                else if(bandend==1 && bandDesp==1)
                {
                    printf("\n=== ERROR=== : Se encontraron lineas despues del END");
                    bandDesp=2;
                }
                if(CONTLOC<=65535)
                    printf("\nLONGITUD EN BYTES: %i\n",CONTLOC-DIR_INIC);
                else// imprime cero cuando la cantidad de bytes es mayor al rango
                    printf("\nLONGITUD EN BYTES: %i\n",0);
            //system("cls");
            revisaTemporal(fd2, temp,cont,c,tab,tabsim,programa);
            }
            else
            {
               perror("\nArchivo TABOP.txt no pudo ser abierto\n");
               system("pause");
               return EXIT_FAILURE;
            }
        }
        else
        {
            printf("\nArchivo %s",programa);
            perror(" no pudo ser abierto\n");
        }
        close(fd);
        do
        {
            printf("\n\n\t\tDESEA ABRIR OTRO ARCHIVO?(Y/N): ");
            fflush(stdin);
            gets(resp);
            if(strcmp(strupr(resp),"N")!=0 && strcmp(strupr(resp),"Y")!=0)
            {
                printf("\n\n\t==== RESPUESTA INVALIDA ====\n");
                system("pause");
            }
        }while(strcmp(strupr(resp),"N")!=0 && strcmp(strupr(resp),"Y")!=0);

        system("cls");
    }while(strcmp(strupr(resp),"N")!=0);


    return EXIT_SUCCESS;
}

