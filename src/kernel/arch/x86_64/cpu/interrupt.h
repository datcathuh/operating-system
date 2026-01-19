#pragma once

inline void cli(void) { __asm__ volatile("cli"); }
inline void sti(void) { __asm__ volatile("sti"); }
