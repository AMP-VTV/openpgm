  * C Reference for OpenPGM version 2
  * [Contents](OpenPgm2CReference.md)
  * [Programmer's Checklist](OpenPgm2CReferenceProgrammersChecklist.md)
    * [Build Library](OpenPgm2CReferenceBuildLibrary.md)
    * [Programming Environment](OpenPgm2CReferenceProgrammingEnvironment.md)
    * [Include these Header Files](OpenPgmCReferenceIncludeTheseHeaderFiles.md)
    * [Set these Parameters](OpenPgm2CReferenceSetTheseParameters.md)
    * [Link these Library Files](OpenPgmCReferenceLinkTheseLibraryFiles.md)
    * [Run with these Capabilities](OpenPgmCReferenceRunWithTheseCapabilities.md)
  * [Migration from version 1.0](OpenPgm2CReferenceMigration.md)
  * [Environment](OpenPgm2CReferenceEnvironment.md)
    * <tt><a href='OpenPgm2CReferencePgmInit.md'>pgm_init()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmSupported.md'>pgm_supported()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmDropSuperUser.md'>pgm_drop_superuser()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmShutdown.md'>pgm_shutdown()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmCheckVersion.md'>pgm_major_version</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmCheckVersion.md'>pgm_minor_version</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmCheckVersion.md'>pgm_micro_version</a></tt>
  * [Events](OpenPgm2CReferenceEvents.md)
  * [PGM Socket Buffers (SKBs)](OpenPgm2CReferencePgmSkbs.md)
    * <tt><a href='OpenPgm2CReferencePgmSkBuffT.md'>pgm_sk_buff_t</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAllocSkb.md'>pgm_alloc_skb()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAllocSkb.md'>pgm_skb_get()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAllocSkb.md'>pgm_free_skb()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmSkbPut.md'>pgm_skb_put()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmSkbPut.md'>pgm_skb_pull()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmSkbPut.md'>pgm_skb_reserve()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmSkbCopy.md'>pgm_skb_copy()</a></tt>
  * [Interface](OpenPgm2CReferenceInterface.md)
    * <tt><a href='OpenPgm2CReferencePgmTransportInfoT.md'>pgm_transport_info_t</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmIfGetTransportInfo.md'>pgm_if_get_transport_info()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmIfGetTransportInfo.md'>pgm_if_free_transport_info()</a></tt>
  * [Timing](OpenPgm2CReferenceTiming.md)
    * <tt><a href='OpenPgm2CReferencePgmTimeT.md'>pgm_time_t</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTimeSinceEpoch.md'>pgm_time_since_epoch()</a></tt>
  * [Transport](OpenPgm2CReferenceTransport.md)
    * <tt><a href='OpenPgm2CReferencePgmGsiT.md'>pgm_gsi_t</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmGsiCreateFromAddr.md'>pgm_gsi_create_from_addr()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmGsiCreateFromHostname.md'>pgm_gsi_create_from_hostname()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmGsiCreateFromString.md'>pgm_gsi_create_from_string()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmGsiCreateFromData.md'>pgm_gsi_create_from_data()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmGsiPrint.md'>pgm_gsi_print()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmGsiPrint.md'>pgm_gsi_print_r()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmGsiEqual.md'>pgm_gsi_equal()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTsiT.md'>pgm_tsi_t</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTsiPrint.md'>pgm_tsi_print()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTsiPrint.md'>pgm_tsi_print_r()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTsiEqual.md'>pgm_tsi_equal()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTsiEqual.md'>pgm_tsi_hash()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmIoStatus.md'>PGMIOStatus</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmIoVec.md'>pgm_iovec</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmMsgvT.md'>pgm_msgv_t</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportT.md'>pgm_transport_t</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmRecv.md'>pgm_recv()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmRecv.md'>pgm_recvfrom()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmRecv.md'>pgm_recvmsg()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmRecvMsgv.md'>pgm_recvmsgv()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmSend.md'>pgm_send()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmSendv.md'>pgm_sendv()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmSendSkbv.md'>pgm_send_skbv()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportBind.md'>pgm_transport_bind()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportCreate.md'>pgm_transport_create()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportDestroy.md'>pgm_transport_destroy()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportGetRateRemaining.md'>pgm_transport_get_rate_remaining()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportGetTimerPending.md'>pgm_transport_get_timer_pending()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportGetRecvFd.md'>pgm_transport_get_recv_fd()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportGetRecvFd.md'>pgm_transport_get_pending_fd()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportGetRecvFd.md'>pgm_transport_get_repair_fd()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportGetRecvFd.md'>pgm_transport_get_send_fd()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSelectInfo.md'>pgm_transport_select_info()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportPollInfo.md'>pgm_transport_poll_info()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportEpollCtl.md'>pgm_transport_epoll_ctl()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportMaxTsdu.md'>pgm_transport_max_tsdu()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetAmbientSpm.md'>pgm_transport_set_ambient_spm()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetHeartbeatSpm.md'>pgm_transport_set_heartbeat_spm()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetAbortOnReset.md'>pgm_transport_set_abort_on_reset()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetFec.md'>pgm_transport_set_fec()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetHops.md'>pgm_transport_set_hops()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetMaxTpdu.md'>pgm_transport_set_max_tpdu()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetMulticastLoop.md'>pgm_transport_set_multicast_loop()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetNonBlocking.md'>pgm_transport_set_nonblocking()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetNakDataRetries.md'>pgm_transport_set_nak_data_retries()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetNakNcfRetries.md'>pgm_transport_set_nak_ncf_retries()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetNakBoIvl.md'>pgm_transport_set_nak_bo_ivl()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetNakRdataIvl.md'>pgm_transport_set_nak_rdata_ivl()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetNakRptIvl.md'>pgm_transport_set_nak_rpt_ivl()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetPeerExpiry.md'>pgm_transport_set_peer_expiry()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetRcvBuf.md'>pgm_transport_set_rcvbuf()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetRxwMaxRte.md'>pgm_transport_set_rxw_max_rte()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetRxwSecs.md'>pgm_transport_set_rxw_secs()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetRxwSqns.md'>pgm_transport_set_rxw_sqns()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetRecvOnly.md'>pgm_transport_set_recv_only()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetSendOnly.md'>pgm_transport_set_send_only()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetSndBuf.md'>pgm_transport_set_sndbuf()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetSpmrExpiry.md'>pgm_transport_set_spmr_expiry()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetTxwMaxRte.md'>pgm_transport_set_txw_max_rte()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetTxwSecs.md'>pgm_transport_set_txw_secs()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportSetTxwSqns.md'>pgm_transport_set_txw_sqns()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportJoinGroup.md'>pgm_transport_join_group()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportJoinGroup.md'>pgm_transport_leave_group()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportBlockSource.md'>pgm_transport_block_source()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportBlockSource.md'>pgm_transport_unblock_source()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportJoinSourceGroup.md'>pgm_transport_join_source_group()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportJoinSourceGroup.md'>pgm_transport_leave_source_group()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmTransportMsFilter.md'>pgm_transport_msfilter()</a></tt>
  * [Asynchronous Receiver](OpenPgm2CReferenceAsynchronousReceiver.md)
    * <tt><a href='OpenPgm2CReferencePgmAsyncT.md'>pgm_async_t</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmEventFnT.md'>pgm_eventfn_t</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAsyncCreate.md'>pgm_async_create()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAsyncDestroy.md'>pgm_async_destroy()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAsyncGetFd.md'>pgm_async_get_fd()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAsyncSetNonBlocking.md'>pgm_async_set_nonblocking()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAsyncAddWatch.md'>pgm_async_add_watch()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAsyncAddWatch.md'>pgm_async_add_watch_full()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAsyncCreateWatch.md'>pgm_async_create_watch()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmAsyncRecv.md'>pgm_async_recv()</a></tt>
  * [Error Handling](OpenPgm2CReferenceErrorHandling.md)
  * [Monitoring and Administration](OpenPgm2CReferenceMonitoringAndAdministration.md)
    * <tt><a href='OpenPgm2CReferencePgmSnmpInit.md'>pgm_snmp_init()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmSnmpShutdown.md'>pgm_snmp_shutdown()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmHttpInit.md'>pgm_http_init()</a></tt>
    * <tt><a href='OpenPgm2CReferencePgmHttpShutdown.md'>pgm_http_shutdown()</a></tt>