/*
 *	$Id$
 */

#ifndef  __KIK_NET_H__
#define  __KIK_NET_H__


#include  "kik_config.h"	/* socklen_t */

#include  <sys/types.h>
#include  <sys/socket.h>	
#include  <sys/un.h>


#ifndef  AF_LOCAL
#define  AF_LOCAL  AF_UNIX
#endif

#ifndef  PF_LOCAL
#define  PF_LOCAL  PF_UNIX
#endif

#endif
