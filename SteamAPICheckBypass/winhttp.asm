ifdef RAX
	.code
		extern OriginalFuncs_winhttp:QWORD
		DllCanUnloadNow proc
			jmp QWORD ptr OriginalFuncs_winhttp[0 * 8]
		DllCanUnloadNow endp
		DllGetClassObject proc
			jmp QWORD ptr OriginalFuncs_winhttp[1 * 8]
		DllGetClassObject endp
		Private1 proc
			jmp QWORD ptr OriginalFuncs_winhttp[2 * 8]
		Private1 endp
		SvchostPushServiceGlobals proc
			jmp QWORD ptr OriginalFuncs_winhttp[3 * 8]
		SvchostPushServiceGlobals endp
		WinHttpAddRequestHeaders proc
			jmp QWORD ptr OriginalFuncs_winhttp[4 * 8]
		WinHttpAddRequestHeaders endp
		WinHttpAddRequestHeadersEx proc
			jmp QWORD ptr OriginalFuncs_winhttp[5 * 8]
		WinHttpAddRequestHeadersEx endp
		WinHttpAutoProxySvcMain proc
			jmp QWORD ptr OriginalFuncs_winhttp[6 * 8]
		WinHttpAutoProxySvcMain endp
		WinHttpCheckPlatform proc
			jmp QWORD ptr OriginalFuncs_winhttp[7 * 8]
		WinHttpCheckPlatform endp
		WinHttpCloseHandle proc
			jmp QWORD ptr OriginalFuncs_winhttp[8 * 8]
		WinHttpCloseHandle endp
		WinHttpConnect proc
			jmp QWORD ptr OriginalFuncs_winhttp[9 * 8]
		WinHttpConnect endp
		WinHttpConnectionDeletePolicyEntries proc
			jmp QWORD ptr OriginalFuncs_winhttp[10 * 8]
		WinHttpConnectionDeletePolicyEntries endp
		WinHttpConnectionDeleteProxyInfo proc
			jmp QWORD ptr OriginalFuncs_winhttp[11 * 8]
		WinHttpConnectionDeleteProxyInfo endp
		WinHttpConnectionFreeNameList proc
			jmp QWORD ptr OriginalFuncs_winhttp[12 * 8]
		WinHttpConnectionFreeNameList endp
		WinHttpConnectionFreeProxyInfo proc
			jmp QWORD ptr OriginalFuncs_winhttp[13 * 8]
		WinHttpConnectionFreeProxyInfo endp
		WinHttpConnectionFreeProxyList proc
			jmp QWORD ptr OriginalFuncs_winhttp[14 * 8]
		WinHttpConnectionFreeProxyList endp
		WinHttpConnectionGetNameList proc
			jmp QWORD ptr OriginalFuncs_winhttp[15 * 8]
		WinHttpConnectionGetNameList endp
		WinHttpConnectionGetProxyInfo proc
			jmp QWORD ptr OriginalFuncs_winhttp[16 * 8]
		WinHttpConnectionGetProxyInfo endp
		WinHttpConnectionGetProxyList proc
			jmp QWORD ptr OriginalFuncs_winhttp[17 * 8]
		WinHttpConnectionGetProxyList endp
		WinHttpConnectionOnlyConvert proc
			jmp QWORD ptr OriginalFuncs_winhttp[18 * 8]
		WinHttpConnectionOnlyConvert endp
		WinHttpConnectionOnlyReceive proc
			jmp QWORD ptr OriginalFuncs_winhttp[19 * 8]
		WinHttpConnectionOnlyReceive endp
		WinHttpConnectionOnlySend proc
			jmp QWORD ptr OriginalFuncs_winhttp[20 * 8]
		WinHttpConnectionOnlySend endp
		WinHttpConnectionSetPolicyEntries proc
			jmp QWORD ptr OriginalFuncs_winhttp[21 * 8]
		WinHttpConnectionSetPolicyEntries endp
		WinHttpConnectionSetProxyInfo proc
			jmp QWORD ptr OriginalFuncs_winhttp[22 * 8]
		WinHttpConnectionSetProxyInfo endp
		WinHttpConnectionUpdateIfIndexTable proc
			jmp QWORD ptr OriginalFuncs_winhttp[23 * 8]
		WinHttpConnectionUpdateIfIndexTable endp
		WinHttpCrackUrl proc
			jmp QWORD ptr OriginalFuncs_winhttp[24 * 8]
		WinHttpCrackUrl endp
		WinHttpCreateProxyList proc
			jmp QWORD ptr OriginalFuncs_winhttp[25 * 8]
		WinHttpCreateProxyList endp
		WinHttpCreateProxyManager proc
			jmp QWORD ptr OriginalFuncs_winhttp[26 * 8]
		WinHttpCreateProxyManager endp
		WinHttpCreateProxyResolver proc
			jmp QWORD ptr OriginalFuncs_winhttp[27 * 8]
		WinHttpCreateProxyResolver endp
		WinHttpCreateProxyResult proc
			jmp QWORD ptr OriginalFuncs_winhttp[28 * 8]
		WinHttpCreateProxyResult endp
		WinHttpCreateUiCompatibleProxyString proc
			jmp QWORD ptr OriginalFuncs_winhttp[29 * 8]
		WinHttpCreateUiCompatibleProxyString endp
		WinHttpCreateUrl proc
			jmp QWORD ptr OriginalFuncs_winhttp[30 * 8]
		WinHttpCreateUrl endp
		WinHttpDetectAutoProxyConfigUrl proc
			jmp QWORD ptr OriginalFuncs_winhttp[31 * 8]
		WinHttpDetectAutoProxyConfigUrl endp
		WinHttpFreeProxyResult proc
			jmp QWORD ptr OriginalFuncs_winhttp[32 * 8]
		WinHttpFreeProxyResult endp
		WinHttpFreeProxyResultEx proc
			jmp QWORD ptr OriginalFuncs_winhttp[33 * 8]
		WinHttpFreeProxyResultEx endp
		WinHttpFreeProxySettings proc
			jmp QWORD ptr OriginalFuncs_winhttp[34 * 8]
		WinHttpFreeProxySettings endp
		WinHttpFreeProxySettingsEx proc
			jmp QWORD ptr OriginalFuncs_winhttp[35 * 8]
		WinHttpFreeProxySettingsEx endp
		WinHttpFreeQueryConnectionGroupResult proc
			jmp QWORD ptr OriginalFuncs_winhttp[36 * 8]
		WinHttpFreeQueryConnectionGroupResult endp
		WinHttpGetDefaultProxyConfiguration proc
			jmp QWORD ptr OriginalFuncs_winhttp[37 * 8]
		WinHttpGetDefaultProxyConfiguration endp
		WinHttpGetIEProxyConfigForCurrentUser proc
			jmp QWORD ptr OriginalFuncs_winhttp[38 * 8]
		WinHttpGetIEProxyConfigForCurrentUser endp
		WinHttpGetProxyForUrl proc
			jmp QWORD ptr OriginalFuncs_winhttp[39 * 8]
		WinHttpGetProxyForUrl endp
		WinHttpGetProxyForUrlEx proc
			jmp QWORD ptr OriginalFuncs_winhttp[40 * 8]
		WinHttpGetProxyForUrlEx endp
		WinHttpGetProxyForUrlEx2 proc
			jmp QWORD ptr OriginalFuncs_winhttp[41 * 8]
		WinHttpGetProxyForUrlEx2 endp
		WinHttpGetProxyForUrlHvsi proc
			jmp QWORD ptr OriginalFuncs_winhttp[42 * 8]
		WinHttpGetProxyForUrlHvsi endp
		WinHttpGetProxyResult proc
			jmp QWORD ptr OriginalFuncs_winhttp[43 * 8]
		WinHttpGetProxyResult endp
		WinHttpGetProxyResultEx proc
			jmp QWORD ptr OriginalFuncs_winhttp[44 * 8]
		WinHttpGetProxyResultEx endp
		WinHttpGetProxySettingsEx proc
			jmp QWORD ptr OriginalFuncs_winhttp[45 * 8]
		WinHttpGetProxySettingsEx endp
		WinHttpGetProxySettingsResultEx proc
			jmp QWORD ptr OriginalFuncs_winhttp[46 * 8]
		WinHttpGetProxySettingsResultEx endp
		WinHttpGetProxySettingsVersion proc
			jmp QWORD ptr OriginalFuncs_winhttp[47 * 8]
		WinHttpGetProxySettingsVersion endp
		WinHttpGetTunnelSocket proc
			jmp QWORD ptr OriginalFuncs_winhttp[48 * 8]
		WinHttpGetTunnelSocket endp
		WinHttpOpen proc
			jmp QWORD ptr OriginalFuncs_winhttp[49 * 8]
		WinHttpOpen endp
		WinHttpOpenRequest proc
			jmp QWORD ptr OriginalFuncs_winhttp[50 * 8]
		WinHttpOpenRequest endp
		WinHttpPacJsWorkerMain proc
			jmp QWORD ptr OriginalFuncs_winhttp[51 * 8]
		WinHttpPacJsWorkerMain endp
		WinHttpProbeConnectivity proc
			jmp QWORD ptr OriginalFuncs_winhttp[52 * 8]
		WinHttpProbeConnectivity endp
		WinHttpProtocolCompleteUpgrade proc
			jmp QWORD ptr OriginalFuncs_winhttp[53 * 8]
		WinHttpProtocolCompleteUpgrade endp
		WinHttpProtocolReceive proc
			jmp QWORD ptr OriginalFuncs_winhttp[54 * 8]
		WinHttpProtocolReceive endp
		WinHttpProtocolSend proc
			jmp QWORD ptr OriginalFuncs_winhttp[55 * 8]
		WinHttpProtocolSend endp
		WinHttpQueryAuthSchemes proc
			jmp QWORD ptr OriginalFuncs_winhttp[56 * 8]
		WinHttpQueryAuthSchemes endp
		WinHttpQueryConnectionGroup proc
			jmp QWORD ptr OriginalFuncs_winhttp[57 * 8]
		WinHttpQueryConnectionGroup endp
		WinHttpQueryDataAvailable proc
			jmp QWORD ptr OriginalFuncs_winhttp[58 * 8]
		WinHttpQueryDataAvailable endp
		WinHttpQueryHeaders proc
			jmp QWORD ptr OriginalFuncs_winhttp[59 * 8]
		WinHttpQueryHeaders endp
		WinHttpQueryHeadersEx proc
			jmp QWORD ptr OriginalFuncs_winhttp[60 * 8]
		WinHttpQueryHeadersEx endp
		WinHttpQueryOption proc
			jmp QWORD ptr OriginalFuncs_winhttp[61 * 8]
		WinHttpQueryOption endp
		WinHttpReadData proc
			jmp QWORD ptr OriginalFuncs_winhttp[62 * 8]
		WinHttpReadData endp
		WinHttpReadDataEx proc
			jmp QWORD ptr OriginalFuncs_winhttp[63 * 8]
		WinHttpReadDataEx endp
		WinHttpReadProxySettings proc
			jmp QWORD ptr OriginalFuncs_winhttp[64 * 8]
		WinHttpReadProxySettings endp
		WinHttpReadProxySettingsHvsi proc
			jmp QWORD ptr OriginalFuncs_winhttp[65 * 8]
		WinHttpReadProxySettingsHvsi endp
		WinHttpReceiveResponse proc
			jmp QWORD ptr OriginalFuncs_winhttp[66 * 8]
		WinHttpReceiveResponse endp
		WinHttpRefreshProxySettings proc
			jmp QWORD ptr OriginalFuncs_winhttp[67 * 8]
		WinHttpRefreshProxySettings endp
		WinHttpRegisterProxyChangeNotification proc
			jmp QWORD ptr OriginalFuncs_winhttp[68 * 8]
		WinHttpRegisterProxyChangeNotification endp
		WinHttpResetAutoProxy proc
			jmp QWORD ptr OriginalFuncs_winhttp[69 * 8]
		WinHttpResetAutoProxy endp
		WinHttpResolverGetProxyForUrl proc
			jmp QWORD ptr OriginalFuncs_winhttp[70 * 8]
		WinHttpResolverGetProxyForUrl endp
		WinHttpSaveProxyCredentials proc
			jmp QWORD ptr OriginalFuncs_winhttp[71 * 8]
		WinHttpSaveProxyCredentials endp
		WinHttpSendRequest proc
			jmp QWORD ptr OriginalFuncs_winhttp[72 * 8]
		WinHttpSendRequest endp
		WinHttpSetCredentials proc
			jmp QWORD ptr OriginalFuncs_winhttp[73 * 8]
		WinHttpSetCredentials endp
		WinHttpSetDefaultProxyConfiguration proc
			jmp QWORD ptr OriginalFuncs_winhttp[74 * 8]
		WinHttpSetDefaultProxyConfiguration endp
		WinHttpSetOption proc
			jmp QWORD ptr OriginalFuncs_winhttp[75 * 8]
		WinHttpSetOption endp
		WinHttpSetProxySettingsPerUser proc
			jmp QWORD ptr OriginalFuncs_winhttp[76 * 8]
		WinHttpSetProxySettingsPerUser endp
		WinHttpSetSecureLegacyServersAppCompat proc
			jmp QWORD ptr OriginalFuncs_winhttp[77 * 8]
		WinHttpSetSecureLegacyServersAppCompat endp
		WinHttpSetStatusCallback proc
			jmp QWORD ptr OriginalFuncs_winhttp[78 * 8]
		WinHttpSetStatusCallback endp
		WinHttpSetTimeouts proc
			jmp QWORD ptr OriginalFuncs_winhttp[79 * 8]
		WinHttpSetTimeouts endp
		WinHttpTimeFromSystemTime proc
			jmp QWORD ptr OriginalFuncs_winhttp[80 * 8]
		WinHttpTimeFromSystemTime endp
		WinHttpTimeToSystemTime proc
			jmp QWORD ptr OriginalFuncs_winhttp[81 * 8]
		WinHttpTimeToSystemTime endp
		WinHttpUnregisterProxyChangeNotification proc
			jmp QWORD ptr OriginalFuncs_winhttp[82 * 8]
		WinHttpUnregisterProxyChangeNotification endp
		WinHttpWebSocketClose proc
			jmp QWORD ptr OriginalFuncs_winhttp[83 * 8]
		WinHttpWebSocketClose endp
		WinHttpWebSocketCompleteUpgrade proc
			jmp QWORD ptr OriginalFuncs_winhttp[84 * 8]
		WinHttpWebSocketCompleteUpgrade endp
		WinHttpWebSocketQueryCloseStatus proc
			jmp QWORD ptr OriginalFuncs_winhttp[85 * 8]
		WinHttpWebSocketQueryCloseStatus endp
		WinHttpWebSocketReceive proc
			jmp QWORD ptr OriginalFuncs_winhttp[86 * 8]
		WinHttpWebSocketReceive endp
		WinHttpWebSocketSend proc
			jmp QWORD ptr OriginalFuncs_winhttp[87 * 8]
		WinHttpWebSocketSend endp
		WinHttpWebSocketShutdown proc
			jmp QWORD ptr OriginalFuncs_winhttp[88 * 8]
		WinHttpWebSocketShutdown endp
		WinHttpWriteData proc
			jmp QWORD ptr OriginalFuncs_winhttp[89 * 8]
		WinHttpWriteData endp
		WinHttpWriteProxySettings proc
			jmp QWORD ptr OriginalFuncs_winhttp[90 * 8]
		WinHttpWriteProxySettings endp
else
	.model flat, C
	.stack 4096
	.code
		extern OriginalFuncs_winhttp:DWORD
		DllCanUnloadNow proc
			jmp DWORD ptr OriginalFuncs_winhttp[0 * 4]
		DllCanUnloadNow endp
		DllGetClassObject proc
			jmp DWORD ptr OriginalFuncs_winhttp[1 * 4]
		DllGetClassObject endp
		Private1 proc
			jmp DWORD ptr OriginalFuncs_winhttp[2 * 4]
		Private1 endp
		SvchostPushServiceGlobals proc
			jmp DWORD ptr OriginalFuncs_winhttp[3 * 4]
		SvchostPushServiceGlobals endp
		WinHttpAddRequestHeaders proc
			jmp DWORD ptr OriginalFuncs_winhttp[4 * 4]
		WinHttpAddRequestHeaders endp
		WinHttpAddRequestHeadersEx proc
			jmp DWORD ptr OriginalFuncs_winhttp[5 * 4]
		WinHttpAddRequestHeadersEx endp
		WinHttpAutoProxySvcMain proc
			jmp DWORD ptr OriginalFuncs_winhttp[6 * 4]
		WinHttpAutoProxySvcMain endp
		WinHttpCheckPlatform proc
			jmp DWORD ptr OriginalFuncs_winhttp[7 * 4]
		WinHttpCheckPlatform endp
		WinHttpCloseHandle proc
			jmp DWORD ptr OriginalFuncs_winhttp[8 * 4]
		WinHttpCloseHandle endp
		WinHttpConnect proc
			jmp DWORD ptr OriginalFuncs_winhttp[9 * 4]
		WinHttpConnect endp
		WinHttpConnectionDeletePolicyEntries proc
			jmp DWORD ptr OriginalFuncs_winhttp[10 * 4]
		WinHttpConnectionDeletePolicyEntries endp
		WinHttpConnectionDeleteProxyInfo proc
			jmp DWORD ptr OriginalFuncs_winhttp[11 * 4]
		WinHttpConnectionDeleteProxyInfo endp
		WinHttpConnectionFreeNameList proc
			jmp DWORD ptr OriginalFuncs_winhttp[12 * 4]
		WinHttpConnectionFreeNameList endp
		WinHttpConnectionFreeProxyInfo proc
			jmp DWORD ptr OriginalFuncs_winhttp[13 * 4]
		WinHttpConnectionFreeProxyInfo endp
		WinHttpConnectionFreeProxyList proc
			jmp DWORD ptr OriginalFuncs_winhttp[14 * 4]
		WinHttpConnectionFreeProxyList endp
		WinHttpConnectionGetNameList proc
			jmp DWORD ptr OriginalFuncs_winhttp[15 * 4]
		WinHttpConnectionGetNameList endp
		WinHttpConnectionGetProxyInfo proc
			jmp DWORD ptr OriginalFuncs_winhttp[16 * 4]
		WinHttpConnectionGetProxyInfo endp
		WinHttpConnectionGetProxyList proc
			jmp DWORD ptr OriginalFuncs_winhttp[17 * 4]
		WinHttpConnectionGetProxyList endp
		WinHttpConnectionOnlyConvert proc
			jmp DWORD ptr OriginalFuncs_winhttp[18 * 4]
		WinHttpConnectionOnlyConvert endp
		WinHttpConnectionOnlyReceive proc
			jmp DWORD ptr OriginalFuncs_winhttp[19 * 4]
		WinHttpConnectionOnlyReceive endp
		WinHttpConnectionOnlySend proc
			jmp DWORD ptr OriginalFuncs_winhttp[20 * 4]
		WinHttpConnectionOnlySend endp
		WinHttpConnectionSetPolicyEntries proc
			jmp DWORD ptr OriginalFuncs_winhttp[21 * 4]
		WinHttpConnectionSetPolicyEntries endp
		WinHttpConnectionSetProxyInfo proc
			jmp DWORD ptr OriginalFuncs_winhttp[22 * 4]
		WinHttpConnectionSetProxyInfo endp
		WinHttpConnectionUpdateIfIndexTable proc
			jmp DWORD ptr OriginalFuncs_winhttp[23 * 4]
		WinHttpConnectionUpdateIfIndexTable endp
		WinHttpCrackUrl proc
			jmp DWORD ptr OriginalFuncs_winhttp[24 * 4]
		WinHttpCrackUrl endp
		WinHttpCreateProxyList proc
			jmp DWORD ptr OriginalFuncs_winhttp[25 * 4]
		WinHttpCreateProxyList endp
		WinHttpCreateProxyManager proc
			jmp DWORD ptr OriginalFuncs_winhttp[26 * 4]
		WinHttpCreateProxyManager endp
		WinHttpCreateProxyResolver proc
			jmp DWORD ptr OriginalFuncs_winhttp[27 * 4]
		WinHttpCreateProxyResolver endp
		WinHttpCreateProxyResult proc
			jmp DWORD ptr OriginalFuncs_winhttp[28 * 4]
		WinHttpCreateProxyResult endp
		WinHttpCreateUiCompatibleProxyString proc
			jmp DWORD ptr OriginalFuncs_winhttp[29 * 4]
		WinHttpCreateUiCompatibleProxyString endp
		WinHttpCreateUrl proc
			jmp DWORD ptr OriginalFuncs_winhttp[30 * 4]
		WinHttpCreateUrl endp
		WinHttpDetectAutoProxyConfigUrl proc
			jmp DWORD ptr OriginalFuncs_winhttp[31 * 4]
		WinHttpDetectAutoProxyConfigUrl endp
		WinHttpFreeProxyResult proc
			jmp DWORD ptr OriginalFuncs_winhttp[32 * 4]
		WinHttpFreeProxyResult endp
		WinHttpFreeProxyResultEx proc
			jmp DWORD ptr OriginalFuncs_winhttp[33 * 4]
		WinHttpFreeProxyResultEx endp
		WinHttpFreeProxySettings proc
			jmp DWORD ptr OriginalFuncs_winhttp[34 * 4]
		WinHttpFreeProxySettings endp
		WinHttpFreeProxySettingsEx proc
			jmp DWORD ptr OriginalFuncs_winhttp[35 * 4]
		WinHttpFreeProxySettingsEx endp
		WinHttpFreeQueryConnectionGroupResult proc
			jmp DWORD ptr OriginalFuncs_winhttp[36 * 4]
		WinHttpFreeQueryConnectionGroupResult endp
		WinHttpGetDefaultProxyConfiguration proc
			jmp DWORD ptr OriginalFuncs_winhttp[37 * 4]
		WinHttpGetDefaultProxyConfiguration endp
		WinHttpGetIEProxyConfigForCurrentUser proc
			jmp DWORD ptr OriginalFuncs_winhttp[38 * 4]
		WinHttpGetIEProxyConfigForCurrentUser endp
		WinHttpGetProxyForUrl proc
			jmp DWORD ptr OriginalFuncs_winhttp[39 * 4]
		WinHttpGetProxyForUrl endp
		WinHttpGetProxyForUrlEx proc
			jmp DWORD ptr OriginalFuncs_winhttp[40 * 4]
		WinHttpGetProxyForUrlEx endp
		WinHttpGetProxyForUrlEx2 proc
			jmp DWORD ptr OriginalFuncs_winhttp[41 * 4]
		WinHttpGetProxyForUrlEx2 endp
		WinHttpGetProxyForUrlHvsi proc
			jmp DWORD ptr OriginalFuncs_winhttp[42 * 4]
		WinHttpGetProxyForUrlHvsi endp
		WinHttpGetProxyResult proc
			jmp DWORD ptr OriginalFuncs_winhttp[43 * 4]
		WinHttpGetProxyResult endp
		WinHttpGetProxyResultEx proc
			jmp DWORD ptr OriginalFuncs_winhttp[44 * 4]
		WinHttpGetProxyResultEx endp
		WinHttpGetProxySettingsEx proc
			jmp DWORD ptr OriginalFuncs_winhttp[45 * 4]
		WinHttpGetProxySettingsEx endp
		WinHttpGetProxySettingsResultEx proc
			jmp DWORD ptr OriginalFuncs_winhttp[46 * 4]
		WinHttpGetProxySettingsResultEx endp
		WinHttpGetProxySettingsVersion proc
			jmp DWORD ptr OriginalFuncs_winhttp[47 * 4]
		WinHttpGetProxySettingsVersion endp
		WinHttpGetTunnelSocket proc
			jmp DWORD ptr OriginalFuncs_winhttp[48 * 4]
		WinHttpGetTunnelSocket endp
		WinHttpOpen proc
			jmp DWORD ptr OriginalFuncs_winhttp[49 * 4]
		WinHttpOpen endp
		WinHttpOpenRequest proc
			jmp DWORD ptr OriginalFuncs_winhttp[50 * 4]
		WinHttpOpenRequest endp
		WinHttpPacJsWorkerMain proc
			jmp DWORD ptr OriginalFuncs_winhttp[51 * 4]
		WinHttpPacJsWorkerMain endp
		WinHttpProbeConnectivity proc
			jmp DWORD ptr OriginalFuncs_winhttp[52 * 4]
		WinHttpProbeConnectivity endp
		WinHttpProtocolCompleteUpgrade proc
			jmp DWORD ptr OriginalFuncs_winhttp[53 * 4]
		WinHttpProtocolCompleteUpgrade endp
		WinHttpProtocolReceive proc
			jmp DWORD ptr OriginalFuncs_winhttp[54 * 4]
		WinHttpProtocolReceive endp
		WinHttpProtocolSend proc
			jmp DWORD ptr OriginalFuncs_winhttp[55 * 4]
		WinHttpProtocolSend endp
		WinHttpQueryAuthSchemes proc
			jmp DWORD ptr OriginalFuncs_winhttp[56 * 4]
		WinHttpQueryAuthSchemes endp
		WinHttpQueryConnectionGroup proc
			jmp DWORD ptr OriginalFuncs_winhttp[57 * 4]
		WinHttpQueryConnectionGroup endp
		WinHttpQueryDataAvailable proc
			jmp DWORD ptr OriginalFuncs_winhttp[58 * 4]
		WinHttpQueryDataAvailable endp
		WinHttpQueryHeaders proc
			jmp DWORD ptr OriginalFuncs_winhttp[59 * 4]
		WinHttpQueryHeaders endp
		WinHttpQueryHeadersEx proc
			jmp DWORD ptr OriginalFuncs_winhttp[60 * 4]
		WinHttpQueryHeadersEx endp
		WinHttpQueryOption proc
			jmp DWORD ptr OriginalFuncs_winhttp[61 * 4]
		WinHttpQueryOption endp
		WinHttpReadData proc
			jmp DWORD ptr OriginalFuncs_winhttp[62 * 4]
		WinHttpReadData endp
		WinHttpReadDataEx proc
			jmp DWORD ptr OriginalFuncs_winhttp[63 * 4]
		WinHttpReadDataEx endp
		WinHttpReadProxySettings proc
			jmp DWORD ptr OriginalFuncs_winhttp[64 * 4]
		WinHttpReadProxySettings endp
		WinHttpReadProxySettingsHvsi proc
			jmp DWORD ptr OriginalFuncs_winhttp[65 * 4]
		WinHttpReadProxySettingsHvsi endp
		WinHttpReceiveResponse proc
			jmp DWORD ptr OriginalFuncs_winhttp[66 * 4]
		WinHttpReceiveResponse endp
		WinHttpRefreshProxySettings proc
			jmp DWORD ptr OriginalFuncs_winhttp[67 * 4]
		WinHttpRefreshProxySettings endp
		WinHttpRegisterProxyChangeNotification proc
			jmp DWORD ptr OriginalFuncs_winhttp[68 * 4]
		WinHttpRegisterProxyChangeNotification endp
		WinHttpResetAutoProxy proc
			jmp DWORD ptr OriginalFuncs_winhttp[69 * 4]
		WinHttpResetAutoProxy endp
		WinHttpResolverGetProxyForUrl proc
			jmp DWORD ptr OriginalFuncs_winhttp[70 * 4]
		WinHttpResolverGetProxyForUrl endp
		WinHttpSaveProxyCredentials proc
			jmp DWORD ptr OriginalFuncs_winhttp[71 * 4]
		WinHttpSaveProxyCredentials endp
		WinHttpSendRequest proc
			jmp DWORD ptr OriginalFuncs_winhttp[72 * 4]
		WinHttpSendRequest endp
		WinHttpSetCredentials proc
			jmp DWORD ptr OriginalFuncs_winhttp[73 * 4]
		WinHttpSetCredentials endp
		WinHttpSetDefaultProxyConfiguration proc
			jmp DWORD ptr OriginalFuncs_winhttp[74 * 4]
		WinHttpSetDefaultProxyConfiguration endp
		WinHttpSetOption proc
			jmp DWORD ptr OriginalFuncs_winhttp[75 * 4]
		WinHttpSetOption endp
		WinHttpSetProxySettingsPerUser proc
			jmp DWORD ptr OriginalFuncs_winhttp[76 * 4]
		WinHttpSetProxySettingsPerUser endp
		WinHttpSetSecureLegacyServersAppCompat proc
			jmp DWORD ptr OriginalFuncs_winhttp[77 * 4]
		WinHttpSetSecureLegacyServersAppCompat endp
		WinHttpSetStatusCallback proc
			jmp DWORD ptr OriginalFuncs_winhttp[78 * 4]
		WinHttpSetStatusCallback endp
		WinHttpSetTimeouts proc
			jmp DWORD ptr OriginalFuncs_winhttp[79 * 4]
		WinHttpSetTimeouts endp
		WinHttpTimeFromSystemTime proc
			jmp DWORD ptr OriginalFuncs_winhttp[80 * 4]
		WinHttpTimeFromSystemTime endp
		WinHttpTimeToSystemTime proc
			jmp DWORD ptr OriginalFuncs_winhttp[81 * 4]
		WinHttpTimeToSystemTime endp
		WinHttpUnregisterProxyChangeNotification proc
			jmp DWORD ptr OriginalFuncs_winhttp[82 * 4]
		WinHttpUnregisterProxyChangeNotification endp
		WinHttpWebSocketClose proc
			jmp DWORD ptr OriginalFuncs_winhttp[83 * 4]
		WinHttpWebSocketClose endp
		WinHttpWebSocketCompleteUpgrade proc
			jmp DWORD ptr OriginalFuncs_winhttp[84 * 4]
		WinHttpWebSocketCompleteUpgrade endp
		WinHttpWebSocketQueryCloseStatus proc
			jmp DWORD ptr OriginalFuncs_winhttp[85 * 4]
		WinHttpWebSocketQueryCloseStatus endp
		WinHttpWebSocketReceive proc
			jmp DWORD ptr OriginalFuncs_winhttp[86 * 4]
		WinHttpWebSocketReceive endp
		WinHttpWebSocketSend proc
			jmp DWORD ptr OriginalFuncs_winhttp[87 * 4]
		WinHttpWebSocketSend endp
		WinHttpWebSocketShutdown proc
			jmp DWORD ptr OriginalFuncs_winhttp[88 * 4]
		WinHttpWebSocketShutdown endp
		WinHttpWriteData proc
			jmp DWORD ptr OriginalFuncs_winhttp[89 * 4]
		WinHttpWriteData endp
		WinHttpWriteProxySettings proc
			jmp DWORD ptr OriginalFuncs_winhttp[90 * 4]
		WinHttpWriteProxySettings endp
endif
end