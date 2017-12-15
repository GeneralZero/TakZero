#pragma once

uint16_t cfg_max_playouts = 50000;
uint8_t cfg_num_threads = 3;
bool cfg_quiet = false;
bool cfg_noise = true;

FILE* cfg_logfile_handle;

uint8_t cfg_random_cnt = 30;

double cfg_resignpct = 0.0;

double cfg_puct = 2.8;