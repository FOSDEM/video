use <audio_board.scad>
use <power_board.scad>
use <switch_board.scad>
use <hdmi.scad>
use <radxa_x2l.scad>
use <radxa_x4.scad>
use <onlyfans.scad>
use <display.scad>
use <case.scad>
use <nuts.scad>

$fn=120;


switch_transl = [187.5,53,0];
power_transl = [523.5,-60,0];
audio_transl = [170,150,0];
radxa_x2l_transl = [315,191,0];
radxa_x4_transl = [155,130,0];
hdmi_transl = [394, 194.8, 0];
onlyfans_transl = [0, 34.5, 0];
display_transl = [135, 0, 0];

module baseplate() {
    color("#d5c5a4") linear_extrude(2) baseplate_shape();
}

module baseplate_shape() {
    difference() {
        case_fit_shape();

        translate(switch_transl) rotate(180) scale([1, -1, 1]) switch_holes();
        translate(power_transl) rotate(180) scale([1, -1, 1]) power_holes();
        translate(audio_transl) scale([1, -1, 1]) audio_holes();
        translate(radxa_x2l_transl) rotate(180) radxa_x2l_holes();
        translate(radxa_x4_transl) radxa_x4_holes();
        translate(hdmi_transl) rotate(180) hdmi_holes();
        translate(onlyfans_transl) onlyfans_cutout();
        translate(display_transl) display_cutout();
    };
}

module boards() {
    translate(switch_transl) rotate(180) switch();
    translate(power_transl) rotate(180) power();
    translate(audio_transl) audio();
    translate(radxa_x2l_transl) rotate(180) radxa_x2l();
    translate([0, 0, 5]) translate(radxa_x4_transl) radxa_x4();
    translate(hdmi_transl) rotate(180) hdmi();
    translate(onlyfans_transl) onlyfans();
    translate(display_transl) display();
}

module box() {
    translate([0, 0, 4]) boards();
    case();
    baseplate();
}

// box()
baseplate_shape();