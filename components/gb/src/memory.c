/**
 * @file memory.c
 * @author Matheus T. dos Santos (tenoriomatheus0@gmail.com)
 * @brief
 * @version 0.1
 * @date 04-03-2021
 *
 * @copyright MT White todos os direitos reservados (c) 2021
 *
 */
#include "memory.h"

uint8_t memory_get(memory_t *m, uint16_t idx)
{
    return m->get(m, idx);
}

void memory_set(memory_t *m, uint16_t idx, uint8_t val)
{
    m->set(m, idx, val);
}

uint16_t memory_get_word(memory_t *m, uint16_t idx)
{
    return ((uint16_t) m->get(m, idx)) | (((uint16_t) m->get(m, idx + 1)) << 8);
}

void memory_set_word(memory_t *m, uint16_t idx, uint16_t val)
{
    m->set(m, idx, val & 0xFF);
    m->set(m, idx, val >> 8);
}
