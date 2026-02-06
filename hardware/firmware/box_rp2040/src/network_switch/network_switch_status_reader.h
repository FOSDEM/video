#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t bmcr;  // Basic mode control register
    uint32_t bmsr;  // Basic mode status register
    uint32_t an;    // Auto negotiation settings
    uint32_t lpa ;  // Link partner ability
    uint32_t gbec;
    uint32_t gbes;
} nsw_port_regs_t;

typedef enum {
    nsw_link_unknown,
    nsw_link_down,
    nsw_link_full_duplex,
    nsw_link_half_duplex
} nsw_link_state_t;

typedef uint16_t vlan_id_t;

typedef struct {
	vlan_id_t vid;
	uint16_t mbr;
	uint16_t untag;
	uint16_t ivl_en;
	uint16_t fid_msti;
	uint16_t envlanpol;
	uint16_t meteridx;
	uint16_t vbpen;
	uint16_t vbpri;
} nsw_vlan_cfg_t;

typedef struct {
	uint16_t index;
	vlan_id_t vid;
	uint16_t mbr;
} nsw_mc_cfg_t;

void nsw_init(void);
bool nsw_identify(uint32_t* id, uint32_t* ver);
bool nsw_read_port_regs(uint8_t port, nsw_port_regs_t* regs);
nsw_link_state_t nsw_link_state(nsw_port_regs_t* regs);
uint16_t nsw_link_speed_mbps(nsw_port_regs_t* regs);
void nsw_dump_vlan_table();
void nsw_dump_member_config();
void nsw_dump_pvids();

bool nsw_config_vlans(bool enable);
bool nsw_vlan_set(nsw_vlan_cfg_t *cfg);
bool nsw_vlan_get(vlan_id_t vid, nsw_vlan_cfg_t *cfg);

typedef enum vlan_port_filter {
	PORT_ACCEPT_ALL = 0,
	PORT_ACCEPT_TAGGED_ONLY = 1,
	PORT_ACCEPT_UNTAGGED_ONLY = 2,
} vlan_port_filter_t;
bool nsw_port_vlan_filtering(int port, vlan_port_filter_t filtering);
bool nsw_mc_set(int mcindex, vlan_id_t vid, uint32_t members);
bool nsw_port_set_mc(int port, int mcindex);

typedef enum tagmode {
	PORT_TAGMODE_ORIGINAL = 0,
	PORT_TAGMODE_KEEP_FORMAT,
	PORT_TAGMODE_REAL_KEEP_FORMAT,
	PORT_TAGMODE_PRI,
} tagmode_t;
bool nsw_port_set_tagmode(int port, tagmode_t tagmode);

void nsw_vlan_init();