#ifndef ARBOL_H_INCLUDED
#define ARBOL_H_INCLUDED



struct Nodo{

    string dato;
    struct Nodo *der;
    struct Nodo *izq;
};

typedef struct Nodo *TApt;

void inicializa(TApt *R);
void inserta(string x,TApt *R, int *et_duplicada);
TApt busca(TApt R,string x);

void inicializa(TApt *R){

    *R=NULL;
}


void inserta(string x,TApt *R, int *et_duplicada)
{


    if(*R == NULL){
        *R=(TApt)malloc(sizeof(struct Nodo));
        strcpy((*R)->dato,x);
        (*R)->der=NULL;
        (*R)->izq=NULL;
    }
    else {
        if(strcmp(x,(*R)->dato)>0)
            inserta(x,&(*R)->der,et_duplicada);
         else
            if(strcmp(x,(*R)->dato)<0)
            inserta(x,&(*R)->izq,et_duplicada);
            else{
                printf("\n\n\t=== ERROR: ETIQUETA REPETIDA===");
                *et_duplicada= 1;
            }
    }

}


TApt busca(TApt R,string x){
    TApt P;
    P=R;

    if(P == NULL)
        return NULL;

    if(strcmp(x,P->dato)>0)
        return busca(P->der,x);

    if(strcmp(x,R->dato)<0)
        return busca(P->izq,x);

    return P;
}

#endif // ARBOL_H_INCLUDED
