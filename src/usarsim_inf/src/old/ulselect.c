/*
  DISCLAIMER:
  This software was produced by the National Institute of Standards
  and Technology (NIST), an agency of the U.S. government, and by statute is
  not subject to copyright in the United States.  Recipients of this software
  assume all responsibility associated with its operation, modification,
  maintenance, and subsequent redistribution.

  See NIST Administration Manual 4.09.07 b and Appendix I. 
*/

/*!
  \file ulselect.c

  \brief For those functions that differ between ULAPI interfaces to
  Unix or RTAI, this selects which particular ULAPI implementation to
  call, allowing dynamic switching between them. It also provides
  all the functions that are the same for each, so that code isn't
  duplicated.
*/

/*
  Some of these functions are implemented exactly the same way, with
  slightly different names, in unix_rtapi.c. If you change them here,
  change them there.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "ulapi.h"		/* these decls */
#include "unix_ulapi.h"		/* decls for Unix */
#ifdef HAVE_RTAI
#include "rtai_ulapi.h"		/* decls for RTAI */
#else
#include <stddef.h>		/* NULL */
#endif
#include <stdlib.h>		/* malloc */
#include <string.h>		/* memset */
#include <ctype.h>		/* isspace */
#include <pthread.h>		/* pthread_create(), pthread_mutex_t */
#include <time.h>		/* struct timespec, nanosleep */
#include <sys/time.h>		/* gettimeofday(), struct timeval */
#include <unistd.h>		/* select(), write() */
#include <sys/ipc.h>		/* IPC_* */
#include <sys/shm.h>		/* shmget() */
#include <sys/sem.h>
#include <errno.h>
#include <fcntl.h>		/* O_RDONLY, O_NONBLOCK */
#include <sys/types.h>		/* fd_set, FD_ISSET() */
#include <sys/socket.h>		/* PF_INET, socket(), listen(), bind(), etc. */
#include <netinet/in.h>		/* struct sockaddr_in */
#include <netdb.h>		/* gethostbyname */
#include <arpa/inet.h>		/* inet_addr */
#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

static ulapi_integer ulapi_debug_level = 0;

#define PERROR(x) if (ulapi_debug_level & ULAPI_DEBUG_ERROR) perror(x)

static ulapi_integer selected = UL_USE_UNIX;
static ulapi_integer _ulapi_wait_offset_nsec = 1;

/*
  'ulapi_time' returns the current time with respect to some arbitrary
  origin that remains constant for the life of the program.

  Several clock functions are considered, in this order:

  'clock_gettime(CLOCK_MONOTONIC_HR)' gets the high-resolution time
  from an arbitrary reference (usually the system uptime, but
  that's not guaranteed). Monotonic means it doesn't go backwards.

  'clock_gettime(CLOCK_MONOTONIC)' is similar but at lower resolution.

  'clock_gettime(CLOCK_REALTIME)' uses the real-time clock.  Its
  resolution varies but is seldom better than 1/18th of a second. It
  is very expensive to read, taking quite a bit of I/O. It is also
  inaccurate and cannot really be disciplined, largely because of its
  low resolution. In comparison, the other time sources are much more
  stable and have enough resolution to be disciplined. Most modern
  computers read the RTC only on startup.

  'gettimeofday' gets the system's best guess at wall time based on
  the Unix epoch, January 1, 1970. It is not monotonic and can go
  backwards. 
*/

ulapi_real
ulapi_time (void)
{
#ifdef HAVE_CLOCK_GETTIME
#if defined (CLOCK_MONOTONIC_RT)
  struct timespec ts;
  clock_gettime (CLOCK_MONOTONIC_RT, &ts);
  return (ulapi_real) (((double) ts.tv_sec) + ((double) ts.tv_nsec) * 1.0e-9);
#elif defined (CLOCK_MONOTONIC)
  struct timespec ts;
  clock_gettime (CLOCK_MONOTONIC, &ts);
  return (ulapi_real) (((double) ts.tv_sec) + ((double) ts.tv_nsec) * 1.0e-9);
#elif defined (CLOCK_REALTIME)
  struct timespec ts;
  clock_gettime (CLOCK_REALTIME, &ts);
  return (ulapi_real) (((double) ts.tv_sec) + ((double) ts.tv_nsec) * 1.0e-9);
#else
  /* should have at least CLOCK_REALTIME, so fall back on gettimeofday */
  struct timeval tv;
  gettimeofday (&tv, NULL);
  return (ulapi_real) (((double) tv.tv_sec) + ((double) tv.tv_usec) * 1.0e-6);
#endif

#else
  struct timeval tv;
  gettimeofday (&tv, NULL);
  return (ulapi_real) (((double) tv.tv_sec) + ((double) tv.tv_usec) * 1.0e-6);
#endif
}

void
ulapi_sleep (ulapi_real secs)
{
  int isecs, insecs;
  struct timespec ts;

  isecs = (int) secs;
  insecs = (int) ((secs - isecs) * 1.0e9);

  ts.tv_sec = isecs;
  ts.tv_nsec = insecs;

  (void) nanosleep (&ts, NULL);
}

ulapi_result
ulapi_init (ulapi_integer sel)
{
  if (UL_USE_UNIX == sel || UL_USE_DEFAULT == sel)
    {
      selected = UL_USE_UNIX;
      return unix_ulapi_init (sel);
    }

  if (UL_USE_RTAI == sel)
    {
#ifdef HAVE_RTAI
      selected = sel;
      return rtai_ulapi_init (sel);
#else
      return ULAPI_IMPL_ERROR;
#endif
    }

  return ULAPI_BAD_ARGS;
}

ulapi_result
ulapi_exit (void)
{
  return selected == UL_USE_UNIX ? unix_ulapi_exit () :
#ifdef HAVE_RTAI
    rtai_ulapi_exit ();
#else
    ULAPI_IMPL_ERROR;
#endif
}

ulapi_integer
ulapi_to_argv (const char *src, char ***argv)
{
  char *cpy;
  char *ptr;
  ulapi_integer count;
  ulapi_flag inquote;

  *argv = NULL;
  cpy = malloc (strlen (src) + 1);
  count = 0;
  inquote = 0;

  for (;; count++)
    {
      while (isspace (*src))
	src++;
      if (0 == *src)
	return count;
      ptr = cpy;
      while (0 != *src)
	{
	  if ('"' == *src)
	    {
	      inquote = !inquote;
	    }
	  else if ((!inquote) && isspace (*src))
	    {
	      break;
	    }
	  else
	    {
	      *ptr++ = *src;
	    }
	  src++;
	}
      *ptr++ = 0;
      *argv = realloc (*argv, (count + 1) * sizeof (**argv));
      (*argv)[count] = malloc (strlen (cpy) + 1);
      strcpy ((*argv)[count], cpy);
    }

  free (cpy);

  return count;
}

void
ulapi_free_argv (ulapi_integer argc, char **argv)
{
  ulapi_integer t;

  for (t = 0; t < argc; t++)
    {
      free ((char *) argv[t]);
    }
  free (argv);

  return;
}

void
ulapi_set_debug (ulapi_integer mask)
{
  ulapi_debug_level = mask;
}

ulapi_integer
ulapi_get_debug (void)
{
  return ulapi_debug_level;
}

ulapi_prio
ulapi_prio_highest (void)
{
  return 1;
}

ulapi_prio
ulapi_prio_lowest (void)
{
  return 31;
}

ulapi_prio
ulapi_prio_next_higher (ulapi_prio prio)
{
  if (prio == ulapi_prio_highest ())
    return prio;

  return prio - 1;
}

ulapi_prio
ulapi_prio_next_lower (ulapi_prio prio)
{
  if (prio == ulapi_prio_lowest ())
    return prio;

  return prio + 1;
}

void *
ulapi_task_new (void)
{
  return malloc (sizeof (pthread_t));
}

ulapi_result
ulapi_task_delete (void *task)
{
  if (NULL != task)
    free (task);

  return ULAPI_OK;
}

typedef void *(*pthread_task_code) (void *);

ulapi_result
ulapi_task_start (void *task,
		  void (*taskcode) (void *),
		  void *taskarg, ulapi_prio prio, ulapi_integer period_nsec)
{
  pthread_attr_t attr;
  struct sched_param sched_param;

  pthread_attr_init (&attr);
  sched_param.sched_priority = prio;
  pthread_attr_setschedparam (&attr, &sched_param);
  pthread_create ((pthread_t *) task, &attr, (pthread_task_code) taskcode,
		  taskarg);
  pthread_setschedparam (*((pthread_t *) task), SCHED_OTHER, &sched_param);
  /* ignore period_nsec, since we can't handle it in pthreads, so the
     application must call ulapi_wait(period_nsec) */

  return ULAPI_OK;
}

ulapi_result
ulapi_task_stop (void *task)
{
  return (pthread_cancel (*((pthread_t *) task)) ==
	  0 ? ULAPI_OK : ULAPI_ERROR);
}

ulapi_result
ulapi_task_pause (void *task)
{
  return ULAPI_OK;
}

ulapi_result
ulapi_task_resume (void *task)
{
  return ULAPI_OK;
}

ulapi_result
ulapi_task_set_period (void *task, ulapi_integer period_nsec)
{
  return ULAPI_OK;
}

ulapi_result
ulapi_task_init (void)
{
  if (0 != pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL))
    return ULAPI_ERROR;
  if (0 != pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS, NULL))
    return ULAPI_ERROR;

  return ULAPI_OK;
}

ulapi_result
ulapi_self_set_period (ulapi_integer period_nsec)
{
  return ULAPI_OK;
}

ulapi_result
ulapi_wait (ulapi_integer period_nsec)
{
  struct timespec ts;

  if (period_nsec < _ulapi_wait_offset_nsec + 1)
    period_nsec = 1;
  else
    period_nsec -= _ulapi_wait_offset_nsec;

  ts.tv_sec = 0;
  ts.tv_nsec = period_nsec;

  (void) nanosleep (&ts, NULL);

  return ULAPI_OK;
}

ulapi_result
ulapi_task_exit (void)
{
  return ULAPI_OK;
}

ulapi_result
ulapi_task_join (void *task)
{
  return (pthread_join (*((pthread_t *) task), NULL) ==
	  0 ? ULAPI_OK : ULAPI_ERROR);
}

ulapi_integer
ulapi_task_id (void)
{
  return (ulapi_integer) pthread_self ();
}

void *
ulapi_mutex_new (ulapi_id key)
{
  pthread_mutex_t *mutex;

  mutex = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  if (NULL == (void *) mutex)
    return NULL;

  /* initialize mutex to default attributes, and give it */
  if (0 == pthread_mutex_init (mutex, NULL))
    {
      (void) pthread_mutex_unlock (mutex);
      return (void *) mutex;
    }
  /* else got an error, so free the mutex and return null */

  free (mutex);
  return NULL;
}

ulapi_result
ulapi_mutex_delete (void *mutex)
{
  if (NULL == (void *) mutex)
    return ULAPI_ERROR;

  (void) pthread_mutex_destroy ((pthread_mutex_t *) mutex);
  free (mutex);

  return ULAPI_OK;
}

ulapi_result
ulapi_mutex_give (void *mutex)
{
  return (0 ==
	  pthread_mutex_unlock ((pthread_mutex_t *) mutex) ? ULAPI_OK :
	  ULAPI_ERROR);
}

ulapi_result
ulapi_mutex_take (void *mutex)
{
  return (0 ==
	  pthread_mutex_lock ((pthread_mutex_t *) mutex) ? ULAPI_OK :
	  ULAPI_ERROR);
}

#define SEM_TAKE (-1)		/* decrement sembuf.sem_op */
#define SEM_GIVE (1)		/* increment sembuf.sem_op */

/* destroy binary semaphore */
static void
semdestroy (int semid)
{
  (void) semctl (semid, 0, IPC_RMID);
  return;
}

/* create binary semaphore, initially given, returning the id */
static int
semcreate (int key)
{
  int semid;			/* semaphore id returned */
  int semflg;			/* flag for perms, create, etc. */
  struct sembuf sops;

  semflg = 0664;		/* permissions */
  /* leave IPC_CREAT and IPC_EXCL out, we want to see if it's there
     and delete it if so with the semid returned */

  semid = semget ((key_t) key, 1, semflg);
  if (-1 == semid)
    {
      if (errno != ENOENT)
	{
	  /* some other error */
	  return -1;
	}
      /* else it just wasn't there, which is what we expected */
    }
  else
    {
      /* it's already there, so return it */
      return semid;
    }

  semflg |= IPC_CREAT;		/* create it afresh */
  if ((semid = semget ((key_t) key, 1, semflg)) == -1)
    {
      return -1;
    }
  /* initialize it to the given state */
  sops.sem_num = 0;
  sops.sem_flg = 0;		/* default is wait for it */
  sops.sem_op = SEM_GIVE;
  if (semop (semid, &sops, 1) == -1)
    {
      return -1;
    }

  return semid;
}

/* take a binary semaphore */
static int
semtake (int semid)
{
  struct sembuf sops;

  sops.sem_num = 0;		/* only one semaphore in set */
  sops.sem_flg = 0;		/* wait indefinitely */
  sops.sem_op = SEM_TAKE;

  return semop (semid, &sops, 1);
}

/* give a binary semaphore */
static int
semgive (int semid)
{
  struct sembuf sops;

  sops.sem_num = 0;		/* only one semaphore in set */
  sops.sem_flg = 0;		/* wait indefinitely */
  sops.sem_op = SEM_GIVE;

  if (semctl (semid, 0, GETVAL) == 1)
    {
      /* it's given-- leave it alone */
      return 0;
    }
  /* it's taken-- suppose now others take it again before
     we give it? they block, and this semgive will release
     one of them */
  return semop (semid, &sops, 1);
}

void *
ulapi_sem_new (ulapi_id key)
{
  int *id;

  id = (int *) malloc (sizeof (int));
  if (NULL == (void *) id)
    return NULL;

  *id = semcreate ((key_t) key);

  if (*id == -1)
    {
      free (id);
      return NULL;
    }

  return (void *) id;
}

ulapi_result
ulapi_sem_delete (void *sem)
{
  if (NULL != sem)
    {
      semdestroy (*((int *) sem));
      free (sem);
      return ULAPI_OK;
    }

  return ULAPI_ERROR;
}

ulapi_result
ulapi_sem_give (void *sem)
{
  return semgive (*((int *) sem)) == 0 ? ULAPI_OK : ULAPI_ERROR;
}

ulapi_result
ulapi_sem_take (void *sem)
{
  return semtake (*((int *) sem)) == 0 ? ULAPI_OK : ULAPI_ERROR;
}

void *
ulapi_cond_new (ulapi_id key)
{
  pthread_cond_t *cond;

  cond = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  if (NULL == (void *) cond)
    return NULL;

  if (0 == pthread_cond_init (cond, NULL))
    {
      return (void *) cond;
    }
  /* else got an error, so free the condition variable and return null */

  free (cond);
  return NULL;
}

ulapi_result
ulapi_cond_delete (void *cond)
{
  if (NULL == (void *) cond)
    return ULAPI_ERROR;

  (void) pthread_cond_destroy ((pthread_cond_t *) cond);
  free (cond);

  return ULAPI_OK;
}

ulapi_result
ulapi_cond_signal (void *cond)
{
  return (0 ==
	  pthread_cond_signal ((pthread_cond_t *) cond) ? ULAPI_OK :
	  ULAPI_ERROR);
}

ulapi_result
ulapi_cond_broadcast (void *cond)
{
  return (0 ==
	  pthread_cond_broadcast ((pthread_cond_t *) cond) ? ULAPI_OK :
	  ULAPI_ERROR);
}

ulapi_result
ulapi_cond_wait (void *cond, void *mutex)
{
  return (0 ==
	  pthread_cond_wait ((pthread_cond_t *) cond,
			     (pthread_mutex_t *) mutex) ? ULAPI_OK :
	  ULAPI_ERROR);
}

void *
ulapi_shm_new (ulapi_id key, ulapi_integer size)
{
  return selected == UL_USE_UNIX ? unix_ulapi_shm_new (key, size) :
#ifdef HAVE_RTAI
    rtai_ulapi_shm_new (key, size);
#else
    NULL;
#endif
}

void *
ulapi_shm_addr (void *shm)
{
  return selected == UL_USE_UNIX ? unix_ulapi_shm_addr (shm) :
#ifdef HAVE_RTAI
    rtai_ulapi_shm_addr (shm);
#else
    NULL;
#endif
}

ulapi_result
ulapi_shm_delete (void *shm)
{
  return selected == UL_USE_UNIX ? unix_ulapi_shm_delete (shm) :
#ifdef HAVE_RTAI
    rtai_ulapi_shm_delete (shm);
#else
    ULAPI_IMPL_ERROR;
#endif
}

ulapi_result
ulapi_fifo_new (ulapi_integer key, ulapi_integer * fd, ulapi_integer size)
{
  return selected == UL_USE_UNIX ? unix_ulapi_fifo_new (key, fd, size) :
#ifdef HAVE_RTAI
    rtai_ulapi_fifo_new (key, fd, size);
#else
    ULAPI_IMPL_ERROR;
#endif
}

ulapi_result
ulapi_fifo_delete (ulapi_integer key, ulapi_integer fd, ulapi_integer size)
{
  return selected == UL_USE_UNIX ? unix_ulapi_fifo_delete (key, fd, size) :
#ifdef HAVE_RTAI
    rtai_ulapi_fifo_delete (key, fd, size);
#else
    ULAPI_IMPL_ERROR;
#endif
}

ulapi_integer
ulapi_fifo_write (ulapi_integer fd, const char *buf, ulapi_integer size)
{
  return selected == UL_USE_UNIX ? unix_ulapi_fifo_write (fd, buf, size) :
#ifdef HAVE_RTAI
    rtai_ulapi_fifo_write (fd, buf, size);
#else
    -1;
#endif
}

ulapi_integer
ulapi_fifo_read (ulapi_integer fd, char *buf, ulapi_integer size)
{
  return selected == UL_USE_UNIX ? unix_ulapi_fifo_read (fd, buf, size) :
#ifdef HAVE_RTAI
    rtai_ulapi_fifo_read (fd, buf, size);
#else
    -1;
#endif
}

ulapi_integer
ulapi_socket_get_client_id (ulapi_integer port, const char *hostname)
{
  int socket_fd;
  struct sockaddr_in server_addr;
  struct hostent *ent;
  struct in_addr *in_a;

  if (NULL == (ent = gethostbyname (hostname)))
    {
      PERROR ("gethostbyname");
      return -1;
    }
  in_a = (struct in_addr *) ent->h_addr_list[0];

  memset (&server_addr, 0, sizeof (struct sockaddr_in));
  server_addr.sin_family = PF_INET;
  server_addr.sin_addr.s_addr = in_a->s_addr;
  server_addr.sin_port = htons (port);

  if (-1 == (socket_fd = socket (PF_INET, SOCK_STREAM, 0)))
    {
      PERROR ("socket");
      return -1;
    }

  if (-1 == connect (socket_fd,
		     (struct sockaddr *) &server_addr,
		     sizeof (struct sockaddr_in)))
    {
      PERROR ("connect");
      close (socket_fd);
      return -1;
    }

  return socket_fd;
}

ulapi_integer
ulapi_socket_get_server_id (ulapi_integer port)
{
  int socket_fd;
  struct sockaddr_in myaddr;
  int on;
  struct linger mylinger = { 0 };
  enum
  { BACKLOG = 5 };

  if (-1 == (socket_fd = socket (PF_INET, SOCK_STREAM, 0)))
    {
      PERROR ("socket");
      return -1;
    }

  /*
     Turn off bind address checking, and allow
     port numbers to be reused - otherwise the
     TIME_WAIT phenomenon will prevent binding
     to these address.port combinations for
     (2 * MSL) seconds.
   */
  on = 1;
  if (-1 ==
      setsockopt (socket_fd,
		  SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof (on)))
    {
      PERROR ("setsockopt");
      close (socket_fd);
      return -1;
    }

  /*
     When connection is closed, there is a need
     to linger to ensure all data is transmitted.
   */
  mylinger.l_onoff = 1;
  mylinger.l_linger = 30;
  if (-1 ==
      setsockopt (socket_fd,
		  SOL_SOCKET,
		  SO_LINGER,
		  (const char *) &mylinger, sizeof (struct linger)))
    {
      PERROR ("setsockopt");
      close (socket_fd);
      return -1;
    }

  memset (&myaddr, 0, sizeof (struct sockaddr_in));
  myaddr.sin_family = PF_INET;
  myaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  myaddr.sin_port = htons (port);

  if (-1 == bind (socket_fd, (struct sockaddr *) &myaddr,
		  sizeof (struct sockaddr_in)))
    {
      PERROR ("bind");
      close (socket_fd);
      return -1;
    }

  if (-1 == listen (socket_fd, BACKLOG))
    {
      PERROR ("listen");
      close (socket_fd);
      return -1;
    }

  return socket_fd;
}

ulapi_integer
ulapi_socket_get_connection_id (ulapi_integer socket_fd)
{
  fd_set rfds;
  struct sockaddr_in client_addr;
  unsigned int client_len;
  int client_fd;
  int retval;

  do
    {
      FD_ZERO (&rfds);
      FD_SET (socket_fd, &rfds);
      retval = select (socket_fd + 1, &rfds, NULL, NULL, NULL);
    }
  while (0 == retval || !FD_ISSET (socket_fd, &rfds));

  if (retval == -1)
    {
      PERROR ("select");
      return -1;
    }

  memset (&client_addr, 0, sizeof (struct sockaddr_in));
  client_len = sizeof (struct sockaddr_in);
  client_fd =
    accept (socket_fd, (struct sockaddr *) &client_addr, &client_len);
  if (-1 == client_fd)
    {
      PERROR ("accept");
      return -1;
    }

  return client_fd;
}

ulapi_integer
ulapi_socket_get_broadcaster_id (void)
{
  int fd;
  int perm;

  fd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (0 > fd)
    {
      return -1;
    }

  perm = 1;
  if (0 >
      setsockopt (fd, SOL_SOCKET, SO_BROADCAST, (void *) &perm,
		  sizeof (perm)))
    {
      return -1;
    }

  return fd;
}

ulapi_integer
ulapi_socket_get_broadcastee_id (ulapi_integer port)
{
  struct sockaddr_in addr;
  int fd;
  int retval;

  /* Create a best-effort datagram socket using UDP */
  fd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (0 > fd)
    {
      return -1;
    }

  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl (INADDR_ANY);
  addr.sin_port = htons (port);

  retval = bind (fd, (struct sockaddr *) &addr, sizeof (addr));

  if (0 > retval)
    {
      close (fd);
      return -1;
    }

  return fd;
}

char *
ulapi_address_to_hostname (ulapi_integer address)
{
  static char string[4 * DIGITS_IN (int) + 3 + 1];

  sprintf (string, "%d.%d.%d.%d",
	   (int) ((address >> 24) & 0xFF),
	   (int) ((address >> 16) & 0xFF),
	   (int) ((address >> 8) & 0xFF), (int) (address & 0xFF));

  return string;
}

ulapi_integer
ulapi_hostname_to_address (const char *hostname)
{
  struct hostent *ent;

  ent = gethostbyname (hostname);
  if (NULL == ent)
    return (127 << 24) + (0 << 16) + (0 << 8) + 1;
  if (4 != ent->h_length)
    return 0;

  /* FIXME-- could use ntohl here */
  return ((ent->h_addr_list[0][0] & 0xFF) << 24)
    + ((ent->h_addr_list[0][1] & 0xFF) << 16)
    + ((ent->h_addr_list[0][2] & 0xFF) << 8)
    + (ent->h_addr_list[0][3] & 0xFF);
}

ulapi_integer
ulapi_get_host_address (void)
{
  enum
  { HOSTNAMELEN = 256 };
  char hostname[HOSTNAMELEN];

  if (0 != gethostname (hostname, HOSTNAMELEN))
    return 0;
  hostname[HOSTNAMELEN - 1] = 0;	/* force null termination */

  return ulapi_hostname_to_address (hostname);
}

ulapi_result
ulapi_socket_set_nonblocking (ulapi_integer fd)
{
  int flags;

  flags = fcntl (fd, F_GETFL);
  if (-1 == fcntl (fd, F_SETFL, flags | O_NONBLOCK))
    {
      return ULAPI_ERROR;
    }

  return ULAPI_OK;
}

ulapi_result
ulapi_socket_set_blocking (ulapi_integer fd)
{
  int flags;

  flags = fcntl (fd, F_GETFL);
  if (-1 == fcntl (fd, F_SETFL, flags & ~O_NONBLOCK))
    {
      return ULAPI_ERROR;
    }

  return ULAPI_OK;
}

ulapi_integer
ulapi_socket_read (ulapi_integer id, char *buf, ulapi_integer len)
{
  return read (id, buf, len);
}

ulapi_integer
ulapi_socket_write (ulapi_integer id, const char *buf, ulapi_integer len)
{
  return write (id, buf, len);
}

ulapi_integer
ulapi_socket_broadcast (ulapi_integer id, ulapi_integer port, const char *buf,
			ulapi_integer len)
{
  struct sockaddr_in addr;
  char ip[] = "xxx.xxx.xxx.xxx";

  strncpy (ip, ulapi_address_to_hostname (ulapi_get_host_address () | 0xFF),
	   sizeof (ip));
  ip[sizeof (ip) - 1] = 0;

  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr (ip);
  addr.sin_port = htons (port);

  return sendto (id, buf, len, 0, (struct sockaddr *) &addr, sizeof (addr));
}

ulapi_result
ulapi_socket_close (ulapi_integer id)
{
  return 0 == close ((int) id) ? ULAPI_OK : ULAPI_ERROR;
}

/* File descriptor interface */

void *
ulapi_fd_new (void)
{
  return malloc (sizeof (int));
}

ulapi_result
ulapi_fd_delete (void *id)
{
  if (NULL != id)
    free (id);

  return ULAPI_OK;
}

ulapi_result
ulapi_std_open (ulapi_stdio io, void *id)
{
  if (ULAPI_STDIN == io)
    {
      *((int *) id) = STDIN_FILENO;
      return ULAPI_OK;
    }
  if (ULAPI_STDOUT == io)
    {
      *((int *) id) = STDOUT_FILENO;
      return ULAPI_OK;
    }
  if (ULAPI_STDERR == io)
    {
      *((int *) id) = STDERR_FILENO;
      return ULAPI_OK;
    }
  return ULAPI_ERROR;
}

ulapi_result
ulapi_fd_open (const char *port, void *id)
{
  int fd;

  fd = open (port, O_RDWR);

  if (fd < 0)
    return ULAPI_ERROR;

  *((int *) id) = fd;
  return ULAPI_OK;
}

ulapi_result
ulapi_fd_set_nonblocking (void *id)
{
  int fd;
  int flags;

  fd = *((int *) id);
  flags = fcntl (fd, F_GETFL);
  if (-1 == fcntl (fd, F_SETFL, flags | O_NONBLOCK))
    {
      return ULAPI_ERROR;
    }

  return ULAPI_OK;
}

ulapi_result
ulapi_fd_set_blocking (void *id)
{
  int fd;
  int flags;

  fd = *((int *) id);
  flags = fcntl (fd, F_GETFL);
  if (-1 == fcntl (fd, F_SETFL, flags & ~O_NONBLOCK))
    {
      return ULAPI_ERROR;
    }

  return ULAPI_OK;
}

ulapi_integer
ulapi_fd_read (void *id, char *buf, ulapi_integer len)
{
  return read (*((int *) id), buf, len);
}

ulapi_integer
ulapi_fd_write (void *id, const char *buf, ulapi_integer len)
{
  return write (*((int *) id), buf, len);
}

ulapi_result
ulapi_fd_close (void *id)
{
  return 0 == close (*((int *) id)) ? ULAPI_OK : ULAPI_ERROR;
}

/* Serial interface is implemented using the file descriptor interface,
   which is the same */

void *
ulapi_serial_new (void)
{
  return ulapi_fd_new ();
}

ulapi_result
ulapi_serial_delete (void *id)
{
  return ulapi_fd_delete (id);
}

ulapi_result
ulapi_serial_open (const char *port, void *id)
{
  return ulapi_fd_open (port, id);
}

ulapi_result
ulapi_serial_set_nonblocking (void *id)
{
  return ulapi_fd_set_nonblocking (id);
}

ulapi_result
ulapi_serial_set_blocking (void *id)
{
  return ulapi_fd_set_blocking (id);
}

ulapi_integer
ulapi_serial_read (void *id, char *buf, ulapi_integer len)
{
  return ulapi_fd_read (id, buf, len);
}

ulapi_integer
ulapi_serial_write (void *id, const char *buf, ulapi_integer len)
{
  return ulapi_fd_write (id, buf, len);
}

ulapi_result
ulapi_serial_close (void *id)
{
  return ulapi_fd_close (id);
}

#ifdef HAVE_DLFCN_H

void *
ulapi_dl_open (const char *objname, char *errstr, int errlen)
{
  void *handle;
  void *error;

  dlerror ();
  handle = dlopen (objname, RTLD_LAZY);
  error = dlerror ();

  if (NULL == handle)
    {
      if (NULL != errstr && NULL != error)
	{
	  strncpy (errstr, error, errlen);
	  errstr[errlen - 1] = 0;
	}
      return NULL;
    }

  if (NULL != errstr)
    {
      errstr[0] = 0;
    }
  return handle;
}

void
ulapi_dl_close (void *handle)
{
  if (NULL != handle)
    {
      dlclose (handle);
    }
}

void *
ulapi_dl_sym (void *handle, const char *name, char *errstr, int errlen)
{
  void *sym;
  void *error;

  dlerror ();
  sym = dlsym (handle, name);
  error = dlerror ();

  if (error != NULL)
    {
      if (NULL != errstr)
	{
	  strncpy (errstr, error, errlen);
	  errstr[errlen - 1] = 0;
	}
      return NULL;
    }

  if (NULL != errstr)
    {
      errstr[0] = 0;
    }
  return sym;
}

#endif

ulapi_result
ulapi_system (const char *prog, int *result)
{
  int retval;

  retval = system (prog);

  if (-1 == retval)
    return ULAPI_ERROR;

  *result = retval >> 8;

  return ULAPI_OK;
}
