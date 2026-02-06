include <modules/rcube.scad>

$fn=65;

// rough stamp of a Sunon MF35100V1-1000U-G99
module fan() {
    sink=1;

    module screwhole() {
        translate([1.6,1.6,-sink]) cylinder(h=10+sink*2, d=3.5);
        translate([1.6,1.6,-sink]) cylinder(h=2.5+sink, d=6);
    }

    color("black") {
        translate([17.5,17.5,-sink]) cylinder(h=10+sink*2, d=33);
        translate([1.4,1.4,0]) screwhole();
        translate([1.4,30.4,0]) screwhole();
        translate([30.4,30.4,0]) screwhole();
        translate([30.4,1.4,0]) screwhole();
    }
}

module ycyl(h=10,d=10) {
    translate([0,h,0]) rotate([90, 0, 0]) cylinder(h=h, d=d);
}

module assembly() {
    difference() {
        framet = 3;
        frameh = 35;
        //framew = 78.8+(11.2*2);
        framew = 123;

        union() {
            // main frame
            cube([framew, frameh, framet]);

            // screwhole ledge L
            translate([0,5,0]) {
                translate([5,0,0]) cube([10,3,8]);
                translate([5,0,3]) ycyl(d=10,h=3);
            }

            // screwhole ledge L
            translate([framew-15,5,0]) {
                translate([0,0,0]) cube([10,3,8]);
                translate([10,0,3]) ycyl(d=10,h=3);
            }
        }

        // clean up the back
        translate([0,0,-5]) cube([150,150,5]);

        // cutout over baseplate
        translate([0,0,-1]) cube([11,5,framet+2]);
        translate([framew-10,0,-1]) cube([11,5,framet+2]);

        // screwhole in the ledges
        translate([4.5  , 4, 4]) ycyl(h=5, d=3.5);
        translate([118.5, 4, 4]) ycyl(h=5, d=3.5);

        // fan is 35x35
        translate([20, 0, 0])           fan(); // fan L
        translate([framew-35-20, 0, 0]) fan(); // fan R

        // saving some material with a cutout in the middle
        saverh = 30;
        saverw = 10;
        translate([(framew/2)-(saverw/2), (frameh/2)-saverh/2, -1]) rcube([saverw, saverh, framet+2], 1);

        // cutout on the side
        sidecutw = 20;
        sidecuth = 100;
        translate([-2, 8, -1]) rcube([sidecutw, sidecuth, framet+2], 2);
        translate([framew-sidecutw+2, 8, -1]) rcube([sidecutw, sidecuth, framet+2], 2);
    }
}

assembly();