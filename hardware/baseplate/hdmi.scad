use <nuts.scad>

hdmi_hole_half_dist = 52 / 2;
hdmi_main_square = 58.7;
hdmi_centre = hdmi_main_square / 2;

module hdmi_holes() {
    margin = hdmi_centre - hdmi_hole_half_dist;
    far = hdmi_centre + hdmi_hole_half_dist;

    translate([margin,far,0]) hole_for_standoff_m4_6mm();
    translate([far,far,0]) hole_for_standoff_m4_6mm();

    // make front holes M3 since there is no space for M4
    // hope that if back holes are screwed in first, the
    // hagibis will not move around too much
    translate([margin,margin,0]) hole_for_standoff_m3_6mm();
    translate([far,margin,0]) hole_for_standoff_m3_6mm();
}

module hdmi_connectors() {
    hdmi_conn_w = 15.2;
    usb_conn_w = 13.2;

    union() {
        linear_extrude(5.5)
        union() {
            translate([hdmi_centre - 20, -1.6, 0])
            rrect(hdmi_conn_w, 12, 0.1);
            translate([hdmi_centre + 20 - hdmi_conn_w, -1.6, 0])
            rrect(hdmi_conn_w, 12, 0.1);
        }

        translate([0, 0, -1.2])
        linear_extrude(5.5)
        union() {
            translate([7.5, 47, 0])
            rrect(usb_conn_w, 14.2, 0.1);
        }

        translate([hdmi_main_square - 13, hdmi_main_square + 3.5, 2.5])
        rotate([90, 0, 0])
        linear_extrude(14)
        circle(r=2.5);
    }
}

module hdmi() {
    brd_thickness = 1;
    
    union() {
        color("gray") translate([0, 0, brd_thickness]) hdmi_connectors();
        linear_extrude(brd_thickness)
        difference() {
            rrect(hdmi_main_square, hdmi_main_square,2);
            hdmi_holes();
        }
    }
}

hdmi();
