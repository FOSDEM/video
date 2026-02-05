import type.magic;

fn format_edid_timing_x_resolution(u8 raw) {
    if (raw == 0) {
        return "reserved";
    }
    return (raw+31)*8;
};

fn format_edid_timing_aspect(u8 raw) {
    if(raw == 0) {
        return "16:10";
    }
    if(raw == 1) {
        return "4:3";
    }
    if(raw == 2) {
        return "5:4";
    }
    if(raw == 3) {
        return "16:9";
    }
};

fn format_edid_timing_freq(u8 raw) {
    return raw+60;
};

bitfield EDIDTimingAspect {
    aspect: 2 [[format("format_edid_timing_aspect")]];
    frequency: 6 [[format("format_edid_timing_freq")]];
};

struct EDID_Timing {
    u8 x_resolution [[format("format_edid_timing_x_resolution")]];
    EDIDTimingAspect aspect;
};

struct EDID_DTD {
    u8 blob[18];
};

fn format_edid_year(u8 raw) {
    return raw+1990;
};

fn format_gamma(u8 raw) {
    return ((raw+100)/100.0);
};

bitfield EDIDVideoInputParameters {
    depth: 3;
    interface: 4;
    analog: 1;
};

bitfield EDIDSupportedFeatures {
    continuous_timing: 1;
    preferred_timing: 1;
    srgb: 1;
    display_type: 2;
    dpms_off: 1;
    dpms_suspend: 1;
    dpms_standby: 1;
};

bitfield EDIDEstablishedTimings1 {
    t800x600p60: 1;
    t800x600p56: 1;
    t800x600p75: 1;
    t640x480p72: 1;
    t640x480p67: 1;
    t640x480p60: 1;
    t720x400p88: 1;
    t720x400p70: 1;
};

bitfield EDIDEstablishedTimings2 {
    t1280x1024p75: 1;
    t1024x768p75: 1;
    t1024x768p70: 1;
    t1024x768p60: 1;
    t1024x768i87: 1;
    t832x624p75: 1;
    t800x600p75: 1;
    t800x600p70: 1;
};

bitfield EDIDEstablishedTimings3 {
    manufacturer: 7;
    t1152x870p75: 1;
};

struct EDID  {
    // Header
    type::Magic<"\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00"> magic [[color("FF0000")] ];
    u16 manufacturer_id;
    u16 product_code;
    u32 serial;
    u8 week_of_manufacture;
    u8 year_of_manufacture [[format("format_edid_year")]];
    u8 edid_version;
    u8 edid_revision;
    
    // Basic display parameters
    EDIDVideoInputParameters video_input_parameters;
    u8 horizontal_screen_size;
    u8 vertical_screen_size;
    u8 display_gamma [[format("format_gamma")]];
    EDIDSupportedFeatures supported_features;
    
    // Chromasity coordinates
    u8 chromasity_lsb_rg;
    u8 chromasity_lsb_bw;
    u8 chromasity_red_x;
    u8 chromasity_red_y;
    u8 chromasity_green_x;
    u8 chromasity_green_y;
    u8 chromasity_blue_x;
    u8 chromasity_blue_y;
    u8 chromasity_white_x;
    u8 chromasity_white_y;
    
    // Established timings
    EDIDEstablishedTimings1 established_timings_1;
    EDIDEstablishedTimings2 established_timings_2;
    EDIDEstablishedTimings3 established_timings_3;
    
    // Standard timing information
    EDID_Timing standard_timings[8];
    
    // Display timing / monitor descriptors
    EDID_DTD descriptor[4];
    
    // Extensions
    u8 extension_count;
    u8 checksum;
};

enum SampleRate :u8 {
    Rate48k = 0x00,
    Rate96k = 0x01,
};

enum I2SInputMode : u8 {
    Slave = 0x00,
    Master = 0x01,
};

bitfield Mix3Mode {
    HDMI: 1;
    I2S1: 1;
    I2S2: 1;
};

enum I2S1Mode : u8 {
    HDMI = 9,
    MIX3_USB = 10,
    MIX3 = 11,
    USB = 12,
};

enum UsbMute : u8 {
    Unmute = 0,
    Mute = 1,
};

enum LK621 : u8 {
    None = 0x00,
    LK26 = 0x01,
    LK20 = 0x02,
    BOTH = 0x03,
};

struct audio_route_t {
    SampleRate sample_rate;
    I2SInputMode i2s_in_mode;
    Mix3Mode mix3_mode;
    I2S1Mode i2s1_mode;
    UsbMute usb_out_mute;
    LK621 lk621_en;
};

EDID edid @ 0x5a;

struct user_string_t {
    u8 length;
    u8;
    char16 text[length];
};

struct user_config {
    user_string_t manufacturer;
    user_string_t product;
    user_string_t u2product;
};

struct string {
    u8 len;
    char value[len-1];
};

// Pretty sure, based on the windows utility.
struct Flash {
    type::Magic<"\x3c\xc3"> magic [[color("FF0000")] ];
    be u16 code_len;
    u16 usb_vid;
    u16 usb_pid;
    u8 unknown[8];
    string video_name;
    u8 unknown2[8];
    string audio_name;
    u8 unknown3[8];
};

Flash flash @ 0;

// From ms-tools
u16 header_checksum @flash.code_len;
u16 code_checksum @flash.code_len+2;

// Right location it seems, but invalid parser
user_config cfg @ 0x2CE4;

// Best guess for the location of the struct, might actually be relative to flash.code_len
audio_route_t audio @ 0xf333;
