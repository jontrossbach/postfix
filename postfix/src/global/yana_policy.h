#ifndef _YANA_POLICY_H_
#define _YANA_POLICY_H_

/*++
/* NAME
/*	yana_policy 3h
/* SUMMARY
/*	yet another peer-name/address policy
/* SYNOPSIS
/*	#include <yana_policy.h>
/* DESCRIPTION
* .nf

 /*
  * External interface.
  */
typedef struct YANA_POLICY {
    struct MAPS *maps;
    int     match_parent;
    int     error;
} YANA_POLICY;

extern YANA_POLICY *yana_policy_create(const char *, const char *, int);
extern const char *yana_policy_lookup(YANA_POLICY *, const char *, const char *);
extern void yana_policy_free(YANA_POLICY *);

#define yana_policy_error(policy) ((policy)->error)

/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	porcupine.org
/*--*/

#endif
