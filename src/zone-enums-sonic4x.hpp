#pragma once

enum class ZoneSonic4X {
    // New Maps
    /* 
     * Format
     * "NEW_MAP," 
    */
  
};

static const char* ZoneSonic4XToString(ZoneSonic4D zone) {
    static const char* str[] = {
        // New Maps
        /* 
         * Format
         * "NEW Map",
        */
    };

    return str[(int)zone];
};