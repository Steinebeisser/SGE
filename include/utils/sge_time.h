//
// Created by Geisthardt on 26.02.2025.
//

#ifndef SGE_TIME_H
#define SGE_TIME_H
#include <stdint.h>

char* current_time_formatted(void);
uint64_t get_current_ms_time(void);
int get_current_year(void);
int get_current_month(void);
int get_current_day(void);


#endif //SGE_TIME_H
