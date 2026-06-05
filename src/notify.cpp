#include "notify.h"
#include <Arduino.h>

#define POS(x) ((x)>=0?(x):(0))

struct sBadge
{
  byte UID[7];
  byte lnUID;  
  Profile profile;
  char nomUtilisateur[35];
  bool activite = false;
  int  nbAcces =0;
} ;

#define lnRes 10
static int nbBadgeCreer =0;
static struct sBadge reserve[lnRes];


// creation
tBadge badgeCreer()
{
    if(nbBadgeCreer>=10)
        return nullptr;
    tBadge b=&reserve[nbBadgeCreer];
    nbBadgeCreer++;

    return b;
}

void badgeInit(tBadge *b,byte lnUID,const byte UID[7],Profile profile,const char nomUtilisateur[35],bool activite,int nbAcces)
{
    if(b==nullptr || *b==nullptr)
    {
        fprintf(stderr,"Initialisation echec depuis badgeInit\n");
        return ;
    }
    
    (*b)->lnUID=lnUID;

    for(int i=0;i<lnUID;i++)
        (*b)->UID[i]=UID[i];

    (*b)->profile=profile;
  
    for(int i=0;nomUtilisateur[i]!='\0' && i<34;i++)
        (*b)->nomUtilisateur[i]=nomUtilisateur[i];
    (*b)->nomUtilisateur[34]='\0';

    (*b)->activite=activite;

    (*b)->nbAcces=nbAcces;
}

//manipulation
tBadge badgeChercher(byte bUID[7],byte bLnUID)
{
    if(bUID==nullptr)return nullptr;

    for(int i=0;i<nbBadgeCreer;i++)
    {
        if(reserve[i].lnUID!=bLnUID)
        continue;
        bool drapeaux=true;
        for(byte b=0;b<reserve[i].lnUID;b++)
        {
            if(reserve[i].UID[b]!=bUID[b])
            {
                drapeaux=false;
                break;
            }
        }  
        if(drapeaux==true)
        return &reserve[i];
    }

    return nullptr;
}
void badgeActiver(tBadge b,bool activite)
{
    for(int i=0;i<lnRes;i++)
    {
        if(&reserve[i]==b)
            reserve[i].activite=activite;
            return;
    }
    fprintf(stderr,"Badge non reconnu\n");
}
void badgeAcceder(tBadge b)
{
    if(b==nullptr)
    return ;
    if(!b->activite)
    return ;

    switch (b->profile)
    {
    case admin :
        break;
    case owner:
        if(b->nbAcces>0)
            b->nbAcces--;
        b->nbAcces=POS(b->nbAcces);
        if(b->nbAcces==0)
            b->activite=false;
        printf("Veuillez revharger votre module d'accée");
        break;
    case temp:
        if(b->nbAcces>0)
            b->nbAcces--;
        b->nbAcces=POS(b->nbAcces);
        if(b->nbAcces==0)
            badgeSupr(b);
        break;
    default:
        break;
    }

}
void badgeSupr(tBadge b)
{
    if(b==nullptr)return;
    
    //recherche d'indice de placement de b en reserve
        int indice =-1;
        for(int i=0;i<nbBadgeCreer;i++)
        {
            if(&reserve[i]==b)
            {
                indice=i;
                break;
            }
        }
        if(indice==-1)return;

    //decalage(soft supression)
        for(int i=indice;i<nbBadgeCreer-1;i++)
            reserve[i]=reserve[i+1];
            
    //nettoyage
        reserve[nbBadgeCreer-1].lnUID=0;
        reserve[nbBadgeCreer-1].activite=false;   
        nbBadgeCreer--;

}


//recuperation de valeur
bool badgeActivite(tBadge b){ return b->activite;}
char* badgeNom (tBadge b){return b->nomUtilisateur;}
Profile badgeProfile(tBadge b){return b->profile;}
int badgeAccesRestant(tBadge b){return b->nbAcces;}
