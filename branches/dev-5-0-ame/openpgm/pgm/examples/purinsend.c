/* vim:ts=8:sts=8:sw=4:noai:noexpandtab
 *
 * プリン PGM sender
 *
 * Copyright (c) 2006-2010 Miru Limited.
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

#include <locale.h>
#include <stdio.h>
#ifdef _WIN32
#	include "getopt.h"
#endif
#include <pgm/pgm.h>


/* globals */

static int		port = 0;
static const char*	network = "";
static bool		use_multicast_loop = FALSE;
static int		udp_encap_port = 0;

static int		max_tpdu = 1500;
static int		max_rte = 400*1000;		/* very conservative rate, 2.5mb/s */
static int		sqns = 100;

static bool		use_fec = FALSE;
static int		rs_k = 8;
static int		rs_n = 255;

static pgm_sock_t*	sock = NULL;

static void usage (const char*) __attribute__((__noreturn__));
static bool create_sock (void);


static void
usage (
	const char*	bin
	)
{
	fprintf (stderr, "Usage: %s [options] message\n", bin);
	fprintf (stderr, "  -n <network>    : Multicast group or unicast IP address\n");
	fprintf (stderr, "  -s <port>       : IP port\n");
	fprintf (stderr, "  -p <port>       : Encapsulate PGM in UDP on IP port\n");
	fprintf (stderr, "  -r <rate>       : Regulate to rate bytes per second\n");
	fprintf (stderr, "  -f <type>       : Enable FEC with either proactive or ondemand parity\n");
	fprintf (stderr, "  -K <k>          : Configure Reed-Solomon code (n, k)\n");
	fprintf (stderr, "  -N <n>\n");
	fprintf (stderr, "  -l              : Enable multicast loopback and address sharing\n");
	fprintf (stderr, "  -i              : List available interfaces\n");
	exit (EXIT_SUCCESS);
}

int
main (
	int	argc,
	char   *argv[]
	)
{
	pgm_error_t* pgm_err = NULL;

	setlocale (LC_ALL, "");

	if (!pgm_init (&pgm_err)) {
		fprintf (stderr, "Unable to start PGM engine: %s\n", pgm_err->message);
		pgm_error_free (pgm_err);
		return EXIT_FAILURE;
	}

/* parse program arguments */
	const char* binary_name = strrchr (argv[0], '/');
	int c;
	while ((c = getopt (argc, argv, "s:n:p:r:f:K:N:lih")) != -1)
	{
		switch (c) {
		case 'n':	network = optarg; break;
		case 's':	port = atoi (optarg); break;
		case 'p':	udp_encap_port = atoi (optarg); break;
		case 'r':	max_rte = atoi (optarg); break;
		case 'f':	use_fec = TRUE; break;
		case 'K':	rs_k = atoi (optarg); break;
		case 'N':	rs_n = atoi (optarg); break;

		case 'l':	use_multicast_loop = TRUE; break;

		case 'i':
			pgm_if_print_all();
			return EXIT_SUCCESS;

		case 'h':
		case '?':
			usage (binary_name);
		}
	}

	if (use_fec && ( !rs_n || !rs_k )) {
		fprintf (stderr, "Invalid Reed-Solomon parameters RS(%d,%d).\n", rs_n, rs_k);
		usage (binary_name);
	}

	if (create_sock())
	{
		while (optind < argc) {
			const int status = pgm_send (sock, argv[optind], strlen (argv[optind]) + 1, NULL);
		        if (PGM_IO_STATUS_NORMAL != status) {
				fprintf (stderr, "pgm_send() failed.\n");
		        }
			optind++;
		}
	}

/* cleanup */
	if (sock) {
		pgm_close (sock, TRUE);
		sock = NULL;
	}
	pgm_shutdown();
	return EXIT_SUCCESS;
}

static
bool
create_sock (void)
{
	struct pgm_addrinfo_t* res = NULL;
	pgm_error_t* pgm_err = NULL;
	sa_family_t sa_family = AF_UNSPEC;

/* parse network parameter into PGM socket address structure */
	if (!pgm_getaddrinfo (network, NULL, &res, &pgm_err)) {
		fprintf (stderr, "Parsing network parameter: %s\n", pgm_err->message);
		goto err_abort;
	}

	sa_family = res->ai_send_addrs[0].gr_group.ss_family;

	if (udp_encap_port) {
		if (!pgm_socket (&sock, sa_family, SOCK_SEQPACKET, IPPROTO_UDP, &pgm_err)) {
			fprintf (stderr, "Creating PGM/UDP socket: %s\n", pgm_err->message);
			goto err_abort;
		}
	} else {
		if (!pgm_socket (&sock, sa_family, SOCK_SEQPACKET, IPPROTO_IP, &pgm_err)) {
			fprintf (stderr, "Creating PGM/IP socket: %s\n", pgm_err->message);
			goto err_abort;
		}
	}

/* Use RFC 2113 tagging for PGM Router Assist */
	const int router_assist = 0;
	pgm_setsockopt (sock, PGM_ROUTER_ALERT, &router_assist, sizeof(router_assist));

	pgm_drop_superuser();

/* set PGM parameters */
	const int send_only = 1,
		  multicast_loop = use_multicast_loop ? 1 : 0,
		  multicast_hops = 16,
		  ambient_spm = pgm_secs (30),
		  heartbeat_spm[] = { pgm_msecs (100),
				      pgm_msecs (100),
                                      pgm_msecs (100),
				      pgm_msecs (100),
				      pgm_msecs (1300),
				      pgm_secs  (7),
				      pgm_secs  (16),
				      pgm_secs  (25),
				      pgm_secs  (30) };

	pgm_setsockopt (sock, PGM_SND_ONLY, &send_only, sizeof(send_only));
	pgm_setsockopt (sock, PGM_MTU, &max_tpdu, sizeof(max_tpdu));
	pgm_setsockopt (sock, PGM_TXW_SQNS, &sqns, sizeof(sqns));
	pgm_setsockopt (sock, PGM_TXW_MAX_RTE, &max_rte, sizeof(max_rte));
	pgm_setsockopt (sock, PGM_MULTICAST_LOOP, &multicast_loop, sizeof(multicast_loop));
	pgm_setsockopt (sock, PGM_MULTICAST_HOPS, &multicast_hops, sizeof(multicast_hops));
	pgm_setsockopt (sock, PGM_AMBIENT_SPM, &ambient_spm, sizeof(ambient_spm));
	pgm_setsockopt (sock, PGM_HEARTBEAT_SPM, &heartbeat_spm, sizeof(heartbeat_spm));
	if (use_fec) {
		const int pro_parity = 0,
			  ondemand_parity = 1,
			  var_pkt_len = 1,
			  block_size = rs_n,
			  group_size = rs_k;

		pgm_setsockopt (sock, PGM_PROACTIVEPKTS, &pro_parity, sizeof(pro_parity));
		pgm_setsockopt (sock, PGM_ONDEMANDPARITY, &ondemand_parity, sizeof(ondemand_parity));
		pgm_setsockopt (sock, PGM_VARPKTLEN, &var_pkt_len, sizeof(var_pkt_len));
		pgm_setsockopt (sock, PGM_FEC_BLOCK_SIZE, &block_size, sizeof(block_size));
		pgm_setsockopt (sock, PGM_FEC_GROUP_SIZE, &group_size, sizeof(group_size));
	}

/* create global session identifier */
	struct pgm_sockaddr_t addr;
	memset (addr, 0, sizeof(addr));
	addr.sa_port = port;
	if (!pgm_gsi_create_from_hostname (&addr.sa_addr, &pgm_err)) {
		fprintf (stderr, "Creating GSI: %s\n", pgm_err->message);
		goto err_abort;
	}

/* assign socket to specified address */
	if (udp_encap_port) {
		struct sockaddr_in udpaddr;
		memset (udpaddr, 0, sizeof(udpaddr));
		udpaddr.sin_family = sa_family;
		udpaddr.sin_port = udp_encap_port;
		if (!pgm_bind_udp (sock, &addr, sizeof(addr), &udpaddr, sizeof(udpaddr), &pgm_err)) {
			fprintf (stderr, "Binding PGM/UDP socket: %s\n", pgm_err->message);
			goto err_abort;
		}
	} else {
		if (!pgm_bind (sock, &addr, sizeof(addr), &pgm_err)) {
			fprintf (stderr, "Binding PGM/IP socket: %s\n", pgm_err->message);
			goto err_abort;
		}
	}

/* join IP multicast groups */
	for (unsigned i = 0; i < res->ai_recv_addrs_len; i++)
		pgm_setsockopt (sock, PGM_JOIN_GROUP, &res->ai_recv_addrs[i], sizeof(struct group_req));
	pgm_setsockopt (sock, PGM_SEND_GROUP, &res->ai_send_addrs[0], sizeof(struct group_req));
	pgm_freeaddrinfo (res);

	return TRUE;

err_abort:
	if (NULL != res) {
		pgm_freeaddrinfo (res);
		res = NULL;
	}
	if (NULL != pgm_err) {
		pgm_error_free (pgm_err);
		pgm_err = NULL;
	}
	if (NULL != sock) {
		pgm_close (sock, FALSE);
		sock = NULL;
	}
	return FALSE;
}

/* eof */
