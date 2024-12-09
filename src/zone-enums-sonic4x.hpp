#pragma once

enum class ZoneSonic4X {
    WINDY_ORGAN,
    NUCLEAR_WASTELAND,
    HARDCORE_PALACE,
    DOOMSDAY_XTREME,
};

static const char* ZoneSonic4XToString(ZoneSonic4D zone) {
    static const char* str[] = {
        "WINDY_ORGAN",
        "NUCLEAR_WASTELAND",
        "HARDCORE_PALACE",
        "DOOMSDAY_XTREME"
    };

    return str[(int)zone];
};