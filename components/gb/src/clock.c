/**
 * @file clock.c
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#include "clock.h"

static struct clock_fields {
    uint32_t period;
    uint32_t n;
    bool is_init;
} this;

int clock_power_up(uint32_t period)
{
    if (this.is_init) {
        return -EAGAIN;
    }

    this.period  = period;
    this.n       = 0x00;
    this.is_init = true;

    return 0;
}

int clock_next(uint32_t cycles, uint32_t *result)
{
    if (!this.is_init) {
        return -ENOENT;
    }

    if (result == NULL) {
        return -ENODEV;
    }

    this.n += cycles;
    *result = this.n / this.period;
    this.n  = this.n % this.period;

    return 0;
}
