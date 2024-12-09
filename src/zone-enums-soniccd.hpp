#pragma once

enum class ZoneSonicCD {
    // New Maps
    /* 
     * Format
     * "NEW_MAP," 
    */
  
};

static const char* ZoneSonicCDToString(ZoneSonicCD zone) {
    static const char* str[] = {
        // New Maps
        /* 
         * Format
         * "NEW Map",
        */
    };

    return str[(int)zone];
};