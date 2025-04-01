#pragma once

// Syncs system time using SNTP and logs the result
void sync_time(void);

// Returns the current UNIX timestamp (seconds since epoch)
unsigned long get_unix_timestamp(void);