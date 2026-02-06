# RTL8723S api

The VLAN handling is processed by two tables. The VLAN table has 4096 entries and is
used for processing incoming traffic with a vlan tag set and defines which ports
are part of that specific VLAN and which ports should have the tag stripped on
egress.

The second table is the MemberConfig table. this only has 16 rows and is for
matching all the incoming untagged traffic. Each port can be assigned to a
MemberConfig entry which will assign the incoming packets a VLAN tag before
it is processed through the main VLAN handling table.

All the port lists are bitfields that refer to the PHY IDs of the switch ports.
These IDs are printed on the PCB silkscreen near every transformer with
the `PHY%` label.

| PHY  | Port                  |
|------|-----------------------|
| PHY0 | Port facing the Radxa |
| PHY1 | Port 4 on the outside |
| PHY2 | Port 3 on the outside |
| PHY3 | Port 2 on the outside |
| PHY4 | Port 1 on the outside |

```c
// Set the MemberConfig for all ports to 0 and enable tag handling on the ports
nsw_vlan_init();

// Enable the VLAN processing in the switch
nsw_config_vlans(true);

// Create a VLAN table entry for VLAN 10 with port 1, 2 and 3 as members
// Port 2 and 3 will be untagged on egress
nsw_vlan_cfg_t vlan10 = {
		.vid = 10,
		.mbr = BIT(0) | BIT(1) | BIT(2),
		.untag = BIT(1) | BIT(2),
};
nsw_vlan_set(&vlan10);

// Create a MemberConfig row to ingress untagged traffic on port 2 and 3
// Use MemberConfig index 1 and vlan 10
nsw_mc_set(1, 10, BIT(1) | BIT(2));

// Point the two untagged ports to MemberConfig index 1. This needs to
// happen _after_ setting the ports as member in the MemberConfig itself
nsw_port_set_mc(1, 1);
nsw_port_set_mc(1, 2);

// Set the untagged ports to drop traffic with a vlan tag set
nsw_port_vlan_filtering(1, PORT_ACCEPT_UNTAGGED_ONLY);
nsw_port_vlan_filtering(2, PORT_ACCEPT_UNTAGGED_ONLY);

// Drop untagged traffic from the trunk port
nsw_port_vlan_filtering(2, PORT_ACCEPT_TAGGED_ONLY);
```