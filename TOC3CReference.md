  * C Reference for OpenPGM version 3
  * [Contents](OpenPgm3CReference.md)
  * [Programmer's Checklist](OpenPgm3CReferenceProgrammersChecklist.md)
    * [Build Library](OpenPgm3CReferenceBuildLibrary.md)
    * [Programming Environment](OpenPgm3CReferenceProgrammingEnvironment.md)
    * [Include these Header Files](OpenPgm3CReferenceIncludeTheseHeaderFiles.md)
    * [Set these Parameters](OpenPgm3CReferenceSetTheseParameters.md)
    * [Link these Library Files](OpenPgm3CReferenceLinkTheseLibraryFiles.md)
    * [Run with these Capabilities](OpenPgm3CReferenceRunWithTheseCapabilities.md)
  * [Migration from version 1.0](OpenPgm3CReferenceMigration.md)
  * [Environment](OpenPgm3CReferenceEnvironment.md)
    * <tt><a href='OpenPgm3CReferencePgmInit.md'>pgm_init()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmSupported.md'>pgm_supported()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmDropSuperUser.md'>pgm_drop_superuser()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmShutdown.md'>pgm_shutdown()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmCheckVersion.md'>pgm_major_version</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmCheckVersion.md'>pgm_minor_version</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmCheckVersion.md'>pgm_micro_version</a></tt>
  * [Events](OpenPgm3CReferenceEvents.md)
  * [PGM Socket Buffers (SKBs)](OpenPgm3CReferencePgmSkbs.md)
    * <tt><a href='OpenPgm3CReferencePgmSkBuffT.md'>pgm_sk_buff_t</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmAllocSkb.md'>pgm_alloc_skb()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmAllocSkb.md'>pgm_skb_get()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmAllocSkb.md'>pgm_free_skb()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmSkbPut.md'>pgm_skb_put()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmSkbPut.md'>pgm_skb_pull()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmSkbPut.md'>pgm_skb_reserve()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmSkbCopy.md'>pgm_skb_copy()</a></tt>
  * [Interface](OpenPgm3CReferenceInterface.md)
    * <tt><a href='OpenPgm3CReferencePgmTransportInfoT.md'>pgm_transport_info_t</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmIfGetTransportInfo.md'>pgm_if_get_transport_info()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmIfGetTransportInfo.md'>pgm_if_free_transport_info()</a></tt>
  * [Timing](OpenPgm3CReferenceTiming.md)
    * <tt><a href='OpenPgm3CReferencePgmTimeT.md'>pgm_time_t</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTimeSinceEpoch.md'>pgm_time_since_epoch()</a></tt>
  * [Transport](OpenPgm3CReferenceTransport.md)
    * <tt><a href='OpenPgm3CReferencePgmGsiT.md'>pgm_gsi_t</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmGsiCreateFromAddr.md'>pgm_gsi_create_from_addr()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmGsiCreateFromHostname.md'>pgm_gsi_create_from_hostname()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmGsiCreateFromString.md'>pgm_gsi_create_from_string()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmGsiCreateFromData.md'>pgm_gsi_create_from_data()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmGsiPrint.md'>pgm_gsi_print()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmGsiPrint.md'>pgm_gsi_print_r()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmGsiEqual.md'>pgm_gsi_equal()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTsiT.md'>pgm_tsi_t</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTsiPrint.md'>pgm_tsi_print()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTsiPrint.md'>pgm_tsi_print_r()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTsiEqual.md'>pgm_tsi_equal()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTsiEqual.md'>pgm_tsi_hash()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmIoVec.md'>pgm_iovec</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmMsgvT.md'>pgm_msgv_t</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportT.md'>pgm_transport_t</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmRecv.md'>pgm_recv()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmRecv.md'>pgm_recvfrom()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmRecv.md'>pgm_recvmsg()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmRecvMsgv.md'>pgm_recvmsgv()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmSend.md'>pgm_send()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmSendv.md'>pgm_sendv()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmSendSkbv.md'>pgm_send_skbv()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportBind.md'>pgm_transport_bind()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportCreate.md'>pgm_transport_create()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportDestroy.md'>pgm_transport_destroy()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportGetRateRemaining.md'>pgm_transport_get_rate_remaining()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportGetTimerPending.md'>pgm_transport_get_timer_pending()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportGetRecvFd.md'>pgm_transport_get_recv_fd()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportGetRecvFd.md'>pgm_transport_get_pending_fd()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportGetRecvFd.md'>pgm_transport_get_repair_fd()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportGetRecvFd.md'>pgm_transport_get_send_fd()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSelectInfo.md'>pgm_transport_select_info()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportPollInfo.md'>pgm_transport_poll_info()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportEpollCtl.md'>pgm_transport_epoll_ctl()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportMaxTsdu.md'>pgm_transport_max_tsdu()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetAmbientSpm.md'>pgm_transport_set_ambient_spm()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetHeartbeatSpm.md'>pgm_transport_set_heartbeat_spm()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetAbortOnReset.md'>pgm_transport_set_abort_on_reset()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetFec.md'>pgm_transport_set_fec()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetHops.md'>pgm_transport_set_hops()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetMaxTpdu.md'>pgm_transport_set_max_tpdu()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetMulticastLoop.md'>pgm_transport_set_multicast_loop()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetNonBlocking.md'>pgm_transport_set_nonblocking()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetNakDataRetries.md'>pgm_transport_set_nak_data_retries()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetNakNcfRetries.md'>pgm_transport_set_nak_ncf_retries()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetNakBoIvl.md'>pgm_transport_set_nak_bo_ivl()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetNakRdataIvl.md'>pgm_transport_set_nak_rdata_ivl()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetNakRptIvl.md'>pgm_transport_set_nak_rpt_ivl()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetPeerExpiry.md'>pgm_transport_set_peer_expiry()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetRcvBuf.md'>pgm_transport_set_rcvbuf()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetRxwMaxRte.md'>pgm_transport_set_rxw_max_rte()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetRxwSecs.md'>pgm_transport_set_rxw_secs()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetRxwSqns.md'>pgm_transport_set_rxw_sqns()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetRecvOnly.md'>pgm_transport_set_recv_only()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetSendOnly.md'>pgm_transport_set_send_only()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetSndBuf.md'>pgm_transport_set_sndbuf()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetSpmrExpiry.md'>pgm_transport_set_spmr_expiry()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetTxwMaxRte.md'>pgm_transport_set_txw_max_rte()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetTxwSecs.md'>pgm_transport_set_txw_secs()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportSetTxwSqns.md'>pgm_transport_set_txw_sqns()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportJoinGroup.md'>pgm_transport_join_group()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportJoinGroup.md'>pgm_transport_leave_group()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportBlockSource.md'>pgm_transport_block_source()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportBlockSource.md'>pgm_transport_unblock_source()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportJoinSourceGroup.md'>pgm_transport_join_source_group()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportJoinSourceGroup.md'>pgm_transport_leave_source_group()</a></tt>
    * <tt><a href='OpenPgm3CReferencePgmTransportMsFilter.md'>pgm_transport_msfilter()</a></tt>
  * [Error Handling](OpenPgm3CReferenceErrorHandling.md)