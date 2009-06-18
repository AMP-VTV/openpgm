/* vim:ts=8:sts=8:sw=4:noai:noexpandtab
 *
 * Listen to PGM packets, note per host details and data loss.
 *
 * Copyright (c) 2006-2009 Miru Limited.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <glib.h>
#include <libsoup/soup.h>
#include <libsoup/soup-server.h>
#include <libsoup/soup-address.h>

#include "pgm/backtrace.h"
#include "pgm/log.h"
#include "pgm/packet.h"


/* typedefs */
struct tsi {
	guint8	gsi[6];			/* transport session identifier TSI */
	guint16	source_port;
};

struct sessionstat {
	gulong	count, snap_count;
	gulong	bytes, snap_bytes;
	gulong	tsdu;

	gulong	duplicate;
	gulong	invalid;

	struct timeval	last;
	struct timeval	last_valid;
	struct timeval	last_invalid;
};

struct netstat {
	struct in_addr	addr;
	gulong		corrupt;
};

struct hoststat {
	struct tsi tsi;

	struct in_addr last_addr;
	struct in_addr nla;

	gulong	txw_secs;		/* seconds of repair data */
	guint32	txw_trail;		/* trailing edge sequence number */
	guint32	txw_lead;		/* leading edge sequence number */
	gulong	txw_sqns;		/* size of transmit window */

	guint32	rxw_trail;		/* oldest repairable sequence number */
	guint32	rxw_lead;		/* last sequence number */

	guint32	rxw_trail_init;		/* first sequence number */
	int rxw_learning;

	guint32	spm_sqn;		/* independent sequence number */

	struct sessionstat	spm,
			poll,
			polr,
			odata,
			rdata,
			nak,
			nnak,
			ncf,
			spmr,

			general;

	struct timeval	session_start;
};


/* globals */

#define	WWW_XMLDECL	"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"><html xmlns=\"http://www.w3.org/1999/xhtml\">"

#define WWW_NOTFOUND    WWW_XMLDECL "<head><title>404</title></head><body><p>lah, 404: not found :)</p><p><a href=\"/\">Return to index.</a></p></body></html>\r\n"

#define WWW_HEADER	WWW_XMLDECL "<head><meta http-equiv=\"refresh\" content=\"10\" /><title>basic_recv</title><link rel=\"stylesheet\" type=\"text/css\" href=\"/main.css\" /></head><body>\n"
#define	WWW_FOOTER	"\n</body></html>\r\n"

#define WWW_ROBOTS	"User-agent: *\nDisallow: /\r\n"

#define WWW_CSS		"html {" \
				"background-color: white;" \
				"font-family: Verdana;" \
				"font-size: 11px;" \
				"color: black" \
			"}\n" \
			"table {" \
				"border-collapse: collapse" \
			"}\n" \
			"thead th,tfoot th {" \
				"background-color: #C30;" \
				"color: #FFB3A6;" \
				"text-align: center" \
				"font-weight: bold" \
			"}\n" \
			"tbody td {" \
				"border-bottom: 1px solid #F0F0F0" \
			"}\n" \
			"th,td {" \
				"border-left: 0;" \
				"padding: 10px" \
			"}\n" \
			"\r\n"


static int g_port = 7500;
static const char* g_network = "226.0.0.1";

static int g_http = 4968;

static int g_mark_interval = 10 * 1000;
static int g_snap_interval = 10 * 1000;
static struct timeval g_last_snap;
static struct timeval g_tv_now;

static GHashTable *g_hosts = NULL;
static GMutex *g_hosts_mutex = NULL;
static GHashTable *g_nets = NULL;
static GMutex *g_nets_mutex = NULL;

static GIOChannel* g_io_channel = NULL;
static GMainLoop* g_loop = NULL;
static SoupServer* g_soup_server = NULL;

static guint tsi_hash (gconstpointer);
static gint tsi_equal (gconstpointer, gconstpointer);

static gchar* print_tsi (gconstpointer);

static void on_signal (int);
static gboolean on_startup (gpointer);
static gboolean on_mark (gpointer);
static gboolean on_snap (gpointer);

static gboolean on_io_data (GIOChannel*, GIOCondition, gpointer);
static gboolean on_io_error (GIOChannel*, GIOCondition, gpointer);

#ifdef CONFIG_LIBSOUP22
static void robots_callback (SoupServerContext*, SoupMessage*, gpointer);
static void default_callback (SoupServerContext*, SoupMessage*, gpointer);
static void css_callback (SoupServerContext*, SoupMessage*, gpointer);
static void index_callback (SoupServerContext*, SoupMessage*, gpointer);
#else
static void robots_callback (SoupServer*, SoupMessage*, const char*, GHashTable*, SoupClientContext*, gpointer);
static void default_callback (SoupServer*, SoupMessage*, const char*, GHashTable*, SoupClientContext*, gpointer);
static void css_callback (SoupServer*, SoupMessage*, const char*, GHashTable*, SoupClientContext*, gpointer);
static void index_callback (SoupServer*, SoupMessage*, const char*, GHashTable*, SoupClientContext*, gpointer);
#endif

static int tsi_callback (SoupMessage*, const char*);


G_GNUC_NORETURN static void
usage (
	const char*	bin
	)
{
        fprintf (stderr, "Usage: %s [options]\n", bin);
        fprintf (stderr, "  -p <port>       : IP port for web interface\n");
        fprintf (stderr, "  -n <network>    : Multicast group or unicast IP address\n");
        fprintf (stderr, "  -s <port>       : IP port\n");
        exit (1);
}

int
main (
	int		argc,
	char*		argv[]
	)
{
	puts ("basic_recv");

	/* parse program arguments */
        const char* binary_name = strrchr (argv[0], '/');
        int c;
        while ((c = getopt (argc, argv, "p:s:n:h")) != -1)
        {
                switch (c) {
                case 'p':       g_http = atoi (optarg); break;
                case 'n':       g_network = optarg; break;
                case 's':       g_port = atoi (optarg); break;

                case 'h':
                case '?': usage (binary_name);
                }
        }

	log_init ();

	g_type_init ();
	g_thread_init (NULL);

/* setup signal handlers */
	signal(SIGSEGV, on_sigsegv);
	signal(SIGINT, on_signal);
	signal(SIGTERM, on_signal);
	signal(SIGHUP, SIG_IGN);

/* delayed startup */
	puts ("scheduling startup.");
	g_timeout_add(0, (GSourceFunc)on_startup, NULL);

/* dispatch loop */
	g_loop = g_main_loop_new(NULL, FALSE);

	puts ("entering main event loop ... ");
	g_main_loop_run(g_loop);

	puts ("event loop terminated, cleaning up.");

/* cleanup */
	g_main_loop_unref(g_loop);
	g_loop = NULL;

	if (g_io_channel) {
		puts ("closing socket.");

		GError *err = NULL;
		g_io_channel_shutdown (g_io_channel, FALSE, &err);
		g_io_channel = NULL;
	}

        if (g_soup_server) {
                g_object_unref (g_soup_server);
                g_soup_server = NULL;
        }

	puts ("finished.");
	return 0;
}

static void
on_signal (
	G_GNUC_UNUSED int signum
	)
{
	puts ("on_signal");

	g_main_loop_quit(g_loop);
}

static gboolean
on_startup (
	G_GNUC_UNUSED gpointer data
	)
{
	int e;

	puts ("startup.");

        puts ("starting soup server.");
        g_soup_server = soup_server_new (SOUP_SERVER_PORT, g_http,
                                        NULL);
        if (!g_soup_server) {
                printf ("soup server failed: %s\n", strerror (errno));
                g_main_loop_quit (g_loop);
                return FALSE;
        }

        char hostname[NI_MAXHOST + 1];
        gethostname (hostname, sizeof(hostname));

        printf ("web interface: http://%s:%i\n",
                hostname,
                soup_server_get_port (g_soup_server));

#ifdef CONFIG_LIBSOUP22
        soup_server_add_handler (g_soup_server, NULL,	NULL, default_callback, NULL, NULL);
        soup_server_add_handler (g_soup_server, "/robots.txt",	NULL, robots_callback, NULL, NULL);
        soup_server_add_handler (g_soup_server, "/main.css",	NULL, css_callback, NULL, NULL);
        soup_server_add_handler (g_soup_server, "/",	NULL, index_callback, NULL, NULL);
#else
        soup_server_add_handler (g_soup_server, NULL,		default_callback, NULL, NULL);
        soup_server_add_handler (g_soup_server, "/robots.txt",	robots_callback, NULL, NULL);
        soup_server_add_handler (g_soup_server, "/main.css",	css_callback, NULL, NULL);
        soup_server_add_handler (g_soup_server, "/",		index_callback, NULL, NULL);
#endif

        soup_server_run_async (g_soup_server);
        g_object_unref (g_soup_server);


/* find PGM protocol id */
// TODO: fix valgrind errors
	int ipproto_pgm = IPPROTO_PGM;
#if HAVE_GETPROTOBYNAME_R
	char b[1024];
	struct protoent protobuf, *proto;
	e = getprotobyname_r("pgm", &protobuf, b, sizeof(b), &proto);
	if (e != -1 && proto != NULL) {
		if (proto->p_proto != ipproto_pgm) {
			printf("Setting PGM protocol number to %i from /etc/protocols.\n");
			ipproto_pgm = proto->p_proto;
		}
	}
#else
	struct protoent *proto = getprotobyname("pgm");
	if (proto != NULL) {
		if (proto->p_proto != ipproto_pgm) {
			printf("Setting PGM protocol number to %i from /etc/protocols.\n", proto
->p_proto);
			ipproto_pgm = proto->p_proto;
		}
	}
#endif

/* open socket for snooping */
	puts ("opening raw socket.");
	int sock = socket(PF_INET, SOCK_RAW, ipproto_pgm);
	if (sock < 0) {
		int _e = errno;
		perror("on_startup() failed");

		if (_e == EPERM && 0 != getuid()) {
			puts ("PGM protocol requires this program to run as superuser.");
		}
		g_main_loop_quit(g_loop);
		return FALSE;
	}

/* drop out of setuid 0 */
	if (0 == getuid()) {
		puts ("dropping superuser privileges.");
		setuid((gid_t)65534);
		setgid((uid_t)65534);
	}

	char _t = 1;
	e = setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &_t, sizeof(_t));
	if (e < 0) {
		perror("on_startup() failed");
		close(sock);
		g_main_loop_quit(g_loop);
		return FALSE;
	}

/* buffers */
	int buffer_size = 0;
	socklen_t len = 0;
	e = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &buffer_size, &len);
	if (e == 0) {
		printf ("receive buffer set at %i bytes.\n", buffer_size);
	}
	e = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &buffer_size, &len);
	if (e == 0) {
		printf ("send buffer set at %i bytes.\n", buffer_size);
	}

/* bind */
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	e = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	if (e < 0) {
		perror("on_startup() failed");
		close(sock);
		g_main_loop_quit(g_loop);
		return FALSE;
	}

/* multicast */
	struct ip_mreq mreq;
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	printf ("listening on interface %s.\n", inet_ntoa(mreq.imr_interface));
	mreq.imr_multiaddr.s_addr = inet_addr(g_network);
	printf ("subscription on multicast address %s.\n", inet_ntoa(mreq.imr_multiaddr));
	e = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
	if (e < 0) {
		perror("on_startup() failed");
		close(sock);
		g_main_loop_quit(g_loop);
		return FALSE;
	}

/* multicast loopback */
/* multicast ttl */

/* add socket to event manager */
	g_io_channel = g_io_channel_unix_new (sock);
	printf ("socket opened with encoding %s.\n", g_io_channel_get_encoding(g_io_channel));

	/* guint event = */ g_io_add_watch (g_io_channel, G_IO_IN | G_IO_PRI, on_io_data, NULL);
	/* guint event = */ g_io_add_watch (g_io_channel, G_IO_ERR | G_IO_HUP | G_IO_NVAL, on_io_error, NULL);

/* period timer to indicate some form of life */
// TODO: Gnome 2.14: replace with g_timeout_add_seconds()
	g_timeout_add(g_mark_interval, (GSourceFunc)on_mark, NULL);

/* periodic timer to snapshot statistics */
	g_timeout_add(g_snap_interval, (GSourceFunc)on_snap, NULL);


	puts ("startup complete.");
	return FALSE;
}

static gboolean
on_mark (
	G_GNUC_UNUSED gpointer data
	)
{
	g_message ("-- MARK --");
	return TRUE;
}

static gboolean
snap_stat (
	G_GNUC_UNUSED gpointer key,
	gpointer	value,
	G_GNUC_UNUSED gpointer user_data
	)
{
	struct hoststat* hoststat = value;

#define SNAP_STAT(name)		\
		{ \
			hoststat->name.snap_count = hoststat->name.count; \
			hoststat->name.snap_bytes = hoststat->name.bytes; \
		} 

	SNAP_STAT(spm);
	SNAP_STAT(poll);
	SNAP_STAT(polr);
	SNAP_STAT(odata);
	SNAP_STAT(rdata);
	SNAP_STAT(nak);
	SNAP_STAT(nnak);
	SNAP_STAT(ncf);
	SNAP_STAT(spmr);

	SNAP_STAT(general);

	return FALSE;
}

static gboolean
on_snap (
	G_GNUC_UNUSED gpointer data
	)
{
	if (!g_hosts) return TRUE;

	g_mutex_lock (g_hosts_mutex);
	gettimeofday(&g_last_snap, NULL);
	g_hash_table_foreach (g_hosts, (GHFunc)snap_stat, NULL);
	g_mutex_unlock (g_hosts_mutex);

	return TRUE;
}

static gboolean
on_io_data (
	GIOChannel*	source,
	G_GNUC_UNUSED GIOCondition condition,
	G_GNUC_UNUSED gpointer data
	)
{
//	printf ("on_data: ");

	struct pgm_sk_buff_t* skb = pgm_alloc_skb (4096);
	static struct timeval tv;
	gettimeofday(&tv, NULL);

	int fd = g_io_channel_unix_get_fd(source);
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	skb->len = recvfrom(fd, skb->head, 4096, MSG_DONTWAIT, (struct sockaddr*)&addr, &addr_len);
	skb->tail = (guint8*)skb->tail + skb->len;

//	printf ("%i bytes received from %s.\n", len, inet_ntoa(addr.sin_addr));

	int e = pgm_parse_raw (skb);

	switch (e) {
	case -2:
/* corrupt packet:
 *
 * we cannot trust the content, as its corrupt, so we don't know the TSI, we could
 * guess from the sender address/multicast group/source port combo or just list separately.
 */ 
		{
			if (!g_nets) {
				g_nets = g_hash_table_new (g_int_hash, g_int_equal);
				g_nets_mutex = g_mutex_new ();
			}

			struct netstat* netstat = g_hash_table_lookup (g_nets, &addr.sin_addr);
			if (netstat == NULL) {
				netstat = g_malloc0(sizeof(struct netstat));
				netstat->addr = addr.sin_addr;
				g_mutex_lock (g_nets_mutex);
				g_hash_table_insert (g_nets, (gpointer)&netstat->addr, (gpointer)netstat);
				g_mutex_unlock (g_nets_mutex);
			}

			netstat->corrupt++;
		}

	case -1:
		fflush(stdout);
		pgm_free_skb (skb);
		return TRUE;

	default: break;
	}

/* search for existing session */
	if (!g_hosts) {
		g_hosts = g_hash_table_new (tsi_hash, tsi_equal);
		g_hosts_mutex = g_mutex_new ();
	}

//	printf ("tsi %s\n", print_tsi (&skb->tsi));

	struct hoststat* hoststat = g_hash_table_lookup (g_hosts, &skb->tsi);
	if (hoststat == NULL) {

/* New TSI */
		printf ("new tsi %s\n", print_tsi (&skb->tsi));

		hoststat = g_malloc0(sizeof(struct hoststat));
		memcpy (&hoststat->tsi, &skb->tsi, sizeof(struct tsi));

		hoststat->session_start = tv;

		g_mutex_lock (g_hosts_mutex);
		g_hash_table_insert (g_hosts, (gpointer)&hoststat->tsi, (gpointer)hoststat);
		g_mutex_unlock (g_hosts_mutex);
	}

/* increment statistics */
	memcpy (&hoststat->last_addr, &addr.sin_addr, sizeof(addr.sin_addr));
	hoststat->general.count++;
	hoststat->general.bytes += skb->len;
	hoststat->general.last = tv;

	skb->data	= (guint8*)skb->data + sizeof(struct pgm_header);
	skb->len       -= sizeof(struct pgm_header);

	gboolean err = FALSE;
        switch (skb->pgm_header->pgm_type) {

/* SPM
 *
 * extract: advertised NLA, used for unicast NAK's;
 *	    SPM sequence number;
 *	    leading and trailing edge of transmit window.
 */
        case PGM_SPM:
		hoststat->spm.count++;
		hoststat->spm.bytes += skb->len;
		hoststat->spm.last = tv;

		err = pgm_verify_spm (skb->pgm_header, skb->data, skb->len);
		g_assert (((struct pgm_spm*)skb->data)->spm_nla_afi == AFI_IP);
		hoststat->nla.s_addr = ((struct pgm_spm*)skb->data)->spm_nla.s_addr;

		if (err) {
//puts ("invalid SPM");
			hoststat->spm.invalid++;
			hoststat->spm.last_invalid = tv;
		} else {
/* TODO: protect against UINT32 wrap-around */
			if (g_ntohl( ((struct pgm_spm*)skb->data)->spm_sqn ) <= hoststat->spm_sqn)
			{
				hoststat->general.duplicate++;
				break;
			}

			hoststat->spm_sqn = g_ntohl( ((struct pgm_spm*)skb->data)->spm_sqn );
			hoststat->txw_trail = g_ntohl( ((struct pgm_spm*)skb->data)->spm_trail );
			hoststat->txw_lead = g_ntohl( ((struct pgm_spm*)skb->data)->spm_lead );
			hoststat->rxw_trail = hoststat->txw_trail;
//printf ("SPM: tx window now %lu - %lu\n", 
//		hoststat->txw_trail, hoststat->txw_lead);

		}
		break;

        case PGM_POLL:
		hoststat->poll.count++;
		hoststat->poll.bytes += skb->len;
		hoststat->poll.last = tv;
		break;

        case PGM_POLR:
		hoststat->polr.count++;
		hoststat->polr.bytes += skb->len;
		hoststat->polr.last = tv;
		break;

/* ODATA
 *
 * extract: trailing edge of transmit window;
 *	    DATA sequence number.
 *
 * TODO: handle UINT32 wrap-arounds.
 */
#define ABS_IN_TRANSMIT_WINDOW(x) \
	( (x) >= hoststat->txw_trail && (x) <= hoststat->txw_lead )

#define IN_TRANSMIT_WINDOW(x) \
	( (x) >= hoststat->txw_trail && (x) <= (hoststat->txw_trail + ((UINT32_MAX/2) - 1)) )

#define IN_RECEIVE_WINDOW(x) \
	( (x) >= hoststat->rxw_trail && (x) <= hoststat->rxw_lead )

#define IS_NEXT_SEQUENCE_NUMBER(x) \
	( (x) == (hoststat->rxw_lead + 1) )

        case PGM_ODATA:
		hoststat->odata.count++;
		hoststat->odata.bytes += skb->len;
		hoststat->odata.last = tv;

		((struct pgm_data*)skb->data)->data_sqn = g_ntohl (((struct pgm_data*)skb->data)->data_sqn);
		((struct pgm_data*)skb->data)->data_trail = g_ntohl (((struct pgm_data*)skb->data)->data_trail);
		if ( !IN_TRANSMIT_WINDOW( ((struct pgm_data*)skb->data)->data_sqn ) )
		{
			printf ("not in tx window %" G_GUINT32_FORMAT ", %" G_GUINT32_FORMAT " - %" G_GUINT32_FORMAT "\n",
				((struct pgm_data*)skb->data)->data_sqn,
				hoststat->txw_trail,
				hoststat->txw_lead);
			err = TRUE;
			hoststat->odata.invalid++;
			hoststat->odata.last_invalid = tv;
			break;
		}

/* first packet */
		if (hoststat->rxw_learning == 0)
		{
puts ("first packet, learning ...");
			hoststat->rxw_trail_init = ((struct pgm_data*)skb->data)->data_sqn;
			hoststat->rxw_learning++;

/* we need to jump to avoid loss detection
 * TODO: work out better arrangement */
			hoststat->rxw_lead = ((struct pgm_data*)skb->data)->data_sqn - 1;
		}
/* subsequent packets */
		else if (hoststat->rxw_learning == 1 &&
			((struct pgm_data*)skb->data)->data_trail > hoststat->rxw_trail_init )
		{
puts ("rx window trail is now past first packet sequence number, stop learning.");
			hoststat->rxw_learning++;
		}

/* dupe */
		if ( IN_RECEIVE_WINDOW( ((struct pgm_data*)skb->data)->data_sqn ) )
		{
			hoststat->general.duplicate++;
			break;
		}

/* lost packets */
		if ( !IS_NEXT_SEQUENCE_NUMBER( ((struct pgm_data*)skb->data)->data_sqn ) )
		{
			printf ("lost %i packets.\n",
				(int)( ((struct pgm_data*)skb->data)->data_sqn - hoststat->rxw_lead - 1 ));
		}

		hoststat->txw_trail = ((struct pgm_data*)skb->data)->data_trail;
		if (hoststat->txw_trail > hoststat->txw_lead)
			hoststat->txw_lead = hoststat->txw_trail -1;
		hoststat->rxw_trail = hoststat->txw_trail;
		hoststat->rxw_lead = ((struct pgm_data*)skb->data)->data_sqn;

		hoststat->odata.tsdu += g_ntohs (skb->pgm_header->pgm_tsdu_length);
		break;

        case PGM_RDATA:
		hoststat->rdata.count++;
		hoststat->rdata.bytes += skb->len;
		hoststat->rdata.last = tv;
		break;

        case PGM_NAK:
		hoststat->nak.count++;
		hoststat->nak.bytes += skb->len;
		hoststat->nak.last = tv;
		break;

        case PGM_NNAK:
		hoststat->nnak.count++;
		hoststat->nnak.bytes += skb->len;
		hoststat->nnak.last = tv;
		break;

        case PGM_NCF:
		hoststat->ncf.count++;
		hoststat->ncf.bytes += skb->len;
		hoststat->ncf.last = tv;
		break;

        case PGM_SPMR:
		hoststat->spmr.count++;
		hoststat->spmr.bytes += skb->len;
		hoststat->spmr.last = tv;
		break;

	default:
		puts ("unknown packet type :(");
		err = TRUE;
		break;
	}

	if (err) {
		hoststat->general.invalid++;
		hoststat->general.last_invalid = tv;
	} else {
		hoststat->general.last_valid = tv;
	}

	fflush(stdout);
	pgm_free_skb (skb);
	return TRUE;
}

static gboolean
on_io_error (
	GIOChannel*	source,
	G_GNUC_UNUSED GIOCondition condition,
	G_GNUC_UNUSED gpointer data
	)
{
	puts ("on_error.");

	GError *err;
	g_io_channel_shutdown (source, FALSE, &err);

/* remove event */
	return FALSE;
}

static gchar*
print_tsi (
	gconstpointer	v
	)
{
	const guint8* gsi = v;
	guint16 source_port = *(const guint16*)(gsi + 6);
	static char buf[sizeof("000.000.000.000.000.000.00000")];
	snprintf(buf, sizeof(buf), "%i.%i.%i.%i.%i.%i.%i",
		gsi[0], gsi[1], gsi[2], gsi[3], gsi[4], gsi[5], g_ntohs (source_port));
	return buf;
}

/* convert a transport session identifier TSI to a hash value
 */

static guint
tsi_hash (
	gconstpointer	v
	)
{
	return g_str_hash(print_tsi(v));
}

/* compare two transport session identifier TSI values and return TRUE if they are equal
 */

static gint
tsi_equal (
	gconstpointer	v,
	gconstpointer	v2
	)
{
	return memcmp (v, v2, (6 * sizeof(guint8)) + sizeof(guint16)) == 0;
}

/* request on web interface
 */

#ifdef CONFIG_LIBSOUP22
static void
robots_callback (
	G_GNUC_UNUSED SoupServerContext*	context,
	SoupMessage*	msg,
	G_GNUC_UNUSED gpointer	data
                )
{
	soup_message_set_response (msg, "text/html", SOUP_BUFFER_STATIC,
				WWW_ROBOTS, strlen(WWW_ROBOTS));
}
#else
static void
robots_callback (
	G_GNUC_UNUSED SoupServer*	server,
	SoupMessage*	msg,
	G_GNUC_UNUSED const char*	path,
	G_GNUC_UNUSED GHashTable* query,
	G_GNUC_UNUSED SoupClientContext* client,
	G_GNUC_UNUSED gpointer data
	)
{
	soup_message_set_response (msg, "text/html", SOUP_MEMORY_STATIC,
				WWW_ROBOTS, strlen(WWW_ROBOTS));
}
#endif

#ifdef CONFIG_LIBSOUP22
static void
css_callback (
	G_GNUC_UNUSED SoupServerContext* context,
	SoupMessage*	msg,
	G_GNUC_UNUSED gpointer data
	)
{
	soup_message_set_response (msg, "text/css", SOUP_BUFFER_STATIC,
				WWW_CSS, strlen(WWW_CSS));
}
#else
static void
css_callback (
	G_GNUC_UNUSED SoupServer*	server,
	SoupMessage*	msg,
	G_GNUC_UNUSED const char*	path,
	G_GNUC_UNUSED GHashTable* query,
	G_GNUC_UNUSED SoupClientContext* client,
	G_GNUC_UNUSED gpointer data
	)
{
	soup_message_set_response (msg, "text/css", SOUP_MEMORY_STATIC,
				WWW_CSS, strlen(WWW_CSS));
}
#endif

#ifdef CONFIG_LIBSOUP22
static void
default_callback (
	G_GNUC_UNUSED SoupServerContext*	context,
	SoupMessage*            msg,
	G_GNUC_UNUSED gpointer	data
	)
{
       	char* path = soup_uri_to_string (soup_message_get_uri (msg), TRUE);
	if (g_hosts && strncmp ("/tsi/", path, strlen("/tsi/")) == 0)
	{
		int e = tsi_callback (msg, path);
		if (e)
		{
		        soup_message_set_response (msg, "text/html", SOUP_BUFFER_STATIC,
		                                        WWW_NOTFOUND, strlen(WWW_NOTFOUND));
		        soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
		        soup_message_add_header (msg->response_headers, "Connection", "close");
		}
		g_free (path);
	}
}
#else
static void
default_callback (
	G_GNUC_UNUSED SoupServer*	server,
	SoupMessage*	msg,
	const char*	path,
	G_GNUC_UNUSED GHashTable* query,
	G_GNUC_UNUSED SoupClientContext* client,
	G_GNUC_UNUSED gpointer data
	)
{
	if (g_hosts && strncmp ("/tsi/", path, strlen("/tsi/")) == 0)
	{
		int e = tsi_callback (msg, path);
		if (e)
		{
		        soup_message_set_response (msg, "text/html", SOUP_MEMORY_STATIC,
		                                        WWW_NOTFOUND, strlen(WWW_NOTFOUND));
		        soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
		        soup_message_headers_append (msg->response_headers, "Connection", "close");
		}
	}
}
#endif

/* transport session identifier TSI index
 */

static const char*
print_si (
	float*		v
	)
{
	static char prefix[5] = "";

	if (*v > 100 * 1000 * 1000) {
		strcpy (prefix, "giga");
		*v /= 1000.0 * 1000.0 * 1000.0;
	} else if (*v > 100 * 1000) {
		strcpy (prefix, "mega");
		*v /= 1000.0 * 1000.0;
	} else if (*v > 100) {
		strcpy (prefix, "kilo");
		*v /= 1000.0;
	} else {
		*prefix = 0;
	}

	return prefix;
}

static gboolean
index_tsi_row (
	G_GNUC_UNUSED gpointer key,
	gpointer	value,
	gpointer	user_data
	)
{
	struct hoststat* hoststat = value;
	GString *response = user_data;

	float secs = (g_tv_now.tv_sec - g_last_snap.tv_sec) +
			( (g_tv_now.tv_usec - g_last_snap.tv_usec) / 1000.0 / 1000.0 );
	float bitrate = ((float)( hoststat->general.bytes - hoststat->general.snap_bytes ) * 8.0 / secs);
	const char* bitprefix = print_si (&bitrate);
	char* tsi_string = print_tsi (&hoststat->tsi);

	g_string_append_printf (response, 
			"<tr>"
				"<td><a href=\"/tsi/%s\">%s</a></td>"
				"<td>%lu</td>"
				"<td>%lu</td>"
				"<td>%.1f pps</td>"
				"<td>%.1f %sbit/s</td>"
				"<td>%3.1f%%</td>"
				"<td>%3.1f%%</td>"
				"<td>%3.1f%%</td>"
			"</tr>",
			tsi_string, tsi_string,
			hoststat->general.count,
			hoststat->general.bytes,
			( hoststat->general.count - hoststat->general.snap_count ) / secs,
			bitrate, bitprefix,
			(100.0 * hoststat->odata.tsdu) / hoststat->general.bytes,
			hoststat->general.invalid ? (100.0 * hoststat->general.invalid) / hoststat->general.count : 0.0,
			hoststat->general.duplicate ? (100.0 * hoststat->general.duplicate) / hoststat->general.count : 0.0
			);

	return FALSE;
}

static gboolean
index_nets_row (
	G_GNUC_UNUSED	gpointer key,
	gpointer	value,
	gpointer	user_data
	)
{
	struct netstat* netstat = value;
	GString *response = user_data;

	g_string_append_printf (response,
			"<tr>"
				"<td>%s</td>"
				"<td>%lu</td>"
			"</tr>",
			inet_ntoa (netstat->addr),
			netstat->corrupt
			);

	return FALSE;
}

#ifdef CONFIG_LIBSOUP22
static void
index_callback (
	G_GNUC_UNUSED SoupServerContext* context,
	SoupMessage*	msg,
	G_GNUC_UNUSED gpointer data
	)
#else
static void
index_callback (
	G_GNUC_UNUSED SoupServer*	server,
	SoupMessage*	msg,
	G_GNUC_UNUSED const char*	path,
	G_GNUC_UNUSED GHashTable* query,
	G_GNUC_UNUSED SoupClientContext* client,
	G_GNUC_UNUSED gpointer data
	)
#endif
{
	GString *response;

	response = g_string_new (WWW_HEADER "<p>");

	if (!g_hosts) {
		g_string_append (response, "<i>No TSI's.</i>");
	} else {
		g_string_append (response, "<table>"
						"<thead><tr>"
							"<th>TSI</th>"
							"<th># Packets</th>"
							"<th># Bytes</th>"
							"<th>Packet Rate</th>"
							"<th>Bitrate</th>"
							"<th>% Data</th>"
							"<th>% Invalid</th>"
							"<th>% Duplicate</th>"
						"</tr></thead>");

		gettimeofday(&g_tv_now, NULL);
		g_mutex_lock (g_hosts_mutex);
		g_hash_table_foreach (g_hosts, (GHFunc)index_tsi_row, response);
		g_mutex_unlock (g_hosts_mutex);
		g_string_append (response, "</table>");
	}

	g_string_append (response, "</p><p>");

	if (!g_nets) {
		g_string_append (response, "<i>No corrupted packets detected.</i>");
	} else {
		g_string_append (response, "<table>"
						"<thead><tr>"
							"<th>Host</th>"
							"<th>Corrupt packets</th>"
						"</tr></thead>");
		g_mutex_lock (g_nets_mutex);
		g_hash_table_foreach (g_nets, (GHFunc)index_nets_row, response);
		g_mutex_unlock (g_nets_mutex);
		g_string_append (response, "</table>");
	}

	g_string_append (response, "</p>" WWW_FOOTER);
	gchar* resp = g_string_free (response, FALSE);
	soup_message_set_response (msg, "text/html",
#ifdef CONFIG_LIBSOUP22
					SOUP_BUFFER_SYSTEM_OWNED,
#else
					SOUP_MEMORY_TAKE,
#endif
					resp, strlen(resp));
}

/* transport session identifier TSI details
 */

static char*
print_stat (
	const char*	name,
	struct sessionstat*	sessionstat,
	float		secs,
	const char*	el		/* xml element name */
	)
{
	static char buf[1024];
	float bitrate = ((float)( sessionstat->bytes - sessionstat->snap_bytes ) * 8.0 / secs);
	const char* bitprefix = print_si (&bitrate);
	
	snprintf (buf, sizeof(buf),
		"<tr>"
			"<%s>%s</%s>"			/* type */
			"<%s>%lu</%s>"			/* # packets */
			"<%s>%lu</%s>"			/* # bytes */
			"<%s>%.1f pps</%s>"		/* packet rate */
			"<%s>%.1f %sbit/s</%s>"		/* bitrate */
			"<%s>%lu / %3.1f%%</%s>"	/* invalid */
		"</tr>",
		el, name, el,
		el, sessionstat->count, el,
		el, sessionstat->bytes, el,
		el, ( sessionstat->count - sessionstat->snap_count ) / secs, el,
		el, bitrate, bitprefix, el,
		el, sessionstat->invalid, sessionstat->invalid ? (100.0 * sessionstat->invalid) / sessionstat->invalid : 0.0, el
		);

	return buf;
}

static int
tsi_callback (
	SoupMessage*	msg,
	const char*	path
	)
{
	struct hoststat* hoststat = NULL;
	struct tsi tsi;

	int e = sscanf (path + strlen("/tsi/"), "%hhu.%hhu.%hhu.%hhu.%hhu.%hhu.%hu",
		&tsi.gsi[0], &tsi.gsi[1], &tsi.gsi[2], &tsi.gsi[3], &tsi.gsi[4], &tsi.gsi[5],
		&tsi.source_port);
	tsi.source_port = g_ntohs (tsi.source_port);

	if (e == 7) {
		hoststat = g_hash_table_lookup (g_hosts, &tsi);
	} else {
		printf ("sscanf found %i elements to tsi, expected 7.\n", e);
	}

	if (!hoststat) {
		return -1;
	}

	GString *response;
	response = g_string_new (WWW_HEADER);

	gettimeofday(&g_tv_now, NULL);
	float secs = (g_tv_now.tv_sec - g_last_snap.tv_sec) +
			( (g_tv_now.tv_usec - g_last_snap.tv_usec) / 1000.0 / 1000.0 );

	char rxw_init[100];
	if (hoststat->rxw_learning)
		snprintf (rxw_init, sizeof(rxw_init), " (RXW_TRAIL_INIT %" G_GUINT32_FORMAT ")", hoststat->rxw_trail_init);

	g_string_append_printf (response, 
				"<p>"
				"<table>"
					"<tr><td><b>TSI:</b></td><td>%s</td></tr>"
					"<tr><td><b>Last IP address:</b></td><td>%s</td></tr>"
					"<tr><td><b>Advertised NLA:</b></td><td>%s</td></tr>"
					"<tr><td><b>Transmit window:</b></td><td>TXW_TRAIL %" G_GUINT32_FORMAT " TXW_LEAD %" G_GUINT32_FORMAT " TXW_SQNS %i</td></tr>"
					"<tr><td><b>Receive window:</b></td><td>RXW_TRAIL %" G_GUINT32_FORMAT "%s RXW_LEAD %" G_GUINT32_FORMAT "</td></tr>"
					"<tr><td><b>SPM sequence number:</b></td><td>%" G_GUINT32_FORMAT "</td></tr>"
				"</table>"
				"</p><p>"
				"<table>"
					"<thead><tr>"
						"<th>Type</th>"
						"<th># Packets</th>"
						"<th># Bytes</th>"
						"<th>Packet Rate</th>"
						"<th>Bitrate</th>"
						"<th>Invalid</th>"
					"</tr></thead>",
				path + strlen("/tsi/"),
				inet_ntoa(hoststat->last_addr),
				inet_ntoa(hoststat->nla),
				hoststat->txw_trail, hoststat->txw_lead,
					(int)( (1 + hoststat->txw_lead) - hoststat->txw_trail ),
				hoststat->rxw_trail, 
					hoststat->rxw_learning ? rxw_init : "",
					hoststat->rxw_lead,
				hoststat->spm_sqn
				);

/* per packet stats */
	g_string_append (response, print_stat ("SPM", &hoststat->spm, secs, "td"));
	g_string_append (response, print_stat ("POLL", &hoststat->poll, secs, "td"));
	g_string_append (response, print_stat ("POLR", &hoststat->polr, secs, "td"));
	g_string_append (response, print_stat ("ODATA", &hoststat->odata, secs, "td"));
	g_string_append (response, print_stat ("RDATA", &hoststat->rdata, secs, "td"));
	g_string_append (response, print_stat ("NAK", &hoststat->nak, secs, "td"));
	g_string_append (response, print_stat ("N-NAK", &hoststat->nnak, secs, "td"));
	g_string_append (response, print_stat ("NCF", &hoststat->ncf, secs, "td"));
	g_string_append (response, print_stat ("SPMR", &hoststat->spmr, secs, "td"));

	g_string_append (response, "<tfoot>");
	g_string_append (response, print_stat ("TOTAL", &hoststat->general, secs, "th"));

	g_string_append (response, 
			"</tfoot>"
			"</table>"
			"</p><p><a href=\"/\">Return to index.</a></p>"
			);

	g_string_append (response, WWW_FOOTER);
	gchar* resp = g_string_free (response, FALSE);
	soup_message_set_response (msg, "text/html",
#ifdef CONFIG_LIBSOUP22
					SOUP_BUFFER_SYSTEM_OWNED,
#else
					SOUP_MEMORY_TAKE,
#endif
					resp, strlen(resp));

	return 0;
}


/* eof */
