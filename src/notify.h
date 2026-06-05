#ifndef NOTIFY_H
#define NOTIFY_H

#include <Arduino.h>

typedef struct sBadge* tBadge;
enum Profile{admin ,owner,temp};

extern tBadge badgeCreer();
extern void badgeInit(tBadge *b,byte lnUID,const byte UID[7],Profile profile,const char nomUtilisateur[35],bool activite,int nbAcces);

extern tBadge badgeChercher(byte UID[7],byte bLnUID);
extern void badgeActiver(tBadge b,bool activite);
extern void badgeAcceder(tBadge b);
extern void badgeSupr(tBadge b);

extern bool badgeActivite(tBadge b);
extern char* badgeNom (tBadge b);
extern Profile badgeProfile(tBadge b);
extern int badgeAccesRestant(tBadge b);

#endif