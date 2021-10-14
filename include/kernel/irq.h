#pragma once

#include <kernel/features/ssp.h>

void irq_init_vectors();
void irq_enable();
void irq_disable();

void enable_interrupt_controller();