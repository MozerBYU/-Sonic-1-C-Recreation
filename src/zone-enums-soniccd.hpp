#pragma once

enum class ZoneSonicCD {
    GREEN_GROVE,
    RUSTY_RUIN,
    SPRING_STADIUM,
    DIAMOND_DUST,
    VOLCANO_VALLEY,
    GENE_GADGET,
    PANIC_PUPPET,
    FINAL_FIGHT,
};

static const char* ZoneSonicCDToString(ZoneSonicCD zone) {
    static const char* str[] = {
        "GREEN_GROVE",
        "RUSTY_RUIN",
        "SPRING_STADIUM",
        "DIAMOND_DUST",
        "VOLCANO_VALLEY",
        "GENE_GADGET",
        "PANIC_PUPPET",
        "FINAL_FIGHT",
    };

    return str[(int)zone];
};