#ifndef __STA_INFO_H_
#define __STA_INFO_H_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <wifi.h>

#define NUM_STA 32
#define NUM_ACL 64


//if mode ==0, then the sta is allowed once the addr is hit.
//if mode ==1, then the sta is rejected once the addr is non-hit.
struct wlan_acl_node {
        _list		        list;
        u8       addr[ETH_ALEN];
        u8       mode;
};

struct wlan_acl_pool {
        struct wlan_acl_node aclnode[NUM_ACL];
};


struct	stainfo_stats	{

	u64	rx_pkts;
	u64	rx_bytes;
	u64	rx_drops;
	
	u64	tx_pkts;
	u64	tx_bytes;
	u64  tx_drops;

};

struct sta_info {

	_lock lock;
	_list list; //free_sta_queue
	_list hash_list; //sta_hash
	//_list asoc_list; //20061114
	//_list sleep_list;//sleep_q
	//_list wakeup_list;//wakeup_q
	
	struct sta_xmit_priv sta_xmitpriv;
	struct sta_recv_priv sta_recvpriv;
	
	_queue sleep_q;
	unsigned int sleepq_len;
	
	uint state;
	uint aid;
	uint mac_id;
	uint qos_option;
	u8	hwaddr[ETH_ALEN];

	uint	ieee8021x_blocked;	//0: allowed, 1:blocked 
	uint	dot118021XPrivacy; //aes, tkip...
	union Keytype	dot11tkiptxmickey;
	union Keytype	dot11tkiprxmickey;
	union Keytype	dot118021x_UncstKey;	
	union pn48		dot11txpn;			// PN48 used for Unicast xmit.
	union pn48		dot11rxpn;			// PN48 used for Unicast recv.


	u8	bssrateset[16];
	uint	bssratelen;
	s32  rssi;
	s32	signal_quality;
	
	unsigned char		cts2self;
	unsigned char		rtsen;

	unsigned char		raid;
	unsigned int 		init_rate;

	struct stainfo_stats sta_stats;

	//for A-MPDU Rx reordering buffer control 
	struct recv_reorder_ctrl recvreorder_ctrl[16];

	//for A-MPDU Tx
	//unsigned char		ampdu_txen_bitmap;

#ifdef CONFIG_80211N_HT
	struct ht_priv	htpriv;	
#endif
	

	//Notes:	
	//STA_Mode:
	//curr_network(mlme_priv/security_priv/qos/ht) + sta_info: (STA & AP) CAP/INFO	
	//scan_q: AP CAP/INFO

	//AP_Mode:
	//curr_network(mlme_priv/security_priv/qos/ht) : AP CAP/INFO
	//sta_info: (AP & STA) CAP/INFO
		
#ifdef CONFIG_NATIVEAP_MLME

	_list asoc_list;
	_list auth_list;
	 
	unsigned int expire_to;
	unsigned int auth_seq;
	unsigned int authalg;
	unsigned char chg_txt[128];

	unsigned int tx_ra_bitmap;

#endif	

	

};



struct	sta_priv {
	
	u8 *pallocated_stainfo_buf;
	u8 *pstainfo_buf;
	_queue	free_sta_queue;
	
	_lock sta_hash_lock;
	_list   sta_hash[NUM_STA];
	int asoc_sta_count;
	_queue sleep_q;
	_queue wakeup_q;
	
	_adapter *padapter;
	
#ifdef CONFIG_NATIVEAP_MLME
    	
	_list asoc_list;
	_list auth_list;

	unsigned int auth_to;  //sec, time to expire in authenticating.
	unsigned int assoc_to; //sec, time to expire before associating.
	unsigned int expire_to; //sec , time to expire after associated.
	
#endif		
	
};


static __inline u32 wifi_mac_hash(u8 *mac)
{
        u32 x;

        x = mac[0];
        x = (x << 2) ^ mac[1];
        x = (x << 2) ^ mac[2];
        x = (x << 2) ^ mac[3];
        x = (x << 2) ^ mac[4];
        x = (x << 2) ^ mac[5];

        x ^= x >> 8;
        x  = x & (NUM_STA - 1);
		
        return x;
}


extern u32	_init_sta_priv(struct sta_priv *pstapriv);
extern u32	_free_sta_priv(struct sta_priv *pstapriv);
extern struct sta_info *alloc_stainfo(struct	sta_priv *pstapriv, u8 *hwaddr);
extern u32	free_stainfo(_adapter *padapter , struct sta_info *psta);
extern void free_all_stainfo(_adapter *padapter);
extern struct sta_info *get_stainfo(struct sta_priv *pstapriv, u8 *hwaddr);
extern u32 init_bcmc_stainfo(_adapter* padapter);
extern struct sta_info* get_bcmc_stainfo(_adapter* padapter);
extern u8 access_ctrl(struct wlan_acl_pool* pacl_list, u8 * mac_addr);

#endif //_STA_INFO_H_
