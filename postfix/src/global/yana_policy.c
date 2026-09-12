/*++
/* NAME
/*	yana_policy 3
/* SUMMARY
/*	yet another peer-name/address policy
/* SYNOPSIS
/*	#include <yana_policy.h>
/*
/*	YANA_POLICY *yana_policy_create(
/*	const char *param_name,
/*	const char *map_names,
/*	int	match_parent)
/*
/*	const char *yana_policy_lookup(
/*	YANA_POLICY *policy,
/*	const char *peer_name,
/*	const char *peer_addr)
/*
/*	int	yana_policy_error(YANA_POLICY *policy)
/*
/*	void	yana_policy_free(YANA_POLICY *policy)
/* DESCRIPTION
/*	yana_policy_init() opens zero or more lookup tables and returns
/*	a pointer to YANA_POLICY object.
/*
/*	yana_policy_log_level() returns the log_level value for the
/*	specified peer name or address, or null if not found. In the
/*	policy specify a "DUNNO" result to terminate the search and to
/*	pretend that nothing was found.
/*
/*	yana_policy_error() returns the dictionary error status from
/*	the last table lookup.
/*
/*	yana_policy_free() destroys a YANA_POLICY object.
/*
/*	Arguments:
/* .IP param_name
/*	The name of the configuration that supplied the map_names.
/* .IP map_names
/*	Zero or more type:table instances, separated by comma or
/*	whitespace. Specify a null pointer or empty string if information
/*	is not available.
/* .IP match_parent
/*	Typically, this will be the result of match_parent_style() lookup.
/*	If non-zero, a domain name in a policy will match the parent
/*	domain of a peer name. Otherwise, a .domain will match.
/* DIAGNOSTICS
/*	yana_policy_lookup() logs a warning when a peer name is malformed
/*	or if it contains an IP address, or when a peer address is
/*	malformed. Such names or addresses will never match policy.
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	porcupine.org
/*--*/

 /*
  * System library.
  */
#include <sys_defs.h>
#include <string.h>

 /*
  * Utility library.
  */
#include <midna_domain.h>
#include <msg.h>
#include <mymalloc.h>
#include <split_at.h>
#include <stringops.h>
#include <valid_hostname.h>

 /*
  * Global library.
  */
#include <maps.h>
#include <yana_policy.h>

 /*
  * Unlike debug_peer_list which has the same effect for all matching peers,
  * this implementation returns a result that may differ between matching
  * peers.
  */

/* yana_policy_create - initialize */

YANA_POLICY *yana_policy_create(const char *map_param,
				        const char *map_names,
				        int match_parent)
{
    YANA_POLICY *policy;

    policy = (YANA_POLICY *) mymalloc(sizeof(*policy));
    policy->maps = maps_create(map_param, map_names,
	      DICT_FLAG_FOLD_FIX | DICT_FLAG_LOCK | DICT_FLAG_UTF8_REQUEST);
    policy->match_parent = match_parent;
    return (policy);
}

/* yana_policy_lookup - look up peer-specific log_level */

const char *yana_policy_lookup(YANA_POLICY *policy,
			               const char *peer_name,
			               const char *peer_addr)
{
    int     delim;
    const char *result;
    char   *addr;
    const char *aname, *name, *next;
    int     flags;

#define FULL    0
#define PARTIAL DICT_FLAG_FIXED

    /*
     * These will be updated only if we actually attempt to look up data.
     */
    result = 0;
    policy->error = 0;

    /*
     * Match the peer name first. To avoid ambiguity (insecurity!) with
     * unnormalized U-label forms and unnormalized label separators, the
     * policy contains A-label forms, and the evaluator converts queries from
     * U-label form to A-label form.
     */
    if (peer_name && *peer_name) {
	if (!valid_hostname(peer_name, DONT_GRIPE)) {
	    msg_warn("%s: ignoring malformed peer name: '%s'",
		     __func__, peer_name);
	} else if (valid_hostaddr(peer_name, DONT_GRIPE)) {
	    msg_warn("%s: ignoring numeric peer name: '%s'",
		     __func__, peer_name);
	} else {
#ifndef NO_EAI
	    if (!allascii(peer_name)) {
		if ((aname = midna_domain_to_ascii(peer_name)) == 0) {
		    msg_warn("%s: ignoring malformed peer name: '%s'",
			     __func__, peer_name);
		    peer_name = "";
		} else {
		    peer_name = aname;
		}
	    }
#endif
	    flags = FULL;
	    for (name = peer_name; *name != 0; name = next) {
		if ((result = maps_find(policy->maps, name, flags)) != 0
		    || (policy->error = policy->maps->error) != 0)
		    break;
		if ((next = strchr(name + 1, '.')) == 0)
		    break;
		if (policy->match_parent)
		    next += 1;
		flags = PARTIAL;
	    }
	}
    }

    /*
     * Match the peer address.
     */
    if (result == 0 && policy->error == 0 && peer_addr && *peer_addr) {
	if (!valid_hostaddr(peer_addr, DONT_GRIPE)) {
	    msg_warn("%s: ignoring malformed peer address: '%s'",
		     __func__, peer_addr);
	} else {
	    addr = mystrdup(peer_addr);
#ifdef HAS_IPV6
	    if (strchr(addr, ':') != 0)
		delim = ':';
	    else
#endif
		delim = '.';
	    flags = FULL;
	    do {
		if ((result = maps_find(policy->maps, addr, flags)) != 0
		    || (policy->error = policy->maps->error) != 0)
		    break;
		flags = PARTIAL;
	    } while (split_at_right(addr, delim));
	    myfree(addr);
	}
    }

    /*
     * Wrap up.
     */
    return (result && strcasecmp(result, "DUNNO") ? result : 0);
}

/* yana_policy_free - release storage */

void    yana_policy_free(YANA_POLICY *policy)
{
    maps_free(policy->maps);
    myfree((void *) policy);
}
