/**
 * @file clock.h
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

int clock_power_up(uint32_t period);

int clock_next(uint32_t cycles, uint32_t *result);

#endif  //__CLOCK_H__
