use <nuts.scad>

module audio_holes() {
    translate([153.4, 116.2, 0])
        circle(r=4.4);
    translate([217.75, 129, 0])
        nuthole_m3();
    translate([74.75, 93.25, 0])
        nuthole_m3();
    translate([217.75, 93.25, 0])
        nuthole_m3();
    translate([74.75, 129, 0])
        nuthole_m3();
}

module audio() {
    brd_thickness = 1.6;
    color("#009900")
    scale([1, -1, 1])
    difference() {
        linear_extrude(brd_thickness) {
            polygon(points = [[222.25,90.75], [222.25,148.25], [222.22538,148.56286], [222.15212,148.86804], [222.03202,149.15798], [221.86804,149.42558], [221.66422,149.66422], [221.42558,149.86804], [221.15798,150.03202], [220.86804,150.15212], [220.56286,150.22538], [220.25,150.25], [72.25,150.25], [71.93714,150.22538], [71.63196,150.15212], [71.34202,150.03202], [71.07442,149.86804], [70.83578,149.66422], [70.63196,149.42558], [70.46798,149.15798], [70.34788,148.86804], [70.27462,148.56286], [70.25,148.25], [70.25,90.75], [70.27462,90.43714], [70.34788,90.13196], [70.46798,89.84202], [70.63196,89.57442], [70.83578,89.33578], [71.07442,89.13196], [71.34202,88.96798], [71.63196,88.84788], [71.93714,88.77462], [72.25,88.75], [220.25,88.75], [220.56286,88.77462], [220.86804,88.84788], [221.15798,88.96798], [221.42558,89.13196], [221.66422,89.33578], [221.86804,89.57442], [222.03202,89.84202], [222.15212,90.13196], [222.22538,90.43714], [222.25,90.75]]);
        }
        translate([0, 0, -1]) linear_extrude(brd_thickness+2) audio_holes();
    }
}