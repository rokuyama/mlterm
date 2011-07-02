/*
 *	$Id$
 */

#include  "ml_pty_intern.h"

#include  <kiklib/kik_debug.h>
#include  <kiklib/kik_mem.h>	/* realloc/alloca */
#include  <kiklib/kik_path.h>
#include  <kiklib/kik_str.h>
#include  <string.h>
#include  <unistd.h>		/* ttyname */
#include  <stdio.h>		/* sscanf */


#if  0
#define  __DEBUG
#endif


/* --- global functions --- */

ml_pty_t *
ml_pty_new(
	char *  cmd_path ,	/* can be NULL */
	char **  cmd_argv ,	/* can be NULL(only if cmd_path is NULL) */
	char **  env ,		/* can be NULL */
	char *  host ,
	char *  pass ,		/* can be NULL */
	char *  pubkey ,	/* can be NULL */
	char *  privkey ,	/* can be NULL */
	u_int  cols ,
	u_int  rows
	)
{
#ifndef  USE_WIN32API
	if( ! pass)
	{
		/* host is DISPLAY => unix pty */
		return  ml_pty_unix_new( cmd_path , cmd_argv , env , host , cols , rows) ;
	}
	else
#endif
	{
		/* host is not DISPLAY => ssh */
	#if  defined(USE_LIBSSH2)
		return  ml_pty_ssh_new( cmd_path , cmd_argv , env , host , pass ,
				pubkey , privkey , cols , rows) ;
	#elif  defined(USE_WIN32API)
		return  ml_pty_pipe_new( cmd_path , cmd_argv , env , host , pass , cols , rows) ;
	#else
		return  NULL ;
	#endif
	}
}

int
ml_pty_delete(
	ml_pty_t *  pty
	)
{
	if( pty->pty_listener && pty->pty_listener->closed)
	{
		(*pty->pty_listener->closed)( pty->pty_listener->self) ;
	}

	free( pty->buf) ;

	return  (*pty->delete)( pty) ;
}

int
ml_set_pty_winsize(
	ml_pty_t *  pty ,
	u_int  cols ,
	u_int  rows
	)
{
	return  (*pty->set_winsize)( pty , cols , rows) ;
}

int
ml_pty_set_listener(
  	ml_pty_t *  pty,
  	ml_pty_event_listener_t *  pty_listener
	)
{
  	pty->pty_listener = pty_listener ;

  	return  1 ;
}

/*
 * Return size of lost bytes.
 */
size_t
ml_write_to_pty(
	ml_pty_t *  pty ,
	u_char *  buf ,
	size_t  len		/* if 0, flushing buffer. */
	)
{
	u_char *  w_buf ;
	size_t  w_buf_size ;
	ssize_t  written_size ;
	void *  p ;

	w_buf_size = pty->left + len ;
	if( w_buf_size == 0)
	{
		return  0 ;
	}
#if  0
	/*
	 * Little influence without this buffering.
	 */
	else if( len > 0 && w_buf_size < 16)
	{
		/*
		 * Buffering until 16 bytes.
		 */

		if( pty->size < 16)
		{
			if( ( p = realloc( pty->buf , 16)) == NULL)
			{
			#ifdef  DEBUG
				kik_warn_printf( KIK_DEBUG_TAG
					" realloc failed. %d characters not written.\n" , len) ;
			#endif

				return  len ;
			}
			
			pty->size = 16 ;
			pty->buf = p ;
		}

		memcpy( &pty->buf[pty->left] , buf , len) ;
		pty->left = w_buf_size ;

	#if  0
		kik_debug_printf( "buffered(not written) %d characters.\n" , pty->left) ;
	#endif
	
		return  0 ;
	}
#endif

	if( /* pty->buf && */ len == 0)
	{
		w_buf = pty->buf ;
	}
  	else if( /* pty->buf == NULL && */ pty->left == 0)
        {
          	w_buf = buf ;
        }
  	else if( ( w_buf = alloca( w_buf_size)))
        {
          	memcpy( w_buf , pty->buf , pty->left) ;
		memcpy( &w_buf[pty->left] , buf , len) ;
	}
	else
	{
	#ifdef  DEBUG
		kik_warn_printf( KIK_DEBUG_TAG
			" alloca() failed. %d characters not written.\n" , len) ;
	#endif
	
		return  len ;
	}

#ifdef  __DEBUG
	{
		int  i ;
		for( i = 0 ; i < w_buf_size ; i++)
		{
			kik_msg_printf( "%.2x" , w_buf[i]) ;
		}
		kik_msg_printf( "\n") ;
	}
#endif

	written_size = (*pty->write)( pty , w_buf , w_buf_size) ;
	if( written_size < 0)
	{
		kik_warn_printf( KIK_DEBUG_TAG " write() failed.\n") ;
		written_size = 0 ;
	}

	if( written_size == w_buf_size)
	{
		pty->left = 0 ;

		return  0 ;
	}

	/* w_buf_size - written_size == not_written_size */
	if( w_buf_size - written_size > pty->size)
	{
		if( ( p = realloc( pty->buf , w_buf_size - written_size)) == NULL)
		{
			size_t  lost ;
			
			if( pty->size == 0)
			{
				lost = w_buf_size - written_size ;
				pty->left = 0 ;
			}
			else
			{
				lost = w_buf_size - written_size - pty->size ;
				memcpy( pty->buf , &w_buf[written_size] , pty->size) ;
				pty->left = pty->size ;
			}

		#ifdef  DEBUG
			kik_warn_printf( KIK_DEBUG_TAG
				" realloc failed. %d characters are not written.\n" , lost) ;
		#endif

			return  lost ;
		}
		else
		{
			pty->size = pty->left = w_buf_size - written_size ;
			pty->buf = p ;
		}
	}
	else
	{
		pty->left = w_buf_size - written_size ;
	}
	
	memcpy( pty->buf , &w_buf[written_size] , pty->left) ;

#if  0
	kik_debug_printf( "%d is not written.\n" , pty->left) ;
#endif

	return  0 ;
}

/*
 * Flush pty->buf/pty->left.
 */
size_t
ml_flush_pty(
	ml_pty_t *  pty
	)
{
#if  0
	kik_debug_printf( "flushing buffer.\n") ;
#endif

	return  ml_write_to_pty( pty , NULL , 0) ;
}

size_t
ml_read_pty(
	ml_pty_t *  pty ,
	u_char *  buf ,
	size_t  left
	)
{
	size_t  read_size ;

	read_size = 0 ;
	while( 1)
	{
		ssize_t  ret ;

		ret = (*pty->read)( pty , &buf[read_size] , left) ;
		if( ret <= 0)
		{
			return  read_size ;
		}
		else
		{
			read_size += ret ;
			left -= ret ;
		}
	}
}

pid_t
ml_pty_get_pid(
  	ml_pty_t *  pty
  	)
{
  	return  (pid_t)pty->child_pid ;
}

int
ml_pty_get_master_fd(
	ml_pty_t *  pty
	)
{
	return  pty->master ;
}

/* Return: slave fd or -1 */
int
ml_pty_get_slave_fd(
	ml_pty_t *  pty
	)
{
	return  pty->slave ;
}

char *
ml_pty_get_slave_name(
	ml_pty_t *  pty
	)
{
#ifndef  USE_WIN32API
	if( pty->slave >= 0)
	{
		return  ttyname( pty->slave) ;
	}
	else
#endif
	{
		return  "/dev/mlpty" ;
	}
}