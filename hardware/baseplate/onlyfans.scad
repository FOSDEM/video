use <nuts.scad>

onlyfans_size_x = 16.5;
onlyfans_size_y = 105;
holder_hole_spacing = 4.5;

module onlyfans_holes() {
    translate([holder_hole_spacing, -holder_hole_spacing]) nuthole_m3();
    translate([12, -holder_hole_spacing]) nuthole_m3();
    translate([holder_hole_spacing, holder_hole_spacing + onlyfans_size_y]) nuthole_m3();
    translate([12, holder_hole_spacing + onlyfans_size_y]) nuthole_m3();
}

module onlyfans_footprint() {
    union() {
        rrect(onlyfans_size_x, onlyfans_size_y, 2);
        square([3, onlyfans_size_y]);
    }
}

module onlyfans_cutout() {
    onlyfans_holes();
    onlyfans_footprint();
}

module onlyfans() {
    linear_extrude(20)
    onlyfans_cutout();
}

